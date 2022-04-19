#include <numeric>
#include <iostream>
#include <cmath>
#include <thread>
#include <vector>

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

static inline std::vector<double> shift_left(std::vector<double> v, int shift_by)
{
    for (size_t i = shift_by; i < v.size(); i++) {
        v[i-shift_by] = v[i];
    }

    return v;
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
            //std::cout << "0" << std::flush;
            usleep(transmission_time);
        } else {
            //std::cout << "1" << std::flush;
            high_energy_function(dummy_data, dummy_data_len, cycles_per_microsec, transmission_time);
        }
    }

    std::cout << "\n";

    delete[] message;
}

static inline void measure(Measurement *m, size_t sleep_period)
{
    // need to use raw MSRs, as powercap doesnt seem to work in threads
    m->start_measurement_msr();
    usleep(sleep_period);
    m->stop_measurement_msr();
}

static void receive()
{
    const cycles_t cycles_per_sec = calibrate();
    const size_t sleep_period = 5000;
    Measurement m = Measurement(cycles_per_sec);
    Measurement mm = Measurement(cycles_per_sec);
    Stats s = Stats();
    std::ofstream f;
    f.open("covert_channel.csv");

    cycles_t curr_cycles = get_cycles();

    // listen for about 5 seconds
    while (get_cycles() <= (curr_cycles + cycles_per_sec * 7)) {
        // Get measurement
        measure(&m, sleep_period);

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


// TODO an idea on how to decode the transmission without timing information
// - the hello sequence is 8 alternating bits (10101010) followed by 7 zero-bits an 1 one-bit
// - the receiver measures time and energy between bit flips in the first octet and gets an average
// - the receiver start its clock as soon as it detects the hello sequences last one-bit
// - the receiver can decode incoming data based on measurements sampled from the hello sequence
// It is a two-step method, where for the first step a (kinda-working) draft is below...
/* static void receive()
{
    const cycles_t cycles_per_sec = calibrate();
    const size_t sleep_period = 5000;
    Measurement m = Measurement(cycles_per_sec);
    Measurement mm = Measurement(cycles_per_sec);
    Stats s = Stats();
    std::ofstream f;
    f.open("covert_channel.csv");

    cycles_t curr_cycles = get_cycles();

    // Set up moving window for gathering moving average
    const size_t window_size = 10;
    std::vector<double> moving_window = std::vector<double>(window_size);
    for (size_t i = 0; i < window_size; i++) {
        measure(&m, sleep_period);
        moving_window[i] = m.get_energy();
    }

    double moving_avg = 0;
    double last_avg = 0;
    size_t similar_avg_cnt = 0;
    size_t cnt_tolerance = 0;
    const size_t cnt_tolerance_threshold = 5; // Probably has to be smaller than the mod value for "it" (this condition if (it % 20 == 0) last_avg = moving_avg;)
    const double energy_tolerance = 5000;
    double last_bit_energy = 0;

    size_t it = 0;

    // listen for about 5 seconds
    while (get_cycles() <= (curr_cycles + cycles_per_sec * 7)) {
        // Get measurement
        measure(&m, sleep_period);

        // Shift the moving window
        moving_window = shift_left(moving_window, 1);
        moving_window[window_size - 1] = m.get_energy();

        if (it % 20 == 0) last_avg = moving_avg;
        moving_avg = std::accumulate(moving_window.begin(), moving_window.end(), 0.0) / window_size;

        const double avg_diff = abs(last_avg - moving_avg);
        if (avg_diff >= energy_tolerance) {
            similar_avg_cnt += 1;
            cnt_tolerance = 0;
            last_bit_energy = moving_avg;
        } else {
            cnt_tolerance += 1;
        }

        if (cnt_tolerance == cnt_tolerance_threshold) {
            const double energy_threshold = 35000;//abs(moving_avg - last_bit_energy) / 2.0;
            if (moving_avg >= energy_threshold) {
                std::cout << "1" << std::flush;
            } else {
                std::cout << "0" << std::flush;
            }
        }
        it++;

        f << m.get_energy() << '\n';
    }

    f.close();
} */
