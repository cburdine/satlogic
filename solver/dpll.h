#include <stdio.h>
#include <stdlib.h>

/* assumes all vars in clauses are deduped */
Bool dpll(Clause* clauses, int numClauses, int numVariables);

/* improvement upon old dpll */
Bool dpllStatic(Clause clauses, int numClauses, int numVariables);

/* dedups and converts variables into clauses */
Bool dpll3Sat(int sentence[], int sentenceLen);
