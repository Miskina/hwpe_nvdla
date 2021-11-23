#ifndef CSB_SLAVE_H
#define CSB_SLAVE_H

#include <cstdint>
#include <cstdio>
#include <queue>
#include <random>

#include "Connections.h"

// template<typename Slave>
class CsbSlave
{

public:

    // CsbSlave(Slave* slave, float stall_chance = 0.5) noexcept : slave_(slave), stall_chance_(stall_chance) { }
    
    CsbSlave(const char* name,
             const CSBConnections& slave,
             float stall_chance = 0.5) noexcept
                : name_(name),
                  slave_(slave),
                  stall_chance_(stall_chance) { }

    void eval() noexcept
    {
        // defaults
        *slave_.ready = 0;
        *slave_.r_valid = 0;
        *slave_.r_data = 0;
        *slave_.wr_complete = 0;

        if (is_stall_())
        {
            return;
        }

        *slave_.ready = 1;
        if (is_request_())
        {
            printf("[%s] - Got request\n", name_);
            if (*slave_.write)
            {
                printf("[%s] - Writing\n", name_);
                if (*slave_.nposted)
                {
                    printf("[%s] - Sending write response.\n", name_);
                    write_response_();
                }
            }
            else
            {
                printf("[%s] Read response\n", name_);
                read_response_();
            }
        }
    }

private:

    const char* name_;
    CSBConnections slave_;
    // Slave* slave_;
    float stall_chance_;

    bool is_request_() noexcept
    {
        return *slave_.valid && *slave_.ready;
    }

    bool is_stall_() noexcept
    {
        return rand() < int(stall_chance_ * RAND_MAX);
    }

    void write_response_() noexcept
    {
        *slave_.wr_complete = 1;
    }

    void read_response_() noexcept
    {
        *slave_.valid = 1;
        *slave_.wdat  = 0xDEADBEEF;
    }
};

#endif // CSB_SLAVE_H