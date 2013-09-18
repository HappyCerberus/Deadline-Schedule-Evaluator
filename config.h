#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>

struct config
{
    static std::fstream input;
    static std::fstream jobs;
    static std::fstream users;
    static std::fstream deadlines;
    static std::fstream consumption;

    static long long cpu_capacity;
    static long long ram_capacity;
};

#endif // CONFIG_H
