module fifo #(
    parameter DATA_WIDTH = 8,
    parameter DEPTH_LOG2 = 4
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
    localparam  _addr_width = DEPTH_LOG2-1;

    logic [_addr_width:0] w_addr;
    logic [_addr_width:0] w_addr_nxt;
    logic [_addr_width:0] w_addr_prv;
    logic [_addr_width:0] r_addr;
    logic [_addr_width:0] r_addr_nxt;
    logic [_addr_width:0] r_addr_prv;

    logic full_q;
    logic empty_q;

    always @(posedge CLK_W or negedge RST_N) begin
        if(!RST_N) begin
            w_addr <= '0;
            w_addr_nxt <= 1;
            w_addr_prv <= '1;
            full_q <= '0;
        end else begin
            if(WEN && !full_q) begin
                w_addr_nxt <= w_addr_nxt + 1;
                w_addr <= w_addr_nxt;
                w_addr_prv <= w_addr;
                //assert full on write transaction
                full_q <= w_addr_nxt == r_addr;
            end
            if(full_q) begin
                full_q <= w_addr == r_addr;
            end
        end
    end

    always @(posedge CLK_R or negedge RST_N) begin
        if(!RST_N) begin
            r_addr <= '0;
            r_addr_nxt <= 1;
            r_addr_prv <= '1;
            empty_q <= '1;
        end else begin
            if(REN && !empty_q) begin
                r_addr_nxt <= r_addr_nxt + 1;
                r_addr <= r_addr_nxt;
                r_addr_prv <= r_addr;

                empty_q <= r_addr_nxt == w_addr;
            end else begin
                if(empty_q) begin
                    empty_q <= r_addr == w_addr;
                end
            end
        end
    end

    dp_ram #(
        .DEPTH(2**DEPTH_LOG2),
        .WIDTH(DATA_WIDTH),
        .STRB_WIDTH(DATA_WIDTH)
    )
    u_sram (
        .RST_N              (RST_N              ), //input
        //A
        .CLK_A              (CLK_W              ), //input
        .ADDR_A             (w_addr             ), //input  [_ADDR_WIDTH-1:0]
        .W_DATA_A           (W_DATA             ), //input  [WIDTH-1:0]
        .W_EN_A             (WEN                ), //input  [_WEN_WIDTH-1:0]
        .R_DATA_A           (/*NC*/             ), //output [WIDTH-1:0]
        //B
        .CLK_B              (CLK_R              ), //input
        .ADDR_B             (r_addr             ), //input  [_ADDR_WIDTH-1:0]
        .W_DATA_B           ('0                 ), //input  [WIDTH-1:0]
        .W_EN_B             ('0                 ), //input  [_WEN_WIDTH-1:0]
        .R_DATA_B           (R_DATA             ), //output [WIDTH-1:0]

        .ARBITRATION_ERR    (/*NC*/             ) //output
    );

    assign EMPTY = empty_q;
    assign FULL = full_q;


endmodule
