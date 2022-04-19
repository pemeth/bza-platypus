#include <iostream>
#include <string>

#include "covert_channel.hpp"
#include "instr.hpp"
#include "rapl-msr.hpp"
#include "Measurement.hpp"
#include "Stats.hpp"

// TSC ticks per second
const cycles_t calibration = calibrate();

void save_empty_loop_stats(std::string filename, Measurement m, uint64_t runs, uint64_t loop_iterations, uint64_t empty_loop_iterations)
{
    std::ofstream f;
    f.open(filename);

    f << "runs:" << runs << '\n';
    f << "loop_iterations:" << loop_iterations << '\n';
    f << "empty_loop_iterations:" << empty_loop_iterations << '\n';
    f << "Joules:" << m.get_energy() / FROM_MICRO_CONVERSION_CONST << '\n';
    f << "seconds:" << m.get_time() << '\n';

    f.close();
}

void measure_all_instructions(volatile uint64_t *mem, Measurement m, uint64_t runs, uint64_t iterations)
{
    // Measure instrucions
    // TODO I measure the PACKAGE energy, which does not contain the DRAM domain - measure that as well and add them up...
    // might help with the statistical data
    Stats xor_stats = instr::measure_xor(1, &m, runs, iterations); std::cerr << "xor done\n";
    xor_stats.dump_energies_to_file("xor.csv");

    Stats inc_stats = instr::measure_inc(&m, runs, iterations); std::cerr << "inc done\n";
    inc_stats.dump_energies_to_file("inc.csv");

    Stats mov_stats = instr::measure_mov(12, mem, &m, runs, iterations); std::cerr << "mov done\n";
    mov_stats.dump_energies_to_file("mov.csv");

    Stats imul_stats = instr::measure_imul(1, &m, runs, iterations); std::cerr << "imul done\n";
    imul_stats.dump_energies_to_file("imul.csv");

    Stats fscale_stats = instr::measure_fscale(&m, runs, iterations); std::cerr << "fscale done\n";
    fscale_stats.dump_energies_to_file("fscale.csv");

    Stats rdtsc_stats = instr::measure_rdtsc(&m, runs, iterations); std::cerr << "rdtsc done\n";
    rdtsc_stats.dump_energies_to_file("rdtsc.csv");

    Stats rdrand_stats = instr::measure_rdrand(&m, runs, iterations); std::cerr << "rdrand done\n";
    rdrand_stats.dump_energies_to_file("rdrand.csv");

    Stats clflush_stats = instr::measure_clflush(&m, runs, iterations); std::cerr << "clflush done\n";
    clflush_stats.dump_energies_to_file("clflush.csv");

    Stats nop_stats = instr::measure_nop(&m, runs, iterations); std::cerr << "nop done\n";
    nop_stats.dump_energies_to_file("nop.csv");
}

void measure_imul_operands(Measurement m, uint64_t runs, uint64_t iterations)
{
    Stats imul_stats;

    imul_stats = instr::measure_imul(0x0, &m, runs, iterations); std::cerr << "imul done\n";
    imul_stats.dump_energies_to_file("imul_hw_0.csv");

    imul_stats = instr::measure_imul(0xffffffff, &m, runs, iterations); std::cerr << "imul done\n";
    imul_stats.dump_energies_to_file("imul_hw_32.csv");

    imul_stats = instr::measure_imul(0xffffffffffffffff, &m, runs, iterations); std::cerr << "imul done\n";
    imul_stats.dump_energies_to_file("imul_hw_64.csv");
}

/* Measurement::start_measurement() and Measurement::stop_measurement()
    have to be modified according to which domain is being measured. */
void measure_rapl_refresh_rate()
{
    Measurement m = Measurement(calibration, 0, 1);
    if (m.init_rapl() < 0) {
        return;
    }

    double min_time = 10000000000;
    get_cycles();get_cycles();get_cycles();
    Stats s = Stats();

    // RAPL refresh
    for (int r = 0; r < 100; r++) {
        min_time = 10000000000;
        for (int i = 0; i < 1000; i++) {

            m.start_measurement();
            do {
                m.stop_measurement();
            } while (m.get_energy() >= -1 && m.get_energy() <= 1);

            if (m.get_time() < min_time) min_time = m.get_time();
        }

        s.add_time((min_time));
    }

    s.dump_times_to_file("RAPL_PACKAGE");
}

int main(int argc, char const *argv[])
{
    // Init
    Measurement m = Measurement(calibration);
    if (m.init_rapl() < 0) {
        return EXIT_FAILURE;
    }

    uint8_t bits[] = {1,1,1,1,0,0,0,0};
    uint64_t  bits_len = 8;
    covert_channel(bits, bits_len);
    return 0;

    const int iterations = 50000;
    const int runs = 10000; // runs have changed their meaning - now it is the number of valid (nonzero) runs of measurements.
    volatile uint64_t mem[iterations*15]; // TODO do something with this... not sure if it does anything at all and causes segfault if iterations is too high

    // Individual measurments
    measure_all_instructions(mem, m, runs, iterations);
    //measure_imul_operands(m, runs, iterations);
    //measure_rapl_refresh_rate();

    // Measure empty loop energy draw
    const int eval_multiplicator = 50;
    m.start_measurement();
    for(int i = 0; i < iterations * eval_multiplicator; i++) asm("");
    m.stop_measurement();

    std::cout << "Empty loop energy draw on " << iterations * eval_multiplicator << " iterations:\n";
    m.print();

    save_empty_loop_stats("loop_stats.txt", m, runs, iterations, iterations * eval_multiplicator);

    return 0;
}
