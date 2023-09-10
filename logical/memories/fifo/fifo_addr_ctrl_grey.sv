module fifo_addr_ctrl_grey #(
    parameter ADDR_WIDTH = 8
) (
    input CLK_A,
    input CLK_B,
    input RST_N,

    input WEN_A,
    input REN_B,

    output [ADDR_WIDTH-1:0] W_ADDR_A,
    output [ADDR_WIDTH-1:0] R_ADDR_B,

    output FULL,
    output EMPTY
);

    logic [ADDR_WIDTH:0] w_addr_grey_i; //1 bit wider to indicate full or empty
    logic [ADDR_WIDTH:0] w_addr_grey_r_sync [1:0]; //1 bit wider to indicate full or empty
    logic [ADDR_WIDTH:0] w_addr_grey_nxt; //1 bit wider to indicate full or empty
    logic [ADDR_WIDTH:0] w_addr_bin_i; //1 bit wider to indicate full or empty
    
    logic [ADDR_WIDTH:0] r_addr_grey_i; //1 bit wider to indicate full or empty
    logic [ADDR_WIDTH:0] r_addr_grey_w_sync; //1 bit wider to indicate full or empty
    logic [ADDR_WIDTH:0] r_addr_grey_nxt; //1 bit wider to indicate full or empty
    logic [ADDR_WIDTH:0] r_addr_bin_i; //1 bit wider to indicate full or empty

    logic full_val;
    logic full_reg;
    logic empty_val;
    logic empty_reg;

    logic incr_w;
    logic incr_r;

    assign incr_w = WEN_A & !FULL;
    assign incr_r = REN_B & !EMPTY;

    fifo_addr_gen_grey #(
        .WIDTH(WIDTH+1)
    ) u_fifo_addr_gen_w_a (
        .CLK(CLK_A),
        .RST_N(RST_N),
        .INCR(incr_w),
        .ADDR_GREY(w_addr_grey_i),
        .ADDR_GREY_NXT(w_addr_grey_nxt),
        .ADDR_BIN(w_addr_bin_i)
    );

    fifo_addr_gen_grey #(
        .WIDTH(WIDTH+1)
    ) u_fifo_addr_gen_r_b (
        .CLK(CLK_B),
        .RST_N(RST_N),
        .INCR(incr_r),
        .ADDR_GREY(r_addr_grey_i),
        .ADDR_GREY(r_addr_grey_nxt),
        .ADDR_BIN(r_addr_bin_i)
    );

    //Sync addr
    always @(posedge CLK_A) begin
        if(!RST_N) begin
            r_addr_grey_w_sync[0] <= '0;
            r_addr_grey_w_sync[1] <= '0;
        end else begin
            r_addr_grey_w_sync = {r_addr_grey_w_sync[0], w_addr_grey_i};
        end
    end

    always @(posedge CLK_B) begin
        if(!RST_N) begin
            w_addr_grey_r_sync[0] <= '0;
            w_addr_grey_r_sync[1] <= '0;
        end else begin
            w_addr_grey_r_sync = {w_addr_grey_r_sync[0], r_addr_grey_i};
        end
    end

    //Empty
    assign empty_val = (r_addr_grey_nxt == w_addr_grey_r_sync);
    always @(posedge CLK_B) begin
        if(!RST_N)
            empty_reg <= '0;
        else
            empty_reg <= empty_val;
    end

    assign EMPTY = empty_reg;

    //Full

    assign full_val = (w_addr_grey_nxt[ADDR_WIDTH] != r_addr_grey_w_sync[ADDR_WIDTH]) &&
                      (w_addr_grey_nxt[ADDR_WIDTH-1] != r_addr_grey_w_sync[ADDR_WIDTH-1]) &&
                      (w_addr_grey_nxt[ADDR_WIDTH-2:0] == r_addr_grey_w_sync[ADDR_WIDTH-2:0]);
    always @(posedge CLK_A) begin
        if(!RST_N) begin
            full_reg <= '0;
        end else begin
            full_reg <= full_val;
        end
    end
    assign FULL = full_reg;
endmodule