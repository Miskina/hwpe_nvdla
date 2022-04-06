#include "conv.h"
#include "opendla.h"


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

#define GET_STATUS_REG(_comp_name, _comp_reg_read, group_id)                \
	DLA_MACRO_START                                                         \
	reg = _comp_reg_read(S_STATUS);                                         \
	mask = NVDLA_ ## _comp_name ## _S_STATUS_0_STATUS_0_MASK +              \
           NVDLA_ ## _comp_name ## _S_STATUS_0_STATUS_1_MASK * group_id;    \
	shift = NVDLA_ ## _comp_name ## _S_STATUS_0_STATUS_0_SHIFT +            \
            NVDLA_ ## _comp_name ## _S_STATUS_0_STATUS_1_SHIFT * group_id;  \
    DLA_MACRO_END                                                           \

int32_t dla_setup_conv(dla_conv_op* conv_op, dla_proccesor_group* group)
{
	int reg, mask;
	int group_id = group->id;

	GET_STATUS_REG(CACC, cacc_reg_read, group_id);
	reg = (reg & mask) >> shift;
	// TODO: Izadji ako je zauzet ili nekam enquaj

	GET_STATUS_REG(CMAC_A, cmac_a_reg_read, group_id);
	reg = (reg & mask) >> shift;
	// TODO: Izadji ako je zauzet ili nekam enquaj

	GET_STATUS_REG(CMAC_B, cmac_b_reg_read, group_id);
	reg = (reg & mask) >> shift;
	// TODO: Izadji ako je zauzet ili nekam enquaj
}

#endif // DLA_CONV_MEASURE_STATS
