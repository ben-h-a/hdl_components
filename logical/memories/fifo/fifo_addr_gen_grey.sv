module fifo_addr_gen_grey #(
    parameter WIDTH = 8
) (
    input CLK,
    input RST_N,

    input INCR,

    output [WIDTH-1:0] ADDR_GREY,
    output [WIDTH-1:0] ADDR_GREY_NXT,
    output [WIDTH-1:0] ADDR_BIN
);

    logic [WIDTH-1:0] addr_bin_reg;
    logic [WIDTH-1:0] addr_grey;
    logic [WIDTH-1:0] addr_grey_reg;

    always @(posedge CLK, negedge RST_N) begin
        if(!RST_N) begin
            addr_bin_reg <= '0;
        end else begin
            if(INCR) begin
                addr_bin_reg <= addr_bin_reg+1;
            end
        end
    end

    bin_to_grey #(
        .WIDTH(WIDTH)
    ) (
        .BIN(addr_bin_reg),
        .GREY(addr_grey)
    );

    always @(posedge CLK) begin
        addr_grey_reg<=addr_grey;
    end

    assign ADDR_GREY = addr_grey_reg;
    assign ADDR_GREY_NXT = addr_grey;
    assign ADDR_BIN = addr_bin_reg;
endmodule