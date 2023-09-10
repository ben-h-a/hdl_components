module bin_to_grey #(
    parameter WIDTH = 8,
    parameter ENDIAN = "BIG"
    ) (
    input   [WIDTH-1:0] BIN,
    output  [WIDTH-1:0] GREY
);
    generate
        if(ENDIAN=="BIG")
            assign GREY = BIN ^ (BIN >> 1);
        else if  (ENDIAN == "LITTLE")
            assign GREY = BIN ^ (BIN << 1);
        else 
            $error("Invalid value %s of parameter 'ENDIAN' passed to module %m. heir path: %s",
            ENDIAN, $root);
    endgenerate

endmodule