/**
 * Inspired by:
 * https://web.eece.maine.edu/~vweaver/projects/rapl/rapl-read.c
 *
 * Intel MSR reference (section 14.9):
 * https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-3b-part-2-manual.pdf
 *
 */

#include "rapl-msr.hpp"

int open_msr(int core)
{
    char msr_filename[BUFSIZ];
    int fd;

    sprintf(msr_filename, "/dev/cpu/%d/msr", core);
    fd = open(msr_filename, O_RDONLY);
    if ( fd < 0 ) {
        if ( errno == ENXIO ) {
            fprintf(stderr, "rdmsr: No CPU %d\n", core);
            exit(2);
        } else if ( errno == EIO ) {
            fprintf(stderr, "rdmsr: CPU %d doesn't support MSRs\n",
                    core);
            exit(3);
        } else {
            perror("rdmsr:open");
            fprintf(stderr,"Trying to open %s\n", msr_filename);
            exit(127);
        }
    }

    return fd;
}

long long read_msr(int fd, int which)
{
    uint64_t data;

    if ( pread(fd, &data, sizeof data, which) != sizeof data ) {
        perror("rdmsr:pread");
        exit(127);
    }

    return (long long)data;
}

/**
 * @brief Return energy reading from the `core`'s MSR for `domain`.
 *
 * @param core The core of the CPU.
 * @param domain MSR_PKG_ENERGY_STATUS for CPU package or MSR_PP0_ENERGY_STATUS for CPU core.
 * @return uint64_t The read value in micro Joules.
 */
uint64_t rapl_msr(int core, int domain)
{
    int fd;
    fd = open_msr(core);

    long long result = read_msr(fd,MSR_RAPL_POWER_UNIT);
    double cpu_energy_units = pow(0.5, (double)((result>>8) & 0x1f));
    uint64_t energy;

    /* Package Energy in micro Joules */
    result = read_msr(fd, domain);
    energy = (uint64_t) ((double)result * (cpu_energy_units * 1000000.0));

    close(fd);

    return energy;
}