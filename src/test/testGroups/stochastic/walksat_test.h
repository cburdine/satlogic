#include <stdio.h>
#include "../../test_types.h"
#include "../../test_assert.h"

#include "../../../stochastic/walksat.h"
#include "../../../stochastic/stochastic_structures.h"

int testUprop(FILE* testLog);
int testRedundantSentence(FILE* testLog);
int testUnsat(FILE* testLog);

static TestCase WALKSAT_TEST_CASE_ARR[] = {
    {"Small Unit Prop SAT Problem", testUprop},
    {"Small UnSAT Problem", testUnsat},
    {"Sentence With Redundant Literals", testRedundantSentence},
    END_OF_TEST_CASES
};

static TestGroup WALKSAT_TESTS = {
    "WalkSAT Tests",
    WALKSAT_TEST_CASE_ARR
};


int testUprop(FILE* testLog){

    int sentence[] = {
        -1,     2,      3,
        -1,     2,      -3,
        -1,     -2,     3,
        -1,     -2,     -3,
        -4,     5,      6,
        -4,     5,      -6,
        -4,     -5,     6,
        -4,     -5,     -6,
        -7,      4,      1 
    };

    Bool solnArr[] = { 0, 
        1,1,1,1,1,1,1 
    };

    int maxSteps = 1000000;

    if(!walksat3CNF(sentence, 9, solnArr, maxSteps, 0.01)){
        fprintf(testLog, "\nWalksat failed to find a solution after %d steps.\n", maxSteps);
        fprintf(testLog, "Either this is a highly unlikely event or something is wrong.\n");
        return FAIL;
    }

    ASSERT_INT_EQUAL(solnArr[0], 0);
    ASSERT_INT_EQUAL(solnArr[1], 0);
    ASSERT_INT_EQUAL(solnArr[4], 0);
    ASSERT_INT_EQUAL(solnArr[7], 0);

    return PASS;
}

int testUnsat(FILE* testLog){

    int sentence[] = {
        1,      2,      3,
        1,      2,      -3,
        1,      -2,     3,
        1,      -2,     -3,
        -1,     2,      3,
        -1,     2,      -3,
        -1,     -2,     3,
        -1,     -2,     -3,
    };

    Bool solnArr[] = { 0, 
        1,1,1,
    };

    int maxSteps = 1000000;

    ASSERT(walksat3CNF(sentence, 8, solnArr, maxSteps, 0.1) == FALSE);

    return PASS;
}

int testRedundantSentence(FILE* testLog){

    /*
        The pure assignments 5 = F and 4 = T are trivial.
        The unique solution the subsentence after this assignment
        is 1 = T, 2 = F, 3 = T.
    */
    int sentence[] = {
        1,  2,  3,  0,
        1,  2,  4,  4,
        1,  2,  2, -3,
        1, -2,  3,  3,
        1, -2, -3,  1,
       -1,  2,  3, -1,
       -1,  2,  3, -1,
       -1,  0, -2,  3,
       -1, -2,  3,  0,
       -1, -2, -3,  0,
        0, -3, -1, -2,
        -5, -4,  3,  2,
    };

    Bool solnArr[] = { 0, 
        0, 0, 1, 0, 1
    };

    int maxSteps = 1000000;

    ASSERT(walksatKCNF(sentence, 12, 4, solnArr, maxSteps, 0.01));
    ASSERT(!solnArr[0] && "solnArr[0] should be cleared");
    ASSERT(solnArr[1] == TRUE);
    ASSERT(solnArr[2] == FALSE);
    ASSERT(solnArr[3] == TRUE);
    ASSERT(solnArr[4] == TRUE);
    ASSERT(solnArr[5] == FALSE);

    return PASS;    
}
