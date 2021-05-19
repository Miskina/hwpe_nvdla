#ifndef TCDM_MEMORY_H
#define TCDM_MEMORY_H

#include<string>
#include<queue>
#include<cstdint>
#include<cstdio>

#include<verilated.h>

#include "Memory.h"
#include "MemoryController.h"


struct TcdmBusConnection {
    bool*     req;
    bool*     gnt;
    uint32_t* add;
    bool*     wen;
    uint8_t*  be;
    uint32_t* data;
    uint32_t* r_data;
    bool*     r_valid;
};

class SingleTcdmController : public MemoryController
{

public:

    constexpr SingleTcdmController(std::string&& name, TcdmBusConnection& slave)
            : name_(name), slave_(slave) { }

    constexpr SingleTcdmController() = default;

    void eval() noexcept;

private:

    std::string name_;
    TcdmBusConnection slave_;
    std::queue<uint32_t> response_queue_;

    bool is_stall();

};

template <int N>
class TcdmMemoryController
{
    friend class SingleTcdmController;
    
public:
    TcdmMemoryController(std::array<TcdmBusConnection, N>&& tcdmBusConnections)
    {
        for (int i = 0; i < N; ++i)
        {
            std::string name = "TCDM[" + i + "]";
            tcdm_buses_[i] = SingleTcdmController(std::move(name), tcdmBusConnections[i]);
        }
    }

	virtual bool is_ready() override
    {
        return true;
    }

    virtual void eval() override
    {
        for (int i = 0; i < N; ++i)
        {
            tcdm_buses_[i].eval();
        }
    }

private:
    std::array<SingleTcdmController, N> tcdm_buses_;

};

#endif // TCDM_MEMORY_H