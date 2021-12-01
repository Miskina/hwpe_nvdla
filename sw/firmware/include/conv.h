#ifndef __DLA_CONV_H__
#define __DLA_CONV_H__

#include "common.h"

#define DLA_CONV_MEASURE_STATS DLA_CONV_STAT_ENABLE || DLA_STAT_ENABLE

#if DLA_CONV_MEASURE_STATS

struct dla_conv_stats
{
    uint32_t data_read_stall;
    uint32_t weight_read_stall;
    uint32_t data_read_latency;
    uint32_t weight_read_latency;
    uint32_t nan_data_num;
    uint32_t nan_weight_num;
    uint32_t inf_data_num;
    uint32_t inf_weight_num;
    uint32_t saturation_count;
};

void dla_get_conv_stats(struct dla_conv_stats* conv_stats);

#endif // DLA_CONV_MEASURE_STAT

struct dla_conv_vec
{
    uint32_t x;
    uint32_t y;
};

struct dla_conv_addr
{
    uint64_t weight;
    uint64_t wgs;
    uint64_t input;
    uint64_t input;
    uint64_t output;
};

struct dla_conv_cfg
{
    struct
    {
        union
        {
            uint16_t atom_size;
            int weight_compress_support: 1;
        };
        uint32_t val;
    };
};

struct dla_conv_op_desc
{
    uint32_t reg;
    uint32_t hight;
    uint32_t low;
    uint32_t shift;
    uint32_t mask;

    struct dla_conv_vec stride;
    struct dla_conv_vec pad;

    struct dla_conv_addr address;

    struct dla_conv_cfg config;

};

int32_t init_dla_conv_op_desc(struct dla_conv_op_desc* conv_op);

#define DLA_CONV_OP_DESC(var_name)    \
    struct dla_conv_op_desc var_name; \
    init_dla_conv_op_desc(var_name)   \


#endif // __DLA_CONV_H__
