#ifndef CSB_SLAVE_H
#define CSB_SLAVE_H


#include <cstdio>

#include <systemc.h>

#include "Stallable.h"

// using logic = bool;
// using logic_15_0 = int16_t;
// using logic_31_0 = int;
// using logic_63_0 = int64_t;

// template<typename Logic>
// struct Input : public sc_in<Logic> { };

// template<typename Logic>
// struct Output : public sc_out<Logic> { };

struct CsbSlave : sc_module, public Stallable
{
    sc_in_clk clk;
    // Request channel 
    sc_in<bool>    valid;
    sc_out<bool>   ready;
    sc_in<short> addr;
    sc_in<unsigned int>     wdat;
    sc_in<bool>    write;
    sc_in<bool>    nposted;

    // Read data channel
    sc_out<bool>   r_valid;
    sc_out<unsigned int>    r_data;

    // Write response channel
    sc_out<bool>   wr_complete;

    void slave_work();
    
    SC_CTOR(CsbSlave)
    {
        SC_THREAD(slave_work);
        sensitive << clk.pos();
    }

private:
    bool request() const;

    bool write_request() const;
};

#endif // CSB_SLAVE_H