#include <stdio.h>
#include "../test.h"
#include "../../dpll.h"
#include "../../structures.h"


int testSimpleSAT(FILE* testLog);
int testSimpleUnSAT(FILE* testLog);
int testSimpleSat2(FILE* testLog);

static TestCase DPLL_TEST_CASE_ARR[] = {
    {"Simple SAT Problem", testSimpleSAT},
    {"Simple SAT Problem 2", testSimpleSat2},
    {"Simple UnSAT Problem", testSimpleUnSAT},
    END_OF_TEST_CASES
};

static TestGroup DPLL_TESTS = {
    "DPLL Tests",
    DPLL_TEST_CASE_ARR
};


int testSimpleSAT(FILE* testLog){
    
    int simpleSatVars[24] = {
         1,  2,  3,
        -3,  2,  4,
         1, -2,  3,
         1,  2, -3,
        -1, -5,  6,
         1, -2, -3,
        -1,  3, -3,
        -2, -2, -3
    };

    Bool solnArr[7];

    ASSERT(dpll3Sat(simpleSatVars, 8, solnArr) == TRUE);

    return PASS;
}

int testSimpleSat2(FILE* testLog){
    int simpleUnSatVars[24] = {
         1,  2, -3,
        -3,  2,  1,
         4,  2, -3,
         2,  1, -4,
        -4,  5, -3,
         3,  4,  1,
         2,  5, -1,
         3,  2, -5
    };

    Bool solnArr[6];

    ASSERT(dpll3Sat(simpleUnSatVars, 8, solnArr) == TRUE);

    return PASS;
}

int testSimpleUnSAT(FILE* testLog){
    
    int simpleUnSatVars[24] = {
         1,  2,  3,
        -1,  2,  3,
         1, -2,  3,
         1,  2, -3,
        -1, -2,  3,
         1, -2, -3,
        -1,  2, -3,
        -1, -2, -3
    };

    Bool solnArr[4];

    ASSERT(dpll3Sat(simpleUnSatVars, 8, solnArr) == FALSE);

    return PASS;
}