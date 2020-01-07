
#include "structures.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

/* Clause Operations */
void printClauses(Clause* clauses, int numClauses, FILE* out){
    int c,l;
    for(c = 0; c < numClauses; ++c){
        fputs("( ", out);
        if(clauses[c].numActiveLiterals > 0){
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
    stack->poppedSentence = malloc(sizeof(Clause)*maxNumClauses);

    stack->top = -1;
}

int* pushNewEmptySentence(SentenceStack* stack, Clause** newClauses){

    int* newSentenceLen;

    ++(stack->top);
    *newClauses = (stack->sentences[stack->top]);
    newSentenceLen = &(stack->sentenceLengths[stack->top]);
    *newSentenceLen = 0;
    return newSentenceLen;
}


void pushSentenceCopy(SentenceStack* stack, Clause* sentence, int numClauses){
    int c;

    ++(stack->top);
    for(c = 0; c < numClauses; ++c){
        stack->sentences[stack->top][c] = sentence[c];
    }
    stack->sentenceLengths[stack->top] = numClauses;
}

void popSentence(SentenceStack* stack){
    Clause* tmpSentencePtr;

    /* swap poppedSentence with top of stack */
    tmpSentencePtr = stack->poppedSentence;
    stack->poppedSentence = (stack->sentences[stack->top]);
    stack->sentences[stack->top] = tmpSentencePtr;

    stack->popedSentenceLen = stack->sentenceLengths[stack->top];
    
    --(stack->top);
}

void destroySentenceStack(SentenceStack* stack){
    int i;
    
    for(i = 0; i < stack->maxStackheight; ++i){
        free(stack->sentences[i]);
    }

    free(stack->sentences);
    free(stack->sentenceLengths);
    free(stack->poppedSentence);
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

    assert(abs(literal) > 0);
    assert(abs(literal) <= lset->maxLitVal);

    if(!lset->contains[abs(literal)]){
        
        lset->contains[abs(literal)] = TRUE;
        lset->literals[lset->size] = literal;
        ++(lset->size);
    }
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
        map->clauseIndices[i] = malloc(maxNumClauses * sizeof(Byte));
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

void destroyLiteralToClauseMap(LiteralToClauseMap* map){
    int i;

    for(i = 0; i <= map->maxNumVariables; ++i){
        free(map->clauseOccurrences[i]);
        free(map->clauseIndices[i]);
    }

    free(map->clauseOccurrences);
    free(map->clauseIndices);
    free(map->literalFrequency);
}

/* VSIDSMap operations */
void initVSIDSMap(VSIDSMap* map, int maxLitVal, double decayFactor, double initBumpValue){
    int i;

    assert(initBumpValue > 0.0);
    assert(decayFactor < 1.0 && decayFactor > 0.0);

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
        map->scores[i] = initBumpValue;
    }

    /* initialize starting heap and heap inverse */
    for(i =0; i < maxLitVal; ++i){
        map->scorePQ[i] = i+1;
        map->scorePQInverse[i+1] = i;
    }
}

void setVSIDSMapToVarFrequencies(VSIDSMap* map, Clause* sentence, int numClauses){
    int c, l, var, pqVarInd,
        pqParentVar, pqParentVarInd;
    double varScore, pqParentVarScore;
    Bool promoted;

    /* clear vsids map */
    clearVSIDSMap(map);

    /* increment score by 1.0 for each occurrence of a variable */
    for(c = 0; c < numClauses; ++c){
        for(l = 0; l < CLAUSE_SIZE; ++l){
            if(sentence[c].active[l] && sentence[c].literals[l]){
                var = abs(sentence[c].literals[l]);
                pqVarInd = map->scorePQInverse[var]; /* BUFFER ERROR HERE */
                map->scores[var] += 1.0;
                varScore = map->scores[var];
                promoted = FALSE;

                /* determine parent in PQ */
                pqParentVarInd = (pqVarInd-1)/2;
                pqParentVar = map->scorePQ[pqParentVarInd];
                pqParentVarScore = map->scores[pqParentVar];

                while(pqVarInd > 0 && varScore > pqParentVarScore){

                    /* demote parent */
                    map->scorePQ[pqVarInd] = pqParentVar;
                    map->scorePQInverse[pqParentVar] = pqVarInd;

                    /* promote child index */
                    pqVarInd = pqParentVarInd;
                
                    /* find next parent in PQ */
                    pqParentVarInd = (pqVarInd-1)/2;
                    pqParentVar = map->scorePQ[pqParentVarInd];
                    pqParentVarScore = map->scores[pqParentVar];

                    promoted = TRUE;
                }

                if(promoted){
                    /* set child value */
                    map->scorePQ[pqVarInd] = var;
                    map->scorePQInverse[var] = pqVarInd;
                }
            }
        }
    }

    assert(isValidVSIDSMap(map));
}

void clearVSIDSMap(VSIDSMap* map){
    int i;

    /* re-initialize score map */
    for(i = 1; i <= map->maxLitVal; ++i){
        map->scores[i] = 0.0;
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

        if(conflictVar){
            pqConflictVarInd = map->scorePQInverse[conflictVar];
            if(map->scores[conflictVar] >= 0){
                map->scores[conflictVar] += map->bumpValue;
            } else {
                map->scores[conflictVar] -= map->bumpValue;
            }
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
    }

    /* scale down map and bump value to prevent overflow if necessary */
    if(scaleDownMap){
        map->bumpValue /= map->scaleDownFactor;
        for(l = 1; l <= map->maxLitVal; ++l){
            map->scores[l] /= map->scaleDownFactor;
        }
    }
}

void deactivateVariable(VSIDSMap* map, int var){
    
    int pqVarInd, 
        pqLeftChildVar, pqLeftChildVarInd,
        pqRightChildVar, pqRightChildVarInd,
        pqNextChildVar, pqNextChildVarInd;

    double varScore, pqLeftChildVarScore, pqRightChildVarScore, pqNextChildVarScore;
    Bool demoted;

    assert(var > 0);

    /* negate element in pq to deactivate it */
    pqVarInd = map->scorePQInverse[var];
    map->scores[var] *= -1.0;
    varScore = map->scores[var];
    demoted = FALSE;

    assert(varScore < 0.0);

    pqNextChildVarScore = 1.0;

    while(pqNextChildVarScore > 0.0){

        /* determine which child to promote */
        pqLeftChildVarInd = (pqVarInd*2 + 1);
        pqRightChildVarInd = (pqVarInd*2 + 2);

        /* if both children are in the priority queue */
        if(pqLeftChildVarInd < map->maxLitVal && pqRightChildVarInd < map->maxLitVal){

            pqRightChildVar = map->scorePQ[pqRightChildVarInd];
            pqLeftChildVar = map->scorePQ[pqLeftChildVarInd];
            pqRightChildVarScore = map->scores[pqRightChildVar];
            pqLeftChildVarScore = map->scores[pqLeftChildVar];

            if(pqLeftChildVarScore > 0 && pqRightChildVarScore > 0){
                pqNextChildVarInd = (pqLeftChildVarScore > pqRightChildVarScore)? pqLeftChildVarInd : pqRightChildVarInd;
                pqNextChildVar = map->scorePQ[pqNextChildVarInd];
            } else {
                pqNextChildVarInd = (pqLeftChildVarScore > 0)? pqLeftChildVarInd : pqRightChildVarInd;
                pqNextChildVar = map->scorePQ[pqNextChildVarInd];
            }
            
        /* if left child is in the priority queue */
        } else if(pqLeftChildVarInd < map->maxLitVal){

            pqNextChildVarInd = pqLeftChildVarInd;
            pqNextChildVar = map->scorePQ[pqNextChildVarInd];
            pqNextChildVarScore = map->scores[pqNextChildVar];

        /* if current element is a leaf */
        } else {
            pqNextChildVarScore = 0.0;
        }

        /* if selected child's variable is active, promote it */
        if(pqNextChildVarScore > 0.0){

            /* promote child */
            map->scorePQ[pqVarInd] = pqNextChildVar;
            map->scorePQInverse[pqNextChildVar] = pqVarInd;

            /* demote parent */
            pqVarInd = pqNextChildVarInd;

            demoted = TRUE;
        }
    }

    if(demoted){
        /* set parent value */
        map->scorePQ[pqVarInd] = var;
        map->scorePQInverse[var] = pqVarInd;
    }
}

void reactivateVariable(VSIDSMap* map, int var){
    int l, pqVarInd,
           pqParentVar, pqParentVarInd;
    double varScore, pqParentVarScore;
    Bool promoted;

    assert(var > 0);
    assert(map->scores[var] < 0.0);
    

    varScore = abs(map->scores[var]);
    map->scores[var] = varScore;
    pqVarInd = map->scorePQInverse[var];
    

    if(pqVarInd > 0){

        /* update PQ & PQInverse */
        pqParentVarInd = (pqVarInd-1)/2;
        pqParentVar = map->scorePQ[pqParentVarInd];
        pqParentVarScore = map->scores[pqParentVar];

        while(pqVarInd > 0 && varScore > pqParentVarScore){

            /* demote parent */
            map->scorePQ[pqVarInd] = pqParentVar;
            map->scorePQInverse[pqParentVar] = pqVarInd;

            /* promote child index */
            pqVarInd = pqParentVarInd;
        
            /* find next parent in PQ */
            pqParentVarInd = (pqVarInd-1)/2;
            pqParentVar = map->scorePQ[pqParentVarInd];
            pqParentVarScore = map->scores[pqParentVar];

            promoted = TRUE;
        }

        if(promoted){
            /* set child value */
            map->scorePQ[pqVarInd] = var;
            map->scorePQInverse[var] = pqVarInd;
        }
    }
}


void destroyVSIDSMap(VSIDSMap* map){
    free(map->scorePQ);
    free(map->scorePQInverse);
    free(map->scores);
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
        /* ensure priority queue topology is correct */
        if(map->scores[map->scorePQ[i]] > 0 && 
           map->scores[map->scorePQ[i]] > map->scores[map->scorePQ[(i-1)/2]]){
            return FALSE;
        }

        /* ensure inverse-lookup table is correct */
        if(map->scorePQInverse[map->scorePQ[i]] != i){
            return FALSE;
        }
    }

    return TRUE;
}

/* LiteralAssignmentStack operations */

void initLiteralAssignmentStack(LiteralAssignmentStack* stack, int maxLitVal){
    int i;

    assert(maxLitVal > 0);

    stack->maxNumVariables = 2*(maxLitVal+2);
    stack->top = -1;
    stack->variableAssignments = malloc(sizeof(int*) * stack->maxNumVariables);
    stack->numAssignments = malloc(sizeof(int) * stack->maxNumVariables);
    stack->branchVariables = malloc(sizeof(int) * stack->maxNumVariables);
    stack->onSecondBranch = malloc(sizeof(Bool) * stack->maxNumVariables);

    for(i = 0; i < stack->maxNumVariables; ++i ){
        stack->variableAssignments[i] = malloc(sizeof(int) * stack->maxNumVariables);
        stack->numAssignments[i] = 0;
    }
}

void clearLiteralAssignmentStack(LiteralAssignmentStack* stack){
    int i;

    for(i = 0; i < stack->maxNumVariables; ++i){
        stack->numAssignments[i] = 0;
    }
    stack->top = -1;
}

void addLiteral(LiteralAssignmentStack* stack, int literal){
    stack->variableAssignments[stack->top][stack->numAssignments[stack->top]] = literal;
    ++(stack->numAssignments[stack->top]);
}

void destroyLiteralAssignmentStack(LiteralAssignmentStack* stack){
    int i;

    for(i = 0; i < stack->maxNumVariables; ++i){
        free(stack->variableAssignments[i]);
    }

    free(stack->variableAssignments);
    free(stack->numAssignments);
    free(stack->branchVariables);
    free(stack->onSecondBranch);
}

void backtrackToNextStackFrame(LiteralAssignmentStack* stack, VSIDSMap* map){
    int newTopInd, numUnitVars, i;

    newTopInd = stack->top;
    while(newTopInd >= 0 && 
        (!(stack->branchVariables[newTopInd]) || stack->onSecondBranch[newTopInd]) ){

        /* reactivate variables in VSIDS map */
        numUnitVars = stack->numAssignments[newTopInd];
        
        for(i = 0; i < numUnitVars; ++i){
            reactivateVariable(map, abs(stack->variableAssignments[newTopInd][i]));
        }

        /* reactivate branch variable */
        if(stack->branchVariables[newTopInd]){
            reactivateVariable(map, abs(stack->branchVariables[newTopInd]));
        }

        --newTopInd;
    }
    
    if(newTopInd >= 0){
        /* toggle branch that we backtrack to */
        stack->onSecondBranch[newTopInd] = TRUE;
        stack->branchVariables[newTopInd] *= -1;
    }

    stack->top = newTopInd;
}

void pushNewFrame(LiteralAssignmentStack* stack){
    ++(stack->top);
    stack->numAssignments[stack->top] = 0;
    stack->branchVariables[stack->top] = 0;
    stack->onSecondBranch[stack->top] = FALSE;
}

void recordVariableAssignments(LiteralAssignmentStack* stack, Bool* solnArr){
    int stackInd, litInd, assignedVar;

    for(stackInd = stack->top; stackInd >= 0; --stackInd){
        
        /* write branching variable assignments */
        assignedVar = stack->branchVariables[stackInd];
        if(assignedVar){
            solnArr[abs(assignedVar)] = (assignedVar > 0);
        }

        /* write unit propagation variable assignments */
        for(litInd = 0; litInd < stack->numAssignments[stackInd]; ++litInd){
            assignedVar = stack->variableAssignments[stackInd][litInd];
            solnArr[abs(assignedVar)] = (assignedVar > 0);
        }
    }
}

void printLiteralAssignmentStack(LiteralAssignmentStack* stack, FILE* out){
    int i,j, var;

    if(stack->top < 0){
        fputs("<EMPTY>\n", out);
        return;
    }

    for(i = stack->top; i >= 0; --i){
        fprintf(out, "[%d] ", i);
        for(j = 0; j < stack->numAssignments[i]; ++j){
            var = stack->variableAssignments[i][j];
            fprintf(out, "%d=%c ", abs(var), (var > 0)? 'T' : 'F');
        }

        var = stack->branchVariables[i];
        if(var == 0){
            fputs("| ?", out);
        }else {
            fprintf(out, "| %d=%c %s", 
                abs(var), 
                (var > 0)? 'T' : 'F', 
                stack->onSecondBranch[i]? "(b2)" :""
            );
        }
        fputs("\n", out);
    }
}