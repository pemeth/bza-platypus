#ifndef RAPL_MSR_HPP
#define RAPL_MSR_HPP

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include <math.h>

#define MSR_PKG_ENERGY_STATUS 0x611
#define MSR_PP0_ENERGY_STATUS 0x639
#define MSR_RAPL_POWER_UNIT 0x606

int open_msr(int core);
long long read_msr(int fd, int which);
uint64_t rapl_msr(int core, int domain);

#endif /* RAPL_MSR_HPP */
