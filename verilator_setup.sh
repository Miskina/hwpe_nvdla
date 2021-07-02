#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

NV_SMALL_VERILATOR_CONFIG="
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/bdma
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/cacc
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/car
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/cbuf
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/cdma
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/cdp
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/cmac
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/csc
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/glb
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/nocif
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/pdp
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/retiming
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/rubik
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/sdp
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/top
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/csb_master
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/cfgrom
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/synth
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/vlibs
-I$SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/include
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/vlibs/RANDFUNC.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/vlibs/nv_assert_no_x.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/nvdla/nocif/NV_NVDLA_XXIF_libs.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_8X66_GL_M1_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_248X82_GL_M2_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_32X192_GL_M1_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_32X224_GL_M1_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_256X80_GL_M2_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_80X256_GL_M1_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_160X82_GL_M2_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_80X226_GL_M1_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_256X144_GL_M2_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_32X288_GL_M1_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_32X256_GL_M1_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_20X288_GL_M1_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_248X144_GL_M2_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_80X15_GL_M2_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_60X168_GL_M1_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_20X80_GL_M1_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_64X226_GL_M1_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_64X288_GL_M1_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_32X32_GL_M1_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_128X6_GL_M2_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_128X11_GL_M2_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_64X10_GL_M2_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_16X256_GL_M1_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_80X72_GL_M1_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_256X4_GL_M2_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_256X7_GL_M2_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_256X8_GL_M2_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_64X116_GL_M1_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_32X16_GL_M1_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_256X11_GL_M4_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_160X16_GL_M2_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_80X14_GL_M2_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_80X16_GL_M2_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_160X144_GL_M2_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_80X288_GL_M1_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_80X66_GL_M1_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_160X65_GL_M2_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_60X22_GL_M1_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_80X9_GL_M2_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_64X66_GL_M1_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_128X18_GL_M2_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_16X272_GL_M1_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMDP_16X64_GL_M1_E2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_512X64_GL_M4_D2.v
-v $SCRIPT_DIR/nvdla_hw/outdir/nv_small/vmod/rams/model/RAMPDP_256X64_GL_M2_D2.v

-DNO_PLI_OR_EMU
-DNO_PLI
-DDESIGNWARE_NOEXIST
-DSYNTHESIS
-Wno-moddup
-Wno-fatal
"


echo "$NV_SMALL_VERILATOR_CONFIG" > $SCRIPT_DIR/test/verilator/verilator_nv_small.f
bender script verilator > $SCRIPT_DIR/test/verilator/verilator.f