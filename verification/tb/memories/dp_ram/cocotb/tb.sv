module tb #(
    parameter DATA_WIDTH = 32,
    parameter DEPTH = 12,
    localparam WE_WIDTH = DATA_WIDTH / 8,
    localparam _sram_addr_width = $clog2(DEPTH)
) (
    input                         RST_N,
    //A
    input                         A_CLK,
    input  [_sram_addr_width-1:0] A_ADDR,
    input  [      DATA_WIDTH-1:0] A_WDATA,
    input  [        WE_WIDTH-1:0] A_WE,
    output [      DATA_WIDTH-1:0] A_RDATA,

    //B
    input                         B_CLK,
    input  [_sram_addr_width-1:0] B_ADDR,
    input  [      DATA_WIDTH-1:0] B_WDATA,
    input  [        WE_WIDTH-1:0] B_WE,
    output [      DATA_WIDTH-1:0] B_RDATA,

    output ARBITRATION_ERR

);
  dp_ram #(
      .WIDTH(DATA_WIDTH),
      .DEPTH(DEPTH),
      .STRB_WIDTH(8)
  ) u_sram (
      .RST_N(RST_N),

      .CLK_A(A_CLK),
      .ADDR_A(A_ADDR),
      .W_DATA_A(A_WDATA),
      .R_DATA_A(A_RDATA),
      .W_EN_A(A_WE),

      .CLK_B(B_CLK),
      .ADDR_B(B_ADDR),
      .W_DATA_B(B_WDATA),
      .R_DATA_B(B_RDATA),
      .W_EN_B(B_WE),

      .ARBITRATION_ERR(ARBITRATION_ERR)
  );

endmodule

