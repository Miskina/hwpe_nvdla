#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <verilated.h>
#include "verilated_vcd_c.h"
#include "PeriphMaster.h"
#include "Vtb_periph_to_csb.h"
#include "Util.h"

#define READ 1
#define WRITE 0

#define tick(top) do{ \
    top->clk = !top->clk; \
    Verilated::timeInc(1); \
    top->eval(); \
    } while(0)

int main(int argc, char** argv, char** env)
{

	srand(time(0));

     Verilated::mkdir("logs");

    Verilated::traceEverOn(true);
	Verilated::debug(0);
	Verilated::randReset(2);
	Verilated::commandArgs(argc, argv);

	auto* top = new Vtb_periph_to_csb{"TOP"};	

    VerilatedVcdC* vcd_dump = new VerilatedVcdC{};
    top->trace(vcd_dump, 10);
    // slave->trace(vcd_dump, 10);
    vcd_dump->open("trace.vcd");

    PeriphConnections periphConns{};
    periphConns.req = &top->periph_req_i;
    periphConns.add = &top->periph_add_i;
    periphConns.wen = &top->periph_wen_i;
    periphConns.be  = &top->periph_be_i;
    periphConns.data = &top->periph_data_i;
    periphConns.id = &top->periph_id_i;
    periphConns.gnt = &top->periph_gnt_o;
    periphConns.r_data = &top->periph_r_data_o;
    periphConns.r_valid = &top->periph_r_valid_o;
    periphConns.r_id = &top->periph_r_id_o;

    PeriphMaster periphMaster{"Periph Master", periphConns};

	top->clk = 1;
    top->rst = 0;
    top->stall = 0;
    Verilated::timeInc(1);
    top->eval();

    // PERIPH: write requesta
    periphMaster.add_op(WRITE, 0xABAB, 0xAABBCCDD, 0xF, 1);
    periphMaster.add_op(READ, 0xCDCD, 0x0, 0xF, 1);
    periphMaster.add_op(WRITE, 0xAABB, 0xABBAABBA, 0xF, 1);

    const int N_CYCLES = 20;

    for (int i = 0; i < N_CYCLES; i++)
    {
        top->stall = 0;

        // if (i % 4 == 0)
        // {
        //     top->stall = 1;
        // }

        periphMaster.eval();
        top->eval();
        tick(top);
        tick(top);
    }

	top->final();	
    vcd_dump->close();

    delete top;
    delete vcd_dump;

	return 0;
}