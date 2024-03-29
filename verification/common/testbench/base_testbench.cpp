/*-----------------------------------------------------------------------------
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
-----------------------------------------------------------------------------*/
#include <stdint.h>
#include <vector>
#include "base_testbench.h"

template <class T>
BaseTestbench<T>::BaseTestbench(T *top_core, VerilatedVcdC *m_trace = NULL)
{
    this->m_core = top_core;
    this->m_changed = false;
    this->m_trace = m_trace;
}

template <class T>
BaseTestbench<T>::BaseTestbench(T *top_core, std::vector<TbClock> clocks, VerilatedVcdC *m_trace = NULL)
{
    this->clocks = clocks;
    this->m_core = top_core;
    this->m_changed = false;
    this->m_trace = m_trace;
}

template <class T>
BaseTestbench<T>::~BaseTestbench()
{
}

template <class T>
void BaseTestbench<T>::add_clock(TbClock clock)
{
    this->clocks.pushback(clock);
}

template <class T>
void BaseTestbench<T>::eval()
{
    m_core->eval();
}

// Empty virtual method
// template <class T>
// void BaseTestbench<T>::clk_assign()
// {
//     // This method can be overridden by derived classes
//     // It takes no arguments and has an empty implementation here
// }

template <class T>
void BaseTestbench<T>::tick()
{
    uint32_t mintime = UINT32_MAX;

    for (int i = 0; i < this->clocks.size(); i++)
    {
        if (this->clocks[i].time_to_tick() < mintime)
        {
            mintime = this->clocks[i].time_to_tick();
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

    for (int i = 0; i < this->clocks.size(); i++)
    {
        if (this->clocks[i].falling_edge())
        {
            if (this->clocks[i]->changed_callback_falling() != nullptr)
            {
                this->clocks[i]->changed_callback_falling();
            }
        }
    }

    for (int i = 0; i < this->clocks.size(); i++)
    {
        if (this->clocks[i].rising_edge())
        {
            if (this->clocks[i]->changed_callback_rising() != nullptr)
            {
                this->clocks[i]->changed_callback_rising();
            }
        }
    }
}
