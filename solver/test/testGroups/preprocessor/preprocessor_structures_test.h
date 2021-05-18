#include <stdio.h>
#include "../../test.h"
#include "../../../preprocessor/preprocessor_structures.h"


int testInitOrderedClauseZeros(FILE* testLog);

int testInitOrderedClauseDuplicates(FILE* testLog);

static TestCase PREPROCESSOR_STRUCTURES_TEST_CASE_ARR[] = {
    {"Test Ordered Clause With Zeros", testInitOrderedClauseZeros},
    {"Test Ordered Clause With Duplicates", testInitOrderedClauseDuplicates},
    END_OF_TEST_CASES
};

static TestGroup PREPROCESSOR_STRUCTURES_TESTS = {
    "Preprocessor Structures Tests",
    PREPROCESSOR_STRUCTURES_TEST_CASE_ARR
};

int testInitOrderedClauseZeros(FILE* testLog){
    OrderedClause oc;

    int clause[] = { 0, 0, -1, 0, 2, 5, 0, 4, 0, 0, 1, 3 ,0};
    int clauseLen = 13;

    initOrderedClause(&oc, clause, clauseLen);

    int reducedClause[] = { -1, 1, 2, 3, 4, 5 };

    ASSERT_INT_EQUAL(oc.size, 6);
    ASSERT_INT_EQUAL(reducedClause[0],oc.literals[0]);
    ASSERT_INT_EQUAL(reducedClause[1],oc.literals[1]);
    ASSERT_INT_EQUAL(reducedClause[2],oc.literals[2]);
    ASSERT_INT_EQUAL(reducedClause[3],oc.literals[3]);
    ASSERT_INT_EQUAL(reducedClause[4],oc.literals[4]);
    ASSERT_INT_EQUAL(reducedClause[5],oc.literals[5]);

    destroyOrderedClause(&oc);

    return PASS;
}

int testInitOrderedClauseDuplicates(FILE* testLog){
    OrderedClause oc;

    int clause[] = { 1, 2, 2, 2, -2, -2, 3, 4, 4, -4,-5, -5, 0, -6, 0, 0 };
    int clauseLen = 16;

    initOrderedClause(&oc, clause, clauseLen);

    int reducedClause[] = { 1, -2, 2, 3, -4, 4, -5, -6 };

    ASSERT_INT_EQUAL(oc.size, 8);
    ASSERT_INT_EQUAL(reducedClause[0],oc.literals[0]);
    ASSERT_INT_EQUAL(reducedClause[1],oc.literals[1]);
    ASSERT_INT_EQUAL(reducedClause[2],oc.literals[2]);
    ASSERT_INT_EQUAL(reducedClause[3],oc.literals[3]);
    ASSERT_INT_EQUAL(reducedClause[4],oc.literals[4]);
    ASSERT_INT_EQUAL(reducedClause[5],oc.literals[5]);
    ASSERT_INT_EQUAL(reducedClause[6],oc.literals[6]);
    ASSERT_INT_EQUAL(reducedClause[7],oc.literals[7]);

    destroyOrderedClause(&oc);

    return PASS;
}
