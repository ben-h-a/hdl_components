#include "Vsp_ram.h"
#include "verilated.h"

vluint64_t main_time = 0;
double sc_time_stamp() {
    return main_time;  // Note does conversion to real, to match SystemC
}

int task_do_write(Vsp_ram * ram, uint32_t * data, uint32_t len){
    if(ram->CLK){

    }
}

class TESTBENCH {
    public:
    unsigned long   m_tickcount;
    Vsp_ram  *m_core;

    TESTBENCH(void) {
        m_core = new Vsp_ram;
        m_tickcount = 0l;
        m_core->CLK = 0;
        m_core->RST_N = 0;
    }

    ~TESTBENCH(void) {
        delete m_core;
        m_core = NULL;
    }

    void reset(void) {
        m_core->RST_N = 0;
        // Make sure any inheritance gets applied
        this->tick();
        m_core->RST_N = 1;
    }

    void write(uint32_t * data, uint32_t * addr, uint32_t len){
        m_core->W_EN=0xF;
        for(uint32_t i=0; i<len; i++){
            m_core->ADDR = *(addr+i);
            m_core->D = *(data+i);
            this->tick();
        }
        m_core->W_EN=0;
    }

    void read(uint32_t * data, uint32_t * addr, uint32_t len){
        m_core->W_EN=0;
            uint32_t * data_ptr;
        for(uint32_t i=0; i<len; i++){
            data_ptr = data+i;
            m_core->ADDR = *(addr+i);
            *data_ptr = m_core->Q;
            tick();
        }
    }


    void    tick(void) {
        // Increment our own internal time reference
        m_tickcount++;

        // Make sure any combinatorial logic depending upon
        // inputs that may have changed before we called tick()
        // has settled before the rising edge of the clock.
        m_core->CLK = 0;
        m_core->eval();

        // Toggle the clock

        // Rising edge
        m_core->CLK = 1;
        m_core->eval();

        // Falling edge
        m_core->CLK = 0;
        m_core->eval();
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
        tb->write(&data, &i, 1);
        tb->read(&r_data, &i, 1);
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
