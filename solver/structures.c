
#include "structures.h"
#include <string.h>

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

int* pushNewSentence(SentenceStack* stack, Clause** newClauses){
    *newClauses = (stack->sentences[stack->top]);

    // increment top after returning current top:
    return &(stack->sentenceLengths[(stack->top)++]);
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


