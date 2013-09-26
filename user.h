#ifndef USER_H
#define USER_H

#include <memory>
#include <vector>
#include "job.h"
#include "resource_plan.h"

enum DeadlineStyle { DeadlineBeforeStart, DeadlineAfterEnd };

class user
{
public:
    user(std::string userid, BigResc cpu_capacity, BigResc ram_capacity);

    /** \brief Access jobs owned by this user */
    std::shared_ptr<job> operator[] (int index) const;
    int get_jobcount() const;

    static void add_job(std::shared_ptr<user> u, std::shared_ptr<job> j);

    void sort_jobs();

    std::string get_userid() const { return p_userid; }

    void calculate_deadlines();
    void calculate_consumptions(BigResc end_time);
    void calculate_deadline_count(BigResc end_time, DeadlineStyle style);

    const std::vector<BigResc>& get_cpu_consumption() const { return p_cpu_consumption; }
    const std::vector<BigResc>& get_ram_consumption() const { return p_ram_consumption; }
    const std::vector<int>& get_deadline_count() const { return p_deadline_count; }

    TinyResc violated_deadline_count() const { return p_violated_deadline_count; }
    BigResc total_violated_deadline_time() const { return p_total_positive_deadline; }
    BigResc total_deadline_diff_time() const { return p_total_deadline; }

    BigResc get_last_end_time() { return p_last_end_time; }

    void write_jobs(std::ostream& f);

private:
    std::string p_userid  = "--";
    std::vector< std::shared_ptr<job> > p_jobs;

    std::vector<BigResc> p_cpu_consumption;
    std::vector<BigResc> p_ram_consumption;
    std::vector<int>     p_deadline_count;

    BigResc p_cpu_capacity;
    BigResc p_ram_capacity;

    TinyResc p_violated_deadline_count = 0;
    BigResc p_total_positive_deadline = 0;
    BigResc p_total_deadline = 0;

    BigResc p_last_end_time;

};

#endif // USER_H
