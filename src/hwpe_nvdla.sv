`define TCDM_CONCAT(signal) { tcdm[3].``signal, tcdm[2].``signal, tcdm[1].``signal, tcdm[0].``signal }
`define TCDM_TO_AXI2MEM                                \
    .tcdm_master_req_o    ( `TCDM_CONCAT(req)     ),   \
    .tcdm_master_add_o    ( `TCDM_CONCAT(add)     ),   \
    .tcdm_master_type_o   ( `TCDM_CONCAT(wen)     ),   \
    .tcdm_master_be_o     ( `TCDM_CONCAT(be)      ),   \
    .tcdm_master_data_o   ( `TCDM_CONCAT(wdata)   ),   \
    .tcdm_master_gnt_i    ( `TCDM_CONCAT(gnt)     ),   \
    .tcdm_master_r_valid_i( `TCDM_CONCAT(r_valid) ),   \
    .tcdm_master_r_data_i ( `TCDM_CONCAT(r_rdata) )

`define AXI_AxSIZE 3'b110

`define AXI_SIGNAL_UNUSED_I '0
`define AXI_SIGNAL_UNUSED_O 


module hwpe_nvdla #(
    parameter ID_WIDTH = 1
) (
    input logic           clk,
    input logic           rst,
    input logic           test_mode_i,
    
    XBAR_TCDM_BUS.Master  tcdm[3:0],
    XBAR_PERIPH_BUS.Slave periph,

    output logic          evt_o
);

    hwpe_ctrl_intf_periph #(.ID_WIDTH(ID_WIDTH)) periph_intf( clk );
    nvdla_csb_intf csb( clk );
    periph_to_csb periph2csb(
        .periph( periph_intf ),
        .*
    );

    nvdla_dbb_intf dbb( clk );
    axi2mem #(
        .AXI_ID_WIDTH(8),
        .AXI_ADDR_WIDTH(32),
        .AXI_DATA_WIDTH(64)
    ) axi (
        .clk_i    ( clk         ),
        .rst_ni   ( ~rst        ),
        .test_en_i( test_mode_i ),
        .busy_o   (             ),

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
        .core_clk (  clk  ),
        .csb_clk  (  clk  ),
        .rst_ni   ( !rst  ),
	.interrupt( evt_o ),
        .*
    );

    always_comb begin : periph_connect_comb
        periph.req   = periph_intf.req;
        periph.add   = periph_intf.add;
        periph.wen   = periph_intf.wen;
        periph.be    = periph_intf.be;
        periph.wdata = periph_intf.data;
        periph.id    = periph_intf.id;

        // What is opc???
        periph.r_opc = '0;

        periph_intf.gnt     = periph.gnt;
        periph_intf.r_id    = periph.r_id;
        periph_intf.r_data  = periph.r_rdata;
        periph_intf.r_valid = periph.r_valid;
        periph_intf.r_id    = periph.r_id;
    end

endmodule // hwpe_nvdla
