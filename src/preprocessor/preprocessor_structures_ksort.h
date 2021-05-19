#ifndef PREPROCESSOR_STRUCTURES_KSORT_H
#define PREPROCESSOR_STRUCTURES_KSORT_H

#include "../klib/ksort.h"
#include "preprocessor_structures.h"

/* Sorting templates for literals in OrderedClauses */
#define literal_lt(a,b) ((abs(a) < abs(b)) || ((abs(a) == abs(b)) && (a < b)))
KSORT_INIT(literal, int, literal_lt)

/* Sorting templates for lists of OrderedClauses */
#define ordered_clause_lt(a,b) (orderedClauseLessThan(&a, &b))
KSORT_INIT(ordered_clause, OrderedClause, ordered_clause_lt)

#endif /* PREPROCESSOR_STRUSTURES_KSORT_H */