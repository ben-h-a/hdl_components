/*-----------------------------------------------------------------------------
//
// Filename:    testbench.h
//
// Project: hdl_components
//
// Purpose: To aide in creation of clocks for Verilator testbenches
//
// Creator: Benjamin Allen
// Email: mail@bhallen.co.uk
//
-----------------------------------------------------------------------------*/

#include "tbclock.h"
#include <exception>
#include <stdexcept>
#include <iostream>


TbClock::TbClock() {}

TbClock::TbClock(
    unsigned int period_ps,
    std::function<void()> changed_callback_rising,
    std::function<void()> changed_callback_falling)
{
    m_increment_ps = period_ps;
    m_now_ps = 0;
    m_last_edge_ps = 0;
    this->changed_callback_rising = changed_callback_rising;
    this->changed_callback_falling = changed_callback_falling;
}
TbClock::~TbClock()
{
}

unsigned long TbClock::time_to_tick(void)
{
    if (m_now_ps < m_last_edge_ps)
    {
        throw std::runtime_error(std::string("time %d < last edge %d", m_now_ps, m_last_edge_ps));
    }
    return m_increment_ps - (m_now_ps - m_last_edge_ps);
}

int TbClock::advance(unsigned long itime)
{
    int clk = 0;
    m_now_ps += itime;
    // full period
    if (m_now_ps >= m_last_edge_ps + (2 * m_increment_ps))
    {
        clk = 1;
    }
    // half period
    else if (m_now_ps >= m_last_edge_ps + m_increment_ps)
    {
        clk = 0;
    }
    else
    {
        clk = 1;
    }
    m_last_edge_ps += m_increment_ps;
    return clk;
}

bool TbClock::rising_edge(void)
{
    if (m_now_ps == m_last_edge_ps)
    {
        return true;
    }
    return false;
}

bool TbClock::falling_edge(void)
{
    if (m_now_ps == m_last_edge_ps + m_increment_ps)
    {
        return true;
    }
    return false;
}
