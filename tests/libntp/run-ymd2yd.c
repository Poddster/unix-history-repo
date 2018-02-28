/* AUTOGENERATED FILE. DO NOT EDIT. */

//=======Test Runner Used To Run Each Test Below=====
#define RUN_TEST(TestFunc, TestLineNum) \
{ \
  Unity.CurrentTestName = #TestFunc; \
  Unity.CurrentTestLineNumber = TestLineNum; \
  Unity.NumberOfTests++; \
  if (TEST_PROTECT()) \
  { \
      setUp(); \
      TestFunc(); \
  } \
  if (TEST_PROTECT() && !TEST_IS_IGNORED) \
  { \
    tearDown(); \
  } \
  UnityConcludeTest(); \
}

//=======Automagically Detected Files To Include=====
#include "unity.h"
#include <setjmp.h>
#include <stdio.h>
#include "config.h"
#include "ntp_stdlib.h"

//=======External Functions This Runner Calls=====
extern void setUp(void);
extern void tearDown(void);
extern void test_NonLeapYearFebruary(void);
extern void test_NonLeapYearJune(void);
extern void test_LeapYearFebruary(void);
extern void test_LeapYearDecember(void);


//=======Suite Setup=====
static void suite_setup(void)
{
extern int change_logfile(const char*, int);
change_logfile("stderr", 0);
}

//=======Test Reset Option=====
void resetTest(void);
void resetTest(void)
{
  tearDown();
  setUp();
}

char const *progname;


//=======MAIN=====
int main(int argc, char *argv[])
{
  progname = argv[0];
  suite_setup();
  UnityBegin("ymd2yd.c");
  RUN_TEST(test_NonLeapYearFebruary, 7);
  RUN_TEST(test_NonLeapYearJune, 8);
  RUN_TEST(test_LeapYearFebruary, 9);
  RUN_TEST(test_LeapYearDecember, 10);

  return (UnityEnd());
}
