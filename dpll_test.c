#include <stdio.h>
#include <stdlib.h>
#include "dpll.h"

int main(int argc, char** argv){

    Bool result;
    Clause clauses[8] = {
        { { 1, 2, 3}, {1,1,1,1} },
        { {-1, 2, 3}, {1,1,1,1} },
        { { 1,-2, 3}, {1,1,1,1} },
        { { 1, 2,-3}, {1,1,1,1} },
        { {-1,-2, 3}, {1,1,1,1} },
        { { 1,-2,-3}, {1,1,1,1} },
        { {-1, 2,-3}, {1,1,1,1} },
        { {-1,-2,-3}, {1,1,1,1} }
    };
    
    result = dpll(clauses, 8, 3);

    printClauses(clauses, 8, stdout);

    fprintf(stdout,"SATISFIABLE: %d\n",result);

    return 0;
}
