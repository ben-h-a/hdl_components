module dp_ram #(
    parameter WIDTH = 8,
    parameter DEPTH = 8,
    parameter STRB_WIDTH = 8,
    localparam _we_width = WIDTH / STRB_WIDTH,
    localparam _ADDR_WIDTH = $clog2(DEPTH)
) (
    input                    RST_N,
    //A
    input                    CLK_A,
    input  [_ADDR_WIDTH-1:0] ADDR_A,
    input  [      WIDTH-1:0] W_DATA_A,
    input  [  _we_width-1:0] W_EN_A,
    output [      WIDTH-1:0] R_DATA_A,

    //B
    input                    CLK_B,
    input  [_ADDR_WIDTH-1:0] ADDR_B,
    input  [      WIDTH-1:0] W_DATA_B,
    input  [  _we_width-1:0] W_EN_B,
    output [      WIDTH-1:0] R_DATA_B,

    output ARBITRATION_ERR
);


  /* verilator lint_off MULTIDRIVEN */
  logic [WIDTH-1:0] mem[DEPTH-1:0];
  /* verilator lint_on MULTIDRIVEN */
  logic arb_err;

  assign arb_err = (ADDR_A == ADDR_B) & |(W_EN_A & W_EN_B);
  assign ARBITRATION_ERR = arb_err;

  initial begin
    for (int i = 0; i < DEPTH; i = i + 1) begin
      mem[i] = '0;
    end
  end

  //------
  //A
  //------

  for (genvar i = 1; i <= _we_width; i = i + 1) begin : g_mem_a
    always @(posedge CLK_A) begin
      if (RST_N & W_EN_A[i-1]) begin
        /* verilator lint_off MULTIDRIVEN */
        mem[ADDR_A][(STRB_WIDTH*i)-1:STRB_WIDTH*(i-1)] <= 
                W_DATA_A[(STRB_WIDTH*i)-1:STRB_WIDTH*(i-1)];
        /* verilator lint_on MULTIDRIVEN */
      end
    end
  end

  assign R_DATA_A = mem[ADDR_A];
  //------
  //B
  //------

  for (genvar i = 1; i <= _we_width; i = i + 1) begin : g_mem_b
    always @(posedge CLK_B) begin
      if (RST_N & W_EN_B[i-1] & ~arb_err) begin

        /* verilator lint_off MULTIDRIVEN */
        mem[ADDR_B][(STRB_WIDTH*i)-1:STRB_WIDTH*(i-1)] <= 
                W_DATA_B[(STRB_WIDTH*i)-1:STRB_WIDTH*(i-1)];
        /* verilator lint_on MULTIDRIVEN */
      end
    end
  end

  assign R_DATA_B = mem[ADDR_B];

endmodule
