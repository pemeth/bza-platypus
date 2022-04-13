#include <iostream>

#include "Measurement.hpp"
#include "Stats.hpp"

// TSC ticks per second
const cycles_t calibration = calibrate();

Stats measure_mov(uint64_t src, Measurement *m, uint64_t runs, uint64_t iterations)
{
    volatile uint64_t dst = 0;

    Stats s = Stats();

    for (uint64_t r = 0; r < runs; r++) {
        m->start_measurement();
        for (uint64_t i = 0; i < iterations; ++i) {
            asm volatile ("mov %0, %1" : "=r"(dst) : "r"(src));
        }
        m->stop_measurement();

        s.add(*m);
    }

    return s;
}

Stats measure_imul(Measurement *m, uint64_t runs, uint64_t iterations)
{
    Stats s = Stats();

    uint64_t a;

    for (uint64_t r = 0; r < runs; r++) {
        m->start_measurement();
        for (uint64_t i = 0; i < iterations; ++i) {
            asm volatile ("imul $0x5, %0" : "=r"(a) : "r"(a));
        }
        m->stop_measurement();

        s.add(*m);
    }

    return s;
}

int main(int argc, char const *argv[])
{
    // Init
    Measurement m = Measurement(calibration);
    if (m.init_rapl() < 0) {
        return EXIT_FAILURE;
    }

    const int iterations = 50000000;
    const int runs = 5;

    Stats mov_stats = measure_mov(12, &m, runs, iterations);
    Stats imul_stats = measure_imul(&m, runs, iterations);

    mov_stats.dump_to_file("mov.csv");
    imul_stats.dump_to_file("imul.csv");

    return 0;
}
