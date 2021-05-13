#ifndef PERIPH_MASTER_H
#define PERIPH_MASTER_H

#include <systemc.h>

#include <queue>
#include <cstdio>

#include "../common/PeriphOp.h"


// template<unsigned int IdWidth = 32>
class PeriphMaster : sc_module
{

public:

    // using id_t = sc_bv<IdWidth>;
    using id_t = bool;


    sc_in_clk       clk;

    sc_out<bool>    req;
    sc_in<bool>     gnt;
    sc_out<unsigned int>     add;
    sc_out<bool>    wen;
    sc_out<uint8_t> be;
    sc_out<unsigned int>     data;
    sc_out<id_t>    id;

    sc_in<unsigned int>      r_data;
    sc_in<bool>     r_valid;
    sc_in<id_t>     r_id;

    sc_event queue_filled_event;

    void masterful_work()
    {
        req  = 0;
        add  = 0;
        wen  = 0;
        be   = 0;
        data = 0;
        id   = 0;

        wait(queue_filled_event);
        printf("[%llu ns] - %s - Queue has been filled, starting...\n", sc_time_stamp().value(), name());

        while(!op_queue.empty())
        {

            wait();

            const PeriphOp& op = op_queue.front();

            req  = !op.is_noop();
            add  = op.addr;
            wen  = op.wen;
            be   = op.be;
            data = op.data;
            id   = op.id;

        }

        wait();
        sc_stop();

    }

    SC_CTOR(PeriphMaster)
    {
        SC_THREAD(masterful_work);
        sensitive <<  clk.pos();
    }

    void add_op(bool wen, uint32_t addr, uint32_t data, uint8_t be, uint32_t id) noexcept
    {
        op_queue.emplace(wen, addr, data, be, id);
    }

private:

    std::queue<PeriphOp> op_queue{};
    
};


#endif // PERIPH_MASTER_H