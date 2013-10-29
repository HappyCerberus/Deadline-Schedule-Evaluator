#include "job.h"
#include "user.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include "resource_gap.h"
#include "resource_plan.h"
using namespace std;

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
using namespace boost::program_options;

#include "schedule.h"
#include "config.h"

int initialize(int argc, char *argv[])
{
    options_description desc("Known options");
    desc.add_options()
            ("help,h","Print this help information.")
            ("cpuAllowance,c", value<int>(),       "set default user allowance for CPU (cores)")
            ("ramAllowance,r", value<long long>(), "set default user allowance for RAM (MB)")
            ("input,i", value<string>(), "Data set to be processed")
            ("ignoreUser,u", value<vector<string> >()->composing(), "Ignore selected user(s)")
            ("ignoreQueue,q", value<vector<string> >()->composing(), "Ignore selected queue(s)")
            ("minimumJobs", value<int>(), "job limit for user to be considered")
            ("minimumDeadlines", value<int>(), "deadline limit for user to be considered")
            ("minimumDeadlineTime", value<int>(), "deadline time limit for user to be considered")
            ("singleQueueMode", value<string>(), "set a single queue to be processed")
            ("singleUserMode", value<string>(), "set a single user to be processed")
            ("heatmapFilename", value<string>(), "output file name for the heatmap")
            ("generateHeatmap", "generate heatmap using gnuplot")
            ("ignoreStart", value<long long>(), "ingore initial seconds in for deadline calculation and heatmap drawing")
            ;

    variables_map vm;
    store(parse_command_line(argc,argv,desc),vm);
    notify(vm);

    if (vm.count("help"))
    {
        cout << desc << endl;
        return 1;
    }

    if (vm.count("cpuAllowance"))
    {
        config::cpu_capacity = vm["cpuAllowance"].as<int>();
    }
    else
    {
        cout << "CPU allowance needs to be set" << endl;
        return 1;
    }

    if (vm.count("ramAllowance"))
    {
        config::ram_capacity = vm["ramAllowance"].as<long long>();
    }
    else
    {
        cout << "RAM allowance needs to be set" << endl;
        return 1;
    }

    if (vm.count("ignoreUser"))
    {
        config::ignored_users = vm["ignoreUser"].as<vector<string> >();
    }

    if (vm.count("ignoreQueue"))
    {
        config::ignored_queues = vm["ignoreQueue"].as<vector<string> >();
    }

    config::minimumJobs = 0;
    if (vm.count("minimumJobs"))
    {
        config::minimumJobs = vm["minimumJobs"].as<int>();
    }

    config::minimumDeadlines = 0;
    if (vm.count("minimumDeadlines"))
    {
        config::minimumDeadlines = vm["minimumDeadlines"].as<int>();
    }

    config::minimumDeadlineTime = 0;
    if (vm.count("minimumDeadlineTime"))
    {
        config::minimumDeadlineTime = vm["minimumDeadlineTime"].as<int>();
    }

    string filename;
    if (vm.count("input"))
    {
        filename = vm["input"].as<string>();
        config::input_filename = filename;
    }
    else
    {
        cout << "Input data set needs to be specified" << endl;
        return 1;
    }

    if (vm.count("heatmapFilename"))
    {
        config::heatmap_filename = vm["heatmapFilename"].as<string>();
    }
    else
    {
        config::heatmap_filename = filename + (".deadlines_heatmap.png");
    }

    config::single_queue_mode = "";
    if (vm.count("singleQueueMode"))
    {
        config::single_queue_mode = vm["singleQueueMode"].as<string>();
    }

    config::single_user_mode = "";
    if (vm.count("singleUserMode"))
    {
        config::single_user_mode = vm["singleUserMode"].as<string>();
    }

    config::generateHeatmap = false;
    if (vm.count("generateHeatmap"))
    {
        config::generateHeatmap = true;
    }

    config::ignore_first_seconds = 0;
    if (vm.count("ignoreStart"))
    {
        config::ignore_first_seconds = vm["ignoreStart"].as<long long>();
    }

    // Open the input file
    config::input.open(filename.c_str(),ios::in);
    if (!config::input.is_open())
    {
        stringstream err;
        err << "Unable to open input file \"" << filename << "\" for reading." << endl;
        throw runtime_error(err.str());
    }

    // Open output files

    {
        string basename = filename;

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

    return 0;
}


void generateHeatmap(const schedule& sch)
{
    string deadlines = config::input_filename + string(".deadlines");
    string consumption = config::input_filename + string(".consumption");
    string heatmap = config::heatmap_filename;

    FILE *gnuplot;
    if ((gnuplot = popen("gnuplot","w")) == NULL)
        throw "";

    BigResc user_count = sch.get_valid_user_count();
    BigResc ysize = user_count*10/0.9;
//    if (config::single_user_mode != "")
//        ysize = 2000;

    fprintf(gnuplot,"%s%lld%s\n","set terminal png large size 5120, ",ysize," enhanced font \"Verdana,50\"");
    fprintf(gnuplot,"set output \"%s\"\n",heatmap.c_str());
    fprintf(gnuplot,"set view map\n"
                    "set multiplot layout 6,1\n"
                    "unset key\n"
                    "set nokey\n"
                    "unset colorbox\n"
                    "unset title\n"
                    "set logscale zcb\n"
                    "set palette color\n"
                    "set palette model RGB\n"
                    "set palette negative defined\n"
                    "set tic scale 0\n"
                    "set bmargin 0\n"
                    "set tmargin 0\n"
                    "set lmargin 3\n"
                    "set rmargin 3\n"
                    "unset xtics\n"
                    "set xrange [%lld:]\n", config::ignore_first_seconds/60);
    fprintf(gnuplot,
                    "unset ytics\n"
                    "set boxwidth 2\n"
                    "set colorbox user origin 0.90,0 size 0.05,1 noborder\n"
                    "set cbtics 1,2,1025\n"
                    "set cbrange [1:2048]\n"
                    "set origin 0,0.2\n"
                    "set size 0.9,0.8\n"
                    "set yrange [-1:%lld]\n",user_count);
    fprintf(gnuplot,"set ylabel \"USERS\"\n"
                    "plot \"%s\" using 2:1:3 with image\n",deadlines.c_str());
    fprintf(gnuplot,"set origin 0,0.1\n"
                    "set size 0.9,0.1\n"
                    "set yrange [0:%lld]\n",sch.get_max_cpu_consumption());
    fprintf(gnuplot,"set ylabel \"CPU\"\n"
                    "plot \"%s\" using 1:2 with boxes\n",consumption.c_str());
    fprintf(gnuplot,"set origin 0,0\n"
                    "set size 0.9,0.1\n"
                    "set yrange [0:%lld]\n",sch.get_max_ram_consumption());
    fprintf(gnuplot,"set ylabel \"RAM\"\n"
                    "plot \"%s\" using 1:3 with boxes\n",consumption.c_str());
    pclose(gnuplot);
}

int main(int argc, char *argv[])
{
    int ret = initialize(argc,argv);
    if (ret != 0)
        return ret;

    schedule sch;

    cout << "Processing input data." << endl;
    sch.process_file(config::input, string("JOBID ARRIVAL WAITTIME RUNTIME CPUREQ RAMREQ USERID QUEUE"));
    cout << "Finished processing input data." << endl << endl;

    cout << "Writing base information to files." << endl;
    sch.write_jobs(config::jobs);
    sch.write_users(config::users);
    cout << "Finished writing base information to files." << endl << endl;

    sch.calculate_consumption();
    sch.calculate_deadlines();

    sch.write_deadlines(config::deadlines);
    sch.write_consumption(config::consumption);

    cerr << "MaxCPU: " << sch.get_max_cpu_consumption() << " MaxRAM: " << sch.get_max_ram_consumption() << endl;

    config::deadlines.close();
    config::consumption.close();

    if (config::generateHeatmap)
        generateHeatmap(sch);

    return 0;
}
