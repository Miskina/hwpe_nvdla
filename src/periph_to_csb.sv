//TODO? Do we check address ID here?
// periph.add[31:16] == `NVDLA_CSB_ID

//TODO check missaligned access and byte enable
// HWPE Peripheral interface doesn't put any restrictions on
// address alignment. Depending on the missalignment and byte enable,
// might need to issue 2 requests to the NVDLA and combine them.

//Optional add a queue for multiple requests
    
module periph_to_csb (
    input logic clk,
    input logic rst,
    hwpe_ctrl_intf_periph.slave periph,
    nvdla_csb_intf.master csb
);

logic [periph.ID_WIDTH-1:0] id = '0;
logic [31:0]                r_data = '0;

// Register inputs
always_ff @(posedge clk) begin
    if (rst) id <= '0;
    else if (csb.valid && csb.ready) id <= periph.id;
end

always_ff @(posedge clk) begin
    if (rst) r_data <= '0;
    else if (csb.r_valid) r_data <= csb.r_data;
end

typedef enum {IDLE, WAITING_RESPONSE, GRANT_REQUEST, SEND_RESPONSE} state_e;
state_e current_state = IDLE, next_state;

always_ff @(posedge clk) begin
    if (rst) current_state = IDLE;
    else current_state = next_state;
end

always_comb begin
    next_state = current_state;
    csb.valid = '0;
    periph.gnt = '0;
    periph.r_valid = '0;
    unique case (current_state)
        IDLE: begin
            csb.valid = periph.req;
            if (periph.req && csb.ready) begin

                if ((csb.write && csb.wr_complete) || (!csb.write && csb.r_valid)) begin
                    next_state = GRANT_REQUEST;
                end else begin
                    next_state = WAITING_RESPONSE;
                end
            end 
        end
        WAITING_RESPONSE: begin
            if ((csb.write && csb.wr_complete) || (!csb.write && csb.r_valid)) begin
                next_state = GRANT_REQUEST;
            end
        end
        GRANT_REQUEST: begin
            periph.gnt = '1;
            next_state = SEND_RESPONSE;
        end
        SEND_RESPONSE: begin
            periph.r_valid = '1;
            next_state = IDLE;
        end
    endcase
end

always_comb begin
    csb.addr  =  periph.add[15:0];
    csb.wdat  =  periph.data;
    csb.write = ~periph.wen;
    csb.nposted = '1; // Always request a write response

    periph.r_data = r_data;
    periph.r_id = id;
end

endmodule
