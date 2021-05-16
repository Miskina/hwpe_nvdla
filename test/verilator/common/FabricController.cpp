#include "FabricController.h"
#include "Util.h"

#define FABRIC_CONTROLLER_LOG(stream, format, ...) fprintf(stream, "[%s] - " format "\n", name_.c_str(), ##__VA_ARGS__)
#define FABRIC_CONTROLLER_INFO(format, ...) FABRIC_CONTROLLER_LOG(stdout, format, __VA_ARGS__)
#define FABRIC_CONTROLLER_ERR(format, ...) FABRIC_CONTROLLER_LOG(stderr, format, __VA_ARGS__) 
#define FABRIC_CONTROLLER_ABORT(...)    \
    HWPE_NVDLA_MACRO_START              \
    FABRIC_CONTROLLER_ERR("Aborting!"); \
    abort();                            \
    HWPE_NVDLA_MACRO_END


FabricController::FabricController(std::string&& name) noexcept
    : name_(std::forward<std::string>(name))
{
}


FabricController::~FabricController() noexcept
{
    util::delete_if(memory_ctrl_);
    util::delete_if(ctrl_intf_);

    if (trace_file_)
    {
        fclose(trace_file_);
        trace_file_ = nullptr;
    }
}

void FabricController::attach(ControlInterface* ctrl_intf, bool delete_old) noexcept
{
    if(ctrl_intf_ && delete_old)
    {
        delete ctrl_intf_;
    }

    ctrl_intf_ = ctrl_intf;
}

void FabricController::attach(MemoryController* mem_ctrl, bool delete_old) noexcept
{
    if(memory_ctrl_ && delete_old)
    {
        delete memory_ctrl_;
    }
    
    memory_ctrl_ = mem_ctrl;
}

void FabricController::load_trace(const std::string& trace_path)
{

    if (trace_file_)
    {
        fclose(trace_file_);
    }

    const char* trace_path_cstr = trace_path.c_str();
    FABRIC_CONTROLLER_INFO("Setting up trace from file: %s", trace_path_cstr); 

    trace_file_ = fopen(trace_path_cstr, "rb");
    if (!trace_file_)
    {
        FABRIC_CONTROLLER_ERR("Failed to open trace file: %s", trace_path_cstr);
        FABRIC_CONTROLLER_ABORT();
    }

}

TraceCommand FabricController::read_trace_command()
{
    TraceCommand cmd = TraceCommand::Invalid;

    fread(&cmd, sizeof(cmd), 1, trace_file_);

    return cmd;
}

void FabricController::read_from_trace(uint8_t* data, size_t size)
{
    if(fread(data, 1, size, trace_file_) != size)
    {
        FABRIC_CONTROLLER_ERR("Failed to read data from trace file");
        FABRIC_CONTROLLER_ABORT();
    }
}

bool FabricController::execute_current_command()
{

    if (current_command == TraceCommand::Invalid)
    {
        FABRIC_CONTROLLER_ERR("Failed to read trace command!");
        FABRIC_CONTROLLER_ABORT();
    }

    switch(current_command)
    {
    case TraceCommand::WFI:
        if (is_interrupted())
        {
            current_command = TraceCommand::Invalid;
            clear_interrupt();
        }
        break;

    case TraceCommand::ReadRegister:

        if (ctrl_intf_->is_ready())
        {
            ControlOperation operation = ControlOperation::Read();
            uint32_t ignored_mask;
            read_from_trace(&operation.addr, &ignored_mask, &operation.data);
            ctrl_intf_->submit_operation(operation);
            current_command = TraceCommand::Invalid;
        }
        break;

    case TraceCommand::WriteRegister:
        if (ctrl_intf_->is_ready())
        {
            ControlOperation operation = ControlOperation::Write();
            read_from_trace(&operation.addr, &operation.data);
            ctrl_intf_->submit_operation(operation);
            current_command = TraceCommand::Invalid;
        }
        break;
    case TraceCommand::DumpMemory:
        FABRIC_CONTROLLER_ERR("Dump memory not implemented!");
        FABRIC_CONTROLLER_ERR("Ignoring...");

        uint32_t addr;
        uint32_t len;

        read_from_trace(&addr, &len);
        fseek(trace_file_, len, SEEK_CUR);

        read_from_trace(&len);
        fseek(trace_file_, len, SEEK_CUR);

        current_command = TraceCommand::Invalid;
        break;
    case TraceCommand::LoadIntoMemory:
        if (memory_ctrl_->is_ready())
        {
            uint32_t addr;
            uint32_t len;
            uint8_t* buffer;

            read_from_trace(&addr, &len);
            buffer = new uint8_t[len];
            
            memory_ctrl_->write(addr, buffer, len);

            current_command = TraceCommand::Invalid;
        }
        break;

    case TraceCommand::RegisterSyncpoint:

        uint32_t id;
        uint32_t mask;

        read_from_trace(&id, &mask);
        sync_points_.emplace(TRACE_SYNCPT_MASK | id, mask);
        ++sync_points_to_process;
        current_command = TraceCommand::Invalid;
        break;

    case TraceCommand::SetInterruptRegisters:
        read_from_trace(&interrupt_status_addr, &interrupt_mask_addr);
        current_command = TraceCommand::Invalid;
        break;

    case TraceCommand::SyncpointCheckCRC:

        uint32_t sp_id;
        read_from_trace(&sp_id);

        SyncPoint& point = sync_points_[sp_id];
        read_from_trace(&point.base, &point.size, &point.crc);

        current_command = TraceCommand::Invalid;
        break;
    
    case TraceCommand::SyncpointCheckNothing:
        uint32_t sp_id;
        read_from_trace(&sp_id);

        current_command = TraceCommand::Invalid;
        break;

    case TraceCommand::Close:
        FABRIC_CONTROLLER_INFO("Finished trace simulation!");
        break;
    }

    return true;
}


uint32_t calculate_crc(uint32_t base_addr, uint32_t size, MemoryController* mem_ctrl) noexcept
{
    uint32_t crc = ~0;
    uint32_t addr = base_addr;
    uint32_t len = size;
    
    if (((addr & 0x80000000) != 0x80000000) || !mem_ctrl)
    {
        fprintf(stderr, "NV SMALL does not support writing to non DBB memory");
        abort();
    }

    while (len)
    {
        uint8_t da;
        mem_ctrl->read(addr, &da, sizeof(da));
        
        crc ^= da;
        for (int i = 0; i < 8; i++)
        {
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
        }
        
        addr++;
        len--;
    }
    
    return ~crc;
}

void FabricController::process_sync_point(SyncPoint& sync_point)
{
    ctrl_intf_->submit_operation(ControlOperation::Write(interrupt_status_addr, sync_point.mask), {});

    if (!sync_point.is_noop())
    {
        uint32_t calculated_crc = calculate_crc(sync_point.base, sync_point.size, memory_ctrl_);
        if (sync_point.crc != calculated_crc)
        {
            FABRIC_CONTROLLER_ERR("CRC check failed, expected = %08x, calculated = %08x",
                                    sync_point.crc, calculated_crc);
        }
    }

    sync_point.processed = true;
    --sync_points_to_process;
}

void FabricController::check_sync_point() noexcept
{
    uint32_t status = ~interrupt_mask & interrupt_status;
    for (auto& [id, sync_point] : sync_points_)
    {
        if (!sync_point.processed && (status & sync_point.mask) == sync_point.mask)
        {
            process_sync_point(sync_point);
            return;
        }

    }
}

bool FabricController::sync_points_finished() const noexcept
{
    return sync_points_to_process > 0;
}

bool FabricController::eval()
{
    if (!trace_file_processing_finished_)
    {
        if (current_command == TraceCommand::Invalid)
        {
            current_command = read_trace_command();
        }

        trace_file_processing_finished_ = execute_current_command();

        return true;
    }

    if (trace_file_processing_finished_ && !sync_points_finished())
    {
        if (ctrl_intf_->is_ready())
        {
            ctrl_intf_->submit_operation(ControlOperation::Read(interrupt_mask_addr),
                    {&interrupt_mask_valid, &interrupt_mask});
        }

        if (ctrl_intf_->is_ready())
        {
            ctrl_intf_->submit_operation(ControlOperation::Read(interrupt_status_addr),
                    {&interrupt_status_valid, &interrupt_status});
        }

        if (interrupt_mask_valid && interrupt_status_valid)
        {
            check_sync_point();
        }

        return true;
    }

    return false;
}