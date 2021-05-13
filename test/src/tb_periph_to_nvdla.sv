

module tb_periph_to_nvdla #(
    parameter ID_WIDTH = 1
) (
    input logic clk,
    input logic rst,

    // HWPE Peripheral slave
    input  logic                 periph_req_i,
    input  logic [31:0]          periph_add_i,
    input  logic                 periph_wen_i,
    input  logic [3:0]           periph_be_i,
    input  logic [31:0]          periph_data_i,
    input  logic [`ID_WIDTH-1:0] periph_id_i,
    output logic                 periph_gnt_o,
    output logic [31:0]          periph_r_data_o,
    output logic                 periph_r_valid_o,
    output logic [`ID_WIDTH-1:0] periph_r_id_o
);
    
    hwpe_ctrl_intf_periph #(.ID_WIDTH(ID_WIDTH)) periph(clk);
    nvdla_csb_intf csb(clk);

    periph_to_csb bridge(.*);
    
    nvdla_dbb_intf dbb(dbb_intf);

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