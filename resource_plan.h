#ifndef RESOURCE_PLAN_H
#define RESOURCE_PLAN_H

#include <set>
#include <iosfwd>
#include "resource_gap.h"

class resource_plan;
std::ostream& operator << (std::ostream& s, const resource_gap& gap);

class resource_plan
{
public:
    resource_plan(BigResc capacity);
    BigResc consume(BigResc start, BigResc length, BigResc consumption);

    friend std::ostream& operator << (std::ostream& s, const resource_plan& plan);

private:
    std::set<resource_gap> p_gaps;
    BigResc p_capacity;

    BigResc p_consume(BigResc start, BigResc max_consumption, BigResc& capacity);
};

#endif // RESOURCE_PLAN_H
