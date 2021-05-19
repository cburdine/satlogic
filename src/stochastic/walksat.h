#ifndef WALKSAT_H
#define WALKSAT_H

#include <stdio.h>
#include <stdlib.h>
#include "stochastic_structures.h"

typedef unsigned long long int Ullint;

Bool runWalkSAT(ClauseMap* clauseMap, Bool solnArr[], 
                    Ullint numSteps, double epsilon);

Bool walksat3CNF(int sentence[], int numClauses, Bool solnArr[], 
                    Ullint numSteps, double epsilon);

Bool walksatKCNF(int sentence[], int numClauses, int k, Bool solnArr[], 
                    Ullint numSteps, double epsilon);

Bool walksatRawKCNF(int sentence[], int numClauses, int k, Bool solnArr[],
                    Ullint numSteps, double epsilon);


#endif /* WALKSAT_H */