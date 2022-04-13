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
    void dump_to_file(void);
    void dump_to_file(std::string filename);
};

#endif /* STATS_H */
