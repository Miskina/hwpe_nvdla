#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include <cstdint>

struct CSBConnections
{
    bool* ready;
    bool* r_valid;
    uint32_t* r_data;
    bool* wr_complete;
    bool* valid;
    uint16_t* addr;
    uint32_t* wdat;
    bool* write;
    bool* nposted;
};

struct PeriphConnections
{
    // struct be_t
    // {
    //     int val : 4;

    // } __attribute__((packed));

    uint8_t* req;
    uint32_t* add;
    uint8_t* wen;
    uint8_t* be;
    uint32_t* data;
    uint8_t* id;
    uint8_t* gnt;
    uint32_t* r_data;
    uint8_t* r_valid;
    uint8_t* r_id;
};


#endif