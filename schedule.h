#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "resources.h"
#include "user.h"

#include <map>
#include <memory>
#include <iosfwd>
#include <string>

class schedule
{
public:
    schedule();

    void set_user_allowance(int user_id, BigResc cpu, BigResc ram);

    void process_file(std::istream& file, const std::string& format);

    BigResc get_last_end_time() { return p_last_end_time; }

    void write_jobs(std::ostream& file);
    void write_users(std::ostream& file);

    void calculate_consumption();
    void calculate_deadlines();

    void write_deadlines(std::ostream& file);
    void write_consumption(std::ostream& file);

private:
    std::map< int, std::shared_ptr<user> > p_user_db;
    std::map< int, std::pair<BigResc,BigResc> > p_allowance;

    BigResc p_def_cpu;
    BigResc p_def_ram;

    BigResc p_last_end_time;

    BigResc p_max_cpu_consumption;
    BigResc p_max_ram_consumption;
};

#endif // SCHEDULE_H
