#ifndef DPLL_H
#define DPLL_H

#include <stdio.h>
#include <stdlib.h>
#include "dpll_structures.h"

/* improvement upon old dpll */
Bool dpllStatic3Sat(Clause* clauses, int numClauses, int numVariables, Bool solnArr[]);

/* dedups and converts variables into clauses, storing result
   in solnArr */
Bool dpll3Sat(int sentence[], int sentenceLen, Bool solnArr[]);

#endif /* DPLL_H */