#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>
#include <vector>
#include <string>

struct config
{
    static std::fstream input;              ///< Input file
    static std::fstream jobs;               ///< Output file for processed jobs and their deadlines
    static std::fstream users;              ///< Output file for processed users
    static std::fstream deadlines;          ///< Output file for deadline heatmap data
    static std::fstream consumption;        ///< Output file for resource consumption overview

    static std::string input_filename;
    static std::string heatmap_filename;

    static long long cpu_capacity;          ///< Default user share of CPU cores
    static long long ram_capacity;          ///< Default user share of RAM capacity

    static std::vector<std::string> ignored_queues;
    static std::vector<std::string> ignored_users;

    static std::string single_queue_mode;

    static int minimumJobs;
    static int minimumDeadlines;
    static int minimumDeadlineTime;

    static bool generateHeatmap;
};

#endif // CONFIG_H
