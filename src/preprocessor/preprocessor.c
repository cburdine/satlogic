#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "preprocessor.h"

#define sign(x) (-(x < 0) + (x > 0))

void reduceKCNF(int sentence[], int* numClauses, int k, int varXlatMap[]){
    
    int i, j;
    assert(k > 0);
    assert(*numClauses >= 0);
    
    /* determine the number of variables */
    int maxVar = 0;
    for(i = 0; i < k*(*numClauses); ++i){
        if(maxVar < abs(sentence[i])){ maxVar = abs(sentence[i]); }
    }
    
    /* clear xlat map */
    for(i = 0; i <= maxVar; ++i){ varXlatMap[i] = FALSE_XLAT_ASSIGNMENT; }

    /* initialize a set of (non-trivial) OrderedClauses */
    OrderedClause* ocArr = malloc(sizeof(OrderedClause)*(*numClauses));
    int numOrderedClauses = 0;
    for(i = 0; i < *numClauses; ++i){
        int* sentencePtr = (sentence+(i*k));
        initOrderedClause(ocArr+numOrderedClauses, sentencePtr, k);
        if(isTrivial(ocArr+numOrderedClauses)){
            destroyOrderedClause(ocArr+numOrderedClauses);
        } else {
            ++numOrderedClauses;
        }
    }
    *numClauses = numOrderedClauses;

    /* sort and remove duplicate clauses */
    Bool* removedClauses = calloc(numOrderedClauses, sizeof(Bool));
    if(numOrderedClauses){
        sortOrderedClauseArr(ocArr, numOrderedClauses);
        for(i = 1; i < *numClauses; ++i){
            if(orderedClauseEqual(ocArr+i,ocArr+(i-1))){
                removedClauses[i] = TRUE;
            }
        }
    }

    /* identify pure literals and resolve them */
    Bool* positiveLits = malloc((maxVar+1)*sizeof(Bool));
    Bool* negativeLits = malloc((maxVar+1)*sizeof(Bool));
    Bool* unassignedLits = malloc((maxVar+1)*sizeof(Bool));
    int lit, var;
    Bool foundPureLiteral = TRUE;
    while(foundPureLiteral){

        for(i = 0; i <= maxVar; ++i){ 
            unassignedLits[i] = positiveLits[i] = negativeLits[i] = FALSE; 
        }
        foundPureLiteral = FALSE;

        for(i = 0; i < numOrderedClauses; ++i){
            for(j = 0; !removedClauses[i] && j < ocArr[i].size; ++j){
                lit = ocArr[i].literals[j];
                if(lit > 0){
                    positiveLits[abs(lit)] = TRUE;
                } else if(lit < 0){
                    negativeLits[abs(lit)] = TRUE;
                }
            }
        }
        for(i = 0; i < numOrderedClauses; ++i){
            for(j = 0; !removedClauses[i] && j < ocArr[i].size; ++j){
                var = abs(ocArr[i].literals[j]);
                if(positiveLits[var] != negativeLits[var]){
                    if(positiveLits[var]){
                        varXlatMap[var] = TRUE_XLAT_ASSIGNMENT;
                    } else {
                        varXlatMap[var] = FALSE_XLAT_ASSIGNMENT;
                    }
                    removedClauses[i] = TRUE;
                    foundPureLiteral = TRUE;
                } else {
                    unassignedLits[var] = (positiveLits[var] || negativeLits[var]);
                }
            }
        }
    }

    /* populate xlat map and construct reduced sentence */
    int numFinalClauses = 0;
    int sentenceIdx = 0;
    for(i = 0; i < numOrderedClauses; ++i){
        if(!removedClauses[i]){
            assert(ocArr[i].size > 0);
            for(j = 0; j < k; ++j){
                if(j < ocArr[i].size){
                    lit = ocArr[i].literals[j];
                    if(varXlatMap[abs(lit)] <= 0){
                        unassignedLits[abs(lit)] = TRUE;
                    }
                    sentence[sentenceIdx] = lit;
                } else {
                    sentence[sentenceIdx] = 0;
                }
                ++sentenceIdx;
            }
            ++numFinalClauses;
        }
    }
    *numClauses = numFinalClauses;

    /* populate xlat map and translate sentence */
    int numFinalVars = 0;
    varXlatMap[0] = 0;
    for(i = 1; i <= maxVar; ++i){
        if(unassignedLits[i]){
            ++numFinalVars;
            varXlatMap[i] = numFinalVars;
        }   
    }
    for(i = 0; i < sentenceIdx; ++i){
        lit = sentence[i];
        sentence[i] = sign(lit) * varXlatMap[abs(lit)];
    }

    /* clean up */
    for(i = 0; i < numOrderedClauses; ++i){
        destroyOrderedClause(ocArr+i);
    }
    free(ocArr);
    free(positiveLits);
    free(negativeLits);
    free(unassignedLits);
    free(removedClauses);
}


Bool toKCNF(int sentence[], int *numClauses, int capactiy, int k, int kNew){
    
    int* newSentence = NULL;
    int i, j, l, w, newSentenceLen;

    assert(k > 0);
    assert(kNew > 0);
    assert(*numClauses >= 0);
    if(kNew == k){ 
        return TRUE;
    } else if(kNew > k){
        newSentenceLen = (*numClauses)*kNew;
        if(newSentenceLen > capactiy){ return FALSE; }
        newSentence = malloc(newSentenceLen*sizeof(int));
        for(i = 0; i < *numClauses; ++i){
            for(j = 0; j < kNew; ++j){
                newSentence[i*kNew+j] = (j < k)? sentence[i*k+j] : 0;
            }
        }
    } else {
        
        /* requre k >= 3 to avoid exponential blowup in sentence size*/
        if(kNew < 3){ return FALSE; }

        /* allote new sentence (if within capacity) */
        int clauseMult = (k + (kNew-3))/(kNew-2);
        newSentenceLen = kNew*clauseMult*(*numClauses);
        if(newSentenceLen > capactiy){ return FALSE; }
        newSentence = malloc(newSentenceLen*sizeof(int));
        

        /* determine the maximum variable */
        int maxVar = 1;
        for(i = 0; i < *numClauses*k; ++i){
            if(abs(sentence[i]) > maxVar){ maxVar = abs(sentence[i]); }
        }

        /* split clauses */
        int nsIdx = 0;
        for(i = 0; i < *numClauses; ++i){
            l = 0;
            for(j = 0; j < clauseMult; ++j){
                /* append clauses prefix */
                newSentence[nsIdx] = (j > 0)? maxVar : 0;
                ++nsIdx;
                /* append clause body */
                for(w = 0; w < (kNew - 2); ++w){
                    newSentence[nsIdx] = (l < k)? sentence[i*k+l] : 0;
                    ++nsIdx;
                    ++l;
                }
                /* append clause suffix */
                newSentence[nsIdx] = (j+1 < clauseMult)? -(++maxVar) : 0;
                ++nsIdx;
            }
        }
        *numClauses *= clauseMult;
    }

    /* copy newSentence to sentence and return successful*/
    for(i = 0; i < newSentenceLen; ++i){
        sentence[i] = newSentence[i];
    }

    free(newSentence);

    return TRUE;
}