#include "resource_gap.h"
#include <algorithm>
#include <iostream>
using namespace std;

resource_gap::resource_gap(BigResc start_time, BigResc duration, BigResc capacity) : p_start(start_time), p_length(duration), p_capacity(capacity)
{
}


bool resource_gap::starts_before(BigResc start_time)
{
    return start_time > p_start;
}

bool resource_gap::provides_more_capacity(BigResc max_consumption)
{
    return p_capacity == -1 || p_capacity > max_consumption;
}

bool resource_gap::has_capacity_tail(BigResc start_time, BigResc max_consumption, BigResc remaining_capacity)
{
    if (p_length == -1) return true;

    BigResc req = remaining_capacity;
    BigResc cap = min(max_consumption,p_capacity);
    BigResc req_length = req / cap + (req % cap != 0);

    return req_length + start_time < p_start + p_length;
}

resource_gap resource_gap::get_head_gap(BigResc start_time)
{
    return resource_gap(p_start,start_time-p_start,p_capacity);
}

resource_gap resource_gap::get_mid_gap(BigResc start_time, BigResc max_consumption, BigResc remaining_capacity)
{
    BigResc req = remaining_capacity;
    BigResc cap = min(max_consumption,p_capacity);
    BigResc req_length = req / cap + (req % cap != 0);

    return resource_gap(start_time,req_length,p_capacity-cap);
}

resource_gap resource_gap::get_tail_gap(BigResc start_time, BigResc max_consumption, BigResc remaining_capacity)
{
    BigResc req = remaining_capacity;
    BigResc cap = min(max_consumption,p_capacity);
    BigResc req_length = req / cap + (req % cap != 0);

    if (p_length == -1)
        return resource_gap(req_length+start_time,-1,p_capacity);
    else
        return resource_gap(req_length+start_time,p_length-req_length-(start_time-p_start),p_capacity);
}

bool resource_gap::operator < (const resource_gap& g) const
{
    return p_start < g.p_start;
}

BigResc resource_gap::consume(BigResc start, BigResc max_consumption, BigResc &remaining_capacity)
{
    BigResc real_start = max(start,p_start);
    BigResc real_consumption = min(max_consumption,p_capacity);
    BigResc max_length;

    BigResc length = remaining_capacity / real_consumption + (remaining_capacity % real_consumption != 0);
    if (p_length == -1)
        max_length = length;
    else
        max_length = (p_length + p_start) - real_start;
    int real_length = min(length,max_length);

    remaining_capacity -= real_length * real_consumption;

    return real_start + real_length;
}

ostream& operator <<(ostream& s, const resource_gap& r)
{
    s << "start: " << r.p_start << " end: " << (r.p_length == -1 ? -1 : r.p_start+r.p_length-1) << " capacity: " << r.p_capacity << endl;
    return s;
}
