`include "axi4_lite_to_sram_defs.sv"
module axi4_lite_to_sram #(
    parameter ADDR_WIDTH = 32,
    parameter DATA_WIDTH = 32,

    parameter ID_WIDTH = 1,

    parameter SRAM_DATA_WIDTH = 32,
    parameter SRAM_ADDR_WIDTH = 8,
    parameter SRAM_STRB_WIDTH = 8,
    localparm _sram_wstrb = SRAM_DATA_WIDTH/SRAM_STRB_WIDTH

) (

    input                           AIX_CLK,
    input                           RST_N,

    input                           AWVALID,
    output                          AWREADY,
    input  [ID_WIDTH-1:0]           AWID,
    input  [ADDR_WIDTH-1:0]         AWADDR,
    input  [2:0]                    AWPROT,

    input                           WVALID,
    output                          WREADY,
    input  [DATA_WIDTH-1:0]         WDATA,
    input  [STRB_WIDTH-1:0]         WSTRB,
    input                           WLAST,

    output                          BVALID,
    input                           BREADY,
    output [ID_WIDTH-1:0]           BID,
    output [BRESP_WIDTH-1:0]        BRESP,

    input                           ARVALID,
    output                          ARREADY,
    input  [ID_WIDTH-1:0]           ARID,
    input  [ADDR_WIDTH-1:0]         ARADDR,

    output                          RVALID,
    input                           RREADY,
    output [ID_WIDTH-1:0]           RID,
    output [DATA_WIDTH-1:0]         RDATA,
    output [RRESP_WIDTH-1:0]        RRESP,

    //SRAM 

    output [SRAM_ADDR_WIDTH-1:0]    SRAM_ADDR,
    output [SRAM_DATA_WIDTH-1:0]    SRAM_DATA_W,
    output [_sram_wstrb-1:0]        SRAM_W_EN,
    input [SRAM_DATA_WIDTH-1:0]     SRAM_DATA_R
);

//AXI write


endmodule
`include "axi4_lite_to_sram_undefs.sv"
