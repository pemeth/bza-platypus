/**
 * @file Measurement.hpp
 * @author Patrik Németh (xnemet04@stud.fit.vutbr.cz)
 * @brief Class for handling individual power and timing measurements.
 * @date 2022-04-20
 */

#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <cstdint>
#include <powercap/powercap.h>
#include <powercap/powercap-rapl.h>

#include "rapl-msr.hpp"
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

    void start_measurement(powercap_rapl_zone zone);
    void stop_measurement(powercap_rapl_zone zone);
    void start_measurement();
    void stop_measurement();
    void start_measurement_msr();
    void stop_measurement_msr();
    void print();

    double get_energy();
    double get_time();
};

#endif /* MEASUREMENT_H */
