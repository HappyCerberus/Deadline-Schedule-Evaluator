#include "resource_plan.h"
#include <algorithm>
#include <cassert>
#include <iostream>
using namespace std;

resource_plan::resource_plan(BigResc capacity) : p_capacity(capacity)
{
    p_gaps.insert(resource_gap(0,-1,capacity));
}

BigResc resource_plan::consume(BigResc start, BigResc length, BigResc consumption)
{
    BigResc remaining_capacity = length * consumption;
    BigResc deadline = p_consume(start,consumption,remaining_capacity, true);
    return deadline;
}

BigResc resource_plan::p_consume(BigResc start, BigResc max_consumption, BigResc &capacity, bool first)
{
    auto iter = p_gaps.upper_bound(start);
    if (iter == p_gaps.end())
    {
        --iter;
        assert(iter->get_length() == -1);
    }

    auto prev = iter;
    --prev;
    if (prev->get_start() + prev->get_length() > start)
    {
        iter = prev;
    }

#if 0
    auto i = p_gaps.begin();
    while(i != p_gaps.end())
    {
        if (i->get_start() + i->get_length() > start || i->get_length() == -1)
            break;
        i++;
    }

    assert(i != p_gaps.end());
#endif

    resource_gap gap = *iter;
    p_gaps.erase(iter);

    if (gap.has_capacity_tail(start,max_consumption,capacity))
    {
        p_gaps.insert(gap.get_tail_gap(start,max_consumption,capacity));
    }

    if (!first && gap.starts_before(start))
    {
        p_gaps.insert(gap.get_head_gap(start));
    }

    if (gap.provides_more_capacity(max_consumption))
    {
        p_gaps.insert(gap.get_mid_gap(start,max_consumption,capacity));
    }

    BigResc deadline = gap.consume(start,max_consumption,capacity);

    if (capacity <= 0)
        return deadline;

    return p_consume(deadline,max_consumption,capacity,false);
}

ostream& operator << (ostream& s, const resource_plan& plan)
{
    for (auto v : plan.p_gaps)
    {
        s << v << endl;
    }
    return s;
}
