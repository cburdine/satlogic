#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "stochastic_structures.h"

/* ClauseMap Operations */
void init3CNFClauseMap(ClauseMap* cm, int sentence[], int numClauses){
    
    const int CLAUSE_SIZE = 3;
    int maxVar, sentenceLen, i, j, clause;
    int* mapLenIdx;

    /* compute number of variables */
    maxVar = 0;
    sentenceLen = CLAUSE_SIZE*numClauses;
    for(i = 0; i < sentenceLen; ++i){
        if(abs(sentence[i]) > maxVar){
            maxVar = abs(sentence[i]);
        }
    }

    /* populate the clause map */
    cm->numVariables = maxVar;
    cm->mapLen = malloc(sizeof(int) * (maxVar+1));
    mapLenIdx = malloc(sizeof(int) * (maxVar+1));

    /* compute the frequencies of each variable */
    for(i = 0; i <= maxVar; ++i){
        cm->mapLen[i] = 0;
        mapLenIdx[i] = 0;
    }

    for(i = 0; i < sentenceLen; ++i){
        assert(sentence[i] != 0);
        ++(cm->mapLen[abs(sentence[i])]);
    }

    /* allocate ClauseMap */
    cm->map = malloc(sizeof(int*) * (maxVar+1));
    for(i = 0; i <= maxVar; ++i){
        cm->map[i] = malloc(sizeof(int) * (cm->mapLen[i]));
    }

    /* populate ClauseMap */
    for(i = 0; i < sentenceLen; ++i){
        j = mapLenIdx[abs(sentence[i])];
        clause = (i/CLAUSE_SIZE)+1;
        if(sentence[i] < 0){
            clause *= -1;
        }
        cm->map[abs(sentence[i])][j] = clause;
        
        ++(mapLenIdx[abs(sentence[i])]);
    }

    free(mapLenIdx);
}

void initKCNFClauseMap(ClauseMap* cm, int sentence[], int k, int numClauses){
    /* TODO */
}

void destroyClauseMap(ClauseMap* cm){
    int i;
    for(i = 0; i <= cm->numVariables; ++i){
        free(cm->map[i]);
    }
    free(cm->map);
    free(cm->mapLen);
}