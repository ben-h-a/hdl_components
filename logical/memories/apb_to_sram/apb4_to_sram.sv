module APB3_to_SRAM_bridge #(
    parameter int ADDR_WIDTH = 32,
    parameter int DATA_WIDTH = 32,
    parameter int MEM_DEPTH  = 1024  // Memory depth parameter
)(
    input  logic                  CLK,
    input  logic                  RST_N,  // Active high reset
`ifdef USE_INTERFACE
    apb3_if.sub APB_IF,
    sram_if.mng SRAM_IF
`else
    input  logic                  PSEL,
    input  logic                  PENABLE,
    input  logic                  PWRITE,
    input  logic  [ADDR_WIDTH-1:0] PADDR,
    input  logic  [DATA_WIDTH-1:0] PWDATA,
    output logic  [DATA_WIDTH-1:0] PRDATA,
    output logic                  PREADY,
    output logic                  PSLVERR,
    output logic  [ADDR_WIDTH-1:0] SRAM_ADDR,
    output logic                  SRAM_CE,
    output logic                  SRAM_WE,
    output logic                  SRAM_OE,
    output logic  [DATA_WIDTH-1:0] SRAM_WDATA,
    input  logic  [DATA_WIDTH-1:0] SRAM_RDATA
`endif
);

    // State definition
    typedef enum logic [1:0] {
        IDLE   = 2'b00,
        SETUP  = 2'b01,
        ACCESS = 2'b10
    } state_t;

    state_t state, next_state;

    // Internal signals
    logic [ADDR_WIDTH-1:0] sram_addr_reg;
    logic [DATA_WIDTH-1:0] prdata_reg;
    logic pready_reg;
    logic pslverr_reg;
    logic sram_ce_reg;
    logic sram_we_reg;
    logic sram_oe_reg;
    logic [DATA_WIDTH-1:0] sram_wdata_reg;

`ifdef USE_INTERFACE
    // Glue logic for interfaces
    logic                  PSEL      = APB_IF.PSEL;
    logic                  PENABLE   = APB_IF.PENABLE;
    logic                  PWRITE    = APB_IF.PWRITE;
    logic  [ADDR_WIDTH-1:0] PADDR    = APB_IF.PADDR;
    logic  [DATA_WIDTH-1:0] PWDATA   = APB_IF.PWDATA;
    assign APB_IF.PRDATA               = prdata_reg;
    assign APB_IF.PREADY               = pready_reg;
    assign APB_IF.PSLVERR              = pslverr_reg;

    logic  [ADDR_WIDTH-1:0] SRAM_ADDR = SRAM_IF.ADDR;
    logic                  SRAM_CE   = SRAM_IF.CE;
    logic                  SRAM_WE   = SRAM_IF.WE;
    logic                  SRAM_OE   = SRAM_IF.OE;
    logic  [DATA_WIDTH-1:0] SRAM_WDATA = SRAM_IF.WDATA;
    logic  [DATA_WIDTH-1:0] SRAM_RDATA = SRAM_IF.RDATA;
`else
    // Assign outputs
    assign PRDATA = prdata_reg;
    assign PREADY = pready_reg;
    assign PSLVERR = pslverr_reg;
    assign SRAM_ADDR = sram_addr_reg;
    assign SRAM_CE = sram_ce_reg;
    assign SRAM_WE = sram_we_reg;
    assign SRAM_OE = sram_oe_reg;
    assign SRAM_WDATA = sram_wdata_reg;
`endif

    // State transition logic
    always_ff @(posedge CLK or negedge RST_N) begin
        if (!RST_N) begin
            state <= IDLE;
        end else begin
            state <= next_state;
        end
    end

    // Next state logic
    always_comb begin
        next_state = state;
        case (state)
            IDLE: begin
                if (PSEL && !PENABLE) begin
                    next_state = SETUP;
                end
            end
            SETUP: begin
                if (PSEL && PENABLE) begin
                    next_state = ACCESS;
                end else begin
                    next_state = IDLE;
                end
            end
            ACCESS: begin
                if (PSEL && PENABLE) begin
                    next_state = IDLE;
                end
            end
        endcase
    end

    // Output logic
    always_ff @(posedge CLK or negedge RST_N) begin
        if (!RST_N) begin
            sram_addr_reg <= '0;
            prdata_reg <= '0;
            pready_reg <= 1'b0;
            pslverr_reg <= 1'b0;
            sram_ce_reg <= 1'b1;
            sram_we_reg <= 1'b1;
            sram_oe_reg <= 1'b1;
            sram_wdata_reg <= '0;
        end else begin
            pslverr_reg <= 1'b0;  // Clear PSLVERR
            case (state)
                IDLE: begin
                    pready_reg <= 1'b0;
                    pslverr_reg <= 1'b0;
                    sram_ce_reg <= 1'b1;
                    sram_we_reg <= 1'b1;
                    sram_oe_reg <= 1'b1;
                end
                SETUP: begin
                    sram_ce_reg <= 1'b0;
                    if (PADDR >= MEM_DEPTH) begin
                        pslverr_reg <= 1'b1;  // Address out of range
                        next_state <= IDLE;  // Transition to IDLE state
                    end else begin
                        sram_addr_reg <= PADDR;
                        if (PWRITE) begin
                            sram_wdata_reg <= PWDATA;
                            sram_we_reg <= 1'b0;
                            sram_oe_reg <= 1'b1;
                        end else begin
                            sram_we_reg <= 1'b1;
                            sram_oe_reg <= 1'b0;
                        end
                    end
                end
                ACCESS: begin
                    pready_reg <= 1'b1;
                    if (!PWRITE) begin
                        prdata_reg <= SRAM_RDATA;
                    end
                end
            endcase
        end
    end

endmodule
