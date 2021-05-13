module csb_slave (
    input clk,
    input rst,

    input   logic        stall,

    output  logic        csb_ready_o,
    output  logic        csb_r_valid_o,
    output  logic [31:0] csb_r_data_o,
    output  logic        csb_wr_complete_o,
    input   logic        csb_valid_i,
    input   logic [15:0] csb_addr_i,
    input   logic [31:0] csb_wdat_i,
    input   logic        csb_write_i,
    input   logic        csb_nposted_i
);

typedef enum {IDLE, REQUEST_STALLED, STALL, READ_RESPONSE, WRITE_RESPONSE} state_e;
state_e current_state = IDLE, next_state;

always_ff @(posedge clk) begin
    if (rst) current_state <= IDLE;
    else current_state <= next_state;
end

logic        write;
logic        nposted;

always_ff @(posedge clk) begin
    if (rst) {write, nposted} <= '0;
    else if (csb_valid_i & csb_ready_o) begin
        write <= csb_write_i;
        nposted <= csb_nposted_i;
    end
end

always_comb begin
    next_state = current_state;
    csb_wr_complete_o = '0;
    csb_r_valid_o = '0;
    csb_r_data_o  = '0;
    csb_ready_o = '0;
    unique case(current_state)
        IDLE: begin
            csb_ready_o = '1;
            if (csb_ready_o && csb_valid_i) begin
                if (stall) next_state = REQUEST_STALLED;
                else next_state = csb_write_i ? WRITE_RESPONSE : READ_RESPONSE;
            end else if (stall) next_state = STALL;
        end
        STALL: begin
            if (!stall) next_state = IDLE;
        end
        REQUEST_STALLED: begin
            if (!stall) next_state = csb_write_i ? WRITE_RESPONSE : READ_RESPONSE;
        end
        WRITE_RESPONSE: begin
            csb_wr_complete_o = '1;
            if (stall) next_state = STALL;
            else next_state = IDLE;
        end
        READ_RESPONSE: begin
            csb_r_valid_o = '1;
            csb_r_data_o  = 32'hDEADBEEF;
            if (stall) next_state = STALL;
            else next_state = IDLE;
        end
        default: ;
    endcase
end

endmodule
