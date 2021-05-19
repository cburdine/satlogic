#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "stochastic_structures.h"

/* ClauseMap Operations */
void init3CNFClauseMap(ClauseMap* cm, int sentence[], int numClauses){
    initKCNFClauseMap(cm,sentence,3,numClauses);
}

void initKCNFClauseMap(ClauseMap* cm, int sentence[], int k, int numClauses){
    int maxVar, sentenceLen, i, j, clause;
    int* mapLenIdx;

    /* compute number of variables */
    maxVar = 0;
    sentenceLen = k*numClauses;
    for(i = 0; i < sentenceLen; ++i){
        if(abs(sentence[i]) > maxVar){
            maxVar = abs(sentence[i]);
        }
    }

    /* allocate the clause map */
    cm->numVariables = maxVar;
    cm->numClauses = numClauses;
    cm->clauseLen = k;
    cm->sentence = malloc(sizeof(int)*(sentenceLen));
    cm->mapLen = malloc(sizeof(int) * (maxVar+1));
    mapLenIdx = malloc(sizeof(int) * (maxVar+1));

    /* copy the sentence */
    for(i = 0; i < sentenceLen; ++i){
        cm->sentence[i] = sentence[i];
    }

    /* compute the frequencies of each variable */
    for(i = 0; i <= maxVar; ++i){
        cm->mapLen[i] = 0;
        mapLenIdx[i] = 0;
    }
    for(i = 0; i < sentenceLen; ++i){
        if(sentence[i]){
            ++(cm->mapLen[abs(sentence[i])]);
        }
    }

    /* allocate ClauseMap */
    cm->map = malloc(sizeof(int*) * (maxVar+1));
    for(i = 0; i <= maxVar; ++i){
        cm->map[i] = malloc(sizeof(int) * (cm->mapLen[i]));
    }

    /* populate ClauseMap */
    for(i = 0; i < sentenceLen; ++i){
        if(sentence[i]){
            j = mapLenIdx[abs(sentence[i])];
            clause = (i/k)+1;
            if(sentence[i] < 0){
                clause *= -1;
            }
            cm->map[abs(sentence[i])][j] = clause;
            ++(mapLenIdx[abs(sentence[i])]);
        }
    }

    free(mapLenIdx);
}

void destroyClauseMap(ClauseMap* cm){
    int i;
    for(i = 0; i <= cm->numVariables; ++i){
        free(cm->map[i]);
    }
    free(cm->map);
    free(cm->mapLen);
    free(cm->sentence);
}

int countNewlyUnsatClauses(ClauseMap* cm, int flipLiteral, int clauseSatLiterals[]){
    int i, clause;
    int mapLen = cm->mapLen[abs(flipLiteral)];
    int count = 0;

    for(i = 0; i < mapLen; ++i){
        clause = cm->map[abs(flipLiteral)][i];
        
        /* check if flipping flipLiteral will cause the clause to be unsatisfied */
        if(((clause ^ flipLiteral) >= 0) && clauseSatLiterals[abs(clause)] == 1){
            ++count;
        }
    }

    return count;
}

int flip(ClauseMap* cm, int flipLiteral, int clauseSatLiterals[], int candidateClauses[]){
    int i, cl, clause;
    int mapLen = cm->mapLen[abs(flipLiteral)];
    int numUnsatClauses = 0;

    for(i = 0; i < mapLen; ++i){
        clause = cm->map[abs(flipLiteral)][i];

        /* if the signs agree, -- count, else ++ count */
        if((clause ^ flipLiteral) >= 0){
            --clauseSatLiterals[abs(clause)];
            assert(clauseSatLiterals[abs(clause)] >= 0);
        } else {
            ++clauseSatLiterals[abs(clause)];
        }
    }

    /* determine new unsatisfied clauses */
    for(cl = 1; cl <= cm->numClauses; ++cl){
        if(!clauseSatLiterals[cl]){
            candidateClauses[numUnsatClauses] = cl;
            ++numUnsatClauses;
        }
    }

    return numUnsatClauses;
}