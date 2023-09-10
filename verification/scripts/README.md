# Verification
This is the root of the verification for the HDL design cells. The target simulator is Verilator. 

## Running the sim
From this directory run:
`make clean run TARGET=<target path>`

`TARGET=` is the desired target TB, this is the root path from $PROJECT_ROOT/verification/tb

e.g. the default is `TARGET=memories/sp_ram` this will run the TB for the sp_ram design cell, the full path to the TB will be $PROJECT_ROOT/verification/tb/sp_ram

This will:
1. include the `TARGET` makefile which will
    1. set ${VERILATOR_INPUT} with the required input files for the TB to run
    2. set ${VERILATOR_TARGET_NAME} with the resulting verilated exectuteable to run
    3. any other TB specific configuration of flag etc
2. Setup the default verilator flags
3. `run` makefile target will verilate the design if required then run the TB

## Creating new TB
Create a new directory under $PROJECT_ROOT/verification/tb/ then include the following:

### makefile
a template file for new testbenches is provided in this directory `./template_tb_makefile`. This has all the required variables for new testbenches. 

### tb.cpp
The tb.cpp is the verilator TB for the module.

### verilator_config.vlt
This file can be created for warning and error waivers for verilator



