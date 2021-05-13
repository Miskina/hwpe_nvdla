`define ID_WIDTH 1

module periph_to_csb_wrapper (
    input  logic clk,
    input  logic rst,

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
    output logic [`ID_WIDTH-1:0] periph_r_id_o,

    // CSB master
    input  logic        csb_ready_i,
    input  logic        csb_r_valid_i,
    input  logic [31:0] csb_r_data_i,
    input  logic        csb_wr_complete_i,
    output logic        csb_valid_o,
    output logic [15:0] csb_addr_o,
    output logic [31:0] csb_wdat_o,
    output logic        csb_write_o,
    output logic        csb_nposted_o
);

    hwpe_ctrl_intf_periph #(.ID_WIDTH(`ID_WIDTH)) periph(clk);
    nvdla_csb_intf csb(clk);

    periph_to_csb dut(.*);

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

        csb.ready = csb_ready_i;
        csb.r_valid = csb_r_valid_i;
        csb.r_data = csb_r_data_i;
        csb.wr_complete = csb_wr_complete_i;

        csb_valid_o = csb.valid;
        csb_addr_o = csb.addr;
        csb_wdat_o = csb.wdat;
        csb_write_o = csb.write;
        csb_nposted_o = csb.nposted;
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

`ifdef FORMAL
    //
    // Setup
    //
    logic f_past_valid = 1'b0;
    always @(posedge clk)
        f_past_valid <= 1'b1;

    always_comb
    if (!f_past_valid)
        assume(rst);


    // For registered design

    // always @(posedge clk)
    // if ((!f_past_valid)||($past(rst)))
    //     assert(!csb_valid_o);

    // always @(posedge i_clk)
    // if ((f_past_valid)&&(!$past(rst))
    //         &&($past(periph_req_i && periph_gnt_o)))
    // begin
    //     // Following any transaction request that we accept,
    //     // csb_valid_o should be true
    //     assert(csb_valid_o);
    //     if ($past(periph_wen_i))
    //         assert(!csb_write_o);
    //     else
    //         assert(csb_write_o);
    // end

    always_comb
    if (!rst && periph_req_i && periph_gnt_o)
    begin
        // When any transaction request is going to be accepted,
        // csb_valid_o should be true
        assert(csb_valid_o);
        if (periph_wen_i)
            assert(!csb_write_o);
        else
            assert(csb_write_o);
    end
`endif // FORMAL
endmodule
