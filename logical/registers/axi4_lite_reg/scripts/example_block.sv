module test #(
    parameter DATA_WIDTH = 8,
    parameter NUM_REG = 4,
    localparam _addr_width = $clog2(NUM_REG)
)(
    input CLK, 
    output RST_N,

    input [_addr_width-1:0] ADDR, 
    input [DATA_WIDTH-1:0]  D, 
    input                   W_EN,
    output [DATA_WIDTH-1:0] Q,

    input  [DATA_WIDTH-1:0]     rw_in_full_reg, //4'h0
    output [DATA_WIDTH-1:0]     rw_out_full_reg, //4'h2

    input  [DATA_WIDTH-1:0]     ro_in_full_reg, //4'h4

    input  [DATA_WIDTH-1:0]     rclr_in_full_reg, //4'h6

    input                      rw_in_0, //4'h8
    output                      rw_out_1,
    input                      ro_in_2,
    input                      rclr_in_3
);

logic [DATA_WIDTH-1:0] mem [NUM_REG-1:0];

//---------------
// SRAM interface
//---------------

always @(posedge CLK, negedge RST_N) begin
    if(!RST_N) begin
        mem[4'h0] = '0;
        mem[4'h2] = '1;
        mem[4'h4] = '0;
        mem[4'h6] = '1;
    //Write behavior
    end else if(W_EN) begin
        case (ADDR)
            4'h0: begin
                // mem[ADDR] <= D; //Input 
            end 
            4'h2: begin
                mem[ADDR] <= D;
            end 
            4'h6: begin //Clear on read
                mem[ADDR] <= '0;
            end 
            4'h8: begin //Clear on read
                mem[ADDR][0] <= D[0]; //RW
                mem[ADDR][1] <= D[1]; //RW
                mem[ADDR][2] <= mem[ADDR][2]; //RO
                mem[ADDR][3] <= mem[ADDR][3]; //RCLR
            end 

            default: 
                mem[ADDR] <= mem[ADDR];
        endcase
    //Read behaviour
    end else begin
        case (ADDR)
            4'h6: begin //Clear on read
                mem[ADDR] <= '0;
            end 
            4'h8: begin //Clear on read
                mem[ADDR][3] <= '0; //RCLR
            end 

            default: 
                mem[ADDR] <= mem[ADDR];
        endcase
    end
end

//Q output assignment
assign Q = mem[ADDR];



//---------------------
// Register assignments
//---------------------

//Output assignments
assign rw_out_full_reg = mem[8'h2];
assign rw_out_1 = mem[8'h8][1];

//input assignments
always @(posedge CLOCK) begin
    if(RST_N) begin
        case (ADDR)
            4'h0: begin
                mem[ADDR] <= rw_in_full_reg; //Input 
            end 
            4'h2: begin
                // mem[ADDR] <= D; //output
            end 
            4'h4: begin //Read only
                mem[ADDR] <= ro_in_full_reg;
            end 
            4'h6: begin //Clear on read
                mem[ADDR] <= rclr_in_full_reg;
            end 
            4'h8: begin //Clear on read
                mem[ADDR][0] <= rw_in_0;
                mem[ADDR][2] <= ro_in_2;
                mem[ADDR][3] <= rclr_in_3;
            end 

            default: 
                mem[ADDR] <= mem[ADDR];
        endcase
    end
end

endmodule