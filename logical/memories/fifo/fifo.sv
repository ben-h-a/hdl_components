module fifo #(
    parameter DATA_WIDTH = 8;
    parameter DEPTH = 8;
) (
    input CLK_A,
    input CLK_B,
    input RST_N,

    input W_DATA_A,
    input WEN_A,

    output R_DATA_B,
    input R_EN_B,

    output FULL,
    output EMPTY
);
    localparam  _addr_width = $clog2(DEPTH)

    logic [_addr_width-1:0] w_addr;
    logic [_addr_width-1:0] r_addr;

    dp_sram #(
        .DEPTH(DEPTH),
        .WIDTH(DATA_WIDTH),
        .STRB_WIDTH(DATA_WIDTH)
    )
    u_sram (
        .RST_N              (RST_N              ), //input
        //A
        .CLK_A              (CLK_A              ), //input
        .ADDR_A             (w_addr             ), //input  [_ADDR_WIDTH-1:0]
        .W_DATA_A           (W_DATA_A           ), //input  [WIDTH-1:0]
        .W_EN_A             (W_EN_A             ), //input  [_WEN_WIDTH-1:0]
        .R_DATA_A           (/*NC*/             ), //output [WIDTH-1:0]
        //B
        .CLK_B              (CLK_B              ), //input
        .ADDR_B             (r_addr             ), //input  [_ADDR_WIDTH-1:0]
        .W_DATA_B           ('0                 ), //input  [WIDTH-1:0]
        .W_EN_B             ('0                 ), //input  [_WEN_WIDTH-1:0]
        .R_DATA_B           (R_DATA_B           ), //output [WIDTH-1:0]

        .ARBITRATION_ERR    (/*NC*/             ), //output
    );

    fifo_addr_ctrl_grey #(
        .ADDR_WIDTH(_addr_width)
    ) u_ctrl (
        .CLK_A    (CLK_A    ), //input
        .CLK_B    (CLK_B    ), //input
        .RST_N    (RST_N    ), //input

        .WEN_A    (W_EN_A   ), //input
        .REN_B    (R_EN_B   ), //input
        .W_ADDR_A (w_addr   ), //output [ADDR_WIDTH-1:0]
        .R_ADDR_B (r_addr   ), //output [ADDR_WIDTH-1:0]
        .FULL     (FULL     ), //output
        .EMPTY    (EMPTY    )  //output
    );

endmodule
