DEFAULT_INDENT = "    "
CLOCK_NAME = "CLK"
RST_NAME = "RST_N"

SRAM_W_DATA_PORT_NAME = "D"
SRAM_R_DATA_PORT_NAME = "W"
SRAM_ADDR_PORT_NAME = "ADDR"
SRAM_W_EN_PORT_NAME = "W_EN"

SRAM_MEM_NAME = "mem"

REG_WIDTH = 32

module_def_template = f"""
module axi4_lite_reg (
    input {CLOCK_NAME},
    input {RST_NAME},

    {{control_interface_ports}}

    {{reg_input_ports}}
    {{reg_output_ports}}
);
"""

axi4_lite_def_template = """

"""


axi4_lite_to_sram_template = """
axi4_lite_to_sram #(
    .AXI_LITE_ADDR_WIDTH({axi_addr_width}),
    .AXI_LITE_DATA_WIDTH({axi_data_width}),
    {axi_id}

    .SRAM_WIDTH({width}),
    .SRAM_DEPTH({depth}),
    .SRAM_STRB_WIDTH({width}),
) u_interface_to_sram (
    //AXI4 lite

    //SRAM


);
"""


###########################
# SRAM assignments
###########################

sram_registers_inst_template = f"""
sram_reg_block u_regs (
    .CLK({CLOCK_NAME}),
    .RST_N({RST_NAME}),
    
    .{SRAM_ADDR_PORT_NAME}({SRAM_ADDR_PORT_NAME}),
    .{SRAM_W_DATA_PORT_NAME}({SRAM_W_DATA_PORT_NAME}),
    .{SRAM_R_DATA_PORT_NAME}({SRAM_R_DATA_PORT_NAME}),
    .{SRAM_W_EN_PORT_NAME}({SRAM_W_EN_PORT_NAME}),

    {{reg_ports}}
);
"""

sram_registers_module_def_template = f"""
module sram_reg_block (
    {CLOCK_NAME},
    {RST_NAME},

    {{reg_ports}}

    input  {CLOCK_NAME},
    input  {RST_NAME},
    input  [{{sram_addr_width}}-1:0]{SRAM_ADDR_PORT_NAME},
    input  [{{sram_data_width}}-1:0]{SRAM_W_DATA_PORT_NAME},
    output [{{sram_data_width}}-1:0]{SRAM_R_DATA_PORT_NAME},
    input  {SRAM_W_EN_PORT_NAME}
);
    logic [{REG_WIDTH}-1:0] {SRAM_MEM_NAME} [{{reg_depth}}-1:0];

    //---------------
    // SRAM interface
    //---------------
always @(posedge CLK, negedge RST_N) begin
    if(!RST_N) begin
        {{sram_reg_rst_assignment}}
    //Write behavior
    end else if(W_EN) begin
        case (ADDR)
            {{sram_reg_write_behaviour}}

            default: 
                mem[ADDR] <= mem[ADDR];
        endcase
    //Read behaviour
    end else begin
        case (ADDR)
            {{sram_reg_read_behaviour}}
            default: 
                mem[ADDR] <= mem[ADDR];
        endcase
    end
end
    assign Q = mem[ADDR];

    

//---------------------
// Register assignments
//---------------------

//Output assignments
{{reg_port_output_assignment}}

//input assignments
always @(posedge CLOCK) begin
    if(RST_N) begin
        case (ADDR)
            {{reg_port_input_assignment}}
            default: 
                mem[ADDR] <= mem[ADDR];
        endcase
    end
end

endmodule
"""

#--------------------
#SRAM mem assignments
#--------------------

addr_case_statement_template = f"""
{{addr_width}}'h{{addr_offset}}: begin
    {{assignment}}
end
"""

sram_mem_assignment_template = f"""
{SRAM_MEM_NAME}[{{addr}}][{{index_end}}:{{index_start}}] <= {{assignment}};
"""

sram_mem_read_assignment_template = f"""

"""

sram_rw_w_assignment_template = f"""
if({SRAM_ADDR_PORT_NAME}=={{addr_offset}}&{SRAM_W_EN_PORT_NAME}) begin
    {SRAM_MEM_NAME}[{{addr_offset}}] = {SRAM_W_DATA_PORT_NAME};
end
"""

reg_port_rw_w_assignment_template = f"""
if({SRAM_ADDR_PORT_NAME}=={{addr_offset}}& !({SRAM_W_EN_PORT_NAME})) begin
    {SRAM_MEM_NAME}[{{addr_offset}}] = {SRAM_W_DATA_PORT_NAME};
end
"""

sram_r_assignment_template = f"""
if({SRAM_ADDR_PORT_NAME}=={{addr_offset}}) begin
    {SRAM_W_DATA_PORT_NAME} = {SRAM_MEM_NAME}[{{addr_offset}}];
end
"""

sram_r_assignment_wrapper_template = f"""
always_comb begin
    {{assignments}}
end
"""

sram_rclr_r_assignment_template = f"""
if({SRAM_ADDR_PORT_NAME}=={{addr_offset}}) begin
    {SRAM_MEM_NAME}[{{addr_offset}}] = '0;
end
"""


VALID_CTRL_INTERFACES = ["axi4_lite", "ahb"]
INTERFACE_TO_SRAM_MAP = {"axi4_lite":axi4_lite_to_sram_template}
INTERFACE_MODULE_INST_MAP = {"axi4_lite":axi4_lite_def_template}


