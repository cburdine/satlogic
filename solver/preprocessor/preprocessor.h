#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "preprocessor_structures.h"
static const int FALSE_XLAT_ASSIGNMENT = 0;
static const int TRUE_XLAT_ASSIGNMENT = -1;

/* 
    Reduces a sentence, removing duplicated literals and pads short clauses
    with zeros. It overrites the existing sentence. FALSE is returned if
    operation fails. Variables may be removed as a result of this function.
    The variable translations are determined by:

        varXlatMap[var] == translatedVar

    If a trivial variable assignment is made, then 

        varXlatMap[var] == FALSE_XLAT_ASSIGNMENT
    or
        varXlatMap[var] == TRUE_XLAT_ASSIGNMENT

    respectively.
*/
void reduceKCNF(int sentence[], int* numClauses, int k, int varXlatMap[]);

/*
    Converts a sentence to 3CNF, assuming the sentence is large enough to hold
    the reduced formula. Variable structure is preserved. Returns False if 
    capacity of sentence is too small, or the operation cannot be performed.
*/
Bool toKCNF(int sentence[], int *numClauses, int capactiy, int k, int kNew);

#endif /* PREPROCESSOR_H */