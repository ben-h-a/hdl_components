module fifo #(
    parameter DATA_WIDTH = 8,
    parameter DEPTH = 4'hF
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
    localparam  _addr_width = $clog2(DEPTH);

    logic [_addr_width-1:0] w_addr;
    logic [_addr_width-1:0] w_addr_nxt;
    logic [_addr_width-1:0] r_addr;
    logic [_addr_width-1:0] r_addr_q;

    logic full_q;
    logic empty_q;

    logic full_comb;
    logic empty_comb;

    assign full_comb = w_addr_nxt == r_addr_q;
    assign empty_comb = r_addr == w_addr;

    always @(posedge CLK_W or negedge RST_N) begin
        if(!RST_N) begin
            w_addr <= '0;
            w_addr_nxt <= '1;
            full_q <= '0;
        end else begin
            if(WEN && !full_comb) begin
                if(w_addr_nxt >= DEPTH-1) begin
                    w_addr_nxt <= '0;
                end else begin
                    w_addr_nxt <= w_addr_nxt + 1;
                end
                w_addr <= w_addr_nxt;
            end
            full_q <= full_comb;
        end
    end

    always @(posedge CLK_R or negedge RST_N) begin
        if(!RST_N) begin
            r_addr <= '0;
            //REVISIT: Verilator doesnt handle this
            r_addr_q <= DEPTH-1; 

            empty_q <= '1;
        end else begin
            if(REN && !empty_comb) begin
                if(r_addr >= DEPTH-1) begin
                    r_addr <= '0;
                end else begin
                    r_addr <= r_addr + 1;
                end
                r_addr_q <= r_addr;
            end
            empty_q <= empty_comb;
        end
    end

    dp_ram #(
        .DEPTH(DEPTH),
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
