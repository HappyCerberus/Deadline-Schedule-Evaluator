#include "progressanim.h"
#include <iostream>
using namespace std;

progressanim::progressanim() : status(0)
{
    cout << "[" << visual[status] << "]" << flush;
}

progressanim::~progressanim()
{
    cout << "\b\b\b[ DONE ]" << endl;
}

void progressanim::tick()
{
    status = (status + 1) % 4;
    cout << "\b\b\b[" << visual[status] << "]" << flush;
}
