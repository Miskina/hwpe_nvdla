
module tb_tcdm_to_axi2mem_nvdla #(
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

    // REQUEST CHANNEL 0
    output logic           tcdm_req_o_0,
    output logic [31:0]    tcdm_add_o_0,
    output logic           tcdm_type_o_0,
    output logic [3:0]     tcdm_be_o_0,
    output logic [31:0]    tcdm_data_o_0,
    input  logic           tcdm_gnt_i_0,

    // RESPONSE CHANNEL 0
    input  logic           tcdm_r_valid_i_0,
    input  logic [31:0]    tcdm_r_data_i_0,

    // REQUEST CHANNEL 1
    output logic           tcdm_req_o_1,
    output logic [31:0]    tcdm_add_o_0,
    output logic           tcdm_type_o_0,
    output logic [3:0]     tcdm_be_o_0,
    output logic [31:0]    tcdm_data_o_0,
    input  logic           tcdm_gnt_i_0,

    // RESPONSE CHANNEL 0
    input  logic           tcdm_r_valid_i_0,
    input  logic [31:0]    tcdm_r_data_i_0,

    // REQUEST CHANNEL 0
    output logic           tcdm_req_o_0,
    output logic [31:0]    tcdm_add_o_0,
    output logic           tcdm_type_o_0,
    output logic [3:0]     tcdm_be_o_0,
    output logic [31:0]    tcdm_data_o_0,
    input  logic           tcdm_gnt_i_0,

    // RESPONSE CHANNEL 0
    input  logic           tcdm_r_valid_i_0,
    input  logic [31:0]    tcdm_r_data_i_0,

    // REQUEST CHANNEL 0
    output logic           tcdm_req_o_0,
    output logic [31:0]    tcdm_add_o_0,
    output logic           tcdm_type_o_0,
    output logic [3:0]     tcdm_be_o_0,
    output logic [31:0]    tcdm_data_o_0,
    input  logic           tcdm_gnt_i_0,

    // RESPONSE CHANNEL 0
    input  logic           tcdm_r_valid_i_0,
    input  logic [31:0]    tcdm_r_data_i_0,
);
    
    logic test_mode_i = 0;

    hwpe_ctrl_intf_periph #(.ID_WIDTH(ID_WIDTH)) periph(clk);
    periph_to_csb bridge(.*);
    nvdla_csb_intf csb(clk);

    
    nvdla_dbb_intf dbb(clk);

    
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
