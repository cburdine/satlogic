#ifndef WALKSAT_H
#define WALKSAT_H

#include <stdio.h>
#include <stdlib.h>
#include "stochastic_structures.h"

Bool walksatStaticCNF(ClauseMap* clauseMap, Bool solnArr[], 
                        unsigned int maxSteps, double epsilon);

Bool walksat3CNF(int sentence[], int numClauses, Bool solnArr[], 
                    unsigned int maxSteps, double epsilon);

Bool walksatKCNF(int sentence[], int numClauses, int k, Bool solnArr[], 
                    unsigned int maxSteps, double epsilon);


#endif /* WALKSAT_H */