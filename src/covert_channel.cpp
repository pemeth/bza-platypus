#include <iostream>
#include <thread>

#include <math.h>

#include "instr.hpp"
#include "Measurement.hpp"
#include "Stats.hpp"
#include "rdtsc.h"

uint32_t hello_seq[] = {1,0,1,0,0};
uint32_t hello_seq_len = 5;

static void transmit()
{
    const cycles_t cycles_per_sec = calibrate();
    const cycles_t cycles_per_microsec = cycles_per_sec / 1000000;

    sleep(1); // wait for receiver to calibrate and start listening

    const int transmission_time = 700000;

    cycles_t curr_cycles = get_cycles();

    const int dummy_data_len = 100;
    double dummy_data[dummy_data_len];
    for (int i = 0; i < dummy_data_len; i++) {
        dummy_data[i] = get_cycles() / 12.3;
    }

    for (uint32_t i = 0; i < hello_seq_len; i++) {
        if (hello_seq[i] == 0) {
            std::cout << "0" << std::flush;
            usleep(transmission_time);
        } else {
            std::cout << "1" << std::flush;
            int k = 0;
            cycles_t curr_tx_cycles = get_cycles();
            while (get_cycles() <= (curr_tx_cycles + cycles_per_microsec * transmission_time)) {
                if (k <= dummy_data_len - 10) {
                    pow(dummy_data[k], sin(dummy_data[k+1]) * dummy_data[k+2] / cos(dummy_data[k+3]));
                } else {
                    k = 0;
                }
            }
        }
    }

    std::cout << "\n";
}

static void receive()
{
    const cycles_t cycles_per_sec = calibrate();
    const cycles_t cycles_per_microsec = cycles_per_sec / 1000000;
    Measurement m = Measurement(cycles_per_sec);
    Measurement mm = Measurement(cycles_per_sec);
    Stats s = Stats();

    cycles_t curr_cycles = get_cycles();


    // listen for about 5 seconds
    while (get_cycles() <= (curr_cycles + cycles_per_sec * 7)) {
        // need to use raw MSRs, as powercap doesnt seem to work in threads
        m.start_measurement_msr();
        usleep(5000);
        m.stop_measurement_msr();

        std::cerr << m.get_energy() << '\n';
    }
}

void covert_channel(uint8_t *bits, uint64_t bits_length)
{
    std::thread tx(transmit);
    std::thread rx(receive);

    tx.join();
    rx.join();
}