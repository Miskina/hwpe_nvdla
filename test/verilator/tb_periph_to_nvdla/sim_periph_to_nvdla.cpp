
#include <stdlib.h>
#include <stdio.h>

#include <verilated.h>

#include "FabricController.h"
#include "AxiMemoryController.h"
#include "PeriphControl.h"



#if VM_TRACE
#include <verilated_vcd_c.h>
VerilatedVcdC* tfp;

void _close_trace()
{
	if (tfp) tfp->close();
}

#endif


int main(int argc, const char **argv, char **env) {

	Vtb_periph_to_nvdla *dla = new Vtb_periph_to_nvdla{"NVDLA"};
	
	AxiMemoryController::connections dbbconn
	{
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

	PeriphConnections periph_connections
	{
		.req  = &dla->periph_req_i,
		.add  = &dla->periph_add_i,
		.wen  = &dla->periph_wen_i,
		.be   = &dla->periph_be_i,
		.data = &dla->periph_data_i,

		.gnt  	 = &dla->periph_gnt_o,
		.r_data  = &dla->periph_r_data_o,
		.r_valid = &dla->periph_r_valid_o,
		.r_id	 = &dla->periph_r_id_o,
	};


	AxiMemoryController* axi_dbb = new AxiMemoryController(dbbconn, "DBB");
	PeriphControl* periph = new PeriphControl(periph_connections, "PeriphCTRL");

	FabricController* fabric_ctrl = new FabricController("FabricCTRL");
	fabric_ctrl->attach(periph);
	fabric_ctrl->attach(axi_dbb);

	util::Release _{dla, axi_dbb, periph, fabric_ctrl};


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
	
	fabric_ctrl->load_trace(argv[1]);

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
	while (fabric_ctrl->eval() && quiesc_timer)
	{

		fabric_ctrl->eval();

		periph_ctrl->eval();
		
		axi_dbb->eval();

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

	// TODO: Handle the test passed in fabric contrller??
	if (!trace->test_passed()) {
		printf("*** FAIL: test failed due to output mismatch\n");
		return 1;
	}
	
	if (!csb->test_passed()) {
		printf("*** FAIL: test failed due to CSB read mismatch\n");
		return 2;
	}
	
	printf("*** PASS\n");

	return 0;
}
