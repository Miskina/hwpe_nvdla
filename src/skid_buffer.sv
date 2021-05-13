module skid_buffer #(
    parameter DW = 8
) (
    input logic clk,
    input logic rst,

    input  logic          valid_i,
    output logic          ready_o,
    input  logic [DW-1:0] data_i,

    output logic          valid_o,
    input  logic          ready_i,
    output logic [DW-1:0] data_o
);

logic          valid_ff;
logic [DW-1:0] data_ff;

always_ff @(posedge clk) begin
    if (rst) valid_ff <= '0;
    else if ((valid_i && ready_o) && (valid_o && !ready_i)) valid_ff <= '1;
    else if (ready_i) valid_ff <= '0;
end

always_ff @(posedge clk) begin
    if (rst) data_ff <= '0;
    else if (valid_i && ready_o) data_ff <= data_i;
end

always_comb begin
    ready_o = ~valid_ff;
    valid_o = ~rst & (valid_i | valid_ff);
    if (valid_ff) data_o = data_ff;
    else if (valid_i) data_o = data_i;
    else data_o = '0;
end

endmodule
