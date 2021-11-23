#ifndef STALL_GEN_H
#define STALL_GEN_H


#include <cstdio>
#include <random>

#include <systemc.h>

#include "Stallable.h"

#include "../common/RandomUtil.h"


template<int UniformDistLow = 0, int UniformDistHigh = 4>
struct StallGen : sc_module
{

    sc_in_clk clock;
    Stallable* stallable = nullptr;

    void stall()
    {
        while(true)
        {
            if (should_stall()) 
            {
                int clocks_to_stall = random_num();
                stallable->stall = true;
                for(int i = 0; i < clocks_to_stall; ++i)
                {
                    wait();
                }
                stallable->stall_finish_e.notify();
            }
        }
    }

    SC_CTOR(StallGen)
    {
        SC_THREAD(stall);
        sensitive << clock.pos();
    }

private:

    bool should_stall() noexcept
    {
        return rnd::get_random(should_stall_dist);
    }
    
    int random_num()
    {
        return rnd::get_random(stall_time_dist);
    }

    std::uniform_int_distribution<int> stall_time_dist{UniformDistLow, UniformDistHigh};
    std::uniform_int_distribution<int> should_stall_dist{0, 1};

};


#endif // STALL_GEN_H