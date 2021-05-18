#include "PeriphController.h"

#define PERIPH_CONTROL_LOG(stream, format, ...) HWPE_NVDLA_COMPONENT_LOG(stream, name_.c_str(), format, ##__VA_ARGS__) 
#define PERIPH_CONTROL_INFO(format, ...) PERIPH_CONTROL_LOG(stdout, format, ##__VA_ARGS__)
#define PERIPH_CONTROL_ERR(format, ...) PERIPH_CONTROL_LOG(stderr, format, ##__VA_ARGS__) 
#define PERIPH_CONTROL_ABORT(...)    \
    HWPE_NVDLA_MACRO_START           \
    PERIPH_CONTROL_ERR("Aborting!"); \
    abort();                         \
    HWPE_NVDLA_MACRO_END


PeriphController::PeriphController(PeriphController::Connections&& connections, std::string&& name) noexcept
    : name_(std::forward<std::string>(name)),
      connections_(std::forward<PeriphController::Connections>(connections)),
      id_(id_gen++)
{
    connections_.id = &id_;
}


void PeriphController::submit_operation(const ControlOperation& op, ControlOperationResponse&& response)
{
    *connections_.req  = 1;
    *connections_.add  = op.addr;
    *connections_.wen  = op.wen;
    *connections_.be   = op.be;
    *connections_.data = op.data;
    *connections_.id   = op.id;

    current_id_       = op.id;
    current_response_ = response;
}

bool PeriphController::is_ready()
{
    return !current_response_;
}

void PeriphController::eval() noexcept
{

    if (current_response_ && connections_.r_valid)
    {
        if (*connections_.r_id != current_id_)
        {
            PERIPH_CONTROL_ERR("Mismatched ID and R_ID: id=%08x, r_id=%08x", current_id_, *connections_.r_id);
            PERIPH_CONTROL_ABORT();
        }

        *current_response_.valid = static_cast<bool>(*connections_.r_valid);
        *current_response_.data  = *connections_.data;

        current_response_.valid = nullptr;
        current_response_.data = nullptr;
    }
}
