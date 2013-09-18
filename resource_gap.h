#ifndef RESOURCE_GAP_H
#define RESOURCE_GAP_H

#include <iosfwd>
#include "resources.h"

class resource_gap;
std::ostream& operator <<(std::ostream& s, const resource_gap& r);

class resource_gap
{
public:
    resource_gap(BigResc start_time = 0, BigResc duration = -1, BigResc capacity = -1);

    /** Check whether resource gap starts before specified time */
    bool starts_before(BigResc start_time);
    /** Check whether resource gap provides more resources than requested */
    bool provides_more_capacity(BigResc max_consumption);
    /** Check whether the consumption will cover the resource gap until its end */
    bool has_capacity_tail(BigResc start_time, BigResc max_consumption, BigResc remaining_capacity);

    resource_gap get_head_gap(BigResc start_time);
    resource_gap get_mid_gap(BigResc start_time, BigResc max_consumption, BigResc remaining_capacity);
    resource_gap get_tail_gap(BigResc start_time, BigResc max_consumption, BigResc remaining_capacity);

    BigResc consume(BigResc start, BigResc max_consumption, BigResc& remaining_capacity);

    friend std::ostream& operator <<(std::ostream& s, const resource_gap& r);

    bool operator < (const resource_gap& g) const;

    BigResc get_start() const { return p_start; }
    BigResc get_length() const { return p_length; }
    BigResc get_capacity() const { return p_capacity; }

private:
    BigResc p_start;
    BigResc p_length;
    BigResc p_capacity;
};

#endif // RESOURCE_GAP_H
