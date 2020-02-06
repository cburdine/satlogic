#ifndef MODEL_STRUCTURES_H
#define MODEL_STRUCTURES_H

#include <stdio.h>

typedef char Byte;
typedef char Bool;

/* macros for Boolean values */
#define TRUE 1
#define FALSE 0

struct ModelTrieNode;
typedef struct ModelTrieNode {
    int var;
    int serialIndex;
    struct ModelTrieNode* pos;
    struct ModelTrieNode* neg;
} ModelTrieNode;

/* denotes the end of the model: */
#define SOLN -1
#define NOT_SOLN 0
static ModelTrieNode END_SOLN_NODE;
static ModelTrieNode END_NOT_SOLN_NODE;

/* other useful data structures */
typedef struct ModelCompressionMap {
    int maxNumVariables;
    int numCompressedNodes;
    int (*nodeChildren)[3];
    int** nodeIndexMap;
} ModelCompressionMap;
#define CHILD_POS_IND 0
#define CHILD_NEG_IND 1
#define NODE_VAR_IND 2

void initModelCompressionMap(ModelCompressionMap* cmap, int maxNumVariables);
void destroyModelCompressionMap(ModelCompressionMap* cmap);

typedef struct Model {
    ModelTrieNode* root;
    ModelCompressionMap* compressionMap;
    int numNodes;
} Model;

void initModelFromClause(Model* model, int* orderedClause, int clauseSize, ModelCompressionMap* cmap);

void compressModel(Model* model);

void unionModels(Model* dest, Model* src);

void intersectModels(Model* dest, Model* src);

void printModel(Model* model, FILE* out);

void destroyModel(Model* model);



#endif /* MODEL_STRUCTURES_H */