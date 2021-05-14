module nvdla (
    input logic core_clk,
    input logic csb_clk,
    
    input logic rst_ni,
    input logic test_mode_i,

    nvdla_csb_intf.slave  csb,
    nvdla_dbb_intf.master dbb,
    
    output logic interrupt
);

    NV_nvdla nv_nvdla 
    (
        .dla_core_clk                  ( core_clk        ),
        .dla_csb_clk                   ( csb_clk         ),
        .global_clk_ovr_on             ( 1'b0            ),
        .tmc2slcg_disable_clock_gating ( 1'b1            ),
        .dla_reset_rstn                ( rst_ni          ),
        .direct_reset_                 ( rst_ni          ),
        .test_mode                     ( test_mode_i     ),
        .csb2nvdla_valid               ( csb.valid       ),              
        .csb2nvdla_ready               ( csb.ready       ),
        .csb2nvdla_addr                ( csb.addr        ),
        .csb2nvdla_wdat                ( csb.wdat        ),
        .csb2nvdla_write               ( csb.write       ),
        .csb2nvdla_nposted             ( csb.nposted     ),
        .nvdla2csb_valid               ( csb.r_valid     ),
        .nvdla2csb_data                ( csb.r_data      ),
        .nvdla2csb_wr_complete         ( csb.wr_complete ),
        .nvdla_core2dbb_aw_awvalid     ( dbb.aw_valid    ),
        .nvdla_core2dbb_aw_awready     ( dbb.aw_ready    ),
        .nvdla_core2dbb_aw_awaddr      ( dbb.aw_addr     ),
        .nvdla_core2dbb_aw_awid        ( dbb.aw_id       ),
        .nvdla_core2dbb_aw_awlen       ( dbb.aw_len      ),
        .nvdla_core2dbb_w_wvalid       ( dbb.w_valid     ),
        .nvdla_core2dbb_w_wready       ( dbb.w_ready     ),
        .nvdla_core2dbb_w_wdata        ( dbb.w_data      ),
        .nvdla_core2dbb_w_wstrb        ( dbb.w_strb      ),
        .nvdla_core2dbb_w_wlast        ( dbb.w_last      ),
        .nvdla_core2dbb_b_bvalid       ( dbb.b_valid     ),
        .nvdla_core2dbb_b_bready       ( dbb.b_ready     ),
        .nvdla_core2dbb_b_bid          ( dbb.b_id        ),
        .nvdla_core2dbb_ar_arvalid     ( dbb.ar_valid    ),
        .nvdla_core2dbb_ar_arready     ( dbb.ar_ready    ),
        .nvdla_core2dbb_ar_araddr      ( dbb.ar_addr     ),
        .nvdla_core2dbb_ar_arid        ( dbb.ar_id       ),
        .nvdla_core2dbb_ar_arlen       ( dbb.ar_len      ),
        .nvdla_core2dbb_r_rvalid       ( dbb.r_valid     ),
        .nvdla_core2dbb_r_rready       ( dbb.r_ready     ),
        .nvdla_core2dbb_r_rid          ( dbb.r_id        ),
        .nvdla_core2dbb_r_rlast        ( dbb.r_last      ),
        .nvdla_core2dbb_r_rdata        ( dbb.r_data      ),
        .dla_intr                      ( interrupt       ),
        .nvdla_pwrbus_ram_c_pd         ( 32'b0           ),
        .nvdla_pwrbus_ram_ma_pd        ( 32'b0           ),
        .nvdla_pwrbus_ram_mb_pd        ( 32'b0           ),
        .nvdla_pwrbus_ram_p_pd         ( 32'b0           ),
        .nvdla_pwrbus_ram_o_pd         ( 32'b0           ),
        .nvdla_pwrbus_ram_a_pd         ( 32'b0           )
    );


endmodule // nvdla
