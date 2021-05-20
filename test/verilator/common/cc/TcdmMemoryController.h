#ifndef TCDM_MEMORY_H
#define TCDM_MEMORY_H

#include<string>
#include<queue>
#include<cstdint>
#include<cstdio>

#include<verilated.h>

#include "Memory.h"
#include "MemoryController.h"


class SingleTcdmController : public MemoryController
{

public:

    constexpr SingleTcdmController(std::string&& name, TcdmMemoryController::Connections&& slave)
            : name_(name), slave_(slave) { }

    constexpr SingleTcdmController() = default;

    void eval() noexcept;

private:

    std::string name_;
    TcdmMemoryController::Connections slave_;
    std::queue<uint32_t> response_queue_;

    bool is_stall();

};

template <int N>
class TcdmMemoryController
{
    friend class SingleTcdmController;
    
public:

    struct Connections
    {
        bool*     req;
        bool*     gnt;
        uint32_t* add;
        bool*     wen;
        uint8_t*  be;
        uint32_t* data;
        uint32_t* r_data;
        bool*     r_valid;
    };

    constexpr TcdmMemoryController(std::array<Connections, N>&& connections, std::string&& name) noexcept
    {
        for (int i = 0; i < N; ++i)
        {
            tcdm_buses_[i] = SingleTcdmController(name + "[" + i + "]", std::move(connections[i]));
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