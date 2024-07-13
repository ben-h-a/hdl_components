module tb #(
    parameter DATA_WIDTH = 8,
    parameter DEPTH = 8
) (
    input CLK_W,
    input CLK_R,
    input RST_N,

    input [DATA_WIDTH-1:0] W_DATA,
    input WEN,

    output [DATA_WIDTH-1:0] R_DATA,
    input REN,

    output FULL,
    output EMPTY
);

fifo #(
    .DATA_WIDTH(DATA_WIDTH),
    .DEPTH_LOG2(DEPTH)
) u_dut (
    .CLK_W(CLK_W),
    .CLK_R(CLK_R),
    .RST_N(RST_N),

    .W_DATA(W_DATA),
    .WEN(WEN),

    .R_DATA(R_DATA),
    .REN(REN),

    .FULL(FULL),
    .EMPTY(EMPTY)
);

endmodule