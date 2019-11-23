
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "dpll.h"
#include "structures.h"

/* assumes all duplicate vars in clauses are removed */

// Bool dpll(Clause* clauses, int numClauses, int numVariables){

//     int c, l, l2, v, branchVar;
//     int numPosLiterals, numNegLiterals, lastPosLiteral, lastNegLiteral;
//     LiteralInstanceSet pureLiterals;
//     Clause *nextClausesA, *nextClausesB;
//     int numNextClausesA, numNextClausesB;
//     Bool litExists, excludeClause;

//     assert(numClauses > 0);
//     assert(numVariables > 0);

//     /* stack initializations */
//     branchVar = 0;
//     init(&pureLiterals, numVariables+1);
//     litExists = TRUE;

//     /* identify types of clauses */
//     for(c = 0; c < numClauses; ++c ){
        
//         numPosLiterals = 0;
//         numNegLiterals = 0;

//         for(l = 0; l < CLAUSE_SIZE; ++l){
//             if(clauses[c].active[l]){
//                 if(clauses[c].literals[l] > 0){
//                     ++numPosLiterals;
//                     lastPosLiteral = l;
//                 }else{
//                     ++numNegLiterals;
//                     lastNegLiteral = l;
//                 }
//             }
//         }

//         /* return FALSE if any empty literal found */
//         if(numPosLiterals + numNegLiterals == 0){
//             clear(&pureLiterals);
//             return FALSE;
//         }else if(numPosLiterals == 1 && numNegLiterals == 0){
//             /* add positive pure literal */
//             insertVar(&pureLiterals, clauses[c].literals[lastPosLiteral]);
//         }else if(numNegLiterals == 1 && numPosLiterals == 0){
//             /* add negative pure literal */
//             insertVar(&pureLiterals, clauses[c].literals[lastNegLiteral]);
//         }
//     }

//     /* printClauses(clauses, numClauses, stdout); */

//     /* perform literal unit propagation */
//     for(c = 0; c < numClauses; ++c){
//         excludeClause = FALSE;
//         for(l = 0; l < numClauses && !excludeClause; ++l){
//             if(clauses[c].active[l]){
//                 v = clauses[c].literals[l];
//                 litStatus = pureLiterals.contains[abs(v)];
//                 if(litStatus == TRUE){
//                     if(v > 0){
//                         /* exclude clause entirely */
//                         excludeClause = TRUE;
//                     }else{
//                         /* eliminate literal */
//                         clauses[c].active[l] = FALSE;
//                     }
//                 }else if(litStatus == TRUE_NEGATIVE){
//                     if(v > 0){
//                         /* eliminae literal */
//                         clauses[c].active[l] = FALSE;
//                     }else{
//                         /* exclude clause entirely */
//                         excludeClause = TRUE;
//                     }
//                 }else{
//                     /* pick greatest (lexicographical) 
//                     impure literal as branch var   */
//                     if(abs(v) > branchVar){
//                         branchVar = abs(v);
//                     }
//                 }
//             }
//         }
//         if(excludeClause){
//             clauses[c].active[ENTIRE_CLAUSE] = FALSE;
//         }
//     }
//     clear(&pureLiterals);

//     /* fputs("After Unit Prop:\n", stdout); */
//     /* printClauses(clauses, numClauses, stdout); */

//     /* branch into two arrays, A & B 
//        where branchVar is T / F respectively */
//     numNextClausesA = 0;
//     numNextClausesB = 0;
//     nextClausesA = malloc(numClauses * sizeof(Clause));
//     nextClausesB = malloc(numClauses * sizeof(Clause));
//     for(c = 0; c < numClauses; ++c){
//         if(clauses[c].active[ENTIRE_CLAUSE]){
//             excludeClause = FALSE;
//             for(l = 0; l < ENTIRE_CLAUSE && !excludeClause; ++l){
//                 if(clauses[c].active[l]){
//                     v = clauses[c].literals[l];
//                     if(v == branchVar){
//                         /* copy clause into B with eliminated literal */
//                         nextClausesB[numNextClausesB] = clauses[c];
//                         nextClausesB[numNextClausesB].active[l] = FALSE;
//                         ++numNextClausesB;
//                         excludeClause = TRUE;
//                     } else if(v == -1 * branchVar){
//                         /* copy clause into A with eliminated literal */
//                         nextClausesA[numNextClausesA] = clauses[c];
//                         nextClausesA[numNextClausesA].active[l] = FALSE;
//                         ++numNextClausesA;
//                         excludeClause = TRUE;
//                     }
//                 }
//             }
//             if(!excludeClause){
//                 /* copy clause verbatim to both next sets of clauses */
//                 nextClausesA[numNextClausesA] = clauses[c];
//                 nextClausesB[numNextClausesB] = clauses[c];
//                 ++numNextClausesA;
//                 ++numNextClausesB;
//             }
//         }
//     }
    
//     if(numNextClausesA == 0 || numNextClausesB == 0){
//         free(nextClausesA);
//         free(nextClausesB);
//         return TRUE;
//     }

//     /* Reurn DPLL(A) OR DPLL(B) */
//     /* fprintf(stdout,"Setting %d TRUE:\n", branchVar); */
//     litStatus = dpll(nextClausesA, numNextClausesA, branchVar+1);
    
//     /* fprintf(stdout,"Setting %d FALSE:\n", branchVar); */
    
//     litStatus = (litStatus || dpll(nextClausesB, numNextClausesB, branchVar+1));

//     /* free memory */
//     free(nextClausesA);
//     free(nextClausesB);

//     return litStatus;
// }


/* improvement upon old dpll */
Bool dpllStatic(Clause* clauses, int numClauses, int numVariables){

    /* vsids decay & bump factor */
    static const double VSIDS_DECAY_FACTOR = 2.5;
    static const double VSIDS_INIT_BUMP_VALUE = 1.0;
    
    /* aux variables */
    int c, l, l2, pureLitInd, pureLit, currentClauseInd, branchVar;
    
    /* iteration variables */
    int *numNextClausesA, *numNextClausesB;
    Clause* currentClause;
    Bool litStatus, excludeClause, backtrack;

    /* useful data structures */
    LiteralInstanceSet pureLiterals;
    LiteralToClauseMap unitPropMap;
    VSIDSMap vsidsMap;

    assert(numClauses > 0);
    assert(numVariables > 0);

    /* data structure initialization */
    SentenceStack sentenceStack;
    initSentenceStack(&sentenceStack, numClauses, numVariables);
    initLiteralInstanceSet(&pureLiterals, numVariables+1);
    initVSIDSMap(&vsidsMap, numVariables, VSIDS_DECAY_FACTOR, VSIDS_INIT_BUMP_VALUE);

    /* generate ClauseToLiteral map */
    initLiteralToClauseMap(&unitPropMap, numVariables, numClauses);

    /* push initial sentence on stack */
    pushExistingSentence(&sentenceStack, clauses, numClauses);


    /* iterate until search tree is exhausted: */
    while(sentenceStack.top >= 0){

        /* pop a sentence from the stack */
        numClauses = popSentence(&sentenceStack, &clauses);

        clearAllLiterals(&pureLiterals);
        clearAllClauseLiterals(&unitPropMap);
        backtrack = FALSE;

        for(c = 0; c < numClauses; ++c){
            if(clauses[c].numActiveLiterals > 0){

                /* process clause in unit prop map */
                insertClauseLiterals(&unitPropMap, &clauses[c]);

                if(clauses[c].numActiveLiterals == 1){

                    /* if clause contains a single pure literal, 
                       add it to the pure literal set            */
                    for(l = 0; l < CLAUSE_SIZE; ++l){
                        if(clauses[c].active[l])
                        insertLiteral(&pureLiterals, clauses[c].literals[l]);
                    }
                }

            } else {
                /* if an empty clause is found, backtrack */
                bumpConflictClause(&vsidsMap, currentClause);
                backtrack = TRUE;
            }
        }

        if(!backtrack){

            /* perform literal unit propagation */
            for(pureLitInd = 0; pureLitInd < pureLiterals.size && !backtrack; ++pureLitInd){
                pureLit = pureLiterals.literals[pureLitInd];
                litStatus = pureLiterals.contains[pureLitInd];

                for(l = 0; l < unitPropMap.literalFrequency[l] && !backtrack; ++l){
                    currentClause = unitPropMap.clauseOccurrences[abs(pureLit)][l];
                    currentClauseInd = unitPropMap.clauseIndices[abs(pureLit)][l];

                    if(pureLit == currentClause->literals[currentClauseInd]){
                        /* exclude clause entirely */
                        currentClause->numActiveLiterals = 0;
                    } else {
                        /* eliminate literal */
                        currentClause->active[currentClauseInd] = FALSE;
                        --(currentClause->numActiveLiterals);

                        if(currentClause->numActiveLiterals == 0){
                            /* if clause is empty, update vsids map and backtrack */
                            bumpConflictClause(&vsidsMap, currentClause);
                            backtrack = TRUE;

                        }else if(currentClause->numActiveLiterals == 1){
                            /* find and record new pure literal */
                            for(l2 = 0; l2 < CLAUSE_SIZE && !currentClause->active[l2]; ++l2);
                            insertLiteral(&pureLiterals, currentClause->literals[l2]);
                        }
                    }
                }
            }
        }

        printClauses(clauses, numClauses, stdout);

        if(!backtrack){
            /* perform branching assignment based variable with most instances */
        }
    }


    return TRUE;
}

/* dedups and converts variables into clauses */
Bool dpll3Sat(int sentence[], int numClauses){
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
            if(v > maxVar){
                maxVar = v;
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
            initClauses[numInitClauses].active[CLAUSE_SIZE] = TRUE;
            initClauses[numInitClauses].numActiveLiterals = CLAUSE_SIZE - numInactiveLiterals;
            ++numInitClauses;
        }
    }
    
    /* dpll-solve set of clauses */
    result = dpllStatic(initClauses, numInitClauses, maxVar+1);
    
    free(initClauses);

    return result;
}