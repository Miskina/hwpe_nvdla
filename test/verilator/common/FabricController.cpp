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

    // TODO: Sync point processing
}