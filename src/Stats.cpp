#include "Stats.hpp"

#define VECTOR_RESERVE 10000
#define DEFAULT_DUMP_FILENAME "stats_dump.csv"

Stats::Stats()
{
    this->energies.reserve(VECTOR_RESERVE);
    this->times.reserve(VECTOR_RESERVE);
}

Stats::~Stats()
{
}

void Stats::add(Measurement m)
{
    this->energies.push_back(m.get_energy());
    this->times.push_back(m.get_time());
}

/**
 * @brief Add a specific time (in seconds) to the stats
 *
 * @param time the time to be added in seconds.
 */
void Stats::add_time(double time)
{
    this->times.push_back(time);
}

void Stats::dump_energies_to_file(void)
{
    this->dump_energies_to_file(DEFAULT_DUMP_FILENAME);
}

void Stats::dump_energies_to_file(std::string filename)
{
    if (filename.length() == 0) {
        filename = DEFAULT_DUMP_FILENAME;
    }

    std::ofstream f;
    f.open(filename);

    const size_t vector_size = this->energies.size();

    f << "energy\n";
    for (size_t i = 0; i < vector_size; i++) {
        f << this->energies[i] << '\n';
    }

    f.close();
}

void Stats::dump_times_to_file(std::string filename)
{
    if (filename.length() == 0) {
        filename = DEFAULT_DUMP_FILENAME;
    }

    std::ofstream f;
    f.open(filename);

    const size_t vector_size = this->times.size();

    f << "time\n";
    for (size_t i = 0; i < vector_size; i++) {
        f << this->times[i] << '\n';
    }

    f.close();
}
