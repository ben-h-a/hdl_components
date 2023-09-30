//-----------------------------------------------------------------------------
//
// Filename:    testbench.h
//
// Project: hdl_components
//
// Purpose: To aide in creation of Verilator testbenches.
// this is a template class to be inherited from to create custom testbench
// classes
//
// Creator: Benjamin Allen
// Email: mail@bhallen.co.uk
//
//-----------------------------------------------------------------------------

#ifndef _BASE_TESTBENCH_H_
#define _BASE_TESTBENCH_H_
#include <verilated_vcd_c.h>
#include <vector>
#include "tbclock.h"

template <class T>
class BaseTestbench
{
private:
public:
    std::vector<TbClock> clocks;
    int num_clocks;
    T *m_core;
    VerilatedVcdC *m_trace;
    bool m_changed;
    unsigned long time_ps;
    BaseTestbench(T *top_core, VerilatedVcdC *m_trace = NULL);
    BaseTestbench(T *top_core, std::vector<TbClock> clocks, VerilatedVcdC *m_trace = NULL);
    ~BaseTestbench();
    void add_clock(TbClock clock);
    virtual void eval();
    // Empty virtual method
    // virtual void clk_assign();
    void tick();
};
#endif //_BASE_TESTBENCH_H_