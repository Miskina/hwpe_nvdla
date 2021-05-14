
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include <queue>

#include <verilated.h>

#include "AXIResponder.h"
#include "TraceLoader.h"
#include "PeriphMaster.h"
#include "Vperiph_to_nvdla.h"

#if VM_TRACE
#include <verilated_vcd_c.h>
VerilatedVcdC* tfp;

void _close_trace()
{
	if (tfp) tfp->close();
}

#endif


int main(int argc, const char **argv, char **env) {

	Vperiph_to_nvdla *dla = new Vperiph_to_nvdla{"TOP"};
    PeriphMaster* periph = new PeriphMaster{"PeriphMaster"};
	
	AXIResponder::connections dbbconn = {
		.aw_awvalid = &dla->dbb_aw_awvalid,
		.aw_awready = &dla->dbb_aw_awready,
		.aw_awid    = &dla->dbb_aw_awid,
		.aw_awlen   = &dla->dbb_aw_awlen,
		.aw_awaddr  = &dla->dbb_aw_awaddr,
		
		.w_wvalid = &dla->dbb_w_wvalid,
		.w_wready = &dla->dbb_w_wready,
		.w_wdata  = &dla->dbb_w_wdata,
		.w_wstrb  = &dla->dbb_w_wstrb,
		.w_wlast  = &dla->dbb_w_wlast,
		
		.b_bvalid = &dla->dbb_b_bvalid,
		.b_bready = &dla->dbb_b_bready,
		.b_bid    = &dla->dbb_b_bid,

		.ar_arvalid = &dla->dbb_ar_arvalid,
		.ar_arready = &dla->dbb_ar_arready,
		.ar_arid    = &dla->dbb_ar_arid,
		.ar_arlen   = &dla->dbb_ar_arlen,
		.ar_araddr  = &dla->dbb_ar_araddr,
	
		.r_rvalid = &dla->dbb_r_rvalid,
		.r_rready = &dla->dbb_r_rready,
		.r_rid    = &dla->dbb_r_rid,
		.r_rlast  = &dla->dbb_r_rlast,
		.r_rdata  = &dla->dbb_r_rdata,
	};
	AXIResponder* axi_dbb = new AXIResponder(dbbconn, "DBB");

	TraceLoader* trace = new TraceLoader(csb, axi_dbb, axi_cvsram);

#if VM_TRACE
	Verilated::traceEverOn(true);
	tfp = new VerilatedVcdC;
	dla->trace(tfp, 99);
	tfp->open("trace.vcd");
	atexit(_close_trace);
#endif
	
	Verilated::commandArgs(argc, argv);
	if (argc != 2)
    {
		fprintf(stderr, "nvdla requires exactly one parameter (a trace file)\n");
		return 1;
	}
	
	trace->load(argv[1]);

	printf("reset...\n");
	dla->rst = 0;
	dla->eval();
	for (int i = 0; i < 20; i++)
    {
		dla->clk = 1;
		dla->eval();
        Verilated::timeInc(1);
#if VM_TRACE
		tfp->dump(Verilated::time());
#endif
		
		dla->clk = 0;
		dla->eval();
        Verilated::timeInc(1);
#if VM_TRACE
		tfp->dump(Verilated::time());
#endif
	}

	dla->rst = 1;
	dla->eval();
	
	for (int i = 0; i < 20; i++) {
		dla->clk = 1;
		dla->eval();
		Verilated::timeInc(1);
#if VM_TRACE
		tfp->dump(Verilated::time());
#endif
		
		dla->clk = 0;
		dla->eval();
		Verilated::timeInc(1);
#if VM_TRACE
		tfp->dump(Verilated::time());
#endif
	}
	
	dla->rst = 0;
	
	
	printf("letting buffers clear after reset...\n");
	for (int i = 0; i < 8192; i++) {
		dla->clk = 1;
		
		dla->eval();
		Verilated::timeInc(1);
#if VM_TRACE
		tfp->dump(Verilated::time());
#endif
		
		dla->clk = 0;
		
		dla->eval();
		Verilated::timeInc(1);
#if VM_TRACE
		tfp->dump(Verilated::time());
#endif
	}

	printf("running trace...\n");
	uint32_t quiesc_timer = 200;
	int waiting = 0;
	while (!csb->done() || (quiesc_timer--)) {
		int extevent;
		
		extevent = csb->eval(waiting);
		
		if (extevent == TraceLoader::TRACE_AXIEVENT)
			trace->axievent();
		else if (extevent == TraceLoader::TRACE_WFI) {
			waiting = 1;
			printf("(%lu) waiting for interrupt...\n", Verilated::time());
		} else if (extevent & TraceLoader::TRACE_SYNCPT_MASK) {
			trace->syncpt(extevent);
		}
		
		if (waiting && dla->dla_intr) {
			printf("(%lu) interrupt!\n", Verilated::time());
			waiting = 0;
		}
		
		axi_dbb->eval();
		if (axi_cvsram)
			axi_cvsram->eval();

		dla->clk = 1;
		
		dla->eval();
		Verilated::timeInc(1);
#if VM_TRACE
		tfp->dump(Verilated::time());
#endif
		
		dla->clk = 0;
		
		dla->eval();
		Verilated::timeInc(1);
#if VM_TRACE
		tfp->dump(Verilated::time());
#endif
	}
	
	printf("done at %lu Verilated::time()\n", Verilated::time());

	if (!trace->test_passed()) {
		printf("*** FAIL: test failed due to output mismatch\n");
		return 1;
	}
	
	if (!csb->test_passed()) {
		printf("*** FAIL: test failed due to CSB read mismatch\n");
		return 2;
	}
	
	printf("*** PASS\n");
	
    delete periph;
	delete trace;
	delete dla;
	delete axi_dbb;

	return 0;
}
