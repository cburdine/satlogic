
#include <stdio.h>
#include <stdlib.h>

#include "dpll.h"

/* macros for Boolean values */
#define TRUE 1
#define FALSE 0
#define TRUE_NEGATIVE -1
#define ENTIRE_CLAUSE 3

void init(VarSet* vset, int numVars){
    vset->contains = calloc(numVars,sizeof(Bool));
    vset->numValues = 0;
}

void insertVar(VarSet *vset, int var){
    if(!vset->contains[abs(var)]){
        if(var < 0){
            vset->contains[var] = TRUE;
        }else{
            vset->contains[var] = TRUE_NEGATIVE;
        }
        ++(vset->numValues);
    }
}

void clear(VarSet* vset){
    free(vset->contains);
}

void printClauses(Clause* clauses, int numClauses, FILE* out){
    int c,l;
    for(c = 0; c < numClauses; ++c){
        fputs("( ", out);
        if(clauses[c].active[ENTIRE_CLAUSE]){
            for(l = 0; l < ENTIRE_CLAUSE; ++l){
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



/* assumes all duplicate vars in clauses are removed */
Bool dpll(Clause* clauses, int numClauses, int numVariables){
    int c, l, v, branchVar;
    int numPosLiterals, numNegLiterals, lastPosLiteral, lastNegLiteral;
    VarSet pureLiterals;
    Clause *nextClausesA, *nextClausesB;
    int numNextClausesA, numNextClausesB;
    Bool litStatus, excludeClause;

    /* stack initializations */
    ++numVariables;
    branchVar = 0;
    init(&pureLiterals, numVariables);
    litStatus = TRUE;

    /* identify types of clauses */
    for(c = 0; c < numClauses; ++c ){
        
        numPosLiterals = 0;
        numNegLiterals = 0;

        for(l = 0; l < ENTIRE_CLAUSE; ++l){
            if(clauses[c].active[l]){
                if(clauses[c].literals[l] > 0){
                    ++numPosLiterals;
                    lastPosLiteral = l;
                }else{
                    ++numNegLiterals;
                    lastNegLiteral = l;
                }
            }
        }

        /* return FALSE if any empty literal found */
        if(numPosLiterals + numNegLiterals == 0){
            clear(&pureLiterals);
            return FALSE;
        }else if(numPosLiterals == 1 && numNegLiterals == 0){
            /* add positive pure literal */
            insertVar(&pureLiterals, clauses[c].literals[lastPosLiteral]);
        }else if(numNegLiterals == 1 && numPosLiterals == 0){
            /* add negative pure literal */
            insertVar(&pureLiterals, clauses[c].literals[lastNegLiteral]);
        }
    }

    /* perform literal unit propagation */
    for(c = 0; c < numClauses; ++c){
        excludeClause = FALSE;
        for(l = 0; l < ENTIRE_CLAUSE && !excludeClause; ++l){
            if(clauses[c].active[l]){
                v = clauses[c].literals[l];
                litStatus = pureLiterals.contains[abs(v)];
                if(litStatus == TRUE){
                    if(v > 0){
                        /* exclude clause entirely */
                        excludeClause = TRUE;
                    }else{
                        /* eliminate literal */
                        clauses[c].active[l] = FALSE;
                    }
                }else if(litStatus == TRUE_NEGATIVE){
                    if(v < 0){
                        /* eliminae literal */
                        clauses[c].active[l] = FALSE;
                    }else{
                        /* exclude clause entirely */
                        excludeClause = TRUE;
                    }
                }else{
                    /* pick greatest (lexicographical) 
                    impure literal as branch var   */
                    if(abs(v) > branchVar){
                        branchVar = abs(v);
                    }
                }
            }
        }
        if(excludeClause){
            clauses[c].active[ENTIRE_CLAUSE] = FALSE;
        }
    }
    clear(&pureLiterals);

    /* branch into two arrays, A & B 
       where branchVar is T / F respectively */
    numNextClausesA = 0;
    numNextClausesB = 0;
    nextClausesA = malloc(numClauses * sizeof(Clause));
    nextClausesB = malloc(numClauses * sizeof(Clause));
    for(c = 0; c < numClauses; ++c){
        if(clauses[c].active[ENTIRE_CLAUSE]){
            excludeClause = FALSE;
            for(l = 0; l < ENTIRE_CLAUSE && !excludeClause; ++l){
                if(clauses[c].active[l]){
                    v = clauses[c].literals[l];
                    if(v == branchVar){
                        /* copy clause into B with eliminated literal */
                        nextClausesB[numNextClausesB] = clauses[c];
                        nextClausesB[numNextClausesB].active[l] = FALSE;
                        ++numNextClausesB;
                        excludeClause = TRUE;
                    } else if(v == -1 * branchVar){
                        /* copy clause into A with eliminated literal */
                        nextClausesA[numNextClausesA] = clauses[c];
                        nextClausesA[numNextClausesA].active[l] = FALSE;
                        ++numNextClausesA;
                        excludeClause = TRUE;
                    }
                }
            }
            if(!excludeClause){
                /* copy clause to both next sets of clauses */
                nextClausesA[numNextClausesA] = clauses[c];
                nextClausesB[numNextClausesB] = clauses[c];
                ++numNextClausesA;
                ++numNextClausesB;
            }
        }
    }
    
    if(numNextClausesA == 0 || numNextClausesB == 0){
        free(nextClausesA);
        free(nextClausesB);
        return TRUE;
    }

    /* Reurn DPLL(A) OR DPLL(B) */
    litStatus = (dpll(nextClausesA, numNextClausesA, branchVar+1) || 
                 dpll(nextClausesB, numNextClausesB, branchVar+1));

    /* free memory */
    free(nextClausesA);
    free(nextClausesB);

    return litStatus;
}

/* dedups and converts variables into clauses */
Bool dpll3Sat(int* sentence, int numClauses){
    int c,l,lu,v, maxVar;
    Bool copyClause, activeLiteral, result;
    Clause* initClauses;
    int numInitClauses, numInactiveLiterals;

    /* copy into clause structs, deduping vairables */
    initClauses = malloc(numInitClauses*sizeof(numClauses));
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
                }else if(sentence[3*c+l] = -1*sentence[3*c+lu]){
                    copyClause = FALSE;
                }
            }
            initClauses[numInitClauses].active[l] = activeLiteral;
        }
        if(copyClause && numInactiveLiterals < 3){
            initClauses[numInitClauses].active[ENTIRE_CLAUSE] = TRUE;
            ++numInitClauses;
        }
    }

    /* dpll-solve set of clauses */
    result = dpll(initClauses, numInitClauses, maxVar+1);
    
    free(initClauses);

}
