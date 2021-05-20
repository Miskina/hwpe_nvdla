#ifndef TCDM_MEMORY_H
#define TCDM_MEMORY_H

#include<string>
#include<queue>
#include<cstdint>
#include<cstdio>

#include<verilated.h>

#include "Memory.h"
#include "MemoryController.h"

class SingleTcdmController;

struct TcdmConnections
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

template <int N>
class TcdmMemoryController : public MemoryController
{
    friend class SingleTcdmController;
    
public:

    TcdmMemoryController(std::array<TcdmConnections, N>&& connections, std::string&& name) noexcept
    {
        for (char i = '0'; i < N + '0'; ++i)
        {
            tcdm_buses_[i] = SingleTcdmController(name + std::string("[") + i + std::string("]"), std::move(connections[i]));
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

class SingleTcdmController
{

public:

    template<int N>
    SingleTcdmController(std::string&& name, TcdmConnections&& slave)
            : name_(name), slave_(slave) { }

    SingleTcdmController() = default;

    void eval() noexcept;

private:

    std::string name_;
    TcdmConnections slave_;
    std::queue<uint32_t> response_queue_;

    bool is_stall();

};

#endif // TCDM_MEMORY_H