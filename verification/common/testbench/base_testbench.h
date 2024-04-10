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
    std::vector<TbClock*> clocks;
    int num_clocks;
    T *m_core;
    TRACECLASS *m_trace;
    bool m_changed;
    uint32_t time_ps;
    bool pause_trace;
    BaseTestbench();
    BaseTestbench(T *top_core, const char * vcd_name)
    {
        VerilatedVcdC* tfp = new VerilatedVcdC;
        Verilated::traceEverOn(true);
        this->m_trace = tfp;

        this->m_core = top_core;
        this->m_core->trace(m_trace, 99);
        this->m_changed = false;
        pause_trace = false;
        time_ps = 0;
        tfp->open(  "dump.vcd");
    }
    BaseTestbench(T *top_core, std::vector<TbClock> clocks, VerilatedVcdC *m_trace = NULL)
    {
        this->clocks = clocks;
        this->m_core = top_core;
        this->m_changed = false;
        this->m_trace = m_trace;
        pause_trace = false;
        time_ps = 0;
    }
    ~BaseTestbench();
    void add_clock(TbClock &clock)
    {
        clocks.push_back(&clock);
    }

    virtual void opentrace(const char *vcdname, int depth = 99)
    {
        if (!m_trace)
        {
            m_trace = new TRACECLASS;
            m_core->trace(m_trace, 99);
            m_trace->spTrace()->set_time_resolution("ps");
            m_trace->spTrace()->set_time_unit("ps");
            m_trace->open(vcdname);
            pause_trace = false;
        }
    }

    virtual void closetrace(void)
    {
        if (m_trace)
        {
            m_trace->close();
            delete m_trace;
            m_trace = NULL;
        }
    }

    void trace(const char *vcdname)
    {
        opentrace(vcdname);
    }
    virtual void eval()
    {
        m_core->eval();
    }
    // Empty virtual method
    virtual void clk_assign(uint32_t itime)
    {
        return;
    };
    void tick()
    {
        uint32_t mintime = UINT32_MAX;

        for (int i = 0; i < (int)clocks.size(); i++)
        {
            uint32_t time_to_tick = clocks[i]->time_to_tick();
            if (time_to_tick < mintime)
            {
                mintime = time_to_tick;
            }
        }
        assert(mintime > 1);
        eval();

        // if (m_trace){
        //     m_trace->dump(time_ps);
        // }
        if (m_trace)
        {
            m_trace->dump(time_ps*1000);
            m_trace->flush();
        }
        for (int i = 0; i < (int)clocks.size(); i++)
        {
            clocks[i]->advance(mintime);
            
        }
        clk_assign(mintime);

        time_ps += mintime;

        eval();

        for (int i = 0; i < (int)clocks.size(); i++)
        {
            if (clocks[i]->falling_edge())
            {
                clocks[i]->changed_callback_falling();
            }
        }
        for (int i = 0; i < (int)clocks.size(); i++)
        {
            if (clocks[i]->rising_edge())
            {
                clocks[i]->changed_callback_rising();
            }
        }
        eval();
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
template <class T>
BaseTestbench<T>::~BaseTestbench()
{
    if(m_trace){
        m_trace->close();
        delete m_trace;
    }
}
#endif //_BASE_TESTBENCH_H_