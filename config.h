#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>

struct config
{
    static std::fstream input;              ///< Input file
    static std::fstream jobs;               ///< Output file for processed jobs and their deadlines
    static std::fstream users;              ///< Output file for processed users
    static std::fstream deadlines;          ///< Output file for deadline heatmap data
    static std::fstream consumption;        ///< Output file for resource consumption overview

    static long long cpu_capacity;          ///< Default user share of CPU cores
    static long long ram_capacity;          ///< Default user share of RAM capacity
};

#endif // CONFIG_H
