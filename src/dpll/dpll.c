
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "dpll.h"
#include "dpll_structures.h"

/* improvement upon old dpll */
Bool dpllStatic3Sat(Clause* clauses, int numClauses, int numVariables, Bool solnArr[]){

    /* vsids decay & bump factor */
    static const double VSIDS_DECAY_FACTOR = 0.95;
    static const double VSIDS_INIT_BUMP_VALUE = 1.0;
    
    /* aux variables */
    int c, l, l2,
        clauseLit, pureLitInd, pureLit, pureLitFrequency,
        currentClauseInd, branchVar;
    
    /* iteration variables */
    int *numNextClausesAPtr, numNextClausesA, *numNextClausesBPtr, numNextClausesB;
    Clause *currentClause, *nextClausesA, *nextClausesB;
    Bool excludeClause, backtrack, foundSoln;
    
    /* declare useful data structures */
    LiteralInstanceSet pureLiterals;
    LiteralToClauseMap unitPropMap;
    LiteralAssignmentStack assignmentStack;
    VSIDSMap vsidsMap;

    assert(numClauses > 0);
    assert(numVariables > 0);

    /* data structure initialization */
    SentenceStack sentenceStack;
    initSentenceStack(&sentenceStack, numVariables, numClauses);
    initLiteralInstanceSet(&pureLiterals, numVariables+1);
    initLiteralAssignmentStack(&assignmentStack, numVariables);
    initVSIDSMap(&vsidsMap, numVariables, VSIDS_DECAY_FACTOR, VSIDS_INIT_BUMP_VALUE);

    /* populate VSIDS map with initial frequencies */
    setVSIDSMapToVarFrequencies(&vsidsMap, clauses, numClauses);

    /* generate ClauseToLiteral map */
    initLiteralToClauseMap(&unitPropMap, numVariables, numClauses);

    /* push initial sentence on stack */
    pushSentenceCopy(&sentenceStack, clauses, numClauses);

    /* iterate until search tree is exhausted or solution found: */
    foundSoln = FALSE;
    while(!foundSoln && sentenceStack.top >= 0){
        
        /* pop a sentence from the stack; allow for new variable assignments */
        popSentence(&sentenceStack);
        clauses = sentenceStack.poppedSentence;
        numClauses = sentenceStack.popedSentenceLen;

        /* clear data structures */
        clearAllLiterals(&pureLiterals);
        clearAllClauseLiterals(&unitPropMap);
        backtrack = FALSE;

        /* identify pure literal and empty clauses */
        for(c = 0; c < numClauses && !backtrack; ++c){
            if(clauses[c].numActiveLiterals > 0){

                /* process clause in unit prop map */
                insertClauseLiterals(&unitPropMap, &clauses[c]);

                if(clauses[c].numActiveLiterals == 1){

                    /* if clause contains a single pure literal, 
                       add it to the pure literal set            */
                    for(l = 0; l < CLAUSE_SIZE; ++l){
                        if(clauses[c].active[l]){
                            insertLiteral(&pureLiterals, clauses[c].literals[l]);
                        }
                    }
                }

            } else {
                /* if an empty clause is found, backtrack */
                bumpConflictClause(&vsidsMap, &clauses[c]);
                backtrack = TRUE;
            }
        }

        if(!backtrack){

            pushNewFrame(&assignmentStack);

            /* perform unit propagation */
            for(pureLitInd = 0; pureLitInd < pureLiterals.size && !backtrack; ++pureLitInd){

                /* add pure literal to stack of assigned variables */
                pureLit = pureLiterals.literals[pureLitInd];
                pureLitFrequency = unitPropMap.literalFrequency[abs(pureLit)];

                assert(pureLiterals.contains[abs(pureLit)]);

                /* record pure literal assignment */
                addLiteral(&assignmentStack, pureLit);
                deactivateVariable(&vsidsMap, abs(pureLit));

                for(l = 0; l < pureLitFrequency && !backtrack; ++l){
                    currentClause = unitPropMap.clauseOccurrences[abs(pureLit)][l];
                    currentClauseInd = unitPropMap.clauseIndices[abs(pureLit)][l];
                    clauseLit = currentClause->literals[currentClauseInd];

                    if(pureLit == clauseLit){
                        /* exclude clause entirely */
                        currentClause->numActiveLiterals = 0;

                    } else if(pureLit == -1*clauseLit){

                        /* otherwise, eliminate just the literal */
                        currentClause->active[currentClauseInd] = FALSE;
                        --(currentClause->numActiveLiterals);

                        /* if clause becomes empty, update vsids map and backtrack */
                        if(currentClause->numActiveLiterals == 0){
                            bumpConflictClause(&vsidsMap, currentClause);
                            backtrack = TRUE;

                        }else if(currentClause->numActiveLiterals == 1){

                            /* find and record new pure literal */
                            for(l2 = 0; l2 < (CLAUSE_SIZE-1) && !currentClause->active[l2]; ++l2);
                            insertLiteral(&pureLiterals, currentClause->literals[l2]);

                            assert(l2 < CLAUSE_SIZE);
                            assert(vsidsMap.scores[abs(currentClause->literals[l2])] > 0.0);
                        }
                    }
                }
            }
        }
        
        if(backtrack){
            
            backtrackToNextStackFrame(&assignmentStack, &vsidsMap);

        }else{

            /* perform branching assignment based variable chosen by the VSIDS heuristic */
            branchVar = vsidsMap.scorePQ[0];
            
            if(vsidsMap.scores[branchVar] > 0.0){
                deactivateVariable(&vsidsMap, branchVar);
                setBranchLiteral(&assignmentStack, branchVar);
            } else {
                branchVar = 0;
            }

            /* create new sentences to hold branched sentence */
            numNextClausesBPtr = pushNewEmptySentence(&sentenceStack, &nextClausesB);
            numNextClausesAPtr = pushNewEmptySentence(&sentenceStack, &nextClausesA);

            /* 
                perform branching variable assignment: 
                A is where branchVar := T,
                B is where branchVar := F 
            */
            numNextClausesA = 0;
            numNextClausesB = 0;

            for(c = 0; c < numClauses; ++c){
                if(clauses[c].numActiveLiterals > 0){

                    excludeClause = FALSE;
                    for(l = 0; l < CLAUSE_SIZE && !excludeClause; ++l){
                        if(clauses[c].active[l]){

                            clauseLit = clauses[c].literals[l];
                            if(clauseLit == branchVar){
                                /* Copy clause into B with eliminated literal & exclude in A*/
                                nextClausesB[numNextClausesB] = clauses[c];
                                nextClausesB[numNextClausesB].active[l] = FALSE;
                                --(nextClausesB[numNextClausesB].numActiveLiterals);
                                ++numNextClausesB;
                                excludeClause = TRUE;

                            } else if(clauseLit == (-1*branchVar)){
                                /* Copy clause into A with eliminated literal & exclude in B*/
                                nextClausesA[numNextClausesA] = clauses[c];
                                nextClausesA[numNextClausesA].active[l] = FALSE;
                                --(nextClausesA[numNextClausesA].numActiveLiterals);
                                ++numNextClausesA;
                                excludeClause = TRUE;
                            }
                        }
                    }

                    if(!excludeClause){
                        nextClausesA[numNextClausesA] = clauses[c];
                        nextClausesB[numNextClausesB] = clauses[c];
                        ++numNextClausesA;
                        ++numNextClausesB;
                    }
                }
            }

            /* determine if a solution was found due to the branching */
            if(numNextClausesA == 0){
                /* return TRUE */
                foundSoln = TRUE;
            } else if(numNextClausesB == 0){
                /* return TRUE, signal that the second branch is the satisfying assignment */
                toggleStackFrameBranch(&assignmentStack);
                foundSoln = TRUE;
            } else {
                *numNextClausesAPtr = numNextClausesA;
                *numNextClausesBPtr = numNextClausesB;
            }
        }
    }

    /* write variable assignments if soln is found */
    if(foundSoln){
        recordVariableAssignments(&assignmentStack, solnArr);
    }

    /* clean up data structures */
    destroySentenceStack(&sentenceStack);
    destroyLiteralInstanceSet(&pureLiterals);
    destroyLiteralToClauseMap(&unitPropMap);
    destroyLiteralAssignmentStack(&assignmentStack);
    destroyVSIDSMap(&vsidsMap);

    return foundSoln;
}

/* dedups and converts variables into clauses */
Bool dpll3Sat(int sentence[], int numClauses, Bool solnArr[]){
    int c,l,lu,v, maxVar;
    Bool copyClause, activeLiteral, result;
    Clause* initClauses;
    int numInitClauses, numInactiveLiterals;

    /* copy into clause structs, deduping variables */
    initClauses = malloc(numClauses*sizeof(Clause));
    numInitClauses = 0;
    maxVar = 0;
    
    for(c = 0; c < numClauses; ++c){
        copyClause = TRUE;
        numInactiveLiterals = 0;
        for(l = 0; l < 3; ++l){

            v = sentence[3*c+l];
            initClauses[numInitClauses].literals[l] = v;
            activeLiteral = TRUE;

            /* update maximal var */
            if(abs(v) > maxVar){
                maxVar = abs(v);
            }

            /* check for other variable instances in clause */
            for(lu = l+1; lu < 3; ++lu){
                if(sentence[3*c+l] == sentence[3*c+lu]){
                    activeLiteral = FALSE;
                    ++numInactiveLiterals;
                    break;
                }else if(sentence[3*c+l] == -1*sentence[3*c+lu]){
                    copyClause = FALSE;
                    break;
                }
            }

            initClauses[numInitClauses].active[l] = activeLiteral;
        }

        if(copyClause && numInactiveLiterals < 3){
            initClauses[numInitClauses].numActiveLiterals = CLAUSE_SIZE - numInactiveLiterals;
            ++numInitClauses;
        }
    }

    /* clear solnArr values */
    for(v = 0; v <= maxVar; ++v){ solnArr[v] = 0; }

    /* dpll-solve set of clauses */
    result = dpllStatic3Sat(initClauses, numInitClauses, maxVar, solnArr);

    free(initClauses);

    return result;
}