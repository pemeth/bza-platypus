#include <iostream>
#include <thread>

#include <math.h>

#include "instr.hpp"
#include "Measurement.hpp"
#include "Stats.hpp"
#include "rdtsc.h"

uint8_t hello_seq[] = {1,0,1,0,0};
size_t hello_seq_len = 5;

static inline void high_energy_function(
    double dummy_data[],
    const int dummy_data_len,
    const cycles_t cycles_per_microsec,
    const int transmission_time
    )
{
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

static void transmit(uint8_t *bits, size_t bits_len)
{
    const cycles_t cycles_per_sec = calibrate();
    const cycles_t cycles_per_microsec = cycles_per_sec / 1000000;

    sleep(1); // wait for receiver to calibrate and start listening

    const int transmission_time = 700000;

    // Set up dummy data for the high energy function
    const int dummy_data_len = 100;
    double dummy_data[dummy_data_len];
    for (int i = 0; i < dummy_data_len; i++) {
        dummy_data[i] = get_cycles() / 12.3;
    }

    // Set up the message
    size_t message_len = hello_seq_len + bits_len;
    uint8_t *message = new uint8_t[message_len];
    for (size_t i = 0; i < hello_seq_len; i++) message[i] = hello_seq[i];
    for (size_t i = 0; i < bits_len; i++) message[i+hello_seq_len] = bits[i];

    // TODO remove this loop this is only for debugging
    std::cout << "should transmit: ";
    for (size_t i = 0; i < message_len; i++) std::cout << (int)message[i];
    std::cout << "\n" << std::flush;

    // Transmit the data
    for (uint32_t i = 0; i < message_len; i++) {
        if (message[i] == 0) {
            std::cout << "0" << std::flush;
            usleep(transmission_time);
        } else {
            std::cout << "1" << std::flush;
            high_energy_function(dummy_data, dummy_data_len, cycles_per_microsec, transmission_time);
        }
    }

    std::cout << "\n";

    delete[] message;
}

static void receive()
{
    const cycles_t cycles_per_sec = calibrate();
    Measurement m = Measurement(cycles_per_sec);
    Measurement mm = Measurement(cycles_per_sec);
    Stats s = Stats();
    std::ofstream f;
    f.open("covert_channel.csv");

    cycles_t curr_cycles = get_cycles();


    // listen for about 5 seconds
    while (get_cycles() <= (curr_cycles + cycles_per_sec * 7)) {
        // Get measurement
        // need to use raw MSRs, as powercap doesnt seem to work in threads
        m.start_measurement_msr();
        usleep(5000);
        m.stop_measurement_msr();

        f << m.get_energy() << '\n';
    }

    f.close();
}

void covert_channel(uint8_t *bits, size_t bits_len)
{
    std::thread tx(transmit, bits, bits_len);
    std::thread rx(receive);

    tx.join();
    rx.join();
}