#ifndef STRUCTURES_H
#define STRUCTURES_H


#include <stdio.h>

typedef char Byte;

typedef char Bool;

/* macros for Boolean values */
#define TRUE 1
#define FALSE 0

/* Clause structure */
#define CLAUSE_SIZE 3
typedef struct Clause{
    int literals[CLAUSE_SIZE];
    Bool active[CLAUSE_SIZE];
    Byte numActiveLiterals;
} Clause;

/* Clause Operations */
void printClauses(Clause* clauses, int numClauses, FILE* out);

/* SentenceStack structure */
typedef struct SentenceStack{
    Clause** sentences;
    int* sentenceLengths;
    int top, maxStackheight;
} SentenceStack;

/* SentenceStack Operations */
void initSentenceStack(SentenceStack* stack, int maxNumVariables, int maxNumClauses);
int* pushNewEmptySentence(SentenceStack* stack, Clause** newClauses);
void pushExistingSentence(SentenceStack* stack, Clause* sentence, int numClauses);
int popSentence(SentenceStack* stack, Clause** poppedClauses);
void destroySentenceStack(SentenceStack* stack);

/* LiteralInstanceSet structure */
typedef struct LiteralInstanceSet{
    Bool* contains;
    int* literals;
    int maxLitVal;
    int size;
} LiteralInstanceSet;

/* LiteralInstanceSet Operations */
void initLiteralInstanceSet(LiteralInstanceSet* lset, int maxLitVal);
void insertLiteral(LiteralInstanceSet *lset, int literal);
void clearAllLiterals(LiteralInstanceSet* lset);
void destroyLiteralInstanceSet(LiteralInstanceSet* lset);

/* LiteralToClauseMap */
typedef struct LiteralToClauseMap {
    int maxNumVariables, maxNumClauses;
    int* literalFrequency;
    Clause*** clauseOccurrences;
    Byte** clauseIndices;
} LiteralToClauseMap;

/* LiteralToClauseMap  operations */
void initLiteralToClauseMap(LiteralToClauseMap* map, int maxNumVariables, int maxNumClauses);
void clearAllClauseLiterals(LiteralToClauseMap* map);
void insertClauseLiterals(LiteralToClauseMap* map, Clause* clause);
void destroyLiteralToClauseMap(LiteralToClauseMap* map);

/* Variable VSIDSMap */
/* Note: This will be implemented in the future as
         a priority queue to provide sub-linear insertions
         and instant identification of a branching variable
         (will change bump factor instead of all scores)
*/
typedef struct VSIDSMap {
    int* scorePQ;
    int* scorePQInverse;
    double* scores;
    int maxLitVal;
    double bumpGrowthFactor;
    double bumpValue;
    double scaleDownFactor;
} VSIDSMap;
static const double VSIDS_SCALE_DOWN_THRESHOLD = 1.0e32;


/* VSIDSMap operations */
void initVSIDSMap(VSIDSMap* map, int maxLitVal, double decayFactor, double initBumpValue);
void clearVSIDSMap(VSIDSMap* map, double bumpValue);
void bumpConflictClause(VSIDSMap* map, Clause* conflict);
void printVSIDSMap(VSIDSMap* map, FILE* out);
Bool isValidVSIDSMap(VSIDSMap* map);
void destroyVSIDSMap(VSIDSMap* map);

#endif /* STRUCTURES_H */