#include "TcdmMemoryController.h"

bool SingleTcdmController::is_stall()
{
    //TODO Add random stall
    return false;
}
 
void SingleTcdmController::eval() noexcept
{
    if (is_stall())
    {
        printf("(%lu) %s: stalling\n", Verilated::time(), name_.c_str());
        *slave_.gnt = 0;
    }
    else
    {
        *slave_.gnt = 1;
    }

    if (!response_queue_.empty())
    {
        *slave_.r_valid = true;
        *slave_.r_data = response_queue_.front();
        response_queue_.pop();
        printf("(%lu) %s: response, data %08x\n", Verilated::time(), name_.c_str(), *slave_.r_data);
    }
    else
    {
        *slave_.r_valid = false;
    }

    if (*slave_.req && *slave_.gnt)
    {
        if (!*slave_.wen)
        {
            printf("(%lu) %s: write request, addr %08x, data %08x\n", Verilated::time(), name_.c_str(), *slave_.add, *slave_.data);
            ctrl_->write(*slave_.add, reinterpret_cast<uint8_t*>(slave_.data), sizeof(*slave_.data));
            response_queue_.emplace(0);
        }
        else
        {
            printf("(%lu) %s: read request, addr %08x\n", Verilated::time(), name_.c_str(), *slave_.add);
            uint32_t r_data = 0;
            ctrl_->read(*slave_.add, reinterpret_cast<uint8_t*>(&r_data), sizeof(r_data));
            response_queue_.emplace(r_data);
        }
    }
}