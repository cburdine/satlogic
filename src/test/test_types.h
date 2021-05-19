#ifndef C_UNIT_TEST_TYPES_H
#define C_UNIT_TEST_TYPES_H

#include <stdio.h>

const int PASS = 1;
const int FAIL = 0;

/* This represents a single unit test. Each unit test takes in a 
   log file ptr as a parameter and returns the PASS/FAIL value above
*/
typedef struct TestCase {
    char* name;
    int(*testCase)(FILE*);
} TestCase;

/* This struct represents a group of sentinel-terminated test cases,
   typically grouped by feature. You must end the 'testCases' list with 
   All of the unit tests for a single test group should be contained in a 
   single .h file.
   END_OF_TEST_CASES
*/
typedef struct TestGroup {
    char* groupName;
    TestCase* testCases;
} TestGroup;
#define END_OF_TEST_CASES {0,0}


/*  This type is a sentinel-terminated array of test cases. You
    You must end the 'testGroups' list with END_OF_MANIFEST.

    The global manifest of all test cases for a project, TEST_GROUP_MANIFEST,
    is declared in manifest.h
*/
typedef TestGroup** TestGroupManifest;
#define END_OF_MANIFEST  NULL


#endif /* C_UNIT_TEST_TYPES_H */