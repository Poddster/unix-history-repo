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
#include "ntp_workimpl.h"
#include "ntp_types.h"
#include "sntptest.h"
#include "ntp_stdlib.h"
#include "sntp-opts.h"
#include "kod_management.h"
#include "ntp_io.h"

//=======External Functions This Runner Calls=====
extern void setUp(void);
extern void tearDown(void);
extern void test_SingleEntryHandling(void);
extern void test_MultipleEntryHandling(void);
extern void test_NoMatchInSearch(void);
extern void test_AddDuplicate(void);
extern void test_DeleteEntry(void);


//=======Suite Setup=====
static void suite_setup(void)
{
extern int change_iobufs(int);
extern int change_logfile(const char*, int);
change_iobufs(1);
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
  UnityBegin("kodDatabase.c");
  RUN_TEST(test_SingleEntryHandling, 14);
  RUN_TEST(test_MultipleEntryHandling, 15);
  RUN_TEST(test_NoMatchInSearch, 16);
  RUN_TEST(test_AddDuplicate, 17);
  RUN_TEST(test_DeleteEntry, 18);

  return (UnityEnd());
}
