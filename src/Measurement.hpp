#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <cstdint>
#include <powercap/powercap.h>
#include <powercap/powercap-rapl.h>

#include "rdtsc.h"

#define FROM_MICRO_CONVERSION_CONST 1000000.0

class Measurement
{
private:
    cycles_t cycles_per_sec;
    cycles_t cycles1;
    cycles_t cycles2;
    uint64_t energy1;
    uint64_t energy2;
    powercap_rapl_pkg *pkgs;    //!< All RAPL packages
    powercap_rapl_pkg pkg;      //!< The RAPL package that is used for measurements

    void timer_start();
    void timer_stop();
    cycles_t get_cycles_delta();
    uint64_t get_energy_delta();

public:
    Measurement(cycles_t cycles_per_sec);
    Measurement(cycles_t cycles_per_sec, uint64_t energy1, uint64_t energy2);
    ~Measurement();

    int init_rapl();

    void start_measurement();
    void stop_measurement();
    void print();

    double get_energy();
    double get_time();
};

#endif /* MEASUREMENT_H */
