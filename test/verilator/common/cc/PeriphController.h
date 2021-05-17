#ifndef HWPE_NVDLA_PERIPH_CONTROLLER_H
#define HWPE_NVDLA_PERIPH_CONTROLLER_H

#include <string>

#include "ControlInterface.h"
#include "Connections.h"
#include "Util.h"

class PeriphController : public ControlInterface
{

public:

    struct Connections
    {
        uint8_t* req;
        uint32_t* add;
        uint8_t* wen;
        uint8_t* be;
        uint32_t* data;
        uint8_t* id;
        uint8_t* gnt;
        uint32_t* r_data;
        uint8_t* r_valid;
        uint8_t* r_id;
    };

    PeriphController(Connections&& connections, std::string&& name) noexcept;

    virtual void submit_operation(const ControlOperation& op, ControlOperationResponse&& response) override;

    virtual bool is_ready() override;

    void eval() noexcept;

private:

    std::string name_{};
    Connections connections_{};
    ControlOperationResponse current_response_{};
    uint32_t current_id_{};
};


#endif // HWPE_NVDLA_PERIPH_CONTROLLER_H