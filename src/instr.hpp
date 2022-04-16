#ifndef INSTRUCTIONTEST_H
#define INSTRUCTIONTEST_H

#include "Measurement.hpp"
#include "Stats.hpp"

namespace instr {

Stats measure_mov(uint64_t src, volatile uint64_t *mem, Measurement *m, uint64_t runs, uint64_t iterations);

Stats measure_add(volatile uint16_t op, Measurement *m, uint64_t runs, uint64_t iterations);

Stats measure_imul(volatile uint64_t op, Measurement *m, uint64_t runs, uint64_t iterations);

Stats measure_rdrand(Measurement *m, uint64_t runs, uint64_t iterations);

Stats measure_fscale(Measurement *m, uint64_t runs, uint64_t iterations);

Stats measure_rdtsc(Measurement *m, uint64_t runs, uint64_t iterations);

Stats measure_clflush(Measurement *m, uint64_t runs, uint64_t iterations);

Stats measure_nop(Measurement *m, uint64_t runs, uint64_t iterations);

} /* namespace instr */

#endif /* INSTRUCTIONTEST_H */
