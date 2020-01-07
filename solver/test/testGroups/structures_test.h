#include <stdio.h>
#include <stdlib.h>
#include "../test_types.h"
#include "../test_assert.h"

#include "../../structures.h"


int testSentenceStack(FILE*);
int testLiteralInstanceSet(FILE*);
int testLiteralToClauseMap(FILE*);
int testVSIDSMap(FILE*);
int testLiteralAssignmentStack(FILE*);

static TestCase STRUCTURES_TEST_CASE_ARR[] = {
    {"Test SentenceStack", testSentenceStack},
    /*{"Test Literal Instance Set", testLiteralInstanceSet},*/
    /*{"Test Literal To Clause Map", testLiteralInstanceSet},*/
    {"Test VSIDS Map", testVSIDSMap},
    {"Test Literal Assignment Stack", testLiteralAssignmentStack},
    END_OF_TEST_CASES
};

static TestGroup STRUCTURES_TESTS = {
    "Structures Tests",
    STRUCTURES_TEST_CASE_ARR
};

int testSentenceStack(FILE* testLog){
    
    int i, *newSentencePtr;
    SentenceStack stack;
    Clause cl = {{1, -2, 3}, {TRUE, TRUE, TRUE}, 3};
    Clause* tempClausePtr;
    Clause *sentence1 = malloc(sizeof(Clause)),
           *sentence2 = malloc(sizeof(Clause));
        
    sentence1[0] = cl;
    sentence2[0] = cl;

    initSentenceStack(&stack, 8, 3);

    ASSERT_INT_EQUAL(stack.top, -1);
    ASSERT_INT_GREATER_EQUAL_TO(stack.maxStackheight, 16);

    pushSentenceCopy(&stack, sentence1, 1);
    pushSentenceCopy(&stack, sentence2, 1);

    ASSERT_INT_EQUAL(stack.sentenceLengths[0], 1)
    ASSERT_INT_EQUAL(stack.sentenceLengths[stack.top], 1);

    newSentencePtr = pushNewEmptySentence(&stack, &tempClausePtr);
    ASSERT_INT_EQUAL(stack.sentenceLengths[stack.top], 0);
    
    *newSentencePtr = 25;

    ASSERT_INT_EQUAL(stack.sentenceLengths[stack.top], 25);
    ASSERT_INT_EQUAL(stack.top, 2);

    for(i = 0; i < 3; ++i){
        popSentence(&stack);
    }

    ASSERT_INT_EQUAL(stack.top, -1);

    destroySentenceStack(&stack);

    free(sentence1);
    free(sentence2);

    return PASS;
}

int testLiteralInstanceSet(FILE* testLog){

    /* TODO: write this test */

    return PASS;
}

int testLiteralToClauseMap(FILE* testLog){

    /* TODO: write this test */

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

    clearVSIDSMap(&vsidsMap);

    destroyVSIDSMap(&vsidsMap);

    return PASS;
}

int testLiteralAssignmentStack(FILE* testLog){
    
    int assignments[4][4] = {
        {1, 2, 3, 4},
        {-5, 6, -7, 8},
        {9, 10, -11, -12},
        {13, -14, 15, 16}
    };

    int i, j;
    LiteralAssignmentStack stack;

    initLiteralAssignmentStack(&stack, 16);

    ASSERT_INT_GREATER_EQUAL_TO(stack.maxNumVariables, 16);
    ASSERT_INT_EQUAL(stack.top, -1);

    for(i = 0; i < 4; ++i){
        pushNewFrame(&stack);
        for(j = 0; j < 4; ++j){
            addLiteral(&stack, assignments[i][j]);
        }
        setBranchLiteral(&stack, i+17);
        stack.onSecondBranch[stack.top] = (i > 0);

        ASSERT_INT_EQUAL(stack.numAssignments[stack.top], 4);
    }

    ASSERT_INT_EQUAL(stack.top, 3);

    clearLiteralAssignmentStack(&stack);

    ASSERT_INT_EQUAL(stack.top, -1);

    destroyLiteralAssignmentStack(&stack);

    return PASS;
}

