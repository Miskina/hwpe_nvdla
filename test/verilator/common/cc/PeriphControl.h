#ifndef HWPE_NVDLA_PERIPH_CONTROL_H
#define HWPE_NVDLA_PERIPH_CONTROL_H

#include <string>

#include "ControlInterface.h"
#include "Connections.h"
#include "Util.h"

class PeriphControl : public ControlInterface
{

public:

    PeriphControl(PeriphConnections&& connections, std::string&& name) noexcept;

    virtual void submit_operation(ControlOperation&& op, ControlOperationResponse&& response) override;

    virtual bool is_ready() override
    {
        return current_response_;
    }

    void eval() noexcept;

private:

    std::string name_{};
    PeriphConnections connections_{};
    ControlOperationResponse current_response_{};
    uint32_t current_id_{};
};


#endif // HWPE_NVDLA_PERIPH_CONTROL_H