import cocotb
import random
from math import log2
from cocotb.clock import Clock
from cocotb.triggers import FallingEdge, RisingEdge, Timer

if cocotb.simulator.is_running():
    DATA_W = int(cocotb.top.DATA_WIDTH.value)
    DEPTH = int(cocotb.top.DEPTH.value)


async def generate_clock_w(dut, period):
    """Generate clock pulses."""

    while 1:
        dut.CLK_W.value = 0
        await Timer(period/2, units="ns")
        dut.CLK_W.value = 1
        await Timer(period/2, units="ns")

async def generate_clock_r(dut, period):
    """Generate clock pulses."""

    while 1:
        dut.CLK_R.value = 0
        await Timer(period/2, units="ns")
        dut.CLK_R.value = 1
        await Timer(period/2, units="ns")


async def write_fifo(dut, data):
    if dut.FULL.value:
        await FallingEdge(dut.FULL)
    await RisingEdge(dut.CLK_W)
    dut.WEN.value = 1
    dut.W_DATA.value = data
    await RisingEdge(dut.CLK_W)
    dut.WEN.value = 0


async def read_fifo(dut):
    if dut.EMPTY.value:
        await FallingEdge(dut.EMPTY)
    await RisingEdge(dut.CLK_R)
    dut.REN.value = 1
    data = dut.R_DATA.value
    await RisingEdge(dut.CLK_R)
    dut.REN.value = 0
    return data



# @cocotb.test()
# async def fifo_single_rw(dut):

#     dut.RST_N.value = 0
#     cocotb.start_soon(Clock(dut.CLK_W, 10, units="ns").start())
#     cocotb.start_soon(Clock(dut.CLK_R, 15, units="ns").start())

#     await Timer(5, units="ns")  # wait a bit
#     dut.RST_N.value = 1

#     wdata_lst = []
#     rdata_lst = []
#     #-----------------
#     #test single write/read
#     #-----------------
#     assert dut.EMPTY.value == 1, f"EMPTY not asserted after {DEPTH} writes!"
#     wdata = random.randint(0, (2^DATA_W)-1)
#     await write_fifo(dut, wdata)
#     await RisingEdge(dut.CLK_R)
#     assert dut.EMPTY.value == 0, f"EMPTY not cleared after write!"


#     rdata = await read_fifo(dut)
#     await RisingEdge(dut.CLK_R)
#     assert dut.EMPTY.value == 1, f"EMPTY not asserted after read"
#     assert wdata == rdata, f"W data != R data: {wdata} != {rdata}"


@cocotb.test()
async def fifo_full_rw(dut):
    dut.RST_N.value = 0
    cocotb.start_soon(Clock(dut.CLK_W, 10, units="ns").start())
    cocotb.start_soon(Clock(dut.CLK_R, 15, units="ns").start())

    await Timer(5, units="ns")  # wait a bit
    dut.RST_N.value = 1

    wdata_lst = []
    rdata_lst = []
    #-----------------
    #Write to full
    #-----------------
    for cnt in range(DEPTH+1):
        wdata = random.randint(0, (2^DATA_W)-1)
        wdata_lst.append(wdata)
        
        await write_fifo(dut, wdata)
    # assert dut.FULL.value == 1, f"FULL not asserted after {DEPTH} writes!"

    rdata = await read_fifo(dut)
    rdata_lst.append(rdata)
    # assert dut.FULL.value == 0, f"FULL not cleared after write"
    # assert rdata == wdata_lst[0], f"R data != W data 0: {rdata} != {wdata_lst[0]}"

    #-----------------
    #Read to empty
    #-----------------
    for cnt in range(DEPTH-1):
        rdata = await read_fifo(dut)
        rdata_lst.append(rdata)
    await RisingEdge(dut.CLK_R)
    # assert dut.EMPTY.value == 1, f"EMPTY not asserted after read"
    # assert rdata_lst == wdata_lst, f"Read and write lists are not equal!"

# @cocotb.test()
# async def fifo_reset_test(dut):
#     dut.RST_N.value = 0
#     cocotb.start_soon(Clock(dut.CLK_W, 10, units="ns").start())
#     cocotb.start_soon(Clock(dut.CLK_R, 15, units="ns").start())

#     await Timer(5, units="ns")  # wait a bit
#     dut.RST_N.value = 1

#     await write_fifo(dut, 1)
#     await RisingEdge(dut.CLK_R)
#     await RisingEdge(dut.CLK_R)
#     assert dut.EMPTY.value == 0, "Not EMPTY after reset"

#     await Timer(5, units="ns")  # wait a bit
#     dut.RST_N.value = 0
#     await FallingEdge(dut.CLK_R)
#     dut.RST_N.value = 1

#     assert dut.EMPTY.value == 1, "Not EMPTY after reset"
