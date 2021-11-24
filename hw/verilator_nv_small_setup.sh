#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
TARGET_ROOT=${1:-${SCRIPT_DIR}}
NVDLA_OUTDIR=${TARGET_ROOT}/nvdla/outdir
NVDLA_CONFIGURATION=nv_small
NVDLA_SRC_DIR=${NVDLA_OUTDIR}/${NVDLA_CONFIGURATION}/vmod

echo "[Verilator] Generating verilator file list for ${NVDLA_CONFIGURATION}"
echo "            Source directory path: ${NVDLA_SRC_DIR}"

NV_SMALL_VERILATOR_CONFIG="
-I${NVDLA_SRC_DIR}/nvdla/bdma
-I${NVDLA_SRC_DIR}/nvdla/cacc
-I${NVDLA_SRC_DIR}/nvdla/car
-I${NVDLA_SRC_DIR}/nvdla/cbuf
-I${NVDLA_SRC_DIR}/nvdla/cdma
-I${NVDLA_SRC_DIR}/nvdla/cdp
-I${NVDLA_SRC_DIR}/nvdla/cmac
-I${NVDLA_SRC_DIR}/nvdla/csc
-I${NVDLA_SRC_DIR}/nvdla/glb
-I${NVDLA_SRC_DIR}/nvdla/nocif
-I${NVDLA_SRC_DIR}/nvdla/pdp
-I${NVDLA_SRC_DIR}/nvdla/retiming
-I${NVDLA_SRC_DIR}/nvdla/rubik
-I${NVDLA_SRC_DIR}/nvdla/sdp
-I${NVDLA_SRC_DIR}/nvdla/top
-I${NVDLA_SRC_DIR}/nvdla/csb_master
-I${NVDLA_SRC_DIR}/nvdla/cfgrom
-I${NVDLA_SRC_DIR}/rams/synth
-I${NVDLA_SRC_DIR}/vlibs
-I${NVDLA_SRC_DIR}/include
-v ${NVDLA_SRC_DIR}/vlibs/RANDFUNC.v
-v ${NVDLA_SRC_DIR}/vlibs/nv_assert_no_x.v
-v ${NVDLA_SRC_DIR}/nvdla/nocif/NV_NVDLA_XXIF_libs.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_8X66_GL_M1_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_248X82_GL_M2_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_32X192_GL_M1_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_32X224_GL_M1_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_256X80_GL_M2_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_80X256_GL_M1_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_160X82_GL_M2_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_80X226_GL_M1_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_256X144_GL_M2_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_32X288_GL_M1_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_32X256_GL_M1_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_20X288_GL_M1_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_248X144_GL_M2_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_80X15_GL_M2_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_60X168_GL_M1_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_20X80_GL_M1_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_64X226_GL_M1_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_64X288_GL_M1_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_32X32_GL_M1_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_128X6_GL_M2_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_128X11_GL_M2_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_64X10_GL_M2_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_16X256_GL_M1_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_80X72_GL_M1_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_256X4_GL_M2_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_256X7_GL_M2_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_256X8_GL_M2_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_64X116_GL_M1_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_32X16_GL_M1_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_256X11_GL_M4_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_160X16_GL_M2_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_80X14_GL_M2_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_80X16_GL_M2_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_160X144_GL_M2_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_80X288_GL_M1_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_80X66_GL_M1_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_160X65_GL_M2_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_60X22_GL_M1_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_80X9_GL_M2_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_64X66_GL_M1_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_128X18_GL_M2_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_16X272_GL_M1_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMDP_16X64_GL_M1_E2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_512X64_GL_M4_D2.v
-v ${NVDLA_SRC_DIR}/rams/model/RAMPDP_256X64_GL_M2_D2.v

-DNO_PLI_OR_EMU
-DNO_PLI
-DDESIGNWARE_NOEXIST
-DSYNTHESIS
-Wno-moddup
-Wno-fatal
"

echo "$NV_SMALL_VERILATOR_CONFIG" > $SCRIPT_DIR/test/verilator/verilator_nv_small.f
