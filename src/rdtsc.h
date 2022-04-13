// Taken from
// https://github.com/LITMUS-RT/liblitmus/blob/master/arch/x86/include/asm/cycles.h

#ifndef RDTSC_H
#define RDTSC_H

#define rdtscll(val) do { \
	unsigned int __a,__d; \
	__asm__ __volatile__("rdtsc" : "=a" (__a), "=d" (__d)); \
	(val) = ((unsigned long long)__a) | (((unsigned long long)__d)<<32); \
} while(0)

static __inline__ unsigned long long native_read_tsc(void)
{
	unsigned long long val;

	__asm__ __volatile__("mfence":::"memory");
	rdtscll(val);
	__asm__ __volatile__("mfence":::"memory");

	return val;
}

#define CYCLES_FMT "llu"

typedef unsigned long long cycles_t;

static inline cycles_t get_cycles(void)
{
	return native_read_tsc();
}

/**
 * @brief Get TSC (Time Stamp Counter) cycles per one second.
 *
 * @return The number of TSC cycles per one second.
 */
static inline cycles_t calibrate(void)
{
	// TSC should be a wall clock since pentium 4 - this would not work on older CPUs
	cycles_t start = get_cycles();
	sleep(1);
	cycles_t end = get_cycles();

	return end - start;
}

#endif /* RDTSC_H */