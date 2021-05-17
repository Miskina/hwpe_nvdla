
#include <stdlib.h>
#include <stdio.h>

#include <verilated.h>
#include "Vtb_periph_to_nvdla.h"

#include "FabricController.h"
#include "AxiMemoryController.h"
#include "PeriphController.h"



#if VM_TRACE
#include <verilated_vcd_c.h>
VerilatedVcdC* tfp;

void _close_trace()
{
	if (tfp) tfp->close();
}

#endif


int main(int argc, const char **argv, char **env) {

	Vtb_periph_to_nvdla* dla = new Vtb_periph_to_nvdla{"NVDLA"};
	
	AxiMemoryController::Connections dbbconn
	{
		.aw_awvalid = &dla->dbb_aw_valid,
		.aw_awready = &dla->dbb_aw_ready,
		.aw_awid    = &dla->dbb_aw_id,
		.aw_awlen   = &dla->dbb_aw_len,
		.aw_awaddr  = &dla->dbb_aw_addr,
		
		.w_wvalid = &dla->dbb_w_valid,
		.w_wready = &dla->dbb_w_ready,
		.w_wdata  = &dla->dbb_w_data,
		.w_wstrb  = &dla->dbb_w_strb,
		.w_wlast  = &dla->dbb_w_last,
		
		.b_bvalid = &dla->dbb_b_valid,
		.b_bready = &dla->dbb_b_ready,
		.b_bid    = &dla->dbb_b_id,

		.ar_arvalid = &dla->dbb_ar_valid,
		.ar_arready = &dla->dbb_ar_ready,
		.ar_arid    = &dla->dbb_ar_id,
		.ar_arlen   = &dla->dbb_ar_len,
		.ar_araddr  = &dla->dbb_ar_addr,
	
		.r_rvalid = &dla->dbb_r_valid,
		.r_rready = &dla->dbb_r_ready,
		.r_rid    = &dla->dbb_r_id,
		.r_rlast  = &dla->dbb_r_last,
		.r_rdata  = &dla->dbb_r_data,
	};

	PeriphController::Connections periph_connections
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


	AxiMemoryController* axi_dbb = new AxiMemoryController{std::move(dbbconn), "DBB"};
	PeriphController* periph = new PeriphController{std::move(periph_connections), "PeriphCTRL"};

	FabricController* fabric_ctrl = new FabricController{"FabricCTRL"};
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
		fprintf(stderr, "NVDLA requires exactly one parameter (a trace file)\n");
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

	printf("Running trace...\n");
	uint32_t quiesc_timer = 200;
	while (fabric_ctrl->eval() && quiesc_timer)
	{

		fabric_ctrl->eval();

		periph->eval();
		
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
	
	printf("Done at %lu Verilated::time()\n", Verilated::time());

	if (!fabric_ctrl->test_passed())
	{
		fprintf(stderr, "*** FAIL: test failed due to CRC mismatch");
		return 1;
	}

	printf("*** PASS\n");

	return 0;
}
