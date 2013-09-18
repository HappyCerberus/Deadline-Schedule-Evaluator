#include "user.h"
#include <algorithm>
#include <memory>
#include <iostream>
using namespace std;

user::user(TinyResc userid, BigResc cpu_capacity, BigResc ram_capacity) : p_userid(userid), p_cpu_resc(cpu_capacity), p_ram_resc(ram_capacity)
{
}

shared_ptr<job> user::operator[] (int index) const
{
    return p_jobs.at(index);
}

int user::get_jobcount() const
{
    return p_jobs.size();
}

void user::add_job(std::shared_ptr<user> u, std::shared_ptr<job> j)
{
    u->p_jobs.push_back(j);
    j->set_owner(u);
}

void user::sort_jobs()
{
    sort(p_jobs.begin(),p_jobs.end());
}

void user::calculate_deadlines()
{
    p_violated_deadline_count = 0;
    p_total_deadline = 0;
    p_total_positive_deadline = 0;
    p_last_end_time = 0;

    sort_jobs();

    for (size_t i = 0; i < p_jobs.size(); i++)
    {
        shared_ptr<job> job = p_jobs[i];

        int cpu_deadline = p_cpu_resc.consume(job->get_arrival(),job->get_runtime(),job->get_cpu_req());
        int ram_deadline = p_ram_resc.consume(job->get_arrival(),job->get_runtime(),job->get_ram_req());
        job->set_deadline(max(cpu_deadline,ram_deadline));

        if (!job->deadline_satisfied())
        {
            p_violated_deadline_count++;
            p_total_positive_deadline += job->deadline_diff();
        }
        p_total_deadline += job->deadline_diff();

        if (job->get_finish() > p_last_end_time)
            p_last_end_time = job->get_finish();
    }
}

void user::calculate_consumptions(BigResc end_time)
{
    p_cpu_consumption.clear();
    p_ram_consumption.clear();
    p_cpu_consumption.resize((end_time+1)/60);
    p_ram_consumption.resize((end_time+1)/60);

    for (size_t i = 0; i < p_jobs.size(); i++)
    {
        shared_ptr<job> j = p_jobs[i];
        BigResc start_time = j->get_arrival() + j->get_waittime();
        BigResc end_time = j->get_arrival() + j->get_waittime() + j->get_runtime();

        for (BigResc time = start_time/60; time < end_time/60; time++)
        {
            p_cpu_consumption[time] += j->get_cpu_req();
            p_ram_consumption[time] += j->get_ram_req();
        }
    }
}

void user::calculate_deadline_count(BigResc end_time, DeadlineStyle style)
{
    p_deadline_count.clear();
    p_deadline_count.resize((end_time+1)/60);


    for (size_t i = 0; i < p_jobs.size(); i++)
    {
        shared_ptr<job> j = p_jobs[i];
        if (!j->deadline_satisfied())
        {
            BigResc start_time = j->get_arrival() + j->get_waittime() - j->deadline_diff();
            BigResc end_time = j->get_arrival() + j->get_waittime();

            if (style == DeadlineAfterEnd)
            {
                start_time += j->get_runtime();
                end_time += j->get_runtime();
            }

            for (BigResc time = start_time/60; time < end_time/60; time++)
            {
                p_deadline_count[time] += 1;
            }
        }
    }
}

void user::write_jobs(std::ostream &file)
{
    for (size_t i = 0; i < p_jobs.size(); i++)
    {
        job* j = p_jobs[i].get();

        file << j->get_jobid() << "\t" << j->get_arrival() << "\t" << j->get_waittime() << "\t"
             << j->get_runtime() << "\t" << j->get_cpu_req() << "\t" << j->get_ram_req() << "\t"
             << j->get_userid() << "\t" << (j->deadline_satisfied()?0:1) << "\t" << j->deadline_diff() << endl;
    }
}
