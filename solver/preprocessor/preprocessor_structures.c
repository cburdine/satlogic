#include <assert.h>
#include <stdio.h>

#include "preprocessor_structures.h"
#include "preprocessor_structures_ksort.h"

void initOrderedClause(OrderedClause* oc, int clause[], int clauseLen){
    int i;

    assert(clauseLen > 0);
    oc->literals = malloc(sizeof(int)*clauseLen);
    oc->size = 0;

    int maxVar = 0;
    for(i = 0; i < clauseLen; ++i){
        if(clause[i]){
            if(abs(clause[i]) > maxVar){
                maxVar = abs(clause[i]);
            }
            oc->literals[oc->size] = clause[i];
            ++(oc->size);
        }
    }

    /* sort literals in increasing order of magnitude: */
    ks_introsort(literal, oc->size, oc->literals);

    /* dedup literals */
    int w = (oc->size > 0);
    for(i = 1; i < oc->size; ++i){
        if(oc->literals[i] != oc->literals[i-1]){
            oc->literals[w] = oc->literals[i];
            ++w;
        }
    }
    oc->size = w;
}

Bool isTrivial(OrderedClause* oc){
    int i;
    for(i = 1; i < oc->size; ++i){
        if(oc->literals[i] == -(oc->literals[i-1])){
            return TRUE;
        }
    }

    return (oc->size == 0);
}


Bool orderedClauseLessThan(OrderedClause* l, OrderedClause* r){
    int i;

    for(i = 0; (i < l->size) && (i < r->size); ++i){
        if(literal_lt(l->literals[i], r->literals[i])){
            return TRUE;
        } else if(literal_lt(r->literals[i], l->literals[i])){
            return FALSE;
        }
    }
    return (l->size < r->size);
}

Bool orderedClauseEqual(OrderedClause* l, OrderedClause* r){
    
    int i;

    if(l->size != r->size){ return FALSE; }
    for(i = 0; i < l->size; ++i){
        if(l->literals[i] != r->literals[i]){
            return FALSE;
        }
    }

    return TRUE;
}

void destroyOrderedClause(OrderedClause* oc){
    free(oc->literals);
}

void sortOrderedClauseArr(OrderedClause* arr, int arrLen){
    ks_introsort(ordered_clause, arrLen, arr);
}

void printOrderedClause(OrderedClause* oc, FILE* out){
    fprintf(out, "(");
    for(int i = 0; i < oc->size; ++i){
        fprintf(out, "%4d", oc->literals[i]);
    }
    fprintf(out, ")");
}

void printOrderedClauseArr(OrderedClause* ocArr, int arrLen, FILE* out){
    for(int i = 0; i < arrLen; ++i){
        printOrderedClause(ocArr+i, out);
        fprintf(out, " ");
    }
}