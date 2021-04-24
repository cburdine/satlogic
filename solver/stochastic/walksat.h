#ifndef WALKSAT_H
#define WALKSAT_H

#include <stdio.h>
#include <stdlib.h>
#include "stochastic_structures.h"

Bool walksatStaticCNF(ClauseMap* clauseMap, int numClauses, int numVariables);

Bool walksat3CNF(int sentence[], int sentenceLen, Bool solnArr[], 
                    unsigned int maxLog2Steps, double epsilon);


#endif /* WALKSAT_H */