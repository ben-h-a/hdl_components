#include "tbclock.h"
#include "base_testbench.h"
#include "Vdp_ram.h"
#include <verilated.h>
#include <verilated_vcd_c.h>
#include <functional>

#define A_ADDR_MASK 0xF
#define B_ADDR_MASK 0xF

#define A_DATA_MASK 0xFF
#define B_DATA_MASK 0xFF

#define A_WEN_MASK 0x1
#define B_WEN_MASK 0x1

typedef struct
{
    uint32_t w_data;
    uint32_t r_data;
    uint32_t address;
    uint32_t w_byte_enable;
} SramData;

class DpSramTestbench : public BaseTestbench<Vdp_ram>
{
private:

    TbClock clk_a;
    TbClock clk_b;

public:
    std::vector<SramData> a_data_vec;
    std::vector<SramData> b_data_vec;
    std::vector<SramData> a_data_complete_vec;
    std::vector<SramData> b_data_complete_vec;
    // Default constructor
    DpSramTestbench() {}

    // Destructor
    ~DpSramTestbench() {}
    DpSramTestbench(Vdp_ram *top_core,
                    unsigned int a_clk_period,
                    unsigned int b_clk_period,
                    const char * vcd_name)
        : BaseTestbench(top_core, vcd_name)
    {
        auto clk_a_rise_lambda = [this]()
        {
            this->assign_a_clk_rise();
        };
        auto clk_a_fall_lambda = [this]()
        {
            return;
        };
        auto clk_b_rise_lambda = [this]()
        {
            this->assign_b_clk_rise();
        };
        auto clk_b_fall_lambda = [this]()
        {
            return;
        };
        m_core = top_core;
        clk_a = TbClock(a_clk_period, clk_a_rise_lambda, clk_a_fall_lambda);
        clk_b = TbClock(b_clk_period, clk_b_rise_lambda, clk_b_fall_lambda);
        add_clock(clk_a);
        add_clock(clk_b);
    }

    void assign_a_clk_rise()
    {
        SramData data;
        // Write assignment
        if(!a_data_vec.empty()) {
            data = a_data_vec.back();

            this->m_core->W_EN_A = data.w_byte_enable & A_WEN_MASK;
            this->m_core->ADDR_A = data.address & A_ADDR_MASK;
            this->m_core->W_DATA_A = data.w_data & A_DATA_MASK;

            eval();
            data.r_data = this->m_core->R_DATA_A;
            a_data_complete_vec.push_back(data);
            a_data_vec.pop_back();
        } else {
            this->m_core->W_EN_A = 0;
            this->m_core->ADDR_A = this->m_core->ADDR_A;
        }
    }

    void reset(void)
    {
        m_core->RST_N = 0;
        // Make sure any inheritance gets applied
        this->tick();
        m_core->RST_N = 1;
    }


    void assign_b_clk_rise()
    {
        SramData data;
        // Write assignment
        if(!b_data_vec.empty()) {
            data = b_data_vec.back();

            this->m_core->W_EN_B = data.w_byte_enable & B_WEN_MASK;
            this->m_core->ADDR_B = data.address & B_ADDR_MASK;
            this->m_core->W_DATA_B = data.w_data & B_DATA_MASK;
            
            eval();
            data.r_data = this->m_core->R_DATA_B;

            b_data_complete_vec.push_back(data);
            b_data_vec.pop_back();
        } else {
            this->m_core->W_EN_B = 0;
            this->m_core->ADDR_B = this->m_core->ADDR_B;

        }
    }

    void trans_a(SramData data)
    {
        a_data_vec.push_back(data);
    }
    void trans_b(SramData data)
    {
        b_data_vec.push_back(data);
    }
    void clk_assign(uint32_t itime)
    {
        m_core->CLK_A = clk_a.clk;
        m_core->CLK_B = clk_b.clk;
    }
};

bool comp_data(SramData w, SramData r){
    if(w.w_data != r.r_data){
        printf("W addr = 0x%x data = 0x%x != R addr = 0x%x data = 0x%x\n", w.address, w.w_data, r.address, r.r_data);
        return true;
    }
    return false;
}

int main(int argc, char **argv, char **env)
{
    // This example started with the Verilator example files.
    // Please see those examples for commented sources, here:
    // https://github.com/verilator/verilator/tree/master/examples

    Verilated::debug(0);
    Verilated::randReset(2);
    Verilated::commandArgs(argc, argv);

    Vdp_ram *top = new Vdp_ram;
    DpSramTestbench *tb = new DpSramTestbench(top, 1000, 1100, "dump.vcd");
#ifdef VCD_FILE
    tb->trace(VCD_FILE);
#endif
    SramData data_a;
    SramData data_b;
    uint32_t w_data_comp;
    int error = 0;

    tb->reset();
    for(int i=0; i<10; i++){
        tb->tick();
    }
    printf("Rst released...\n");
    for (uint32_t i = 0; i < 5; i++)
    {
        data_a.address = i;
        data_a.w_data = std::rand() & A_DATA_MASK;
        data_a.w_byte_enable = 0x1;

        tb->trans_a(data_a);
    }
    printf("Start transmit w:a\n");
    while (!tb->a_data_vec.empty())
    {
        tb->tick();
    }
    for (uint32_t i = 0; i < 5; i++)
    {
        data_b.address = i & A_ADDR_MASK;
        data_b.w_byte_enable = 0x0;

        tb->trans_b(data_b);
    }
    printf("Start r:b\n");
    while ( !tb->b_data_vec.empty())
    {
        tb->tick();
    }
    while (!tb->a_data_complete_vec.empty() & !tb->b_data_complete_vec.empty())
    {
        data_a = tb->a_data_complete_vec.back();
        data_b = tb->b_data_complete_vec.back();
        if(comp_data(data_a, data_b)){
            error++;
        }
        tb->a_data_complete_vec.pop_back();
        tb->b_data_complete_vec.pop_back();
    }
    for (uint32_t i = 0; i < 5; i++)
    {
        data_b.address = i & B_ADDR_MASK;
        data_b.w_data = std::rand() & B_DATA_MASK;
        data_b.w_byte_enable = 0x1;

        tb->trans_b(data_b);
    }
    printf("Start transmit w:b\n");
    while (!tb->b_data_vec.empty())
    {
        tb->tick();
    }
    for (uint32_t i = 0; i < 5; i++)
    {
        data_a.address = i;
        data_a.w_byte_enable = 0x0;

        tb->trans_a(data_a);
    }
    printf("Start transmit r:a\n");
    while (!tb->a_data_vec.empty())
    {
        tb->tick();
    }


    printf("compare w:b r:a\n");
    while (!tb->a_data_complete_vec.empty() & !tb->b_data_complete_vec.empty())
    {
        data_a = tb->a_data_complete_vec.back();
        data_b = tb->b_data_complete_vec.back();
        if(comp_data(data_b, data_a)){
            error++;
        }
        tb->a_data_complete_vec.pop_back();
        tb->b_data_complete_vec.pop_back();
    }

    printf("DONE\n");
    if (error)
    {
        printf("FAIL\n");
    }
    else
    {
        printf("PASS\n");
    }
    exit(0);
}
