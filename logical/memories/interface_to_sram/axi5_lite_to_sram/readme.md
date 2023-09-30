# AXI5 to SRAM
This block is to support accessing SRAM blocks via AXI5-lite. 

## Features
1. single write, single read
2. Bursts supported
3. Write strobes supported
4. Out of order accesses not supported at the moment


## Clocking
The AXI bus and SRAM interface must be on the same clock domain

## Resets
A single reset is provided active high
