//
// Copyright (c) 2021 Antmicro
//
//  This file is licensed under the MIT License.
//  Full license text is available in 'LICENSE' file.
//
#include <verilated.h>
#include "VNV_nvdla.h"
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#if VM_TRACE
# include <verilated_vcd_c.h>
#endif

#include "src/renode.h"
#include "src/buses/dbbif-slave.h"
#include "src/buses/csb.h"

RenodeAgent *nvdla;
VNV_nvdla *top = new VNV_nvdla;
VerilatedVcdC *tfp;
vluint64_t main_time = 0;

void eval() {
#if VM_TRACE
    main_time++;
    tfp->dump(main_time);
    tfp->flush();
#endif
    top->eval();
}

RenodeAgent *Init() {
    top->direct_reset_ = 1;

    Dbbif *dbbif = new DbbifSlave(64, 64);

    dbbif->aclk = &top->dla_core_clk;
    dbbif->aresetn = &top->dla_reset_rstn;

    dbbif->awvalid = &top->nvdla_core2dbb_aw_awvalid;
    dbbif->awready = &top->nvdla_core2dbb_aw_awready;
    dbbif->awaddr = (uint64_t * ) & top->nvdla_core2dbb_aw_awaddr;
    dbbif->awlen = &top->nvdla_core2dbb_aw_awlen;
    dbbif->awid = &top->nvdla_core2dbb_aw_awid;

    dbbif->wvalid = &top->nvdla_core2dbb_w_wvalid;
    dbbif->wready = &top->nvdla_core2dbb_w_wready;
    dbbif->wdata = &top->nvdla_core2dbb_w_wdata;
    dbbif->wlast = &top->nvdla_core2dbb_w_wlast;
    dbbif->wstrb = &top->nvdla_core2dbb_w_wstrb;

    dbbif->bvalid = &top->nvdla_core2dbb_b_bvalid;
    dbbif->bready = &top->nvdla_core2dbb_b_bready;
    dbbif->bid = &top->nvdla_core2dbb_b_bid;

    dbbif->arvalid = &top->nvdla_core2dbb_ar_arvalid;
    dbbif->arready = &top->nvdla_core2dbb_ar_arready;
    dbbif->araddr = (uint64_t * ) & top->nvdla_core2dbb_ar_araddr;
    dbbif->arlen = &top->nvdla_core2dbb_ar_arlen;
    dbbif->arid = &top->nvdla_core2dbb_ar_arid;

    dbbif->rvalid = &top->nvdla_core2dbb_r_rvalid;
    dbbif->rready = &top->nvdla_core2dbb_r_rready;
    dbbif->rlast = &top->nvdla_core2dbb_r_rlast;
    dbbif->rdata = &top->nvdla_core2dbb_r_rdata;
    dbbif->rid = &top->nvdla_core2dbb_r_rid;

    dbbif->evaluateModel = &eval;

    CSB *csb = new CSB();

    csb->csb_clock = &top->dla_core_clk;

    csb->csb_valid = &top->csb2nvdla_valid;
    csb->csb_ready = &top->csb2nvdla_ready;
    csb->csb_addr = &top->csb2nvdla_addr;
    csb->csb_wdat = &top->csb2nvdla_wdat;
    csb->csb_write = &top->csb2nvdla_write;
    csb->csb_nposted = &top->csb2nvdla_nposted;

    csb->csb_r_valid = &top->nvdla2csb_valid;
    csb->csb_r_data = &top->nvdla2csb_data;

    csb->csb_wr_complete = &top->nvdla2csb_wr_complete;

    csb->evaluateModel = &eval;

    nvdla = new RenodeAgent(dbbif);
    nvdla->addBus(csb);

#if VM_TRACE
    Verilated::traceEverOn(true);
    tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("simx.vcd");
#endif

    return nvdla;
}

int main(int argc, char **argv, char **env) {
    if(argc < 3) {
        printf("Usage: %s {receiverPort} {senderPort} [{address}]\n", argv[0]);
        exit(-1);
    }
    const char *address = argc < 4 ? "127.0.0.1" : argv[3];

    Verilated::commandArgs(argc, argv);

    Init();
    nvdla->reset();
    nvdla->simulate(atoi(argv[1]), atoi(argv[2]), address);
    top->final();
    exit(0);
}
