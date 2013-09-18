#include "job_test.h"

void job_test::empty()
{
    QCOMPARE("abc","abc");
}

QTEST_MAIN(job_test)
//#include "job_test.moc"
