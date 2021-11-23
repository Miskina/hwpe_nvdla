
module tb_periph_to_nvdla #(
    localparam ID_WIDTH = 1,
    localparam NVDLA_DBB_AW_ADDR_WIDTH = 32,
    localparam NVDLA_DBB_AR_ADDR_WIDTH = 32,
    localparam NVDLA_DBB_W_DATA_WIDTH  = 64,
    localparam NVDLA_DBB_R_DATA_WIDTH  = 64,
    localparam NVDLA_DBB_W_STRB_WIDTH = NVDLA_DBB_W_DATA_WIDTH / 8
) (
    input logic clk,
    input logic rst,

    // HWPE Peripheral slave
    input  logic                periph_req_i,
    input  logic [31:0]         periph_add_i,
    input  logic                periph_wen_i,
    input  logic [3:0]          periph_be_i,
    input  logic [31:0]         periph_data_i,
    input  logic [ID_WIDTH-1:0] periph_id_i,
    output logic                periph_gnt_o,
    output logic [31:0]         periph_r_data_o,
    output logic                periph_r_valid_o,
    output logic [ID_WIDTH-1:0] periph_r_id_o,
    output logic                interrupt,

    // AW (Write request) channel
    output logic                               dbb_aw_valid,
    input  logic                               dbb_aw_ready,
    output logic [3:0]                         dbb_aw_len,
    output logic [NVDLA_DBB_AW_ADDR_WIDTH-1:0] dbb_aw_addr,
    output logic [7:0]                         dbb_aw_id,

    // AR (Read request) channel
    output logic                               dbb_ar_valid,
    input  logic                               dbb_ar_ready,
    output logic [3:0]                         dbb_ar_len,
    output logic [NVDLA_DBB_AR_ADDR_WIDTH-1:0] dbb_ar_addr,
    output logic [7:0]                         dbb_ar_id,

    // W (Write) channel
    output logic                               dbb_w_valid,
    input  logic                               dbb_w_ready,
    output logic [NVDLA_DBB_W_DATA_WIDTH-1:0]  dbb_w_data,
    output logic                               dbb_w_last,
    output logic [NVDLA_DBB_W_STRB_WIDTH-1:0]  dbb_w_strb,

    // B (Write response) channel
    input  logic                               dbb_b_valid,
    output logic                               dbb_b_ready,
    input  logic [7:0]                         dbb_b_id,

    // R (Read data channel)
    input  logic                               dbb_r_valid,
    output logic                               dbb_r_ready,
    input  logic                               dbb_r_last,
    input  logic [NVDLA_DBB_R_DATA_WIDTH-1:0]  dbb_r_data,
    input  logic [7:0]                         dbb_r_id
);
    
    logic test_mode_i = 0;

    hwpe_ctrl_intf_periph #(.ID_WIDTH(ID_WIDTH)) periph(clk);

    nvdla_csb_intf csb(clk);
    nvdla_dbb_intf dbb(clk);

    periph_to_csb bridge(.*);
    
    nvdla dla (
        .core_clk(clk),
        .csb_clk(clk),
        .rst_ni(!rst),
        .*
    );
    

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

        dbb_aw_valid = dbb.aw_valid;
        dbb.aw_ready = dbb_aw_ready;
        dbb_aw_len   = dbb.aw_len;
        dbb_aw_addr  = dbb.aw_addr;
        dbb_aw_id    = dbb.aw_id;
        
        dbb_ar_valid = dbb.ar_valid;
        dbb.ar_ready = dbb_ar_ready;
        dbb_ar_len   = dbb.ar_len;
        dbb_ar_addr  = dbb.ar_addr;
        dbb_ar_id    = dbb.ar_id;
        
        dbb_w_valid = dbb.w_valid;
        dbb.w_ready = dbb_w_ready;
        dbb_w_data  = dbb.w_data;
        dbb_w_last  = dbb.w_last;
        dbb_w_strb  = dbb.w_strb;
        
        dbb.b_valid = dbb_b_valid;
        dbb_b_ready = dbb.b_ready;
        dbb.b_id    = dbb_b_id;
        
        dbb.r_valid = dbb_r_valid;
        dbb_r_ready = dbb.r_ready;
        dbb.r_last  = dbb_r_last;
        dbb.r_data  = dbb_r_data;
        dbb.r_id    = dbb_r_id;
    end


endmodule