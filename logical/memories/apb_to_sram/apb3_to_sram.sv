module apb3_to_sram #(
    parameter int ADDR_WIDTH = 32,
    parameter int DATA_WIDTH = 32,
    parameter int MEM_DEPTH = 1024,  // Memory depth parameter
    localparam int _num_strb = DATA_WIDTH / 8,
    localparam _we_width = DATA_WIDTH / 8,
    localparam _byte_align = _num_strb > 1 ? $clog2(_we_width) : 1,
    localparam int _sram_addr_width = $clog2(MEM_DEPTH),
    localparam int _sram_addr_max = _sram_addr_width + _byte_align
) (
    input logic CLK,
    input logic RST_N, // Active high reset

    input  logic                  PSEL,
    input  logic                  PENABLE,
    input  logic                  PWRITE,
    input  logic [ADDR_WIDTH-1:0] PADDR,
    input  logic [DATA_WIDTH-1:0] PWDATA,
    output logic [DATA_WIDTH-1:0] PRDATA,
    output logic                  PREADY,
    output logic                  PSLVERR,

    output logic [_sram_addr_width-1:0] SRAM_ADDR,
    output logic                        SRAM_CE,
    output logic [       _we_width-1:0] SRAM_WE,
    output logic                        SRAM_OE,
    output logic [      DATA_WIDTH-1:0] SRAM_WDATA,
    input  logic [      DATA_WIDTH-1:0] SRAM_RDATA
);



  // State definition
  typedef enum logic [1:0] {
    IDLE   = 2'b00,
    SETUP  = 2'b01,
    ACCESS = 2'b10
  } state_t;

  state_t state;
  state_t next_state;

  // Internal signals
  logic [_sram_addr_width-1:0] sram_addr_reg;
  logic [DATA_WIDTH-1:0] prdata_reg;
  logic pready_reg;
  logic pslverr_reg;
  logic sram_ce_reg;
  logic [_we_width-1:0] sram_we_reg;
  logic sram_oe_reg;
  logic [DATA_WIDTH-1:0] sram_wdata_reg;

  // Output logic
  always_ff @(posedge CLK or negedge RST_N) begin
    if (!RST_N) begin
      sram_addr_reg <= '0;
      prdata_reg <= '0;
      pready_reg <= '0;
      pslverr_reg <= '0;
      sram_ce_reg <= '0;
      sram_we_reg <= '0;
      sram_oe_reg <= '0;
      sram_wdata_reg <= '0;
    end else begin
      pslverr_reg <= 1'b0;  // Clear PSLVERR
      case (state)
        IDLE: begin
          pready_reg  <= 1'b0;
          pslverr_reg <= 1'b0;
          sram_ce_reg <= 1'b1;
          sram_we_reg <= '0;
          sram_oe_reg <= 1'b1;

          //State transition
          if (PSEL && !PENABLE) begin
            state <= SETUP;
          end
        end
        SETUP: begin
          sram_ce_reg   <= 1'b0;
          //verilator lint_off WIDTHEXPAND
          //address width 32 sram depth 12. 32b data = we_width 4
          //0x0 = 0x0
          //0xc = 0x1
          //...
          //0xdepth + we_width -> 3
          sram_addr_reg <= PADDR[_sram_addr_max-1:_byte_align];
          if (PWRITE) begin
            sram_wdata_reg <= PWDATA;
            sram_we_reg <= '1;
            sram_oe_reg <= 1'b1;
          end else begin
            sram_we_reg <= '0;
            sram_oe_reg <= 1'b0;
          end

          //state transition
          if (PSEL && PENABLE) begin
            state <= ACCESS;
          end else begin
            state <= IDLE;
          end
        end
        ACCESS: begin
          pready_reg <= 1'b1;
          if (PADDR[_sram_addr_max-1:_byte_align] >= MEM_DEPTH) begin
            //verilator lint_on WIDTHEXPAND
            pslverr_reg <= 1'b1;  // Address out of range
            state <= IDLE;  // Transition to IDLE state
          end else begin
            if (!PWRITE) begin
              prdata_reg <= SRAM_RDATA;
            end
            if (PSEL && PENABLE) begin
              state <= IDLE;
            end

          end
        end
        default: begin
          state <= IDLE;
        end
      endcase
    end
  end

  //Output assignments
  assign PRDATA = prdata_reg;
  assign PREADY = pready_reg;
  assign PSLVERR = pslverr_reg;

  assign SRAM_ADDR = sram_addr_reg;
  assign SRAM_CE = sram_ce_reg;
  assign SRAM_WE = sram_we_reg;
  assign SRAM_OE = sram_oe_reg;
  assign SRAM_WDATA = sram_wdata_reg;



endmodule
