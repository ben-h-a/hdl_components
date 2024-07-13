import cocotb
import asyncio
from math import log2, ceil
import random
from cocotb.clock import Clock
from cocotb.triggers import FallingEdge, RisingEdge, Timer, ClockCycles, Combine, Join
from cocotb.binary import BinaryValue
from cocotbext.sram.sram import Sram

"""
# TESTPLAN

## 1. simple_rw
Simple single read single write test at address boundary.

### pass
read data should match write data.



## 2. multi_rw
Consecutive RW to address.
### pass
read data should match write data

## 3. rw_rst
Simple rw test with reset asserted.

### pass
the dut should invalidate data inbetween resets.

## 4. rw_out_of_bounds
Simple rw, but outside the address bounds defined by the dut.

### pass
the dut should assert PSLVERR and driver will raise a APBTransactionError
"""


if cocotb.simulator.is_running():
    DATA_W = int(cocotb.top.DATA_WIDTH.value)
    WE_WIDTH = int(cocotb.top.WE_WIDTH.value)

    DEPTH = 2**int(cocotb.top.DEPTH.value)
    MAX_ADDRESS = (cocotb.top.DEPTH.value - 1)
    DATA_ALL_1 = (1<<DATA_W)-1

CLK_PERIOD_A = (10, "ns")
CLK_PERIOD_B = (15, "ns")


async def setup_dut(dut):
    cocotb.start_soon(Clock(dut.A_CLK, *CLK_PERIOD_A).start())
    cocotb.start_soon(Clock(dut.B_CLK, *CLK_PERIOD_B).start())
    dut.RST_N.value = 0
    await ClockCycles(dut.B_CLK, 2)
    dut.RST_N.value = 1
    await ClockCycles(dut.B_CLK, 2)

def comp_data(addr, wdata, rdata):
    if wdata == rdata:
        return True
    cocotb.logging.error("Addr: {}; wdata {} != rdata {}".format(addr, wdata, rdata))
    return False

@cocotb.test()
async def single_rw(dut):
    sram_a = Sram(dut, "A", dut.A_CLK)
    sram_b = Sram(dut, "B", dut.B_CLK)
    await setup_dut(dut)
    
    cocotb.logging.info("Write to base address")
    wdata = random.randint(0, 2**DATA_W)
    waddr = 0
    await sram_a.transact(waddr, wdata)
    rdata_a0 = await sram_a.transact(waddr)
    rdata_b0 = await sram_b.transact(waddr)
    assert comp_data(waddr, wdata, rdata_a0)
    assert comp_data(waddr, wdata, rdata_b0)

    cocotb.logging.info("Write to max address")
    wdata = random.randint(0, 2**DATA_W)
    waddr = MAX_ADDRESS
    await sram_a.transact(waddr, wdata)
    rdata_amax = await sram_a.transact(waddr)
    rdata_bmax = await sram_b.transact(waddr)
    assert comp_data(waddr, wdata, rdata_amax)
    assert comp_data(waddr, wdata, rdata_bmax)

    #Check for aliasing
    rdata = await sram_a.transact(0)
    assert comp_data(0, int(rdata_a0), int(rdata))

@cocotb.test()
async def sequential_rw(dut):
    sram_a = Sram(dut, "A", dut.A_CLK)
    sram_b = Sram(dut, "B", dut.B_CLK)
    await setup_dut(dut)
    wdata_lst = []

    for i in range(0,10):
        wdata = random.randint(0, 2**DATA_W)
        wdata_lst.append(wdata)
        waddr = i
        await sram_a.transact(waddr, wdata)
    for i in range(0,10):
        waddr = i
        rdata = await sram_b.transact(waddr)
        assert comp_data(waddr, wdata_lst[i], rdata)

    wdata_lst = []

    for i in range(0,10):
        wdata = random.randint(0, 2**DATA_W)
        wdata_lst.append(wdata)
        waddr = i
        await sram_b.transact(waddr, wdata)
    for i in range(0,10):
        waddr = i
        rdata = await sram_a.transact(waddr)
        assert comp_data(waddr, wdata_lst[i], rdata)

@cocotb.test()
async def byte_en(dut):
    sram_a = Sram(dut, "A", dut.A_CLK)
    sram_b = Sram(dut, "B", dut.B_CLK)
    await setup_dut(dut)
    wdata_lst = []

    await sram_a.transact(0, DATA_ALL_1)
    await sram_a.transact(0, 0, 1)
    rdata = await sram_a.transact(0)
    assert int(rdata) == DATA_ALL_1 - ((1<<8)-1)

    await sram_b.transact(0, DATA_ALL_1)
    await sram_b.transact(0, 0, 1)
    rdata = await sram_b.transact(0)
    assert int(rdata) == DATA_ALL_1 - ((1<<8)-1)

    await sram_a.transact(0, DATA_ALL_1)
    await sram_b.transact(0, 0, 1)
    rdata = await sram_b.transact(0)
    assert int(rdata) == DATA_ALL_1 - ((1<<8)-1)

    await sram_b.transact(0, DATA_ALL_1)
    await sram_a.transact(0, 0, 1)
    rdata = await sram_b.transact(0)
    assert int(rdata) == DATA_ALL_1 - ((1<<8)-1)

