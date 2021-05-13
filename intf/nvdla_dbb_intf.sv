interface nvdla_dbb_intf #(
    parameter int unsigned NVDLA_DBB_AW_ADDR_WIDTH = 32,
    parameter int unsigned NVDLA_DBB_AR_ADDR_WIDTH = 32,
    parameter int unsigned NVDLA_DBB_W_DATA_WIDTH  = 64,
    parameter int unsigned NVDLA_DBB_R_DATA_WIDTH  = 64
) (
    input logic clk
);
    // AW (Write request) channel
    logic                                 aw_valid;
    logic                                 aw_ready;
    logic [3:0]                           aw_len;
    logic [NVDLA_DBB_WRITE_ADDR_SIZE-1:0] aw_addr;
    logic [7:0]                           aw_id;

    // AR (Read request) channel
    logic                                 ar_valid;
    logic                                 ar_ready;
    logic [3:0]                           ar_len;
    logic [NVDLA_DBB_READ_ADDR_SIZE-1:0]  aw_addr;
    logic [7:0]                           ar_id;

    // W (Write) channel
    logic                                 w_valid;
    logic                                 w_ready;
    logic [NVDLA_DBB_W_DATA_WIDTH-1:0]    w_data;
    logic                                 w_last;
    logic [NVDLA_DBB_W_STRB_WIDTH-1:0]    w_strb;

    // B (Write response) channel
    logic                                 b_valid;
    logic                                 b_ready;
    logic [7:0]                           b_id;

    // R (Read data channel)
    logic                                 r_valid;
    logic                                 r_ready;
    logic                                 r_last;
    logic [NVDLA_DBB_R_DATA_WIDTH-1:0]    r_data;
    logic [7:0]                           r_id;

    modport nvdla (   
        // AW
        input  aw_ready,
        output aw_valid, aw_len, aw_addr, aw_id,

        // AR
        input  ar_ready,
        output ar_valid, ar_len, ar_addr, ar_id,

        // W
        input  w_ready,
        output w_valid, w_data, w_last, w_strb,

        // B
        input  b_valid, b_id,
        output b_ready,

        // R
        input  r_valid, r_last, r_data, r_id,
        output r_ready
    );

    modport dbb (   
        // AW
        input  aw_valid, aw_len, aw_addr, aw_id,
        output aw_ready,

        // AR
        input  ar_valid, ar_len, ar_addr, ar_id,
        output ar_ready,

        // W
        input  w_valid, w_data, w_last, w_strb,
        output w_ready,

        // B
        input  b_ready,
        output b_valid, b_id,

        // R
        input  r_ready,
        output r_valid, r_last, r_data, r_id
    );

    
    // Uncomment if NVDLA starts supporting configurable sizes (as specified in documentation)
    // if(NVDLA_DBB_AW_ADDR_WIDTH != 32) begin
    //     $error($sformatf("Illegal value for parameter NVDLA_DBB_AW_ADDR_WIDTH (%d), must be 32 (currently hardocded in NV_nvdla", NVDLA_DBB_AW_ADDR_WIDTH))
    // end

    // if(NVDLA_DBB_AW_ADDR_WIDTH != 32 && NVDLA_DBB_AW_ADDR_WIDTH != 64) begin
    //     $error($sformatf("Illegal value for parameter NVDLA_DBB_AW_ADDR_WIDTH (%d), must be 32/64", NVDLA_DBB_AW_ADDR_WIDTH))
    // end

    // if(NVDLA_DBB_AR_ADDR_WIDTH != 32 && NVDLA_DBB_AR_ADDR_WIDTH != 64) begin
    //     $error($sformatf("Illegal value for parameter NVDLA_DBB_AR_ADDR_WIDTH (%d), must be 32/64", NVDLA_DBB_AR_ADDR_WIDTH))
    // end

    // if(NVDLA_DBB_W_DATA_WIDTH == 0 ||
    //    NVDLA_DBB_W_DATA_WIDTH > 512 ||
    //    ((NVDLA_DBB_W_DATA_WIDTH - 1) & NVDLA_DBB_W_DATA_WIDTH) != 0) begin
    //        $error($sformatf("Illegal value for parameter NVDLA_DBB_W_DATA_WIDTH (%d), must be 32/64/128/256/512", NVDLA_DBB_W_DATA_WIDTH))
    // end

    // if(NVDLA_DBB_R_DATA_WIDTH == 0 ||
    //    NVDLA_DBB_R_DATA_WIDTH > 512 ||
    //    ((NVDLA_DBB_R_DATA_WIDTH - 1) & NVDLA_DBB_R_DATA_WIDTH) != 0) begin
    //        $error($sformatf("Illegal value for parameter NVDLA_DBB_R_DATA_WIDTH (%d), must be 32/64/128/256/512", NVDLA_DBB_R_DATA_WIDTH))
    // end


    // TODO: Add assertions
    
endinterface //nvdla_dbb_intf