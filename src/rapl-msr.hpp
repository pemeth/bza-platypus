#ifndef RAPL_MSR_HPP
#define RAPL_MSR_HPP

/**
 * @file rapl-msr.hpp
 * @author Patrik Németh (xnemet04@stud.fit.vutbr.cz)
 * @brief Reporting of RAPL values by reading MSRs directly.
 * Modified code from https://web.eece.maine.edu/~vweaver/projects/rapl/rapl-read.c.
 * @date 2022-04-20
 *
 * Intel MSR reference (section 14.9):
 * https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-3b-part-2-manual.pdf
 *
 */

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
