module tb #(
    parameter DATA_WIDTH = 32,
    parameter DEPTH = 12,
    localparam WE_WIDTH = DATA_WIDTH / 8,
    localparam _sram_addr_width = $clog2(DEPTH)
) (
    input CLK,
    input RST_N,

    input [_sram_addr_width-1:0] SRAM_ADDR,
    input SRAM_CE,
    input [WE_WIDTH-1:0] SRAM_WE,
    input SRAM_OE,
    input [DATA_WIDTH-1:0] SRAM_WDATA,
    output [DATA_WIDTH-1:0] SRAM_RDATA


);
  sp_ram #(
      .WIDTH(DATA_WIDTH),
      .DEPTH(DEPTH),
      .STRB_WIDTH(8)
  ) u_sram (
      .CLK  (CLK),
      .RST_N(RST_N),

      .ADDR(SRAM_ADDR),
      .D(SRAM_WDATA),
      .W_EN(SRAM_WE),
      .Q(SRAM_RDATA)
  );

endmodule

