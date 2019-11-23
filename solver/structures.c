
#include "structures.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* Clause Operations */
void printClauses(Clause* clauses, int numClauses, FILE* out){
    int c,l;
    for(c = 0; c < numClauses; ++c){
        fputs("( ", out);
        if(clauses[c].numActiveLiterals){
            for(l = 0; l < CLAUSE_SIZE; ++l){
                if(clauses[c].active[l]){
                    fprintf(out, "%3d ", clauses[c].literals[l]);
                }else{
                    fputs("    ", out);
                }
            }
        }else{
            fputs("--- --- --- ", out);
        }
        fputs(") ", out);
    }
    fputs("\n", out);
}



/* SentenceStack Operations */
void initSentenceStack(SentenceStack* stack, int maxNumVariables, int maxNumClauses){
    int i;
    
    stack->maxStackheight = 2*(maxNumVariables+2);
    stack->sentences = malloc(sizeof(Clause*) * stack->maxStackheight);
    stack->sentenceLengths = malloc(sizeof(int) * stack->maxStackheight);

    for(i = 0 ; i < stack->maxStackheight; ++i){
        stack->sentences[i] = malloc(sizeof(Clause)*maxNumClauses);
    }

    stack->top = -1;
}

int* pushNewEmptySentence(SentenceStack* stack, Clause** newClauses){
    
    ++(stack->top);
    *newClauses = (stack->sentences[stack->top]);
    return &(stack->sentenceLengths[stack->top]);
}

void pushExistingSentence(SentenceStack* stack, Clause* sentence, int numClauses){
    ++(stack->top);
    free(stack->sentences[stack->top]);
    stack->sentences[stack->top] = sentence;
    stack->sentenceLengths[stack->top] = numClauses;
}

int popSentence(SentenceStack* stack, Clause** poppedClauses){
    *poppedClauses = (stack->sentences[stack->top]);

    // decrement top after returning current top:
    return stack->sentenceLengths[(stack->top)--];
}

void destroySentenceStack(SentenceStack* stack){
    int i;
    
    for(i = 0; i < stack->maxStackheight; ++i){
        free(stack->sentences[i]);
    }

    free(stack->sentences);
    free(stack->sentenceLengths);
}



/* LiteralInstanceSet Operations */
void initLiteralInstanceSet(LiteralInstanceSet* lset, int maxLitVal){
    ++maxLitVal;
    lset->maxLitVal = maxLitVal;
    lset->contains = calloc((maxLitVal),sizeof(Bool));
    lset->literals = malloc((maxLitVal)*sizeof(int));
    lset->size = 0;
}

void insertLiteral(LiteralInstanceSet *lset, int literal){
    if(!lset->contains[abs(literal)]){
        
        lset->contains[abs(literal)] = TRUE;
        lset->literals[lset->size] = literal;
    }
    ++(lset->size);
}

void clearAllLiterals(LiteralInstanceSet* lset){
    memset(lset->contains, FALSE, (lset->maxLitVal)*sizeof(Bool));
    lset->size = 0;
}

void destroyLiteralInstanceSet(LiteralInstanceSet* lset){
    free(lset->contains);
    free(lset->literals);
}

/* LiteralToClauseMap  operations */
void initLiteralToClauseMap(LiteralToClauseMap* map, int maxNumVariables, int maxNumClauses){
    int i;
    
    map->maxNumVariables = maxNumVariables;
    map->maxNumClauses = maxNumClauses;
    map-> clauseOccurrences = malloc((maxNumVariables+1) * sizeof(Clause**));
    map->clauseIndices = malloc((maxNumVariables+1) * sizeof(Byte*));
    map->literalFrequency = calloc((maxNumVariables+1),sizeof(int));

    for(i = 0; i <= maxNumVariables; ++i){
        map->clauseOccurrences[i] = malloc(maxNumClauses * sizeof(Clause*));
        map->clauseIndices = malloc(maxNumClauses * sizeof(Byte));
    }
}

void clearAllClauseLiterals(LiteralToClauseMap* map){

    /* only clear frequency array */
    memset(map->literalFrequency,0, (map->maxNumVariables+1) * sizeof(int));
}

void insertClauseLiterals(LiteralToClauseMap* map, Clause* clause){
    int l, litVal, nextInd;

    for(l = 0; l < CLAUSE_SIZE; ++l){
        if(clause->active[l]){
            litVal = abs(clause->literals[l]);
            nextInd = map->literalFrequency[litVal];
            map->clauseOccurrences[litVal][nextInd] = clause;
            map->clauseIndices[litVal][nextInd] = l;
            ++(map->literalFrequency[litVal]);
        }
    }
}

void destroy(LiteralToClauseMap* map){
    int i;

    for(i = 0; i <= map->maxNumVariables; ++i){
        free(map->clauseOccurrences[i]);
        free(map->clauseIndices[i]);
    }

    free(map->clauseOccurrences);
    free(map->clauseIndices);
}

/* VSIDSMap operations */
void initVSIDSMap(VSIDSMap* map, int maxLitVal, double decayFactor, double initBumpValue){
    int i;

    map->maxLitVal = maxLitVal;
    map->bumpGrowthFactor = 1.0 / decayFactor;
    map->bumpValue = initBumpValue;

    map->scores = malloc((maxLitVal+1)*sizeof(double));
    map->scorePQ = malloc((maxLitVal)*sizeof(int));
    map->scorePQInverse = malloc((maxLitVal+1)*sizeof(int));

    /* set scaleDownFactor */
    map->scaleDownFactor = pow(map->bumpGrowthFactor,
        ceil(log(VSIDS_SCALE_DOWN_THRESHOLD)/log(map->bumpGrowthFactor)));

    /* initialize score map */
    for(i = 1; i <= maxLitVal; ++i){
        map->scores[i] = 0.0;
    }

    /* initialize starting heap and heap inverse */
    for(i =0; i < maxLitVal; ++i){
        map->scorePQ[i] = i+1;
        map->scorePQInverse[i+1] = i;
    }
}

void clearVSIDSMap(VSIDSMap* map, double bumpValue){
    int i;

    map->bumpValue *= map->bumpGrowthFactor;

    /* re-initialize score map */
    for(i = 1; i <= map->maxLitVal; ++i){
        map->scores[i] = 1.0;
    }

    /* re-initialize starting heap */
    for(i = 0; i < map->maxLitVal; ++i){
        map->scorePQ[i] = i+1;
        map->scorePQInverse[i+1] = i;
    }
}

void bumpConflictClause(VSIDSMap* map, Clause* conflict){
    int l, conflictVar, pqConflictVarInd,
           pqParentVar, pqParentVarInd;
    double conflictVarScore, pqParentVarScore;
    Bool scaleDownMap, promoted;

    map->bumpValue *= map->bumpGrowthFactor;
    scaleDownMap = FALSE;

    for(l = 0; l < CLAUSE_SIZE; ++l){
        
        conflictVar = abs(conflict->literals[l]);
        pqConflictVarInd = map->scorePQInverse[conflictVar];
        map->scores[conflictVar] += map->bumpValue;
        conflictVarScore = map->scores[conflictVar];
        promoted = FALSE;

        scaleDownMap |= (conflictVarScore > VSIDS_SCALE_DOWN_THRESHOLD);
        
        /* update PQ & PQInverse */
        pqParentVarInd = (pqConflictVarInd-1)/2;
        pqParentVar = map->scorePQ[pqParentVarInd];
        pqParentVarScore = map->scores[pqParentVar];

        while(pqConflictVarInd > 0 && conflictVarScore > pqParentVarScore){

            /* demote parent */
            map->scorePQ[pqConflictVarInd] = pqParentVar;
            map->scorePQInverse[pqParentVar] = pqConflictVarInd;

            /* promote child index */
            pqConflictVarInd = pqParentVarInd;
        
            /* find next parent in PQ */
            pqParentVarInd = (pqConflictVarInd-1)/2;
            pqParentVar = map->scorePQ[pqParentVarInd];
            pqParentVarScore = map->scores[pqParentVar];

            promoted = TRUE;
        }

        if(promoted){
            /* set child value */
            map->scorePQ[pqConflictVarInd] = conflictVar;
            map->scorePQInverse[conflictVar] = pqConflictVarInd;
        }
    }

    /* scale down map and bump value to prevent overflow if necessary */
    if(scaleDownMap){
        map->bumpValue /= map->scaleDownFactor;
        for(l = 1; l <= map->maxLitVal; ++l){
            map->scores[l] /= map->scaleDownFactor;
        }
    }
}

void printVSIDSMap(VSIDSMap* map, FILE* out){
    int i, j;
    j = 2;
    fputs("=======VSIDS MAP========\n", out);
    for(i = 0; i < map->maxLitVal; ++i){
        if(i == j-1){
            fputs("------------------------\n", out);
            j *= 2;
        }
        fprintf(out, "[%d]%f\n", map->scorePQ[i], map->scores[map->scorePQ[i]]);
    }
    fputs("========================\n", out);
}

Bool isValidVSIDSMap(VSIDSMap* map){
    int i;
    for(i = 1; i < map->maxLitVal; ++i){
        if(map->scores[map->scorePQ[i]] > map->scores[map->scorePQ[(i-1)/2]]){
            return FALSE;
        }
    }

    return TRUE;
}

void destroyVSIDSMap(VSIDSMap* map){
    free(map->scorePQ);
    free(map->scorePQInverse);
    free(map->scores);
}

