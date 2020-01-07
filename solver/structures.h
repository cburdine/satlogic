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
    int top, 
        maxStackheight, 
        popedSentenceLen;
    Clause* poppedSentence;
} SentenceStack;

/* SentenceStack Operations */
void initSentenceStack(SentenceStack* stack, int maxNumVariables, int maxNumClauses);
int* pushNewEmptySentence(SentenceStack* stack, Clause** newClauses);
void pushSentenceCopy(SentenceStack* stack, Clause* sentence, int numClauses);
void popSentence(SentenceStack* stack);
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

static Bool setContainsLiteral(LiteralInstanceSet* lset, int literal){
    return lset->contains[(literal < 0? -1 * literal : literal)];
}

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
void clearVSIDSMap(VSIDSMap* map);
void setVSIDSMapToVarFrequencies(VSIDSMap* map, Clause* sentence, int numClauses);
void bumpConflictClause(VSIDSMap* map, Clause* conflict);
void deactivateVariable(VSIDSMap* map, int var);
void reactivateVariable(VSIDSMap* map, int var);
void destroyVSIDSMap(VSIDSMap* map);

void printVSIDSMap(VSIDSMap* map, FILE* out);
Bool isValidVSIDSMap(VSIDSMap* map);


typedef struct LiteralAssignmentStack {
    int maxNumVariables;
    int top;
    int** variableAssignments;
    int* numAssignments;
    int* branchVariables;
    Bool* onSecondBranch;

} LiteralAssignmentStack;

void initLiteralAssignmentStack(LiteralAssignmentStack* stack, int maxLitVal);
void clearLiteralAssignmentStack(LiteralAssignmentStack* stack);
void addLiteral(LiteralAssignmentStack* stack, int literal);
void pushNewFrame(LiteralAssignmentStack* stack);
void destroyLiteralAssignmentStack(LiteralAssignmentStack* stack);
void backtrackToNextStackFrame(LiteralAssignmentStack* stack, VSIDSMap* literalMap);
void recordVariableAssignments(LiteralAssignmentStack* stack, Bool* solnArr);
void printLiteralAssignmentStack(LiteralAssignmentStack* stack, FILE* out);

static void toggleStackFrameBranch(LiteralAssignmentStack* stack){
    stack->onSecondBranch[stack->top] = !(stack->onSecondBranch[stack->top]);
    stack->branchVariables[stack->top] *= -1;
}

static void setBranchLiteral(LiteralAssignmentStack* stack, int branchLiteral){
    stack->branchVariables[stack->top] = branchLiteral;
}

static void popFrame(LiteralAssignmentStack* stack){ 
    --(stack->top);
}

#endif /* STRUCTURES_H */