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

        sync_point_id_t id;
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
        uint32_t base;
        uint32_t size;
        uint32_t crc; 

        read_from_trace(&sp_id, &base, &size, &crc);

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

        if (trace_file_processing_finished_ && !sync_points_.empty())
        {
            /* TODO:
              1. read interrupt_mask
              2. read interrupt_status
              3. kako provjeriti procitane podatke a da nismo usko vezani uz ControllInterface?
                 - mogli bi imati u operation strukturi pointer za r_data, gdje upisati r_data
                   (ili samo normalno mjesto za spremit, ne pointer) i flag kad je data zapravo procitan
                   i onda tu nakon postavljanja zahtjeva samo provjeravati kad je procitan data.
                   Jedino je pitanje kako poslati op a da se ne unisti pri opqueue pop i da mi dobijemo
                   r_data nazad...
                   Mozda s pointerima onda nije losa ideja jer ce se to spremit u pointere, a mi
                   ne moramo brinut o op
            */
            static bool interrupt_mask_read_request_posted = false, interrupt_status_read_request_posted = false;
            static bool interrupt_mask_read = false, interrupt_status_read = false;
            static uint32_t interrupt_mask, interrupt_status;

            if (!interrupt_mask_read_request_posted && ctrl_intf_->is_ready())
            {
                ControlOperation op = ControlOperation::Read();
                op.addr = interrupt_mask_addr;
                op.r_data = &interrupt_mask;
                op.finished = &interrupt_mask_read;
                ctrl_intf_->submit_operation(op);
            }

            if (!interrupt_status_read_request_posted && ctrl_intf_->is_ready())
            {
                ControlOperation op = ControlOperation::Read();
                op.addr = interrupt_status_addr;
                op.r_data = &interrupt_status;
                op.finished = &interrupt_status_read;
                ctrl_intf_->submit_operation(op);
            }

            if (interrupt_mask_read && interrupt_status_read)
            {
                // Interrupt polling
                uint32_t status = ~interrupt_mask & interrupt_status;
                for (auto& [id, mask] : sync_points_)
                {
                    if ((status & mask) != mask)
                        continue;
                    
                    ControlOperation op = ControlOperation::Write();
                    op.addr = interrupt_status_addr;
                    op.data = mask;
                    ctrl_intf_->submit_operation(op);

                    uint32_t sync_point_id = id & ~TRACE_SYNCPT_MASK;
                    
                    // Check if it->first needs to do a crc check
                    if (is_crc(sync_point_id))
                    {
                        crc(sync_point_id);
                    }
                }
                interrupt_mask_read_request_posted = false;
                interrupt_status_read_request_posted = false;
                interrupt_mask_read = false;
                interrupt_status_read = false;
            }
        }

        if (trace_file_processing_finished_ && sync_points_.empty())
        {
            // TODO: Done
        }

        return true;
    }

    // TODO: Sync point processing
}