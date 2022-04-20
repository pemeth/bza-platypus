/**
 * @file Stats.hpp
 * @author Patrik Németh (xnemet04@stud.fit.vutbr.cz)
 * @brief Class handling measurement statistics gathering.
 * @date 2022-04-20
 */

#ifndef STATS_H
#define STATS_H

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include "Measurement.hpp"
#include "rdtsc.h"

class Stats
{
private:
    std::vector<double> energies; //!< Energy per measurement
    std::vector<double> times; //!< Time per measurement

public:
    Stats(/* args */);
    ~Stats();

    void add(Measurement m);
    void add_time(double time);
    void dump_energies_to_file(void);
    void dump_energies_to_file(std::string filename);
    void dump_times_to_file(std::string filename);
};

#endif /* STATS_H */
