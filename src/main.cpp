#include <iostream>
#include <string>

#include "instr.hpp"
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
    Stats add_stats = instr::measure_add(1, &m, runs, iterations); std::cerr << "add done\n";
    add_stats.dump_to_file("add.csv");

    Stats mov_stats = instr::measure_mov(12, mem, &m, runs, iterations); std::cerr << "mov done\n";
    mov_stats.dump_to_file("mov.csv");

    Stats imul_stats = instr::measure_imul(1, &m, runs, iterations); std::cerr << "imul done\n";
    imul_stats.dump_to_file("imul.csv");

    Stats fscale_stats = instr::measure_fscale(&m, runs, iterations); std::cerr << "fscale done\n";
    fscale_stats.dump_to_file("fscale.csv");

    Stats rdtsc_stats = instr::measure_rdtsc(&m, runs, iterations); std::cerr << "rdtsc done\n";
    rdtsc_stats.dump_to_file("rdtsc.csv");

    Stats rdrand_stats = instr::measure_rdrand(&m, runs, iterations); std::cerr << "rdrand done\n";
    rdrand_stats.dump_to_file("rdrand.csv");

    Stats clflush_stats = instr::measure_clflush(&m, runs, iterations); std::cerr << "clflush done\n";
    clflush_stats.dump_to_file("clflush.csv");

    Stats nop_stats = instr::measure_nop(&m, runs, iterations); std::cerr << "nop done\n";
    nop_stats.dump_to_file("nop.csv");
}

int main(int argc, char const *argv[])
{
    // Init
    Measurement m = Measurement(calibration);
    if (m.init_rapl() < 0) {
        return EXIT_FAILURE;
    }

    const int iterations = 50000;
    const int runs = 10000; // runs have changed their meaning - now it is the number of valid (nonzero) runs of measurements.
    volatile uint64_t mem[iterations*15]; // TODO do something with this... not sure if it does anything at all and causes segfault if iterations is too high

    measure_all_instructions(mem, m, runs, iterations);

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
