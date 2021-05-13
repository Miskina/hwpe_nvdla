#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <verilated.h>

#include "Vperiph_to_csb_wrapper.h"
#include "Vcsb_slave.h"

#include "PeriphMaster.h"
// #include "CsbSlave.h"

#define READ 1
#define WRITE 0

#if !VM_SC

#include "Connections.h"
#include "Util.h"

#if VM_TRACE
#include <verilated_vcd_c.h>
#endif


#define tick(model, dumper) do{ \
    model.update_clk(); \
    Verilated::timeInc(1); \
    model.eval(); \
    dumper->dump(Verilated::time()); \
    } while(0)
    
void eval(Vperiph_to_csb_wrapper* top, Vcsb_slave* slave)
{
    top->csb_ready_i       = slave->csb_ready_o;
    top->csb_r_valid_i     = slave->csb_r_valid_o;
    top->csb_r_data_i      = slave->csb_r_data_o;
    top->csb_wr_complete_i = slave->csb_wr_complete_o;

    slave->csb_valid_i   = top->csb_valid_o;
    slave->csb_addr_i    = top->csb_addr_o;
    slave->csb_wdat_i    = top->csb_wdat_o;
    slave->csb_write_i   = top->csb_write_o;
    slave->csb_nposted_i = top->csb_nposted_o;
}

int main(int argc, char** argv, char** env)
{

	srand(time(0));

    Verilated::mkdir("logs");

    Verilated::traceEverOn(true);
	Verilated::debug(0);
	Verilated::randReset(2);
	Verilated::commandArgs(argc, argv);

	auto* top = new Vperiph_to_csb_wrapper{"TOP"};	
    auto* slave = new Vcsb_slave{"CSB slave"};
    util::VerilatedModel model{top, slave};

    VerilatedVcdC* vcd_dump = new VerilatedVcdC{};
    top->trace(vcd_dump, 10);
    slave->trace(vcd_dump, 10);
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
    // PeriphMaster<Vperiph_to_csb_wrapper> periphMaster(top);
    // CsbSlave<Vperiph_to_csb_wrapper> csbSlave(top);

    model.update_clk();

    top->csb_ready_i       = slave->csb_ready_o;
    top->csb_r_valid_i     = slave->csb_r_valid_o;
    top->csb_r_data_i      = slave->csb_r_data_o;
    top->csb_wr_complete_i = slave->csb_wr_complete_o;

    slave->rst = 0;
    slave->stall = 0;
    slave->csb_valid_i   = top->csb_valid_o;
    slave->csb_addr_i    = top->csb_addr_o;
    slave->csb_wdat_i    = top->csb_wdat_o;
    slave->csb_write_i   = top->csb_write_o;
    slave->csb_nposted_i = top->csb_nposted_o;
    // top->csb_wr_complete_i = 0;
    // top->csb_r_valid_i = 0;
    // top->csb_ready_i = 0;

    // slave->rst = 0;
    // slave->stall = 0;
    // slave->csb_valid_i = 0;
    // slave->csb_addr_i = 0;
    // slave->csb_wdat_i = 0;
    // slave->csb_write_i = 0;
    // slave->csb_nposted_i = 1; 

    model.eval();

    periphMaster.eval(true);
    eval(top, slave);
    model.eval();
    Verilated::timeInc(1);
    model.update_clk();

    // PERIPH: write requesta
    periphMaster.add_op(WRITE, 0xABAB, 0xAABBCCDD, 0xF, 1);
    periphMaster.add_op(READ, 0xCDCD, 0x0, 0xF, 1);

    const int N_CYCLES = 20;
    for (int i = 0; i < N_CYCLES; ++i)
    {
        periphMaster.eval();
        model.eval();
        eval(top, slave);
        tick(model, vcd_dump);
        eval(top, slave);
        tick(model, vcd_dump);
        eval(top, slave);
    }

    model.final();

    vcd_dump->close();
    delete vcd_dump;

	return 0;
}

#else

#include <systemc.h>

#if VM_TRACE
#include <verilated_vcd_sc.h>
#endif

#include "Stallable.h"
#include "StallGen.h"
#include "SignalConverter.h"

int sc_main(int argc, char* argv[])
{

    Verilated::mkdir("logs");
    Verilated::traceEverOn(true);
    Verilated::debug(0);
    Verilated::randReset(2);
    Verilated::commandArgs(argc, argv);

    PeriphMaster master{"Periph Master"};
    // StallGen generator{"Stall Generator"};
    // CsbSlave slave{"CSB Slave"};
    // generator.stall = &slave;
    SignalConverter<unsigned, short> converter{"Address Converter"};

    // name, period, duty_cycle, start_time, posedge_first
    sc_clock csb_clk{"csb_clk", 5, SC_NS, 0.5, 3, SC_NS, true};

    auto* top = new Vperiph_to_csb_wrapper{"Periph2CSB"};
    auto* slave = new Vcsb_slave{"CSB Slave"};

    sc_out<bool> stall;
    sc_out<bool> rst;
    
    // Connect to HWPE Periph 
    top->clk(csb_clk);
    top->periph_req_i(master.req);
    top->periph_add_i(master.add);
    top->periph_wen_i(master.wen);
    top->periph_data_i(master.data);
    top->periph_id_i(master.id);

    master.gnt(top->periph_gnt_o);
    master.r_data(top->periph_r_data_o);
    master.r_valid(top->periph_r_valid_o);
    master.r_id(top->periph_r_id_o);

    // Connect to CSB
    top->csb_ready_i(slave->csb_ready_o);
    top->csb_r_valid_i(slave->csb_r_valid_o);
    top->csb_r_data_i(slave->csb_r_data_o);
    top->csb_wr_complete_i(slave->csb_wr_complete_o);

    slave->clk(csb_clk);
    slave->rst(rst);
    slave->stall(stall);
    slave->csb_valid_i(top->csb_valid_o);
    slave->csb_addr_i(top->csb_addr_o);
    // converter.input(top->csb_addr_o);
    // slave->csb_addr_i(converter.output);
    slave->csb_wdat_i(top->csb_wdat_o);
    slave->csb_write_i(top->csb_write_o);
    slave->csb_nposted_i(top->csb_nposted_o);

    master.add_op(WRITE, 0xAABB, 0xAABBCCDD, 0xF, 1);

    for (int i = 0; i < 20; ++i)
    {
        sc_start(1, SC_NS);
    }
    
    top->final();
    slave->final();
    
    delete top;
    delete slave;
    
    return 0;
}

#endif