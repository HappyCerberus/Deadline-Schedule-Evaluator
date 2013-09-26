#ifndef JOB_H
#define JOB_H

#include <iosfwd>
#include <memory>
#include <string>
#include "resources.h"

class job;
class user;
std::ostream& operator << (std::ostream &s, const job& j);

class job
{
public:
    job(std::istream &s, const std::string& format);
    job(const job&) = default;

    friend std::ostream& operator << (std::ostream &s, const job& j);

    void set_owner(std::shared_ptr<user> owner);
    std::shared_ptr<user> get_owner() const;

    std::string get_userid() const;
    std::string get_jobid() const;

    void set_deadline(BigResc deadline);

    bool deadline_satisfied() const;
    BigResc deadline_diff() const;

    /* sorted by arrival */
    bool operator < (const job& j) const;

    /* arrival time */
    BigResc get_arrival() const;
    /* wait time */
    BigResc get_waittime() const;
    /* run time */
    BigResc get_runtime() const;

    /* start time = arrival + wait */
    BigResc get_start() const;
    /* finish time = arrival + wait + run */
    BigResc get_finish() const;

    /* cpu requirements */
    BigResc get_cpu_req() const;
    /* memory requirements */
    BigResc get_ram_req() const;

    std::string get_queue() const { return p_queue; }

private:
    std::string p_jobid;
    std::string p_userid;

    BigResc p_arrival_time = -1;
    BigResc p_wait_time = -1;
    BigResc p_run_time = -1;

    BigResc p_cpu_req = -1;
    BigResc p_ram_req = -1;

    BigResc p_deadline = -1;

    std::weak_ptr<user> p_owner;

    std::string p_queue;

    friend class job_test;

    bool read_jobid(std::istream& s);
    bool read_arrival(std::istream& s);
    bool read_waittime(std::istream& s);
    bool read_runtime(std::istream& s);
    bool read_cpureq(std::istream& s);
    bool read_ramreq(std::istream& s);
    bool read_userid(std::istream& s);
    bool read_queue(std::istream& s);
};

#endif // JOB_H
