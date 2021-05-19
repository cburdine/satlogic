#include <stdio.h>
#include "../../test_types.h"
#include "../../test_assert.h"

#include "../../../stochastic/stochastic_structures.h"

int testClauseMap(FILE*);

static TestCase STOCHASTIC_STRUCTURES_TEST_CASE_ARR[] = {
    {"Test ClauseMap", testClauseMap},
    END_OF_TEST_CASES
};

static TestGroup STOCHASTIC_STRUCTURES_TESTS = {
    "Stochastic Structures Tests",
    STOCHASTIC_STRUCTURES_TEST_CASE_ARR
};

int testClauseMap(FILE* testLog){
    ClauseMap clauseMap;
    int numClauses = 3;
    int sentence[] = { -1, 2, 3,    2, -1, 4,    5, -8, 2 };

    init3CNFClauseMap(&clauseMap, sentence, numClauses);
    
    ASSERT_INT_EQUAL(clauseMap.numVariables, 8);
    ASSERT_INT_EQUAL(clauseMap.numClauses, numClauses);

    ASSERT_INT_EQUAL(clauseMap.mapLen[1], 2);
    ASSERT_INT_EQUAL(clauseMap.mapLen[2], 3);
    ASSERT_INT_EQUAL(clauseMap.mapLen[3], 1);

    ASSERT_INT_EQUAL(clauseMap.mapLen[6], 0);
    ASSERT_INT_EQUAL(clauseMap.mapLen[7], 0);
    ASSERT_INT_EQUAL(clauseMap.mapLen[8], 1);

    ASSERT_INT_EQUAL(clauseMap.map[1][0], -1);
    ASSERT_INT_EQUAL(clauseMap.map[1][1], -2);
    ASSERT_INT_EQUAL(clauseMap.map[2][0], 1);
    ASSERT_INT_EQUAL(clauseMap.map[2][1], 2);
    ASSERT_INT_EQUAL(clauseMap.map[2][2], 3);

    for(int i = 0; i < 9; ++i){
        ASSERT_INT_EQUAL(clauseMap.sentence[i], sentence[i]);
    }

    destroyClauseMap(&clauseMap);

    return PASS;
}
