#include <stdio.h>
#include "../test_types.h"
#include "../test_assert.h"

#include "../../structures.h"


int testSentenceStack(FILE*);
int testLiteralInstanceSet(FILE*);
int testLiteralToClauseMap(FILE*);
int testVSIDSMap(FILE*);
int testLiteralAssignmentStack(FILE*);

static TestCase STRUCTURES_TEST_CASE_ARR[] = {
    /*{"Test SentenceStack", testSentenceStack},*/
    /*{"Test Literal Instance Set", testLiteralInstanceSet},*/
    /*{"Test Literal To Clause Map", testLiteralInstanceSet},*/
    {"Test VSIDS Map", testVSIDSMap},
    /*{"Test Literal Assignment Stack", testLiteralAssignmentStack},*/
    END_OF_TEST_CASES
};

static TestGroup STRUCTURES_TESTS = {
    "Structures Tests",
    STRUCTURES_TEST_CASE_ARR
};

int testSentenceStack(FILE* testLog){
    return PASS;
}

int testLiteralInstanceSet(FILE* testLog){
    return PASS;
}

int testLiteralToClauseMap(FILE* testLog){
    return PASS;
}

int testVSIDSMap(FILE* testLog){
    const int bumpSequence[18] = {
        1, 3, 4, 2, 6, 5, 7, 8, 5, 7, 3, 3, 3, 3, 2, 3, 1, 8
    };

    const int enableSequence[16] = {
        -1, -2, -3, -4, -5, -6, -7, -8, 1, 3, 5, 7, 2, 4, 6, 8
    };

    int i;
    VSIDSMap vsidsMap;
    Clause bumpClause = { {0, 0, 0}, {TRUE, TRUE, TRUE}, 3};

    initVSIDSMap(&vsidsMap, 8, 0.95, 1.0);
    ASSERT(isValidVSIDSMap(&vsidsMap));

    for(i = 0; i < 16; ++i){
        bumpClause.literals[0] = bumpSequence[i];
        bumpClause.literals[1] = bumpSequence[i+1];
        bumpClause.literals[2] = bumpSequence[i+2];
        
        bumpConflictClause(&vsidsMap, &bumpClause);

        
        if(enableSequence[i] < 0){
            deactivateVariable(&vsidsMap, abs(enableSequence[i]));
        } else {
            reactivateVariable(&vsidsMap, abs(enableSequence[i]));
        }
        

        /* printVSIDSMap(&vsidsMap, testLog); */
        
        ASSERT(isValidVSIDSMap(&vsidsMap));
    }

    /* ensure 3 is at the top of the VSIDS score list */
    ASSERT_INT_EQUAL(vsidsMap.scorePQ[0], 3);

    destroyVSIDSMap(&vsidsMap);

    return PASS;
}

int testLiteralAssignmentStack(FILE* testLog){
    return PASS;
}

