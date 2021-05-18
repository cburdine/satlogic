#ifndef PREPROCESSOR_STRUCTURES_H
#define PREPROCESSOR_STRUCTURES_H

typedef char Bool;

#define TRUE 1
#define FALSE 0

typedef struct OrderedClause {
    int* literals;
    int size;
} OrderedClause;

void initOrderedClause(OrderedClause* oc, int clause[], int clauseLen);

/* returns TRUE if the clause contains a literal and its negation */
Bool isTrivial(OrderedClause* oc);

Bool orderedClauseLessThan(OrderedClause* l, OrderedClause* r);

Bool orderedClauseEqual(OrderedClause* l, OrderedClause* r);

int findVariableOccurrence(OrderedClause* oc, int var);

void destroyOrderedClause(OrderedClause* oc);

void sortOrderedClauseArr(OrderedClause* arr, int arrLen);

void printOrderedClause(OrderedClause* oc, FILE* out);

void printOrderedClauseArr(OrderedClause* ocArr, int arrLen, FILE* out);

#endif /* PREPROCESSOR_STRUCTURES_H */