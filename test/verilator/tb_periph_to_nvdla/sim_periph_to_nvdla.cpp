
#include <stdlib.h>
#include <stdio.h>

#include <verilated.h>
#include "Vtb_periph_to_nvdla.h"

#include "FabricController.h"
#include "AxiMemoryController.h"
#include "PeriphController.h"


#if VM_TRACE
#include <verilated_vcd_c.h>
VerilatedVcdC* tfp = nullptr;

void _close_trace()
{
	if (tfp) tfp->close();
}

#endif



template<typename ClockedVerilatedModel>
void tick(ClockedVerilatedModel* model, VerilatedVcdC* vcd, int n = 1)
{
	for (int i = 0; i < n; ++i)
	{
		model->clk = 1; 
		model->eval();
		Verilated::timeInc(1);
	#if VM_TRACE
		vcd->dump(Verilated::time());
	#endif // VM_TRACE

		model->clk = 0;
		model->eval();
		Verilated::timeInc(1);
	#if VM_TRACE
		vcd->dump(Verilated::time());
	#endif // VM_TRACE
	}
}


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

	Memory<>* ram = new Memory<>{"DBB_RAM"};
	AxiMemoryController* axi_dbb = new AxiMemoryController{std::move(dbbconn), "DBB"};
	axi_dbb->attach(ram);

	PeriphController* periph = new PeriphController{std::move(periph_connections), "PeriphCTRL"};

	FabricController* fabric_ctrl = new FabricController{"FabricCTRL"};
	fabric_ctrl->attach(periph);
	fabric_ctrl->attach(axi_dbb);


	util::Release _{dla, ram, axi_dbb, periph, fabric_ctrl};


	const char* vcd_path = "trace.vcd";
	Verilated::commandArgs(argc, argv);
	if (argc < 2)
    {
		fprintf(stderr, "The simulation requires at least one parameter - path to a trace file\n");
		return 1;
	}

	if (argc == 3)
	{
		vcd_path = argv[2];
	}


#if VM_TRACE
	Verilated::traceEverOn(true);
	tfp = new VerilatedVcdC;
	dla->trace(tfp, 99);
	tfp->open(vcd_path);
	atexit(_close_trace);
#endif
	
	fabric_ctrl->load_trace(argv[1]);

	printf("Reset...\n");
	dla->rst = 0;
	dla->eval();
	tick(dla, tfp, 20);

	dla->rst = 1;
	dla->eval();
	tick(dla, tfp, 20);
	
	dla->rst = 0;
	
	printf("Letting buffers clear after reset...\n");
	tick(dla, tfp, 8192);

	printf("Running trace...\n");

	while (fabric_ctrl->eval())
	{
		periph->eval();
		axi_dbb->eval();
		tick(dla, tfp);
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
