interface nvdla_csb_intf (
    input logic clk
);

    logic        valid;
    logic        ready;
    logic [15:0] addr;
    logic [31:0] wdat;
    logic        write;
    logic        r_valid;
    logic [31:0] r_data;
    logic        wr_complete;
    logic        nposted;

    modport master (
        input  ready, r_valid, r_data, wr_complete,
        output valid, addr, wdat, write, nposted
    );

    modport slave (
        output ready, r_valid, r_data, wr_complete,
        input  valid, addr, wdat, write, nposted
    );
    
    modport monitor (
        input ready, r_valid, r_data, wr_complete,
              valid, addr, wdat, write, nposted
    );


    // TODO: Add assertions

endinterface // nvdla_csb_intf
