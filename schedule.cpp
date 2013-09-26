#include "schedule.h"
#include "config.h"
#include <iostream>
#include "progressanim.h"
using namespace std;

schedule::schedule() : p_valid_user_count(0)
{
}

void schedule::set_user_allowance(std::string user_id, BigResc cpu, BigResc ram)
{
    p_allowance.insert(make_pair(user_id,make_pair(cpu,ram)));
}

void schedule::process_file(std::istream &file, const string& format)
{
    cout << "Reading input file." << endl;
    try
    {
        while (1)
        {
            auto pjob = make_shared<job>(file,format);
            auto id = pjob->get_userid();

            bool ignored = false;
            for (size_t i = 0; i < config::ignored_users.size(); i++)
            {
                if (config::ignored_users[i] == id)
                {
                    ignored = true;
                    break;
                }
            }

            if (ignored)
                continue;

            BigResc cpu = config::cpu_capacity;
            BigResc ram = config::ram_capacity;

            if (p_allowance.find(id) != p_allowance.end())
            {
                cpu = p_allowance[id].first;
                ram = p_allowance[id].second;
            }

            auto i = p_user_db.find(id);
            if (i == p_user_db.end())
            {
                i = p_user_db.insert(make_pair(id,make_shared<user>(id,cpu,ram))).first;
            }
            i->second->add_job(i->second,pjob);
        }
    }
    catch (...)
    {
    }
    cout << "Finished reading input file." << endl;


    cout << "Computing deadlines for users." << endl;
    p_last_end_time = 0;

    for (auto i : p_user_db)
    {
        shared_ptr<user> user = i.second;
        cout << "Processing user \"" << user->get_userid() << "\": " << flush;
        user->calculate_deadlines();

        if (user->get_last_end_time() > p_last_end_time)
            p_last_end_time = user->get_last_end_time();
    }
    cout << "Finished deadlines for users." << endl;
}

void schedule::write_jobs(ostream& file)
{
    for (auto i : p_user_db)
    {
        shared_ptr<user> user = i.second;
        user->write_jobs(file);
    }
}

void schedule::write_users(ostream &file)
{
    for (auto i : p_user_db)
    {
        shared_ptr<user> user = i.second;
        file << user->get_userid() << "\t" << user->get_jobcount() << "\t" << user->violated_deadline_count() << "\t" << user->total_deadline_diff_time() << "\t"
             << user->total_violated_deadline_time() << "\t" << fixed << 100*static_cast<double>(user->violated_deadline_count())/user->get_jobcount() << "\t"
             << static_cast<double>(user->total_deadline_diff_time())/user->get_jobcount() << "\t" << static_cast<double>(user->total_violated_deadline_time())/user->get_jobcount() << endl;
    }
}


void schedule::calculate_consumption()
{
    cout << "Calculating consumption: " << flush;
    progressanim vis;

    for (auto i : p_user_db)
    {
        vis.tick();
        shared_ptr<user> user = i.second;
        user->calculate_consumptions(p_last_end_time);
    }
}

void schedule::calculate_deadlines()
{
    cout << "Calculating deadline count: " << flush;
    progressanim vis;

    for (auto i : p_user_db)
    {
        vis.tick();
        shared_ptr<user> user = i.second;
        user->calculate_deadline_count(p_last_end_time,DeadlineBeforeStart);
    }
}

void schedule::write_deadlines(std::ostream& file)
{
    cout << "Writing deadline count to a file: " << flush;
    progressanim vis;

    size_t userid = 0;
    for (auto i : p_user_db)
    {
        vis.tick();
        shared_ptr<user> user = i.second;

        if (user->get_jobcount() < config::minimumJobs)
            continue;
        if (user->total_violated_deadline_time() < config::minimumDeadlineTime)
            continue;
        if (user->violated_deadline_count() < config::minimumDeadlines)
            continue;

        p_valid_user_count++;

        for (size_t j = 0; j < user->get_deadline_count().size(); j++)
        {
            file << userid << " " << j << " " << user->get_deadline_count()[j] << endl;
        }

        ++userid;
    }
}

void schedule::write_consumption(std::ostream& file)
{
    cout << "Writing consumption to a file: " << flush;
    progressanim vis;

    auto i = p_user_db.begin();

    auto cpu = i->second->get_cpu_consumption();
    auto ram = i->second->get_cpu_consumption();
    i++;

    p_max_cpu_consumption = 0;
    p_max_ram_consumption = 0;

    for (; i != p_user_db.end(); i++)
    {
        vis.tick();

        for (size_t j = 0; j < cpu.size(); j++)
        {
            cpu[j] += i->second->get_cpu_consumption()[j];
            ram[j] += i->second->get_ram_consumption()[j];

            if (cpu[j] > p_max_cpu_consumption) p_max_cpu_consumption = cpu[j];
            if (ram[j] > p_max_ram_consumption) p_max_ram_consumption = ram[j];
        }
    }

    for (size_t j = 0; j < cpu.size(); j++)
    {
        file << j << " " << cpu[j] << " " << ram[j]/1024 << endl;
    }
}
