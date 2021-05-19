#include <stdio.h>
#include "../../test.h"
#include "../../../preprocessor/preprocessor.h"


int testReduceKCNFDuplicates(FILE* testLog);
int testReduceKCNFPure(FILE* testLog);
int testReduceKCNFAllPure(FILE* testLog);
int testReduceKCNFStandard(FILE* testLog);

int testToKCNFLarger(FILE* testLog);
int testToKCNFSmaller(FILE* testLog);
int testToKCNFWithK2(FILE* testLog);

static TestCase PREPROCESSOR_TEST_CASE_ARR[] = {
    {"Test Reduce KCNF With Redundant Clauses/Literals", testReduceKCNFDuplicates},
    {"Test Reduce KCNF With Pure Literals", testReduceKCNFPure},
    {"Test Reduce KCNF With All Pure Literals", testReduceKCNFAllPure},
    {"Test Reduce KCNF With Standard Sentence", testReduceKCNFStandard},
    {"Test To KCNF For Larger K", testToKCNFLarger},
    {"Test To KCNF From K=5 To K=3", testToKCNFSmaller},
    {"Test To KCNF For Invalid K=2", testToKCNFWithK2},
    END_OF_TEST_CASES
};

static TestGroup PREPROCESSOR_TESTS = {
    "Preprocessor Tests",
    PREPROCESSOR_TEST_CASE_ARR
};

int testReduceKCNFDuplicates(FILE* testLog){

    int sentence[] = {
        1, -2, -3,  3,  3,  3, -2, 0,
        0,  1,  2,  3,  4, -5,  5, 5,
        0,  0,  0,  0,  0,  0,  0, 0,
        0, -1,  2,  3,  2, -1,  0, 0,
        0, -1,  2,  3,  3,  3,  3, 3,
    };

    int numClauses = 5;
    int xlatMap[6];

    reduceKCNF(sentence, &numClauses, 8, xlatMap);

    ASSERT_INT_EQUAL(numClauses, 0);
    ASSERT(xlatMap[1] == TRUE_XLAT_ASSIGNMENT || xlatMap[1] == FALSE_XLAT_ASSIGNMENT);
    ASSERT(xlatMap[2] == TRUE_XLAT_ASSIGNMENT || xlatMap[2] == FALSE_XLAT_ASSIGNMENT);
    ASSERT(xlatMap[3] == TRUE_XLAT_ASSIGNMENT || xlatMap[3] == FALSE_XLAT_ASSIGNMENT);
    ASSERT(xlatMap[4] == TRUE_XLAT_ASSIGNMENT || xlatMap[4] == FALSE_XLAT_ASSIGNMENT);
    ASSERT(xlatMap[5] == TRUE_XLAT_ASSIGNMENT || xlatMap[5] == FALSE_XLAT_ASSIGNMENT);

    ASSERT(xlatMap[1] == FALSE_XLAT_ASSIGNMENT 
        || xlatMap[2] == TRUE_XLAT_ASSIGNMENT
        || xlatMap[3] == TRUE_XLAT_ASSIGNMENT );
    return PASS;
}

int testReduceKCNFPure(FILE* testLog){

    int sentence[] = {
        -1, 2, -3,
        4, -1, -3, 
        -3, 4, -1,
        -5, -7, 0,
        -7, 8, -9,
        10, 0, 0,
        11, 0, 0,
    };

    int xlatMap[12];
    int numClauses = 7;

    reduceKCNF(sentence,&numClauses,3,xlatMap);

    ASSERT_INT_EQUAL(numClauses, 0);

    ASSERT(xlatMap[1] == FALSE_XLAT_ASSIGNMENT);
    ASSERT(xlatMap[3] == FALSE_XLAT_ASSIGNMENT);
    ASSERT(xlatMap[5] == FALSE_XLAT_ASSIGNMENT);
    ASSERT(xlatMap[7] == FALSE_XLAT_ASSIGNMENT);
    ASSERT(xlatMap[10] == TRUE_XLAT_ASSIGNMENT);
    ASSERT(xlatMap[11] == TRUE_XLAT_ASSIGNMENT);

    return PASS;
}

int testReduceKCNFAllPure(FILE* testLog){

    int sentence[] = {
       -1,   2,  3,    4,
       -1,   4,  4,   -4,
        2,   4,   3,  -1,
       -2,   1,   0,   0,
        0,   1,  -2,   0,
       -2,   4,  -3,   0,
       -5,  -2,   4,  -3,
        1,   5,   5,  -3,
        1,   5,  -3,   0,
        1,   2,   2,   3,
       -1,   0,   3,   0,
       -6,   0,   0,  -1
    };

    int xlatMap[7];
    int numClauses = 12;

    reduceKCNF(sentence,&numClauses,4,xlatMap);

    ASSERT_INT_EQUAL(numClauses, 0);

    ASSERT(xlatMap[1] == TRUE_XLAT_ASSIGNMENT
        || xlatMap[2] == FALSE_XLAT_ASSIGNMENT
        || xlatMap[6] == FALSE_XLAT_ASSIGNMENT);
    ASSERT(xlatMap[3] == TRUE_XLAT_ASSIGNMENT);
    ASSERT(xlatMap[4] == TRUE_XLAT_ASSIGNMENT);
    ASSERT(xlatMap[5] == TRUE_XLAT_ASSIGNMENT);

    return PASS;
}

int testReduceKCNFStandard(FILE* testLog){
    
    int sentence[] = {
        6,  2,  3,  4,
       -2,  2,  6,  1,
        6, -3, -6, -4,
        6, -6, -3, -4,
        2, -3,  3,  5,
        5,  2,  6,  2,
       -2,  6,  3,  6,
       -5, -3, -6,  2,
        0,  0,  0,  0,
       -1, -2,  6,  3 
    };

    int finalSentence[] = {
        -1, 2, 4, 0,
        1, -2, -3, -4,
        1, 3, 4, 0
    };

    int xlatMap[7];
    int numClauses = 10;

    reduceKCNF(sentence,&numClauses,4,xlatMap);

    ASSERT_INT_EQUAL(numClauses, 3);

    ASSERT(xlatMap[1] == FALSE_XLAT_ASSIGNMENT);
    ASSERT(xlatMap[4] == TRUE_XLAT_ASSIGNMENT);

    ASSERT_INT_EQUAL(xlatMap[2], 1);
    ASSERT_INT_EQUAL(xlatMap[3], 2);
    ASSERT_INT_EQUAL(xlatMap[5], 3);
    ASSERT_INT_EQUAL(xlatMap[6], 4);

    for(int i = 0; i < 12; ++i){
        ASSERT_INT_EQUAL(sentence[i], finalSentence[i])
    }

    return PASS;
}

int testToKCNFLarger(FILE* testLog){
    int sentence[24] = {
        1, 2, 3,
        -4, -5, -6,
        -7, -8, -9,
        10, 11, 12
    };
    int numClauses = 4;

    /* try with a capacity too small */
    ASSERT(!toKCNF(sentence, &numClauses, 23, 3, 6));
    ASSERT(!toKCNF(sentence, &numClauses, 24, 3, 7));

    /* try with a sufficiently large capacity */
    ASSERT(toKCNF(sentence, &numClauses, 24, 3, 6));

    int largerSentence[24] = {
        1, 2, 3,     0, 0, 0,
        -4, -5, -6,  0, 0, 0,
        -7, -8, -9,  0, 0, 0,
        10, 11, 12,  0, 0, 0
    };

    for(int i = 0; i < 24; ++i){
        ASSERT_INT_EQUAL(largerSentence[i],sentence[i]);
    }

    return PASS;
}

int testToKCNFSmaller(FILE* testLog){

    int sentence[45] = {
        1, 2, 3, -4, -5, -6, 100,
        -7, -8, -9,  10, 11, 12, 100,
        -13, -14, -15, 16, 17, 18, 100,
    };

    int reducedSentence[45] = {
        0,   1,  2,    3, -101, /**/ 101, -4, -5, -6, -102, /**/ 102, 100, 0, 0, 0,
        0,  -7, -8,   -9, -103, /**/ 103, 10, 11, 12, -104, /**/ 104, 100, 0, 0, 0,
        0, -13, -14, -15, -105, /**/ 105, 16, 17, 18, -106, /**/ 106, 100, 0, 0, 0
    };
    int numClauses = 3;

    ASSERT(toKCNF(sentence, &numClauses, 1000, 7, 5));
    ASSERT_INT_EQUAL(numClauses, 9);
    for(int i = 0; i < 45; ++i){
        ASSERT_INT_EQUAL(reducedSentence[i], sentence[i]);
    }

    return PASS;
}

int testToKCNFWithK2(FILE* testLog){
    
    int sentence[24] = {
        1, 2, 3,
        -4, -5, -6,
        -7, -8, -9,
        10, 11, 12
    };
    int numClauses = 4;

    ASSERT(!toKCNF(sentence, &numClauses, 24, 3, 2));
    ASSERT(!toKCNF(sentence, &numClauses, 24, 3, 1));
    return PASS;
}