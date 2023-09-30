module sram_reg (
    input           CLK,
    input           RST_N,

    input [{addr_width}-1:0]    ADDR,
    input [{data_width}-1:0]    W_DATA,
    input [{data_width}-1:0]    R_DATA,
    input                       WEN,

    {input_regs}
    {output_regs}
)

logic [{reg_width}-1:0] mem [{num_regs}-1:0];


//sram interface assignment
always @(posedge CLK, negedge RST_N) begin
    if(!RST_N) begin
        {mem_defaults}
    end else begin
        if(W_EN) begin
            mem[ADDR] <= W_DATA;
        end
    end
end

//Output assignments
{output_assignments}

//Input assignments
{input_assignments}

endmodule