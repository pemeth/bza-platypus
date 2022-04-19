#include <iostream> // For debugging

#include "Measurement.hpp"

/**
 * @brief Initialize RAPL measurement instance. `init_rapl()` must be called after.
 *
 * @param cycles_per_sec How many CPU cycles per second.
 */
Measurement::Measurement(cycles_t cycles_per_sec)
{
    this->cycles_per_sec = cycles_per_sec;

    this->cycles1 = 0;
    this->cycles2 = 0;
    this->energy1 = 0;
    this->energy2 = 0;
    this->pkgs = nullptr;
}

/**
 * @brief For debugging purposes mainly.
 *
 * @param cycles_per_sec
 * @param energy1
 * @param energy2
 */
Measurement::Measurement(cycles_t cycles_per_sec, uint64_t energy1, uint64_t energy2)
{
    this->cycles_per_sec = cycles_per_sec;

    this->cycles1 = 0;
    this->cycles2 = 0;
    this->energy1 = 0;
    this->energy2 = 0;
    this->pkgs = nullptr;
}

Measurement::~Measurement()
{
}

void Measurement::start_measurement(powercap_rapl_zone zone)
{
    this->timer_start();
    powercap_rapl_get_energy_uj(
        &this->pkg,
        zone,
        &this->energy1);
}

void Measurement::stop_measurement(powercap_rapl_zone zone)
{
    this->timer_stop();
    powercap_rapl_get_energy_uj(
        &this->pkg,
        zone,
        &this->energy2);
}

void Measurement::start_measurement()
{
    this->timer_start();
    powercap_rapl_get_energy_uj(
        &this->pkg,
        POWERCAP_RAPL_ZONE_PACKAGE,
        &this->energy1);
}

void Measurement::stop_measurement()
{
    this->timer_stop();
    powercap_rapl_get_energy_uj(
        &this->pkg,
        POWERCAP_RAPL_ZONE_PACKAGE,
        &this->energy2);
}

/**
 * @brief Measure using access to the raw MSRs in /dev/cpu/{x}/msr.
 * Kernel module 'msr' must be loaded for this to work via '$ modprobe msr'.
 */
void Measurement::start_measurement_msr()
{
    this->timer_start();
    this->energy1 = rapl_msr(0, MSR_PKG_ENERGY_STATUS);
}

/**
 * @brief Measure using access to the raw MSRs in /dev/cpu/{x}/msr.
 * Kernel module 'msr' must be loaded for this to work via '$ modprobe msr'.
 */
void Measurement::stop_measurement_msr()
{
    this->energy2 = rapl_msr(0, MSR_PKG_ENERGY_STATUS);
    this->timer_stop();
}

/**
 * @brief Get the measured energy in micro Joules.
 *
 * @return Energy between measurements in micro Joules.
 */
double Measurement::get_energy()
{
    return (this->energy2 - this->energy1);
}

/**
 * @brief Get the measured time in seconds.
 *
 * @return Time between measurements in seconds.
 */
double Measurement::get_time()
{
    return (this->cycles2 - this->cycles1) / (double) this->cycles_per_sec;
}

void Measurement::print()
{
    if (this->cycles1 == 0 ||
        this->cycles2 == 0 ||
        this->energy1 == 0 ||
        this->energy2 == 0) {
        std::cout << this->cycles1 << '\n'
                    << this->cycles2 << '\n'
                    << this->energy1 << '\n'
                    << this->energy2 << '\n';
        return;
    }

    const double time_diff   = (this->cycles2 - this->cycles1) / (double) this->cycles_per_sec;
    const double energy_diff = (this->energy2 - this->energy1) / FROM_MICRO_CONVERSION_CONST;
    const double power_diff  = energy_diff / time_diff;

    printf("energy %5.5f J\n", energy_diff);
    printf("power  %5.5f W\n", power_diff);
    printf("time   %5.5f s\n", time_diff);
}

/**
 * @brief Initialize the RAPL library.
 *
 * @return Zero on success, negative value and a perror() message if failed.
 */
int Measurement::init_rapl()
{
    // get number of top-level (parent) RAPL instances
    uint32_t count = powercap_rapl_get_num_instances();
    if (count == 0) {
        // none found (maybe the kernel module isn't loaded?)
        perror("powercap_rapl_get_num_instances");
        return -1;
    }

    this->pkgs = (powercap_rapl_pkg *)malloc(count * sizeof(powercap_rapl_pkg));
    // initialize
    uint32_t i;
    for (i = 0; i < count; i++) {
        if (powercap_rapl_init(i, &(this->pkgs[i]), 0)) {
            // could be that you don't have write privileges
            perror("powercap_rapl_init");
            return -1;
        }
    }

    // TODO maybe check if all pkgs are enabled?
    int check = powercap_rapl_is_enabled(&(this->pkgs[0]), POWERCAP_RAPL_ZONE_PACKAGE);
    if (check < 0) {
        perror("powercap_rapl_is_enabled");
        return -1;
    }

    // Separate first one, as this is the one we will be using
    this->pkg = this->pkgs[0];

    return 0;
}

  /***********************/
 /*       PRIVATE       */
/***********************/

cycles_t Measurement::get_cycles_delta()
{
    return this->cycles2 - this->cycles1;
}

uint64_t Measurement::get_energy_delta()
{
    return this->energy2 - this->energy1;
}

void Measurement::timer_start()
{
    this->cycles1 = get_cycles();
}

void Measurement::timer_stop()
{
    this->cycles2 = get_cycles();
}

  /***********************/
 /*       STATIC        */
/***********************/
