module tb #(
    parameter DATA_WIDTH = 32,
    parameter DEPTH = 12,
    localparam _addr_width = 32,
    localparam _sram_addr_width = $clog2(DEPTH)
) (
    input CLK,
    input RST_N,

    input  logic                   PSEL,
    input  logic                   PENABLE,
    input  logic                   PWRITE,
    input  logic [_addr_width-1:0] PADDR,
    input  logic [ DATA_WIDTH-1:0] PWDATA,
    output logic [ DATA_WIDTH-1:0] PRDATA,
    output logic                   PREADY,
    output logic                   PSLVERR

);
  localparam int _we_width = DATA_WIDTH / 8;

  logic [_sram_addr_width-1:0] sram_addr;
  logic sram_ce;
  logic [_we_width-1:0] sram_we;
  logic sram_oe;
  logic [DATA_WIDTH-1:0] sram_wdata;
  logic [DATA_WIDTH-1:0] sram_rdata;

  apb3_to_sram #(
      .ADDR_WIDTH(_addr_width),
      .DATA_WIDTH(DATA_WIDTH),
      .MEM_DEPTH (DEPTH)
  ) u_to_sram (
      .CLK  (CLK),
      .RST_N(RST_N), // Active high reset

      .PSEL(PSEL),
      .PENABLE(PENABLE),
      .PWRITE(PWRITE),
      .PADDR(PADDR),
      .PWDATA(PWDATA),
      .PRDATA(PRDATA),
      .PREADY(PREADY),
      .PSLVERR(PSLVERR),

      .SRAM_ADDR(sram_addr),
      .SRAM_CE(sram_ce),
      .SRAM_WE(sram_we),
      .SRAM_OE(sram_oe),
      .SRAM_WDATA(sram_wdata),
      .SRAM_RDATA(sram_rdata)
  );

  sp_ram #(
      .WIDTH(DATA_WIDTH),
      .DEPTH(DEPTH),
      .STRB_WIDTH(8)
  ) u_sram (
      .CLK  (CLK),
      .RST_N(RST_N),

      .ADDR(sram_addr),
      .D(sram_wdata),
      .W_EN(sram_we),
      .Q(sram_rdata)
  );

endmodule

