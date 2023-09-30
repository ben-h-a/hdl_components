#include "tbclock.h"
#include "base_testbench.h"
#include "verilated.h"
#include <verilated_vcd_c.h>
#include <functional>
#include "Vdp_ram.h"

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
    SramData *a_data;
    unsigned int a_data_len;
    unsigned int a_data_index;

    SramData *b_data;
    unsigned int b_data_len;
    unsigned int b_data_index;

public:
    bool a_data_complete;
    bool b_data_complete;
    ~DpSramTestbench();
    DpSramTestbench();
    DpSramTestbench(Vdp_ram *top_core,
                    unsigned int a_clk_period,
                    unsigned int b_clk_period,
                    VerilatedVcdC *m_trace = NULL)
        : BaseTestbench(top_core, m_trace)
    {
        auto clk_a_rise_lambda = [this]()
        {
            this->assign_a_clk_rise();
        };
        auto clk_b_rise_lambda = [this]()
        {
            this->assign_b_clk_rise();
        };
        TbClock a_clk = TbClock(a_clk_period, clk_a_rise_lambda, []() {});
        TbClock b_clk = TbClock(b_clk_period, clk_b_rise_lambda, []() {});
    }

    void assign_a_clk_rise()
    {
        // Write assignment
        if (a_data_index >= a_data_len)
        {
            a_data_complete = true;
            return;
        }
        else
        {
            a_data_complete = false;
        }
        this->m_core->W_EN_A = this->a_data[a_data_index].w_byte_enable;
        this->m_core->ADDR_A = this->a_data[a_data_index].address;
        this->a_data[a_data_index].r_data = this->m_core->R_DATA_A;
        if (a_data[a_data_index].w_data != NULL)
        {
            this->m_core->W_DATA_A = this->a_data[a_data_index].w_data;
        }
        else
        {
            this->m_core->W_DATA_A = 0x0;
        }
        a_data_index++;
    }

    void assign_b_clk_rise()
    {
        // Write assignment
        if (b_data_index >= b_data_len)
        {
            b_data_complete = true;
            return;
        }
        else
        {
            b_data_complete = false;
        }
        this->m_core->W_EN_B = this->b_data[b_data_index].w_byte_enable;
        this->m_core->ADDR_B = this->b_data[b_data_index].address;
        this->b_data[b_data_index].r_data = this->m_core->R_DATA_B;
        if (b_data[b_data_index].w_data != NULL)
        {
            this->m_core->W_DATA_B = this->b_data[b_data_index].w_data;
        }
        else
        {
            this->m_core->W_DATA_B = 0x0;
        }
        b_data_index++;
    }

    void trans_a(SramData *data, unsigned int len)
    {
        a_data = data;
        a_data_len = len;
        a_data_index = 0;
    }
    void trans_b(SramData *data, unsigned int len)
    {
        b_data = data;
        b_data_len = len;
        b_data_index = 0;
    }
};

int main(int argc, char **argv, char **env)
{
    // This example started with the Verilator example files.
    // Please see those examples for commented sources, here:
    // https://github.com/verilator/verilator/tree/master/examples

    if (0 && argc && argv && env)
    {
    }

    Verilated::debug(0);
    Verilated::randReset(2);
    Verilated::traceEverOn(true);
    Verilated::commandArgs(argc, argv);
    Verilated::mkdir("logs");

    DpSramTestbench *tb = new DpSramTestbench();
    SramData data_a;
    SramData data_b;
    uint32_t w_data_comp;
    int error = 0;
    for (uint32_t i = 0; i < 5; i++)
    {
        data_a.address = i;
        data_a.w_data = std::rand();
        data_a.w_byte_enable = 0xFF;

        data_b.address = i;
        data_b.w_byte_enable = 0x0;

        tb->trans_a(&data_a, 1);
        tb->trans_b(&data_b, 1);
        tb->tick();
        w_data_comp = data_a.w_data & 0xFF;
        if (w_data_comp != data_b.r_data)
        {
            printf("ERROR addr %x: w_data %x != r_data %x\n",
                   i, w_data_comp, data_b.r_data);
            error = 1;
        }
    }
    for (uint32_t i = 0; i < 5; i++)
    {
        data_b.address = i;
        data_b.w_data = std::rand();
        data_b.w_byte_enable = 0xFF;

        data_a.address = i;
        data_a.w_byte_enable = 0x0;

        tb->trans_a(&data_a, 1);
        tb->trans_b(&data_b, 1);
        tb->tick();
        w_data_comp = data_b.w_data & 0xFF;
        if (w_data_comp != data_a.r_data)
        {
            printf("ERROR addr %x: w_data %x != r_data %x\n",
                   i, w_data_comp, data_a.r_data);
            error = 1;
        }
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

    delete tb;
    tb = NULL;
    exit(0);
}
