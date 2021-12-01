#include "conv.h"


#if DLA_CONV_MEASURE_STATS

void dla_get_conv_stats(struct dla_conv_stats* conv_stats)
{
    conv_stats->data_read_stall = cdma_reg_read(D_PERF_DAT_READ_STALL);
	conv_stats->weight_read_stall = cdma_reg_read(D_PERF_WT_READ_STALL);
	conv_stats->data_read_latency = cdma_reg_read(D_PERF_DAT_READ_LATENCY);
	conv_stats->weight_read_latency = cdma_reg_read(D_PERF_WT_READ_LATENCY);
	conv_stats->nan_data_num = cdma_reg_read(D_NAN_INPUT_DATA_NUM);
	conv_stats->nan_weight_num = cdma_reg_read(D_NAN_INPUT_WEIGHT_NUM);
	conv_stats->inf_data_num = cdma_reg_read(D_INF_INPUT_DATA_NUM);
	conv_stats->inf_weight_num = cdma_reg_read(D_INF_INPUT_WEIGHT_NUM);
	conv_stats->saturation_count = cacc_reg_read(D_OUT_SATURATION);
}

#endif // DLA_CONV_MEASURE_STATS

int32_t init_dla_conv_op_desc(struct dla_conv_op_desc* conv_op)
{
    conv_op->address.weight = 0;
    conv_op->address.wmb = 0;
    conv_op->address.wgs = 0;
    conv_op->address.input = 0;
    conv_op->address.output = 0;
    conv_op->config.val = 0;
}
