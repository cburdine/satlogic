#ifndef MODEL_STRUCTURES_H
#define MODEL_STRUCTURES_H

#include <stdio.h>
#include "../klib/khash_int_2d.h"

typedef char Byte;
typedef char Bool;

/* macros for Boolean values */
#define TRUE 1
#define FALSE 0

struct ModelTrieNode;
typedef struct ModelTrieNode {
    int var;
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
    int (*compressedNodeChildren)[3];
    size_t compressedNodeChildrenSize;
    ModelTrieNode*** nodePartitions;
    size_t* nodePartitionSizes;
    size_t* nodePartitionCapacities;
    KhashInt2d nodeIndexMap;
} ModelCompressionMap;

#define CHILD_POS_IND 0
#define CHILD_NEG_IND 1
#define NODE_VAR_IND 2

void initModelCompressionMap(ModelCompressionMap* cmap, int maxNumVariables, int baseNodesPerLevel);
void clearModelCompressionMap(ModelCompressionMap* cmap);
void destroyModelCompressionMap(ModelCompressionMap* cmap);

typedef struct Model {
    ModelTrieNode* root;
    int numNodes;
} Model;

void initModelFromClause(Model* model, int* orderedClause, int clauseSize, ModelCompressionMap* cmap);

void compressModel(Model* model, ModelCompressionMap* cmap);

void unionModels(Model* dest, Model* src, ModelCompressionMap* cmap);

void intersectModels(Model* dest, Model* src, ModelCompressionMap* cmap);

void printModel(Model* model, int maxNumVariables, FILE* out);

void destroyModel(Model* model);



#endif /* MODEL_STRUCTURES_H */