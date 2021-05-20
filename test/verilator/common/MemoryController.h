#ifndef HWPE_MEMORY_CONTROLLER_H
#define HWPE_MEMORY_CONTROLLER_H

#include <cstdint>

#include "Memory.h"

class MemoryController
{
public:

    void read(uint32_t addr, uint8_t* data, uint32_t data_len)
    {
        for (int i = 0; i < data_len; ++i)
        {
            data[i] = ram->read<uint8_t>(addr + i);
        }
    }

    void write(uint32_t addr, uint8_t* data, uint32_t data_len)
    {
        for (int i = 0; i < data_len; ++i)
        {
            ram->write(addr + i, data[i]);
        }
    }

	void attach(Memory* memory) noexcept
    {
        ram = memory;
    }

    virtual bool is_ready() = 0;

    virtual void eval() = 0;

protected:
	Memory* ram;

};


#endif // HWPE_MEMORY_CONTROLLER_H