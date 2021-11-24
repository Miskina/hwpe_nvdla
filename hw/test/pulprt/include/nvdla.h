#ifndef NVDLA_H
#define NVDLA_H

#include <opendla.h>
#include <hal/nvdla/nvdla.h>
#include <stdint.h>

uint32_t check_crc(uint8_t* data, uint32_t size, uint32_t expected_crc)
{
    int i;
    int j;
    uint32_t crc;

    crc = ~0;
    
    for (i = 0; i < size; ++i)
    {
        crc ^= data[i];
        for (j = 0; j < 8; j++)
        {
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
        }
    }
    
    return ~crc == expected_crc;
}

#endif // NVDLA_H
