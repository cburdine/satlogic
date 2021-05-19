#include "walksat.h"
#include "fastrand.h"
#include "stochastic_structures.h"
#include "../preprocessor/preprocessor.h"

#include <stdlib.h>
#include <time.h>
#include <assert.h>

Bool runWalkSAT(ClauseMap* clauseMap, Bool solnArr[], 
                    Ullint numSteps, double epsilon){
    Ullint step;
    int i, j, cl, mapLen;
    int numClauses = clauseMap->numClauses;
    int numVariables = clauseMap->numVariables;

    /* compute epsilon threshold */
    assert(0.0 <= epsilon && epsilon <= 1.0);
    const int EPSILON_MASK = 0x3fffffff;
    int epsilonThreshold = (epsilon * ((double) EPSILON_MASK));

    /* ensure solution array has proper formatting: */
    assert(solnArr[0] == 0);
    assert(numClauses > 0);
    assert(numVariables > 0);
    
    /* allocate array to hold # of satisfied literals: */
    int* clauseSatLiterals = malloc((numClauses+1)*sizeof(int));
    for(i = 0; i <= numClauses; ++i){
        clauseSatLiterals[i] = 0;
    }

    /* set the number of true literals per clause under solnArray */
    for(i = 1; i <= numVariables; ++i){
        mapLen = clauseMap->mapLen[i];
        for(j = 0; j < mapLen; ++j){
            cl = clauseMap->map[i][j];
            if((solnArr[i]  && (cl > 0)) || 
                !solnArr[i] && (cl < 0)){
                ++clauseSatLiterals[abs(cl)];
            }
        }
    }
    
    // allocate an arrays to hold candidate clauses snd literals:
    int* candidateClauses = malloc(sizeof(int)*numClauses);
    int* candidateLits = malloc(sizeof(int)*numVariables);
    int flipLiteral, clChoice;

    /* find unsatisfied clauses */
    int numUnsatClauses = 0;
    for(cl = 1; cl <= numClauses; ++cl){
        if(clauseSatLiterals[cl] == 0){
            candidateClauses[numUnsatClauses] = cl;
            ++numUnsatClauses;
        }
    }

    /* seed local fastrand number generator */
    INIT_LOCAL_FASTRAND();
    seedFastrand(time(0));

    /* perform search: */
    for(step = 0; (numUnsatClauses > 0) && (step < numSteps); ++step){

        /*
        for(i = 1; i <= numClauses; ++i){
            printf("%d ",clauseSatLiterals[i]);
        }
        printf(" [");
        for(i = 1; i <= numVariables; ++i){
            printf("%d", solnArr[i]);
        }
        printf("] (%d/%d)\n", step, numSteps);
        */

        clChoice = candidateClauses[fastrandN(numUnsatClauses)];
        
        /* determine which literal to flip: */
        int numCandidates = 0;
        int minNewUnsat = clauseMap->numClauses;
        int epsilonInt = (fastrand() & EPSILON_MASK);

        for(i = 0; i < clauseMap->clauseLen; ++i){
            int lit = clauseMap->sentence[i + (clChoice-1)*clauseMap->clauseLen];
            if(lit){
                if(epsilonInt < epsilonThreshold){
                    candidateLits[numCandidates] = lit;
                    ++numCandidates;
                } else {
                    int newUnsat = countNewlyUnsatClauses(clauseMap,lit,clauseSatLiterals);
                    
                    if(newUnsat < minNewUnsat){
                        minNewUnsat = newUnsat;
                        numCandidates = 0;
                    }
                    if(newUnsat == minNewUnsat){
                        candidateLits[numCandidates] = lit;
                        ++numCandidates;
                    }
                }
            }
        }

        /* randomly pick and flip a candidate */
        flipLiteral = abs(candidateLits[fastrandN(numCandidates)]);
        assert(flipLiteral != 0);
        if(!solnArr[flipLiteral]){ flipLiteral = -flipLiteral; }
        numUnsatClauses = flip(clauseMap, flipLiteral, clauseSatLiterals, candidateClauses);
        solnArr[abs(flipLiteral)] ^= TRUE;
    }

    free(clauseSatLiterals);
    free(candidateClauses);
    free(candidateLits);

    return (numUnsatClauses == 0);
}

Bool walksat3CNF(int sentence[], int numClauses, Bool solnArr[], 
                    Ullint numSteps, double epsilon){
    return walksatKCNF(sentence, numClauses, 3, solnArr, numSteps, epsilon);
}

Bool walksatKCNF(int sentence[], int numClauses, int k, Bool solnArr[],
                    Ullint numSteps, double epsilon){
    ClauseMap cm;
    Bool result = TRUE;

    /* reduce the sentence (with the preprocessor)*/
    int sentenceLen = numClauses*k;
    int* reducedSentence = malloc(sentenceLen*sizeof(int));
    int i, maxVar = 0;
    for(i = 0; i < sentenceLen; ++i){
        reducedSentence[i] = sentence[i];
        if(abs(sentence[i]) > maxVar){ maxVar = abs(sentence[i]); }
    }
    int* reducedXlatMap = malloc((maxVar+1)*sizeof(int));
    reduceKCNF(reducedSentence, &numClauses, k, reducedXlatMap);
    Bool* reducedSolnArr = calloc((maxVar+1),sizeof(Bool));
    for(i = 1; i <= maxVar; ++i){
        if(reducedXlatMap[i] > 0){
            reducedSolnArr[reducedXlatMap[i]] = solnArr[i];
        }
    }

    /* solve the sentence if it is nontrivial */
    if(numClauses > 0){

        /* initialize clause map */
        result = walksatRawKCNF(reducedSentence, numClauses, k, reducedSolnArr, numSteps, epsilon);

    }

    /* record result */
    for(i = 1; i <= maxVar; ++i){
        if(reducedXlatMap[i] > 0){
            solnArr[i] = (reducedSolnArr[reducedXlatMap[i]]);
        } else {
            solnArr[i] = (reducedXlatMap[i] == TRUE_XLAT_ASSIGNMENT);
        }
    }

    /* clean up */
    free(reducedSentence);
    free(reducedXlatMap);
    free(reducedSolnArr);

    return result;
}

Bool walksatRawKCNF(int sentence[], int numClauses, int k, Bool solnArr[],
                    Ullint numSteps, double epsilon){
        
    /* run WalkSAT directly (no preprocessing) */
    ClauseMap cm;
    initKCNFClauseMap(&cm, sentence, k, numClauses);
    Bool result = runWalkSAT(&cm, solnArr, numSteps, epsilon);
    destroyClauseMap(&cm);

    return result;
}