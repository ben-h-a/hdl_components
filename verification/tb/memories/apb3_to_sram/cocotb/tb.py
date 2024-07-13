import cocotb
import asyncio
from math import log2, ceil
import random
from cocotb.clock import Clock
from cocotb.triggers import FallingEdge, RisingEdge, Timer, ClockCycles, Combine, Join
from cocotb.binary import BinaryValue
from cocotbext.amba_bus.apb import APB, APBTransactionError

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
    WE_WIDTH = ceil(log2(DATA_W/8))

    DEPTH = 2**int(cocotb.top.DEPTH.value)
    MAX_ADDRESS = (cocotb.top.DEPTH.value - 1)<<WE_WIDTH

CLK_PERIOD = (10, "ns")
APB_BUS_PREFIX = ""


async def setup_dut(dut):
    cocotb.start_soon(Clock(dut.CLK, *CLK_PERIOD).start())
    dut.RST_N.value = 0
    await ClockCycles(dut.CLK, 2)
    dut.RST_N.value = 1
    await ClockCycles(dut.CLK, 2)

def comp_data(addr, wdata, rdata):
    if wdata == rdata:
        return True
    cocotb.logging.error("Addr: {}; wdata {} != rdata {}".format(addr, wdata, rdata))
    return False

@cocotb.test()
async def single_rw(dut):
    apb_m = APB(dut, APB_BUS_PREFIX, dut.CLK)
    await setup_dut(dut)
    
    cocotb.logging.info("Write to base address")
    wdata = random.randint(0, 2**DATA_W)
    waddr = 0
    await apb_m.transact(waddr, wdata)
    rdata = await apb_m.transact(waddr)
    assert comp_data(waddr, wdata, rdata)

    cocotb.logging.info("Write to max address")
    wdata = random.randint(0, 2**DATA_W)
    waddr = MAX_ADDRESS
    await apb_m.transact(waddr, wdata)
    rdata = await apb_m.transact(waddr)
    assert comp_data(waddr, wdata, rdata)

@cocotb.test()
async def sequential_rw(dut):
    apb_m = APB(dut, APB_BUS_PREFIX, dut.CLK)
    wdata_lst = []
    await setup_dut(dut)
    for i in range(0,10):
        wdata = random.randint(0, 2**DATA_W)
        wdata_lst.append(wdata)
        waddr = 0x4*i
        await apb_m.transact(waddr, wdata)
    for i in range(0,10):
        waddr = 0x4*i
        rdata = await apb_m.transact(waddr)
        assert comp_data(waddr, wdata_lst[i], rdata)

@cocotb.test()
async def oob_mem_access(dut):
    perr_raised = 0
    apb_m = APB(dut, APB_BUS_PREFIX, dut.CLK)
    await setup_dut(dut)
    cocotb.logging.info("Write to oob address")
    wdata = random.randint(0, 2**DATA_W)
    waddr = MAX_ADDRESS + 0x4
    try:
        rdata =  await apb_m.transact(waddr)
    except:
        perr_raised = True

    assert perr_raised

