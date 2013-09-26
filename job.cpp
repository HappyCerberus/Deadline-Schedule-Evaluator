#include "job.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
using namespace std;

bool job::read_jobid(istream& s)
{
    s >> p_jobid;
    return s.good();
}

bool job::read_arrival(std::istream& s)
{
    s >> p_arrival_time;
    return s.good();
}

bool job::read_waittime(std::istream& s)
{
    double waittime;
    s >> waittime;

    if (s.good())
        p_wait_time = static_cast<int>(waittime);

    return s.good();
}

bool job::read_runtime(std::istream& s)
{
    double runtime;
    s >> runtime;

    if (s.good())
    {
        p_run_time = static_cast<int>(runtime);
        if (p_run_time == 0)
            p_run_time = 1;
    }

    return s.good();
}

bool job::read_cpureq(std::istream& s)
{
    s >> p_cpu_req;

    if (p_cpu_req == 0)
        p_cpu_req = 1;

    return s.good();
}

bool job::read_ramreq(std::istream& s)
{
    s >> p_ram_req;
    p_ram_req /= 1024;

    if (p_ram_req == 0)
        p_ram_req = 400;

    return s.good();
}

bool job::read_userid(std::istream& s)
{
    s >> p_userid;

    return s.good();
}

bool job::read_queue(std::istream& s)
{
    s >> p_queue;

    return s.good();
}

ostream& operator << (ostream &s, const job& j)
{
    s << j.p_jobid << "\t" << j.p_arrival_time << "\t" << j.p_wait_time << "\t" << j.p_run_time << "\t" << j.p_cpu_req << "\t" << j.p_ram_req << "\t" << j.p_userid << endl;
    return s;
}

job::job(istream &s, const string& format)
{
    // JOBID ARRIVAL WAITTIME RUNTIME CPUREQ RAMREQ USERID QUEUE
    bool jobid = false, arrival = false, waittime = false, runtime = false,
         cpureq = false, ramreq = false, userid = false, queue = false;

    stringstream parser(format);
    string token;

    // PARSE INPUT ACCORDING TO FORMAT STRING
    while (parser >> token)
    {
        if (token == "JOBID")
        {
            if (read_jobid(s))
                jobid = true;
            else
                throw runtime_error("Couldn't parse jobid.");
        }
        if (token == "ARRIVAL")
        {
            if (read_arrival(s))
                arrival = true;
            else
                throw runtime_error("Couldn't parse arrival.");
        }
        if (token == "WAITTIME")
        {
            if (read_waittime(s))
                waittime = true;
            else
                throw runtime_error("Couldn't parse waittime.");
        }
        if (token == "RUNTIME")
        {
            if (read_runtime(s))
                runtime = true;
            else
                throw runtime_error("Couldn't parse runtime.");
        }
        if (token == "CPUREQ")
        {
            if (read_cpureq(s))
                cpureq = true;
            else
                throw runtime_error("Couldn't parse cpu req.");
        }
        if (token == "RAMREQ")
        {
            if (read_ramreq(s))
                ramreq = true;
            else
                throw runtime_error("Couldn't parse ram req.");
        }
        if (token == "USERID")
        {
            if (read_userid(s))
                userid = true;
            else
                throw runtime_error("Couldn't parse user id.");
        }
        if (token == "QUEUE")
        {
            if (read_queue(s))
                queue = true;
            else
                throw runtime_error("Couldn't parse queue.");
        }
    }

    // check missing values / setup defaults
    if (!jobid)
        throw runtime_error("JobID missing.");
    if (!arrival)
        throw runtime_error("Job arrival missing.");
    if (!waittime)
        throw runtime_error("Job wait time missing.");
    if (!runtime)
        throw runtime_error("Job runtime missing.");
    if (!cpureq)
        p_cpu_req = 1;
    if (!ramreq)
        p_ram_req = 409600;
    if (!userid)
        p_userid = 1;
    if (!queue)
        p_queue = "default";

    if (!s.good())
        throw runtime_error("Couldn't parse job line.");
}

void job::set_owner(shared_ptr<user> owner)
{
    p_owner = owner;
}

shared_ptr<user> job::get_owner() const
{
    return p_owner.lock();
}

std::string job::get_userid() const
{
    return p_userid;
}

std::string job::get_jobid() const
{
    return p_jobid;
}

void job::set_deadline(BigResc deadline)
{
    p_deadline = deadline;
}

bool job::deadline_satisfied() const
{
    return p_deadline >= p_arrival_time + p_wait_time + p_run_time;
}

BigResc job::deadline_diff() const
{
    return p_arrival_time + p_wait_time + p_run_time - p_deadline;
}

bool job::operator < (const job& j) const
{
    return p_arrival_time < j.p_arrival_time;
}

BigResc job::get_cpu_req() const { return p_cpu_req; }
BigResc job::get_ram_req() const { return p_ram_req; }

BigResc job::get_arrival() const { return p_arrival_time; }
BigResc job::get_waittime() const { return p_wait_time; }
BigResc job::get_runtime() const { return p_run_time; }

BigResc job::get_start() const { return p_arrival_time + p_wait_time; }
BigResc job::get_finish() const { return this->get_start() + p_run_time; }
