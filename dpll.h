#include <stdio.h>
#include <stdlib.h>

typedef char Bool;

typedef struct Clause{
    int literals[3];
    Bool active[4];
} Clause;


typedef struct VarSet{
    Bool* contains;
    int numValues;
} VarSet;

/* hidden Varset Operations */
void init(VarSet* vset, int numVars);
void insertVar(VarSet *vset, int var);
void clear(VarSet* vset);
void printClauses(Clause* clauses, int numClauses, FILE* out);

/* assumes all vars in clauses are deduped */
Bool dpll(Clause* clauses, int numClauses, int numVariables);

/* dedups and converts variables into clauses */
Bool dpll3Sat(int* sentence, int sentenceLen);
