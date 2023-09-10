#ifndef _TESTBENCH_H_
#define _TESTBENCH_H_
    #include "tbclock.h"
    #include "verilated.h"
    #include <verilated_vcd_c.h>
    
    
    template <class T>
    class TESTBENCH {
    public:
        unsigned long time_ps;
        T * m_core;
        TBCLOCK * clocks;
        int num_clocks;
        VerilatedVcdC* m_trace;
        bool m_changed;
        TESTBENCH(T *top_core, TBCLOCK * clocks, 
        int num_clocks, VerilatedVcdC * m_trace=NULL) {
            // Constructor code, if needed
            this->num_clocks = num_clocks;
            this->clocks = clocks;
            this->m_core = top_core; 
            this->m_changed = false;
            this->m_trace = m_trace;
        }

        ~TESTBENCH() {
            // Destructor code, if needed            
        }

        virtual void eval(){
            m_core->eval();
        }

        // Empty virtual method
        virtual void clk_assign() {
            // This method can be overridden by derived classes
            // It takes no arguments and has an empty implementation here
        }

        void tick() {
            uint32_t mintime = UINT32_MAX;

            for(int i=0; i<num_clks; i++){
                if(this->clocks[i].time_to_tick() < mintime){
                    mintime = this->clocks[i].time_to_tick();
                }
            }
            assert(mintime > 1);
            this->eval();

            if (m_trace) m_trace->dump(m_time_ps+1);

            this->clk_assign();
            
            m_time_ps += mintime;

            this->eval();
            if (m_trace) {
                m_trace->dump(m_time_ps+1);
                m_trace->flush();
            }

            for(int i=0; i<num_clks; i++){
                if(this->clocks[i].falling_edge()){
                    this->m_changed = true;
                }
            }

            if (m_clk.falling_edge()) {
                m_changed = true;
                sim_clk_tick();
            }
            if (m_hdmi_out_clk.falling_edge()) {
                m_changed = true;
                sim_hdmi_out_clk_tick();
            }
            if (m_hdmi_in_clk.falling_edge()) {
                m_changed = true;
                sim_hdmi_in_clk_tick();
            }
            if (m_hdmi_in_hsclk.falling_edge()) {
                m_changed = true;
                sim_hdmi_in_hsclk_tick();
            }
            if (m_clk_200mhz.falling_edge()) {
                m_changed = true;
                sim_clk_200mhz_tick();
            }    
        }
    };
#endif //_TESTBENCH_H_