//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------

#ifndef _TBCLOCK_H_
#define _TBCLOCK_H_
#include <functional>
class TbClock
{
    unsigned long m_increment_ps;
    unsigned long m_now_ps;
    unsigned long m_last_edge_ps;
    std::function<void()> changed_callback_rising;
    std::function<void()> changed_callback_falling;

public:
    TbClock(
        unsigned int period_ps,
        std::function<void()> changed_callback_rising = []() {},
        std::function<void()> changed_callback_falling = []() {});
    ~TbClock();
    unsigned long time_to_tick(void);
    void set_interval_ps(unsigned long interval_ps);
    int advance(unsigned long itime);
    bool rising_edge(void);
    bool falling_edge(void);
};

#endif //_TBCLOCK_H_
