#ifndef __DLA_CONV_H__
#define __DLA_CONV_H__

#include "common.h"

// TODO: Add Doxygen documentation
#define DLA_CONV_MEASURE_STATS DLA_CONV_STAT_ENABLE || DLA_STAT_ENABLE

#if DLA_CONV_MEASURE_STATS

// TODO: Add Doxygen documentation
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

// TODO: Add Doxygen documentation
void dla_get_conv_stats(struct dla_conv_stats* conv_stats);

#endif // DLA_CONV_MEASURE_STATS

// TODO: Add Doxygen documentation
struct dla_conv_addr
{
    uint64_t weight;
    uint64_t wgs;
    uint64_t input;
    uint64_t input;
    uint64_t output;
};

// TODO: Add Doxygen documentation
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

// TODO: Add Doxygen documentation
struct dla_conv_op_desc
{
    uint32_t reg;
    uint32_t high;
    uint32_t low;
    uint32_t shift;
    uint32_t mask;

    struct dla_vec_2d stride;
    struct dla_vec_2d pad;

    struct dla_conv_addr address;

    struct dla_conv_cfg config;

};

/**
 * \brief Initialize a \ref dla_conv_addr structure instance.
 *
 * Macro which sets the values of a \ref dla_conv_addr structure instance
 * to 0.
 * */
#define INIT_DLA_CONV_ADDR(conv_op_addr) \
    DLA_MACRO_START                      \
    conv_op_desc.address.weight = 0;     \
    conv_op_desc.address.wmb    = 0;     \
    conv_op_desc.address.wgs    = 0;     \
    conv_op_desc.address.input  = 0;     \
    conv_op_desc.address.output = 0;     \
    DLA_MACRO_END                        \

/**
 * \brief Initialize a \ref dla_conv_op_desc structure instance.
 *
 * Macro which initializes a \ref dla_conv_op_desc structure instance.
 * It sets its address values to 0 and its \ref dla_conv_cfg values to 0.
 * Calls the \ref INIT_DLA_CONV_ADDR macro to initialize the address field.
 *
 * */
#define INIT_DLA_CONV_OP_DESC(conv_op_desc)  \
    DLA_MACRO_START                          \
    INIT_DLA_CONV_ADDR(conv_op_desc.addres); \
    conv_op_desc.config.val = 0;             \
    DLA_MACRO_END                            \





#endif // __DLA_CONV_H__
