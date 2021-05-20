`define TCDM_CONCAT(signal) {``signal``_3, ``signal``_2, ``signal``_1, ``signal``_0}
`define TCDM_TO_AXI2MEM                                     \
    .tcdm_master_req_o    ( `TCDM_CONCAT(tcdm_req_o)     ), \
    .tcdm_master_add_o    ( `TCDM_CONCAT(tcdm_add_o)     ), \
    .tcdm_master_wen_o    ( `TCDM_CONCAT(tcdm_wen_o)     ), \
    .tcdm_master_be_o     ( `TCDM_CONCAT(tcdm_be_o)      ), \
    .tcdm_master_data_o   ( `TCDM_CONCAT(tcdm_data_o)    ), \
    .tcdm_master_gnt_i    ( `TCDM_CONCAT(tcdm_gnt_i)     ), \
    .tcdm_master_r_valid_i( `TCDM_CONCAT(tcdm_r_valid_i) ), \
    .tcdm_master_r_data_i ( `TCDM_CONCAT(tcdm_r_data_i)  )

`define AXI_BURST_FIXED 2'b00
`define AXI_BURST_INCR  2'b01
`define AXI_BURST_WRAP  2'b10

`define AXI_AxSIZE 3'b110

`define AXI_SIGNAL_UNUSED_I '0
`define AXI_SIGNAL_UNUSED_O 


module tb_hwpe_nvdla #(
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
    output logic           tcdm_wen_o_0,
    output logic [3:0]     tcdm_be_o_0,
    output logic [31:0]    tcdm_data_o_0,
    input  logic           tcdm_gnt_i_0,

    // RESPONSE CHANNEL 0
    input  logic           tcdm_r_valid_i_0,
    input  logic [31:0]    tcdm_r_data_i_0,

    // REQUEST CHANNEL 1
    output logic           tcdm_req_o_1,
    output logic [31:0]    tcdm_add_o_1,
    output logic           tcdm_wen_o_1,
    output logic [3:0]     tcdm_be_o_1,
    output logic [31:0]    tcdm_data_o_1,
    input  logic           tcdm_gnt_i_1,

    // RESPONSE CHANNEL1 
    input  logic           tcdm_r_valid_i_1,
    input  logic [31:0]    tcdm_r_data_i_1,

    // REQUEST CHANNEL 2
    output logic           tcdm_req_o_2,
    output logic [31:0]    tcdm_add_o_2,
    output logic           tcdm_wen_o_2,
    output logic [3:0]     tcdm_be_o_2,
    output logic [31:0]    tcdm_data_o_2,
    input  logic           tcdm_gnt_i_2,

    // RESPONSE CHANNEL 2
    input  logic           tcdm_r_valid_i_2,
    input  logic [31:0]    tcdm_r_data_i_2,

    // REQUEST CHANNEL 3
    output logic           tcdm_req_o_3,
    output logic [31:0]    tcdm_add_o_3,
    output logic           tcdm_wen_o_3,
    output logic [3:0]     tcdm_be_o_3,
    output logic [31:0]    tcdm_data_o_3,
    input  logic           tcdm_gnt_i_3,

    // RESPONSE CHANNEL 3
    input  logic           tcdm_r_valid_i_3,
    input  logic [31:0]    tcdm_r_data_i_3
);
    
    logic test_mode_i = 0;

    nvdla_csb_intf csb(clk);
    hwpe_ctrl_intf_periph #(.ID_WIDTH(ID_WIDTH)) periph(clk);
    periph_to_csb bridge(.*);
    
    nvdla_dbb_intf dbb(clk);
    axi2mem #(
        .AXI_ID_WIDTH(8),
        .AXI_ADDR_WIDTH(32),
        .AXI_DATA_WIDTH(64)
    ) axi (
        .clk_i    ( clk  ),
        .rst_ni   ( ~rst ),
        .test_en_i( '0   ),
        .busy_o   (      ),

        // WRITE ADDRESS CHANNEL
        .axi_slave_aw_valid_i ( dbb.aw_valid         ),
        .axi_slave_aw_addr_i  ( dbb.aw_addr          ),
        .axi_slave_aw_prot_i  ( `AXI_SIGNAL_UNUSED_I ),
        .axi_slave_aw_region_i( `AXI_SIGNAL_UNUSED_I ),
        .axi_slave_aw_len_i   ( {4'h0, dbb.aw_len}   ),
        .axi_slave_aw_size_i  ( `AXI_AxSIZE          ),
        .axi_slave_aw_burst_i ( `AXI_SIGNAL_UNUSED_I ),
        .axi_slave_aw_lock_i  ( `AXI_SIGNAL_UNUSED_I ),
        .axi_slave_aw_cache_i ( `AXI_SIGNAL_UNUSED_I ),
        .axi_slave_aw_qos_i   ( `AXI_SIGNAL_UNUSED_I ),
        .axi_slave_aw_id_i    ( dbb.aw_id            ),
        .axi_slave_aw_user_i  ( `AXI_SIGNAL_UNUSED_I ),
        .axi_slave_aw_ready_o ( dbb.aw_ready         ),

        // READ ADDRESS CHANNEL
        .axi_slave_ar_valid_i ( dbb.ar_valid         ),
        .axi_slave_ar_addr_i  ( dbb.ar_addr          ),
        .axi_slave_ar_prot_i  ( `AXI_SIGNAL_UNUSED_I ),
        .axi_slave_ar_region_i( `AXI_SIGNAL_UNUSED_I ),
        .axi_slave_ar_len_i   ( { 4'h0, dbb.ar_len } ),
        .axi_slave_ar_size_i  ( `AXI_AxSIZE          ),
        .axi_slave_ar_burst_i ( `AXI_SIGNAL_UNUSED_I ),
        .axi_slave_ar_lock_i  ( `AXI_SIGNAL_UNUSED_I ),
        .axi_slave_ar_cache_i ( `AXI_SIGNAL_UNUSED_I ),
        .axi_slave_ar_qos_i   ( `AXI_SIGNAL_UNUSED_I ),
        .axi_slave_ar_id_i    ( dbb.ar_id            ),
        .axi_slave_ar_user_i  ( `AXI_SIGNAL_UNUSED_I ),
        .axi_slave_ar_ready_o ( dbb.ar_ready         ),

        // WRITE DATA CHANNEL
        .axi_slave_w_valid_i( dbb.w_valid          ),
        .axi_slave_w_data_i ( dbb.w_data           ),
        .axi_slave_w_strb_i ( dbb.w_strb           ),
        .axi_slave_w_user_i ( `AXI_SIGNAL_UNUSED_I ),
        .axi_slave_w_last_i ( dbb.w_last           ),
        .axi_slave_w_ready_o( dbb.w_ready          ),

        .axi_slave_b_valid_o( dbb.b_valid          ),
        .axi_slave_b_resp_o ( `AXI_SIGNAL_UNUSED_O ),
        .axi_slave_b_id_o   ( dbb.b_id             ),
        .axi_slave_b_user_o ( `AXI_SIGNAL_UNUSED_O ),
        .axi_slave_b_ready_i( dbb.b_ready          ),

        // READ DATA CHANNEL
        .axi_slave_r_valid_o( dbb.r_valid          ),
        .axi_slave_r_data_o ( dbb.r_data           ),
        .axi_slave_r_resp_o ( `AXI_SIGNAL_UNUSED_O ),
        .axi_slave_r_last_o ( dbb.r_last           ),
        .axi_slave_r_id_o   ( dbb.r_id             ),
        .axi_slave_r_user_o ( `AXI_SIGNAL_UNUSED_O ),
        .axi_slave_r_ready_i( dbb.r_ready          ),

        `TCDM_TO_AXI2MEM
    );

    nvdla dla (
        .core_clk( clk  ),
        .csb_clk ( clk  ),
        .rst_ni  ( !rst ),
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

    end


endmodule
