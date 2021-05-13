//TODO? Do we check address ID here?
// periph.add[31:16] == `NVDLA_CSB_ID

//TODO check missaligned access and byte enable
// HWPE Peripheral interface doesn't put any restrictions on
// address alignment. Depending on the missalignment and byte enable,
// might need to issue 2 requests to the NVDLA and combine them.

//Optional add a queue for multiple requests
    
module periph_to_csb_wip (
    input logic clk,
    input logic rst,
    hwpe_ctrl_intf_periph.slave periph,
    nvdla_csb_intf.master csb
);

logic write = '0;
logic gnt = '0;
logic r_valid = '0;
logic [periph.ID_WIDTH-1:0] id = '0;
logic [31:0] r_data, add = '0;
logic waiting_response = '0;

always_ff @(posedge clk) begin
    if (rst) begin
        id  <= '0;
        write <= '0;
        r_valid <= '0;
    end else begin
        id  <= periph.id;
        write <= ~periph.wen;
        r_valid <= gnt;
    end
end

always_ff @(posedge clk) begin
    if (rst) waiting_response <= '0;
    else if (!waiting_response && periph.req && csb.ready) waiting_response <= '1;
    else if (waiting_response && gnt) waiting_response <= '0;
end

always_ff @(posedge clk) begin
    if (rst) r_data <= '0;
    else if (csb.r_valid) r_data <= csb.r_data;
end

always_comb begin
    csb.valid       =  rst || waiting_response ? '0 : periph.req;
    csb.addr        =  rst || waiting_response ? '0 : periph.add[15:0];
    csb.wdat        =  rst || waiting_response ? '0 : periph.data;
    csb.write       = ~periph.wen; // Read: wen == 1; Write: wen == 0
    csb.nposted     =  1'b1;       // Always request a write response
    
    // Grant a request when we are sure we have an immediate response ready
    if (rst)
        gnt = '0;
    else
        gnt = write ? csb.wr_complete : csb.r_valid;

    periph.gnt     = gnt;
    periph.r_data  = r_data;
    periph.r_valid = r_valid;
    periph.r_id    = id;
end

endmodule
