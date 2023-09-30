from __future__ import annotations
import os
import argparse
import re
from enum import Enum, auto
from math import ceil, log2
from json import JSONDecoder

from config_reg_templates import *
FILE_DIR = os.path.dirname(__file__)
TEMPLATES_DIR = os.path.join(FILE_DIR, "..", "templates")

class RegDef(object):
    _valid_read_behavior = {
        "RW": "Read write",
        "RO": "Read only",
        "RCLR": "Clear on read"
    }
    name:str
    width:int
    start_index:int
    end_index:int
    addr_offset:int
    reg_rw:str
    direction:str

    def __init__(self, name:str, width:int, addr_offset:int, reg_rw_behavior:str, direction:str, rst_val=0x0, start_index=0) -> None:
        if(name == ""):
            raise ValueError(f"Invalid port name: {name}")
        self.name = name

        if(width<=0):
            raise ValueError(f"Invalid port width: {width}, name: {name}")
        self.width = width

        if(start_index<0):
            raise ValueError(f"Invalid port start_index: {start_index}, name: {name}")
        self.start_index = start_index
        self.end_index = start_index+width

        if(reg_rw_behavior not in self._valid_read_behavior.keys()):
            raise ValueError(f"Invalid port rw behaviour: {reg_rw_behavior}, name: {name}")
        self.reg_rw = reg_rw_behavior

        if(addr_offset < 0):
            raise ValueError(f"Invalid port addr_offset: {addr_offset}, name: {name}")
        self.addr_offset = addr_offset

        if(direction not in ["input", "output"]):
            raise ValueError(f"Invalid port direction: {direction}, name: {name}")
        self.direction = direction

        if(rst_val < 0):
            raise ValueError(f"Invalid port rst_val: {rst_val}, name: {name}")
        self.rst_val = rst_val

    def get_vlog_port_def(self)->str:
        """
        Return the port definition for the register
        """
        dir_space = "  " if self.direction == "input" else ""
        return f"{self.direction}{dir_space}[{self.width-1}:0]{self.name}"
    def get_vlog_port_inst(self):
        return f".{self.name}({self.name})"

    def get_vlog_reg_output_assignment(self)->str:
        """
        output register assignment, return "" if not output

        Args: 
        addr_width[int]: address width to set address offset assignment to mem
        """
        if(self.direction == "output"):
            return f"assign {self.name} = {SRAM_MEM_NAME}[{self.addr_offset}][{self.end_index}:{self.start_index}];"
        return ""

    def get_vlog_reg_input_assignment(self, addr_width:int)->str:
        """
        input register assignment to sram

        Args: 
        addr_width[int]: address width to set address offset assignment to mem
        """
        if(self.direction == "inputs"):
            assignment = str.format(sram_mem_assignment_template,
                              addr=self.addr_offset,
                              index_end =self.end_index,
                              index_start = self.start_index,
                              assignment = self.name)

            case_statement = str.format(addr_case_statement_template,
                                        addr_width =addr_width,
                                        addr_offset = self.addr_offset,
                                        assignment = assignment)
            return case_statement
        return ""

    def get_vlog_mem_w_assignment(self, addr_width:int):
        """
        Return memory write assignment. D index value if RW else mem index value

        Args: 
        addr_width[int]: address width to set address offset assignment to mem
        """
        if(self.reg_rw == "RW"):
            mem_assign = f"D[{self.end_index}:{self.start_index}]"
        else:
            mem_assign = f"{SRAM_MEM_NAME}[{self.addr_offset}][{self.end_index}:{self.start_index}]"

        sram_assign = str.format(sram_mem_assignment_template,
                            addr=self.addr_offset,
                            index_end =self.end_index,
                            index_start = self.start_index,
                            assignment = mem_assign)

        case_statement = str.format(addr_case_statement_template,
                                    addr_width =addr_width,
                                    addr_offset = self.addr_offset,
                                    assignment = sram_assign)
        return case_statement

    def get_vlog_mem_r_assignment(self, addr_width:int):
        """
        Return sram read assignment, unchanged if not RCLR else '0

        Args: 
        addr_width[int]: address width to set address offset assignment to mem
        """
        if(self.reg_rw != "RCLR"):
            return ""

        sram_assign = str.format(sram_mem_assignment_template,
                            addr=self.addr_offset,
                            index_end =self.end_index,
                            index_start = self.start_index,
                            assignment = "'0")

        case_statement = str.format(addr_case_statement_template,
                                    addr_width =addr_width,
                                    addr_offset = self.addr_offset,
                                    assignment = sram_assign)
        return case_statement
    
    def get_vlog_mem_rst_assignment(self, addr_width:int):
        """
        Return memory reset assignment for register,

        Args: 
        addr_width[int]: address width to set address offset assignment to mem
        """
        return f"{SRAM_MEM_NAME}[{addr_width}'d{self.addr_offset}][{self.end_index}:{self.start_index}];"

    def get_vlog_sram_w_assignment(self)->str:
        """
        Return the SRAM write assignment string for verilog

        Behaviour:
        RW: assign to mem address if mem address == addr_offset
        else: do not assign to address
        """
        if(self.reg_rw == "RW"):
            return str.format(sram_rw_w_assignment_template, addr_offset = self.addr_offset)
        else:
            return f"//addr_offset {self.addr_offset} is {self._valid_read_behavior[self.reg_rw]}"

    def get_vlog_sram_r_assignment(self)->str:
        """
        Return the SRAM read assignment string for verilog

        Behaviour:
        RCLK: on a read to this address clear the register
        else: no other behaviour required, output data assignemts
        is made with a continuous assignment.
        """
        if(self.reg_rw == "RCLR"):
            return str.format(sram_rclr_r_assignment_template, addr_offset = self.addr_offset)
        return ""

    def get_vlog_sram_r_data_continuous_assignmet(self) -> str:
        """
        Assignment to sram r data port. this should be made inside an
        'always_comb' block.
        """
        return str.format(sram_r_assignment_template, addr_offset = self.addr_offset)

    def __eq__(self, other:RegDef):
        """
        Check if one register port overlaps with another.
        Checks register name, return True if match

        Checks register index values for overlap of the address offset
        is the same. if a index in b index or b index in a index return True
        """
        def overlap(a:RegDef, b:RegDef):
            #b start in a
            if(a.start_index<=b.start_index and a.end_index>=b.start_index):
                return True
            #b end in a
            if(a.start_index<=b.end_index and a.end_index>=b.end_index):
                return True
            return False
    
        if(self.name == other.name):
            return True
        if(self.addr_offset == other.addr_offset):
            if(overlap(self, other)):
                return True
            if(overlap(other, self)):
                return True
        return False

    def __str__(self) -> str:
        return f"name:{self.name}, direction:{self.direction}, width:{self.width}, addr_offset:{self.addr_offset}, RW:{self.reg_rw}, rst_val{self.rst_val}"


class RegConfig(object):
    """
    Register configuration object
    """
    _valid_ctrl_interfaces_str = ", ".join(VALID_CTRL_INTERFACES)

    regs:list[RegDef]
    ctrl_interface:str

    ctrl_addr_width:int
    ctrl_data_width:int

    sram_depth:int
    sram_addr_width:int

    output_dir:str
    top_name:str

    def __init__(self, config:dict) -> None:
        """
        RegConfig initialisation.

        Exceptions: 
        Will raise ValueError on invalid configuration
        """
        self.parse_config(config)
        invalid_regs = self.check_regs()
        if(invalid_regs):
            msg = f"The following pairs of regs are invalid\n"
            for a, b in invalid_regs:
                msg += str(a) + " == " + str(b)

            raise ValueError(msg)

    def parse_port(self, config:dict)->RegDef:
        name = config.get("name")
        width = int(config.get("width"))
        reg_rw = config.get("rw")
        addr_offset = config.get("addr_offset")
        rst_val = config.get("rst_val", 0x0)
        start_index = config.get("start_index", 0x0)
        direction = config.get("direction")
        return RegDef(name, width, addr_offset,
                      reg_rw, direction,rst_val,
                      start_index)

    def check_regs(self)->list[tuple[RegDef, RegDef]]:
        invalid_regs = []
        for reg_a in self.regs:
            for reg_b in self.regs:
                if(reg_a == reg_b):
                    invalid_regs.append((reg_a, reg_b))
        return invalid_regs

    def parse_config(self, config:dict):
        """
        Parse configuration dictionary

        Exceptions:
        Will raise ValueError on invalid configuration values
        """
        for port in config.get("ports"):
            self.regs.append(self.parse_port(port))

        self.ctrl_interface = config.get("control_interface", "axi4_lite")

        self.output_dir = config.get("output_dir", "./output")
        self.top_name = config.get("output_name", "reg_block")

        if(self.ctrl_interface not in VALID_CTRL_INTERFACES):
            raise ValueError(f"Invalid interface port {self.ctrl_interface}, valid interfaces are: {self._valid_ctrl_interfaces_str}")

        self.ctrl_addr_width = int(config.get("ctrl_addr_width"))
        self.ctrl_data_width = int(config.get("ctrl_data_width"))

        self.ctrl_data_width = int(config.get("ctrl_data_width"))

        self.sram_depth = len(self.regs)
        if(self.sram_depth == 0):
            raise ValueError("No input or output register ports ")
        self.sram_addr_width = ceil(log2(self.sram_depth))

    def _get_sram_reg_block(self)->str:
        ports_def = ",\n".join(list(map(lambda a: a.get_vlog_port_def(),self.regs)))
        rst_assignment = "\n".join(list(
            map(lambda a: 
                a.get_vlog_mem_rst_assignment(self.ctrl_addr_width),
                self.regs)))

        sram_reg_write_behaviour = "\n".join(list(
            map(lambda a: 
                a.get_vlog_mem_w_assignment(self.ctrl_addr_width),
                self.regs)))
        reg_port_output_assignment = "\n".join(list(
            map(lambda a: 
                a.get_vlog_reg_output_assignment(),
                self.regs)))

        reg_port_input_assignment = "\n".join(list(
            map(lambda a: 
                a.get_vlog_reg_input_assignment(self.ctrl_addr_width),
                self.regs)))

        module = sram_registers_module_def_template.format(
            input_reg_ports = ports_def,
            sram_addr_width = self.sram_addr_width, 
            sram_data_width = self.ctrl_data_width,
            reg_depth = self.sram_depth,
            sram_reg_rst_assignment = rst_assignment,
            sram_reg_write_behaviour = sram_reg_write_behaviour,
            reg_port_output_assignment = reg_port_output_assignment,
            reg_port_input_assignment = reg_port_input_assignment
        )
        return module
    def _get_sram_reg_block_inst(self)->str:
        reg_ports = ",\n".join(list(
            map(lambda a: 
                a.get_vlog_port_inst(),
                self.regs)))

        inst = sram_registers_inst_template.format(
            reg_ports = reg_ports
        )
        return inst
    def _get_reg_block_module(self)->str:
        
    def gen_files(self):













