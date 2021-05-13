module csb_slave_with_intf (
    input clk,
    input rst,

    input   logic        stall,

    nvdla_csb_intf.slave csb
);

typedef enum {IDLE, REQUEST_STALLED, STALL, READ_RESPONSE, WRITE_RESPONSE} state_e;
state_e current_state = IDLE, next_state = IDLE;

always_ff @(posedge clk) begin
    if (rst) current_state <= IDLE;
    else current_state <= next_state;
end

logic        write;
logic        nposted;

always_ff @(posedge clk) begin
    if (rst) {write, nposted} <= '0;
    else if (csb.valid & csb.ready) begin
        write <= csb.write;
        nposted <= csb.nposted;
    end
end

always_comb begin
    next_state = current_state;
    csb.wr_complete = '0;
    csb.r_valid = '0;
    csb.r_data  = '0;
    csb.ready = '0;
    unique case(current_state)
        IDLE: begin
            csb.ready = '1;
            if (csb.ready && csb.valid) begin
                if (stall) next_state = REQUEST_STALLED;
                // else next_state = csb.write ? WRITE_RESPONSE : READ_RESPONSE;
                else if (csb.write) begin
                    csb.wr_complete = '1;
                end else begin
                    csb.r_valid = '1;
                    csb.r_data  = 32'hDEADBEEF;
                end
            end else if (stall) next_state = STALL;
        end
        STALL: begin
            if (!stall) next_state = IDLE;
        end
        REQUEST_STALLED: begin
            if (!stall) next_state = csb.write ? WRITE_RESPONSE : READ_RESPONSE;
        end
        WRITE_RESPONSE: begin
            csb.wr_complete = '1;
            if (stall) next_state = STALL;
            else next_state = IDLE;
        end
        READ_RESPONSE: begin
            csb.r_valid = '1;
            csb.r_data  = 32'hDEADBEEF;
            if (stall) next_state = STALL;
            else next_state = IDLE;
        end
        default: ;
    endcase
end

endmodule