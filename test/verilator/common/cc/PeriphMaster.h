#ifndef HWPE_NVDLA_PERIPH_MASTER_H
#define HWPE_NVDLA_PERIPH_MASTER_H

#include <cstdint>
#include <cstdio>
#include <queue>

#include "Connections.h" 
#include <verilated.h>


#include "../ControlOperation.h"

struct PeriphVerifier
{

    template<typename Slave>
    bool operator()(const ControlOperation& op, Slave* slave) noexcept
    {
        if(slave->r_valid && last_op_.id != slave->periph_r_id)
        {
            error("ID mismatch", Verilated::time());
            return false;
        }

        if(request_granted_ && !slave->periph_r_valid_o)
        {
            error("R_VALID not asserted after request granted", Verilated::time());
            return false;
        }
        else if(!request_granted_ && slave->periph_r_valid_o)
        {
            error("R_VALID after no request !?", Verilated::time());
            return false;
        }
        else if(request_granted_ && slave->periph_r_valid_o)
        {
            if (request_time_ - Verilated::time() > 2) // 2 because each timestep changes clock for a halfperiod
            {
                error("R_VALID should come immediate clock after request has been granted", Verilated::time());
                return false;
            }
            request_granted_ = false;
            request_time_ = 0;
        }

        if(slave->periph_req_i && slave->periph_gnt_o && request_granted_)
        {
            error("Another request shouldn't be accepted before servicing the last one", Verilated::time());
            return false;
        }

        if(slave->periph_req_i && slave->periph_gnt_o)
        {
            request_granted_ = true;
            request_time_ = Verilated::time();
            last_op_ = op;
        }

        return true;
    }

private:

    void error(const char* msg, uint64_t time) const noexcept
    {
        printf("[PeriphVeriph (%lu)] - [Error]: %s", time, msg);
    }

    ControlOperation last_op_{};
    bool request_granted_;
    uint64_t request_time_;
};

// template<typename Slave>
class PeriphMaster
{
    
public:

    // PeriphMaster(Slave* slave) noexcept : slave_(slave) { }

    PeriphMaster(const char* name,
                 const PeriphConnections& slave) noexcept
                    : slave_(slave),
                      name_(name) { }
    
    void eval(bool is_noop = false) noexcept
    {
        is_noop = is_noop || op_queue_.empty();
        static ControlOperation noop{};
        const ControlOperation& op = is_noop ? noop : op_queue_.front();

        *slave_.req  = is_noop ? 0 : 1;
        *slave_.add  = op.addr;
        *slave_.wen  = op.wen;
        *slave_.be   = op.be;
        *slave_.data = op.data;
        *slave_.id   = op.id;

        if(!op_queue_.empty() && *slave_.gnt)
        {
            op_queue_.pop();
        }
    }

    void add_op(bool wen, uint32_t addr, uint32_t data, uint8_t be, uint32_t id) noexcept
    {
        op_queue_.emplace(wen, addr, data, be, id);
    }

private:

    const char* name_;
    std::queue<ControlOperation> op_queue_;
    PeriphVerifier verify_;
    PeriphConnections slave_;

};


#endif // HWPE_NVDLA_PERIPH_MASTER_H