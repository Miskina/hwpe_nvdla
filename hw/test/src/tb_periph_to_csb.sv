`define ID_WIDTH 1

module tb_periph_to_csb (
    input logic clk,
    input logic rst,
    input logic stall,

    // HWPE Peripheral slave
    input  logic                 periph_req_i,
    input  logic [31:0]          periph_add_i,
    input  logic                 periph_wen_i,
    input  logic [3:0]           periph_be_i,
    input  logic [31:0]          periph_data_i,
    input  logic [`ID_WIDTH-1:0] periph_id_i,
    output logic                 periph_gnt_o,
    output logic [31:0]          periph_r_data_o,
    output logic                 periph_r_valid_o,
    output logic [`ID_WIDTH-1:0] periph_r_id_o
);

    hwpe_ctrl_intf_periph #(.ID_WIDTH(`ID_WIDTH)) periph(clk);
    nvdla_csb_intf csb(clk);

    periph_to_csb dut(.*);
    csb_slave_with_intf csb_slave(.*);


    always_comb begin
        periph.req = periph_req_i;
        periph.add = periph_add_i;
        periph.wen = periph_wen_i;
        periph.be  = periph_be_i;
        periph.data = periph_data_i;
        periph.id  = periph_id_i;

        periph_gnt_o = periph.gnt;
        periph_r_id_o = periph.r_id;
        periph_r_data_o = periph.r_data;
        periph_r_valid_o = periph.r_valid;
        periph_r_id_o = periph.r_id;
    end

`ifdef VERILATOR
    initial begin
        if ($test$plusargs("trace") != 0) begin
            $display("[%0t] Tracing to logs/vlt_dump.vcd...\n", $time);
            $dumpfile("logs/vlt_dump.vcd");
            $dumpvars();
        end
        $display("[%0t] Model running...\n", $time);
    end
`endif // VERILATOR

endmodule