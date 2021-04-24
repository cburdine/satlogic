#ifndef STOCHASTIC_STRUCTURES_H
#define STOCHASTIC_STRUCTURES_H

typedef char Byte;
typedef char Bool;

/* macros for Boolean values */
#define TRUE 1
#define FALSE 0

typedef struct ClauseMap {
    int** map;
    int* mapLen;
    int numVariables;
} ClauseMap;

void init3CNFClauseMap(ClauseMap* cm, int sentence[], int sentenceLen);
void initKCNFClauseMap(ClauseMap* cm, int sentence[], int k, int numClauses);
void destroyClauseMap(ClauseMap* cm);

#endif /* STOCHASTIC_STRUCTURES_H */