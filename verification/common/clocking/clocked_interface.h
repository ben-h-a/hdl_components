#ifndef _CLOCKED_INTERFACE_H_
#define _CLOCKED_INTERFACE_H_

#include "tbclock.h"
#include <verilated_vcd_c.h>

template <class T>
class ClockedInterface
{
public:
    TbClock clock;
    T *m_core;
    VerilatedVcdC *m_trace;

    ClockedInterface(T *v_module, TbClock *clock);
    ~ClockedInterface();

    virtual void eval();
    virtual void clk_assign_rising();
    virtual void clk_assign_falling();
    void tick();
};

#endif