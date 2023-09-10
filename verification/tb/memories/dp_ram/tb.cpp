#include "Vdp_ram.h"
#include "tbclock.h"
#include "verilated.h"
#include <verilated_vcd_c.h>


class TESTBENCH {
    public:
    unsigned long   m_time_ps;
    Vdp_ram  *m_core;
    TBCLOCK clk_a;
    TBCLOCK clk_b;
    VerilatedVcdC* m_trace;


    TESTBENCH(void) {
        m_core = new Vdp_ram;
        m_time_ps = 0l;
        m_core->CLK_A = 0;
        m_core->CLK_B = 0;
        m_core->RST_N = 0;
        m_trace = NULL;
        clk_a.init(10000); //100Mhz
        clk_b.init(10000); //100Mhz
    }

    ~TESTBENCH(void) {
        delete m_core;
        m_core = NULL;
        clk_a = NULL;
        clk_b = NULL;
    }

    void    reset(void) {
        m_core->RST_N = 0;
        // Make sure any inheritance gets applied
        this->tick();
        m_core->RST_N = 1;
    }

    void write_a(uint32_t * data, uint32_t * addr, uint32_t len){
        m_core->W_EN_A=0xF;
        for(uint32_t i=0; i<len; i++){
            m_core->ADDR_A = *(addr+i);
            m_core->W_DATA_A = *(data+i);
            this->tick();
        }
        m_core->W_EN_A=0;
    }

    void read_a(uint32_t * data, uint32_t * addr, uint32_t len){
        m_core->W_EN_A=0;
            uint32_t * data_ptr;
        for(uint32_t i=0; i<len; i++){
            data_ptr = data+i;
            m_core->ADDR_A = *(addr+i);
            *data_ptr = m_core->R_DATA_A;
            this->tick();
        }
    }

    void write_b(uint32_t * data, uint32_t * addr, uint32_t len){
        m_core->W_EN_B=0xF;
        for(uint32_t i=0; i<len; i++){
            m_core->ADDR_B = *(addr+i);
            m_core->W_DATA_B = *(data+i);
            this->tick();
        }
        m_core->W_EN_B=0;
    }

    void read_b(uint32_t * data, uint32_t * addr, uint32_t len){
        m_core->W_EN_B=0;
            uint32_t * data_ptr;
        for(uint32_t i=0; i<len; i++){
            data_ptr = data+i;
            m_core->ADDR_B = *(addr+i);
            *data_ptr = m_core->R_DATA_B;
            this->tick();
        }
    }


    void tick(TBCLOCK * clocks, int num_clks) {
        uint32_t mintime = UINT32_MAX;

        for(int i=0; i<num_clks; i++){
            if(clocks[i].time_to_tick() < mintime){
                mintime = clocks[i].time_to_tick();
            }
        }
        
        assert(mintime > 1);

        m_core->eval();
        if (m_trace) m_trace->dump(m_time_ps+1);

        m_core->i_clk = m_clk.advance(mintime);
        m_core->i_hdmi_out_clk = m_hdmi_out_clk.advance(mintime);
        m_core->i_hdmi_in_clk = m_hdmi_in_clk.advance(mintime);
        m_core->i_hdmi_in_hsclk = m_hdmi_in_hsclk.advance(mintime);
        m_core->i_clk_200mhz = m_clk_200mhz.advance(mintime);

        m_time_ps += mintime;

        m_core->eval();
        if (m_trace) {
            m_trace->dump(m_time_ps+1);
            m_trace->flush();
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

    bool    done(void) { return (Verilated::gotFinish()); }
};

int main(int argc, char** argv, char** env) {
    // This example started with the Verilator example files.
    // Please see those examples for commented sources, here:
    // https://github.com/verilator/verilator/tree/master/examples

    if (0 && argc && argv && env) {}

    Verilated::debug(0);
    Verilated::randReset(2);
    Verilated::traceEverOn(true);
    Verilated::commandArgs(argc, argv);
    Verilated::mkdir("logs");

    TESTBENCH * tb = new TESTBENCH();
    uint32_t data;
    uint32_t r_data;
    uint32_t w_data_comp;
    int error = 0;
    tb->reset();
    for(uint32_t i=0;i<5;i++){
        data = std::rand();
        tb->write_a(&data, &i, 1);
        tb->read_b(&r_data, &i, 1);
        w_data_comp = data & 0xFF;
        if(w_data_comp != r_data){
            printf("ERROR addr %x: w_data %x != r_data %x\n", 
                    i, w_data_comp, r_data);
            error = 1;
        }
    }
    for(uint32_t i=0;i<5;i++){
        data = std::rand();
        tb->write_b(&data, &i, 1);
        tb->read_a(&r_data, &i, 1);
        w_data_comp = data & 0xFF;
        if(w_data_comp != r_data){
            printf("ERROR addr %x: w_data %x != r_data %x\n", 
                    i, w_data_comp, r_data);
            error = 1;
        }
    }

    printf("DONE\n");
    if(error){
        printf("FAIL\n");
    } else {
        printf("PASS\n");
    }

    delete tb;
    tb = NULL;
    exit(0);

}
