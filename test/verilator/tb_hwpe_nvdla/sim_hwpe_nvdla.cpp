
#include <stdlib.h>
#include <stdio.h>
#include <array>

#include <verilated.h>
#include "Vtb_hwpe_nvdla.h"

#include "Memory.h"
#include "TcdmMemoryController.h"
#include "FabricController.h"
#include "PeriphController.h"

#define TCDM_PORT_NAME(name, i) name ## _ ## i

#define TCDM_CONNECTION(i, model)                         \
	.req =     &model->TCDM_PORT_NAME(tcdm_req_o,     i), \
	.gnt =     &model->TCDM_PORT_NAME(tcdm_gnt_i,     i), \
	.add =     &model->TCDM_PORT_NAME(tcdm_add_o,     i), \
	.wen =     &model->TCDM_PORT_NAME(tcdm_wen_o,     i), \
	.be =      &model->TCDM_PORT_NAME(tcdm_be_o,      i), \
	.data =    &model->TCDM_PORT_NAME(tcdm_data_o,    i), \
	.r_data =  &model->TCDM_PORT_NAME(tcdm_r_data_i,  i), \
	.r_valid = &model->TCDM_PORT_NAME(tcdm_r_valid_i, i),


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

	Vtb_hwpe_nvdla* dla = new Vtb_hwpe_nvdla{"HWPE NVDLA"};
	
	std::array<TcdmConnections, 4> tcdm_connections
	{
		{
			TCDM_CONNECTION(0, dla)
		},
		{
			TCDM_CONNECTION(1, dla)
		},
		{
			TCDM_CONNECTION(2, dla)
		},
		{
			TCDM_CONNECTION(3, dla)
		}
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

	Memory<>* ram = new Memory<>{"TCDM_RAM"};
	auto* mem_ctrl = new TcdmMemoryController<4>{std::move(tcdm_connections), "TCDM_MEM"};
	mem_ctrl->attach(ram);

	PeriphController* periph = new PeriphController{std::move(periph_connections), "PeriphCTRL"};

	FabricController* fabric_ctrl = new FabricController{"FabricCTRL"};
	fabric_ctrl->attach(periph);
	fabric_ctrl->attach(mem_ctrl);

	util::Release _{dla, ram, mem_ctrl, periph, fabric_ctrl};

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
		mem_ctrl->eval();
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
