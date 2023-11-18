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

#ifdef TRACE_FST
#define TRACECLASS VerilatedFstC
#include <verilated_fst_c.h>
#else // TRACE_FST
#define TRACECLASS VerilatedVcdC
#include <verilated_vcd_c.h>
#endif

#include <stdint.h>
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
    TRACECLASS *m_trace;
    bool m_changed;
    uint32_t time_ps;
    BaseTestbench();
    BaseTestbench(T *top_core, VerilatedVcdC *m_trace = NULL)
    {
        this->m_core = top_core;
        this->m_changed = false;
        this->m_trace = m_trace;
    }
    BaseTestbench(T *top_core, std::vector<TbClock> clocks, VerilatedVcdC *m_trace = NULL)
    {
        this->clocks = clocks;
        this->m_core = top_core;
        this->m_changed = false;
        this->m_trace = m_trace;
    }
    ~BaseTestbench();
    void add_clock(TbClock clock)
    {
        this->clocks.pushback(clock);
    }
    virtual void eval()
    {
        m_core->eval();
    }
    // Empty virtual method
    virtual void clk_assign()
    {
        return;
    };
    void tick()
    {
        uint32_t mintime = UINT32_MAX;

        for (int i = 0; i < (int)this->clocks.size(); i++)
        {
            if (clocks[i].time_to_tick() < mintime)
            {
                mintime = clocks[i].time_to_tick();
            }
        }
        assert(mintime > 1);
        this->eval();

        if (m_trace)
            m_trace->dump(time_ps + 1);

        this->clk_assign();

        time_ps += mintime;

        this->eval();
        if (m_trace)
        {
            m_trace->dump(time_ps + 1);
            m_trace->flush();
        }

        for (int i = 0; i < (int)this->clocks.size(); i++)
        {
            if (clocks[i].falling_edge())
            {
                clocks[i].changed_callback_falling();
            }
        }

        for (int i = 0; i < (int)this->clocks.size(); i++)
        {
            if (clocks[i].rising_edge())
            {
                clocks[i].changed_callback_rising();
            }
        }
    }
};

template <class T>
BaseTestbench<T>::BaseTestbench()
{
    // Initialize members in the constructor
    num_clocks = 0;
    m_core = nullptr;
    m_trace = nullptr;
    m_changed = false;
    time_ps = 0;
}
#endif //_BASE_TESTBENCH_H_