module sp_ram #(
    parameter WIDTH = 8,
    parameter DEPTH = 8,
    parameter STRB_WIDTH = 8,
    localparam _WEN_WIDTH = WIDTH/STRB_WIDTH,
    localparam  _ADDR_WIDTH = $clog2(DEPTH)
)(
    input                       CLK,
    input                       RST_N,
    input  [_ADDR_WIDTH-1:0]    ADDR,
    input  [WIDTH-1:0]          D,
    input  [_WEN_WIDTH-1:0]     W_EN,
    output [WIDTH-1:0]          Q
);

logic [WIDTH-1:0] mem [DEPTH-1:0];
// logic [WIDTH-1:0] q_reg;
for(genvar i=1; i<=_WEN_WIDTH; i=i+1) begin:g_mem
    always @(posedge CLK) begin
        if(W_EN[i-1])
            mem[ADDR][(STRB_WIDTH*i)-1:STRB_WIDTH*(i-1)] <= 
                D[(STRB_WIDTH*i)-1:STRB_WIDTH*(i-1)];
    end
end

assign Q = RST_N ? mem[ADDR] : '0;

endmodule
