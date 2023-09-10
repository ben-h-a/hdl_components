module grey_counter #(
    parameter WIDTH = 8
) (
    input               CLK,
    input               INCR,
    input  [WIDTH-1:0]  IN_GREY,
    output [WIDTH-1:0]  OUT_GREY
);

    logic [WIDTH-1:0] bin_i;
    logic [WIDTH-1:0] bin_q;

    grey_to_bin #(.WIDTH(WIDTH))
    u_grey_to_bin
    (
        .GREY(IN_GREY),
        .BIN(bin_i)
    );

    always @(posedge CLK) begin
        if(INCR)
            bin_q <= bin_i + 1;
        else
            bin_q <= bin_i;
    end

    bin_to_grey #(.WIDTH(WIDTH)) (
        .GREY(OUT_GREY),
        .BIN(bin_q)
    );

endmodule