#include "schedule.h"
#include "config.h"
#include <iostream>
using namespace std;

schedule::schedule()
{
}

void schedule::set_user_allowance(int user_id, BigResc cpu, BigResc ram)
{
    p_allowance.insert(make_pair(user_id,make_pair(cpu,ram)));
}

void schedule::process_file(std::istream &file, const string& format)
{
    try
    {
        while (1)
        {
            auto pjob = make_shared<job>(file,format);
            auto id = pjob->get_userid();

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

    p_last_end_time = 0;

    for (auto i : p_user_db)
    {
        shared_ptr<user> user = i.second;
        user->calculate_deadlines();

        if (user->get_last_end_time() > p_last_end_time)
            p_last_end_time = user->get_last_end_time();
    }
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
    for (auto i : p_user_db)
    {
        shared_ptr<user> user = i.second;
        user->calculate_consumptions(p_last_end_time);
    }
}

void schedule::calculate_deadlines()
{
    for (auto i : p_user_db)
    {
        shared_ptr<user> user = i.second;
        user->calculate_deadline_count(p_last_end_time,DeadlineBeforeStart);
    }
}

void schedule::write_deadlines(std::ostream& file)
{
    size_t userid = 0;
    for (auto i : p_user_db)
    {
        shared_ptr<user> user = i.second;

        for (size_t j = 0; j < user->get_deadline_count().size(); j++)
        {
            file << userid << " " << j << " " << user->get_deadline_count()[j] << endl;
        }

        ++userid;
    }
}

void schedule::write_consumption(std::ostream& file)
{
    auto i = p_user_db.begin();

    auto cpu = i->second->get_cpu_consumption();
    auto ram = i->second->get_cpu_consumption();
    i++;

    p_max_cpu_consumption = 0;
    p_max_ram_consumption = 0;

    for (; i != p_user_db.end(); i++)
    {
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

    cerr << "MaxCPU: " << p_max_cpu_consumption << " MaxRAM: " << p_max_ram_consumption << endl;
}
