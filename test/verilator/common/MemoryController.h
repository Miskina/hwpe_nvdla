#ifndef HWPE_MEMORY_CONTROLLER_H
#define HWPE_MEMORY_CONTROLLER_H

#include <cstdint>

class MemoryController
{
public:

    virtual void read(uint32_t addr, uint8_t* data, uint32_t data_len) = 0;

    virtual void write(uint32_t addr, uint8_t* data, uint32_t data_len) = 0;

    virtual bool is_ready() = 0;

};


#endif // HWPE_MEMORY_CONTROLLER_H