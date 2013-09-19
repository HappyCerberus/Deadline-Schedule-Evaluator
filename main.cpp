#include "job.h"
#include "user.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include "resource_gap.h"
#include "resource_plan.h"
using namespace std;

#include "schedule.h"
#include "config.h"

void initialize(int argc, char *argv[])
{
    // Check correct number of paramters
    if (argc != 4)
    {
        stringstream err;
        err << "This program expects exactly 3 commandline parameters." << endl << "\t" << argv[0] << " " << "input cpu_capacity ram_capacity" << endl;
        throw invalid_argument(err.str());
    }

    // Open the input file
    config::input.open(argv[1],ios::in);
    if (!config::input.is_open())
    {
        stringstream err;
        err << "Unable to open input file \"" << argv[1] << "\" for reading." << endl;
        throw runtime_error(err.str());
    }

    // Read the rest of the cmd line paramters
    char *end = NULL;
    config::cpu_capacity = strtoll(argv[2],&end,10);
    if (end == argv[2])
    {
        stringstream err;
        err << "Unable to parse \"" << argv[2] << "\" as a long long variable." << endl;
        throw invalid_argument(err.str());
    }

    config::ram_capacity = strtoll(argv[3],&end,10);
    if (end == argv[3])
    {
        stringstream err;
        err << "Unable to parse \"" << argv[3] << "\" as a long long variable." << endl;
        throw invalid_argument(err.str());
    }

    // Open output files

    {
        string basename = argv[1];

        // Jobs file
        string jobs = basename + string(".jobs");
        config::jobs.open(jobs.c_str(),ios::out | ios::trunc);
        if (!config::jobs.is_open())
        {
            stringstream err;
            err << "Unable to open output job file \"" << jobs << "\" for writing." << endl;
            throw runtime_error(err.str());
        }

        // Users file
        string users = basename + string(".users");
        config::users.open(users.c_str(),ios::out | ios::trunc);
        if (!config::users.is_open())
        {
            stringstream err;
            err << "Unable to open output users file \"" << users << "\" for writing." << endl;
            throw runtime_error(err.str());
        }

        // Deadlines file
        string deadlines = basename + string(".deadlines");
        config::deadlines.open(deadlines.c_str(),ios::out | ios::trunc);
        if (!config::deadlines.is_open())
        {
            stringstream err;
            err << "Unable to open output deadlines file \"" << deadlines << "\" for writing." << endl;
            throw runtime_error(err.str());
        }

        // Consumption file
        string consumption = basename + string(".consumption");
        config::consumption.open(consumption.c_str(),ios::out | ios::trunc);
        if (!config::consumption.is_open())
        {
            stringstream err;
            err << "Unable to open output consumption file \"" << consumption << "\" for writing." << endl;
            throw runtime_error(err.str());
        }
    }

}


int main(int argc, char *argv[])
{
    initialize(argc,argv);

    schedule sch;

    sch.process_file(config::input, string("JOBID ARRIVAL WAITTIME RUNTIME CPUREQ RAMREQ USERID QUEUE"));
    sch.write_jobs(config::jobs);
    sch.write_users(config::users);

    sch.calculate_consumption();
    sch.calculate_deadlines();

    sch.write_deadlines(config::deadlines);
    sch.write_consumption(config::consumption);

    return 0;
}
