
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
int* pushNewSentence(SentenceStack* stack, Clause** newClauses);
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
void destroy(LiteralToClauseMap* map);


