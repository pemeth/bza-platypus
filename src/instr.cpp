#include <iostream>

#include "instr.hpp"

namespace instr {

Stats measure_mov(uint64_t src, volatile uint64_t *mem, Measurement *m, uint64_t runs, uint64_t iterations)
{
    volatile uint64_t dst = 0;

    Stats s = Stats();

   uint64_t added_cnt = 0;
   while (added_cnt != runs) {
        m->start_measurement();
        for (uint64_t i = 0; i < iterations; ++i) {
            asm volatile ("mov %0, %1" : "=r"(dst) : "m"(mem[i]));
        }
        m->stop_measurement();

        if (m->get_energy() != 0) {
            added_cnt++;
            s.add(*m);
        }
    }

    return s;
}

Stats measure_xor(volatile uint64_t op, Measurement *m, uint64_t runs, uint64_t iterations)
{
    Stats s = Stats();

    volatile uint64_t a = 2;

   uint64_t added_cnt = 0;
   while (added_cnt != runs) {
        m->start_measurement();
        for (uint64_t i = 0; i < iterations; ++i) {
            asm volatile ("xor %0, %1" : "=r"(a) : "r"(op));
        }
        m->stop_measurement();

        if (m->get_energy() != 0) {
            added_cnt++;
            s.add(*m);
        }
    }

    return s;
}

Stats measure_inc(Measurement *m, uint64_t runs, uint64_t iterations)
{
    Stats s = Stats();

    volatile uint64_t a = 0;

   uint64_t added_cnt = 0;
   while (added_cnt != runs) {
        m->start_measurement();
        for (uint64_t i = 0; i < iterations; ++i) {
            asm volatile ("inc %0" : "=r"(a));
        }
        m->stop_measurement();

        if (m->get_energy() != 0) {
            added_cnt++;
            s.add(*m);
        }
    }

    return s;
}

Stats measure_imul(volatile uint64_t op, Measurement *m, uint64_t runs, uint64_t iterations)
{
    Stats s = Stats();

    uint64_t a;

    uint64_t added_cnt = 0;
    while (added_cnt != runs) {
        m->start_measurement();
        for (uint64_t i = 0; i < iterations; ++i) {
            asm volatile ("imul $0x8, %1" : "=r"(a) : "r"(op));
        }
        m->stop_measurement();

        if (m->get_energy() != 0) {
            added_cnt++;
            s.add(*m);
        }
    }

    return s;
}

Stats measure_rdrand(Measurement *m, uint64_t runs, uint64_t iterations)
{
    Stats s = Stats();

    uint64_t a;

    // std::cerr << "rdrand\n";

   uint64_t added_cnt = 0;
   while (added_cnt != runs) {
        m->start_measurement();
        for (uint64_t i = 0; i < iterations; ++i) {
            asm volatile ("rdrand %0" : "=r"(a));
        }
        m->stop_measurement();

        //if (r % (runs / 10) == 0) std::cerr << "|"; // TODO remove - only for debugging

        if (m->get_energy() != 0) {
            added_cnt++;
            s.add(*m);
        }
    }

    return s;
}

Stats measure_fscale(Measurement *m, uint64_t runs, uint64_t iterations)
{
    Stats s = Stats();

    // std::cerr << "fscale\n";

   uint64_t added_cnt = 0;
   while (added_cnt != runs) {
        m->start_measurement();
        for (uint64_t i = 0; i < iterations; ++i) {
            asm volatile ("fscale");
        }
        m->stop_measurement();

        //if (r % (runs / 10) == 0) std::cerr << "|"; // TODO remove - only for debugging

        if (m->get_energy() != 0) {
            added_cnt++;
            s.add(*m);
        }
    }

    return s;
}

Stats measure_rdtsc(Measurement *m, uint64_t runs, uint64_t iterations)
{
    Stats s = Stats();

    volatile uint32_t a, d;

    // std::cerr << "rdtsc\n";

   uint64_t added_cnt = 0;
   while (added_cnt != runs) {
        m->start_measurement();
        for (uint64_t i = 0; i < iterations; ++i) {
            asm volatile ("rdtsc" : "=a" (a), "=d" (d));
        }
        m->stop_measurement();

        //if (r % (runs / 10) == 0) std::cerr << "|"; // TODO remove - only for debugging

        if (m->get_energy() != 0) {
            added_cnt++;
            s.add(*m);
        }
    }

    return s;
}

Stats measure_clflush(Measurement *m, uint64_t runs, uint64_t iterations)
{
    Stats s = Stats();

    volatile uint64_t a;

    // std::cerr << "clflush\n";

   uint64_t added_cnt = 0;
   while (added_cnt != runs) {
        m->start_measurement();
        for (uint64_t i = 0; i < iterations; ++i) {
            asm volatile ("clflush (%0)" :: "r"(&a));
        }
        m->stop_measurement();

        //if (r % (runs / 10) == 0) std::cerr << "|"; // TODO remove - only for debugging

        if (m->get_energy() != 0) {
            added_cnt++;
            s.add(*m);
        }
    }

    return s;
}

Stats measure_nop(Measurement *m, uint64_t runs, uint64_t iterations)
{
    Stats s = Stats();

   uint64_t added_cnt = 0;
   while (added_cnt != runs) {
        m->start_measurement();
        for (uint64_t i = 0; i < iterations; ++i) {
            asm volatile ("nop");
        }
        m->stop_measurement();

        if (m->get_energy() != 0) {
            added_cnt++;
            s.add(*m);
        }
    }

    return s;
}

} /* namespace instr */
