#ifndef STOCHASTIC_STRUCTURES_H
#define STOCHASTIC_STRUCTURES_H

typedef char Byte;
typedef char Bool;

/* macros for Boolean values */
#define TRUE 1
#define FALSE 0

typedef struct ClauseMap {

    /* map of: variable -> [ clause occurrences ] */
    int** map;

    /* map of: variable -> number of clause occurrences */
    int* mapLen;

    /* sentence (in sequential form) */
    int* sentence;

    /* clause length (k) */
    int clauseLen;

    /* number of variables present in the sentence */
    int numVariables;

    /* number of clauses present in the sentence */
    int numClauses;
    
} ClauseMap;

void init3CNFClauseMap(ClauseMap* cm, int sentence[], int numClauses);
void initKCNFClauseMap(ClauseMap* cm, int sentence[], int k, int numClauses);
void destroyClauseMap(ClauseMap* cm);
int countNewlyUnsatClauses(ClauseMap* cm, int flipLiteral, int clauseSatLiterals[]);
int flip(ClauseMap* cm, int flipLiteral, int clauseSatLiterals[], int candidateClauses[]);

#endif /* STOCHASTIC_STRUCTURES_H */