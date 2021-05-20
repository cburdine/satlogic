
#include "model_structures.h"

#include <stdlib.h>
#include <assert.h>
#include "../klib/khash_int_2d.h"

/* const initializations */
static ModelTrieNode END_SOLN_NODE = { SOLN, NULL, NULL };
static ModelTrieNode END_NOT_SOLN_NODE = { NOT_SOLN, NULL, NULL };

void initModelCompressionMap(ModelCompressionMap* cmap, int maxNumVariables, int baseNodesPerLevel){

    int i;

    cmap->maxNumVariables = maxNumVariables;

    cmap->compressedNodeChildren = malloc((maxNumVariables+2)*(maxNumVariables+2) * sizeof(int[3]));

    /* allocate array for partitioning of nodes: */
    cmap->nodePartitions = malloc(sizeof(ModelTrieNode**)*(maxNumVariables+1));
    cmap->nodePartitionSizes = malloc(sizeof(size_t)*(maxNumVariables+1));
    cmap->nodePartitionCapacities = malloc(sizeof(size_t)*(maxNumVariables+1));

    cmap->nodePartitions[0] = NULL;
    for(i = 1; i <= maxNumVariables; ++i){
        cmap->nodePartitions[i] = malloc(sizeof(ModelTrieNode*)*baseNodesPerLevel);
        cmap->nodePartitionSizes[i] = 0;
        cmap->nodePartitionCapacities[i] = baseNodesPerLevel;
    }
    
    /* increment to allow '-1' indexing */
    ++(cmap->compressedNodeChildren);
    cmap->compressedNodeChildrenSize = 1;

    /* set children of terminal nodes */
    cmap->compressedNodeChildren[0][CHILD_POS_IND] = NOT_SOLN;
    cmap->compressedNodeChildren[0][CHILD_NEG_IND] = NOT_SOLN;
    cmap->compressedNodeChildren[0][NODE_VAR_IND] = NOT_SOLN;

    cmap->compressedNodeChildren[-1][CHILD_POS_IND] = SOLN;
    cmap->compressedNodeChildren[-1][CHILD_NEG_IND] = SOLN;
    cmap->compressedNodeChildren[-1][NODE_VAR_IND] = SOLN;

    /* initialize node index hash map */
    initKhashInt2d(&(cmap->nodeIndexMap));
}

void resetModelCompressionMap(ModelCompressionMap* cmap){
    int i;

    for(i = 0; i <= cmap->maxNumVariables; ++i){
        cmap->nodePartitionSizes[i] = 0;
    }

    cmap->compressedNodeChildrenSize = 1;
}

void destroyModelCompressionMap(ModelCompressionMap* cmap){
    int i;

    /* free node partitions array */
    for(i = 1; i <= cmap->maxNumVariables; ++i){
        free(cmap->nodePartitions[i]);
    }

    free(cmap->nodePartitions);
    free(cmap->nodePartitionSizes);
    free(cmap->nodePartitionCapacities);

    /* decrement to shift array from '-1' index */
    --(cmap->compressedNodeChildren);
    free(cmap->compressedNodeChildren);

    /* destroy node index hashmap */
    destroyKhashInt2d(&(cmap->nodeIndexMap));
}

void initModelFromClause(Model* model, int* orderedClause, int clauseSize, ModelCompressionMap* cmap){

    ModelTrieNode *root, **current;
    int numNodes = 0;

    assert(clauseSize >= 0);

    root = NULL;
    current = &root;

    for(int i =0; i < clauseSize; ++i){

        assert(orderedClause[i] != 0);

        ++numNodes;
        (*current) = malloc(sizeof(ModelTrieNode));
        (*current)->var = abs(orderedClause[i]);

        if(orderedClause[i] > 0){
            (*current)->neg = &END_NOT_SOLN_NODE;
            current = &((*current)->pos);
        } else {
            (*current)->pos = &END_NOT_SOLN_NODE;
            current = &((*current)->neg);
        }
    }

    (*current) = &END_SOLN_NODE;

    model->root = root;
    model->numNodes = numNodes;
}

ModelTrieNode** auxDestroyModel(ModelTrieNode* subtrie, ModelTrieNode** freeList);

void destroyModel(Model* model){

    ModelTrieNode **freeList, **freeListEnd;

    freeList = malloc(sizeof(ModelTrieNode*)*model->numNodes);

    freeListEnd = auxDestroyModel(model->root, freeList);

    /* free each element in free list */
    while(freeListEnd > freeList){
        --freeListEnd;
        free(*freeListEnd);
    }
    
    free(freeList);
}

ModelTrieNode** auxDestroyModel(ModelTrieNode* subtrie, ModelTrieNode** freeList){
    if(subtrie != NULL && subtrie->var > 0){
        
        /* mark node and add to free list */
        subtrie->var = 0;
        *freeList = subtrie;
        ++freeList;

        /* recursively delete subtrie */
        freeList = auxDestroyModel(subtrie->pos, freeList);
        freeList = auxDestroyModel(subtrie->neg, freeList);
    }

    return freeList;
}

void partitionModel(ModelTrieNode* subtrie, ModelCompressionMap* cmap);
void compressModel(Model* model, ModelCompressionMap* cmap){
    
    int i, var, posInd, negInd, ncInd, currentPos, currentNeg;
    ModelTrieNode** compressedNodes;
    ModelTrieNode* current;

    assert(model->root != NULL);

    /* partition the nodes of the model */
    partitionModel(model->root, cmap);

    for(var = cmap->maxNumVariables; var > 0; --var){

        for(int i = 0; i < cmap->nodePartitionSizes[var]; ++i){
            
            current = cmap->nodePartitions[var][i];


            assert(current->var == var);

            /*  read the map indices of the children 
                (note their vars are overridden by map indices) */
            currentPos = current->pos->var;
            currentNeg = current->neg->var;


            /* if both subtries are isomorphic, ignore this node */
            if(currentPos == currentNeg){
                ncInd = currentPos;

            } else {


                /* if index has not been put in compression map, add it */
                ncInd = hashGet(&(cmap->nodeIndexMap), currentPos, currentNeg);
                if(!(cmap->nodeIndexMap.last_result)){
                    ncInd = cmap->compressedNodeChildrenSize;
                    hashPut(&(cmap->nodeIndexMap), currentPos, currentNeg, ncInd);
                    cmap->compressedNodeChildren[ncInd][CHILD_POS_IND] = currentPos;
                    cmap->compressedNodeChildren[ncInd][CHILD_NEG_IND] = currentNeg;
                    cmap->compressedNodeChildren[ncInd][NODE_VAR_IND] = var;
                    ++(cmap->compressedNodeChildrenSize);
                }
            }

            /* override node's variable with new index in map (saves extra recursion)*/
            current->var = ncInd;
        }

        clear(&(cmap->nodeIndexMap));
    }
    
    compressedNodes = malloc((cmap->compressedNodeChildrenSize+1)*sizeof(ModelTrieNode*));

    for(i = 1; i < cmap->compressedNodeChildrenSize; ++i){

        posInd = cmap->compressedNodeChildren[i][CHILD_POS_IND];
        negInd = cmap->compressedNodeChildren[i][CHILD_NEG_IND];

        /* allocate new node */
        compressedNodes[i] = malloc(sizeof(ModelTrieNode));
        compressedNodes[i]->var = cmap->compressedNodeChildren[i][NODE_VAR_IND];
        
        if(posInd > 0){
            compressedNodes[i]->pos = compressedNodes[posInd];
        } else if(posInd == NOT_SOLN){
            compressedNodes[i]->pos = &END_NOT_SOLN_NODE;
        } else {
            assert(posInd == SOLN);
            compressedNodes[i]->pos = &END_SOLN_NODE;
        }
        
        if(negInd > 0){
            compressedNodes[i]->neg = compressedNodes[negInd];
        } else if (negInd == NOT_SOLN){
            compressedNodes[i]->neg = &END_NOT_SOLN_NODE;
        } else {
            compressedNodes[i]->neg = &END_SOLN_NODE;
        }
    }

    /* delete old model */
    destroyModel(model);

    /* replace old model with compresed model */
    model->numNodes = (cmap->compressedNodeChildrenSize-1);
    model->root = compressedNodes[model->numNodes];

    /* free serial array */
    free(compressedNodes);

    /* reset compression map */
    resetModelCompressionMap(cmap);

}

void partitionModel(ModelTrieNode* subtrie, ModelCompressionMap* cmap){
    
    int var;
    size_t next_size;
    var = subtrie->var;
    
    if(var > 0){

        /* resize if needed */
        if(cmap->nodePartitionSizes[var] >= cmap->nodePartitionCapacities[var]){
            /* double size of node Partition */
            next_size = (cmap->nodePartitionCapacities[var]<<1);
            if((cmap->nodePartitions[var] = realloc(cmap->nodePartitions[var], next_size* sizeof(ModelTrieNode*))) == NULL){
                /* if realloc fails */
                perror("realloc() failure (model compression)");
                exit(1);
            }
            cmap->nodePartitionCapacities[var] = next_size;
        }

        /* add to node partition list */
        cmap->nodePartitions[var][cmap->nodePartitionSizes[var]] = subtrie;
        ++(cmap->nodePartitionSizes[var]);

        /* recursively partition subtrie */
        partitionModel(subtrie->pos, cmap);
        partitionModel(subtrie->neg, cmap);
    }
}


ModelTrieNode* auxUnionModels(ModelTrieNode* a, ModelTrieNode* b, size_t* modelSize);
void unionModels(Model* dest, Model* src, ModelCompressionMap* cmap){

    /* TODO: this could possibly be made more efficient by merging this
             with the compressModel function and memoizing calls
    */
    ModelTrieNode* newModel;
    size_t newModelSize;

    newModelSize = 0;
    newModel = auxUnionModels(dest->root, src->root, &newModelSize);

    /* destroy and replace old model with unioned model */
    destroyModel(dest);
    dest->root = newModel;
    dest->numNodes = newModelSize;

    compressModel(dest, cmap);
}

ModelTrieNode* auxUnionModels(ModelTrieNode* a, ModelTrieNode* b, size_t* modelSize){
    
    ModelTrieNode *newNode, *a_neg, *b_neg;

    assert(a != NULL);
    assert(b != NULL);

    if((a->var == SOLN) || (b->var == SOLN)){
        /* TRUE base case for OR operation */
        return &END_SOLN_NODE;

    } else if(a->var == NOT_SOLN && b->var == NOT_SOLN){
        /* FALSE base case for OR operation */
        return &END_NOT_SOLN_NODE;

    }

    /* recursively OR submodels */
    newNode = malloc(sizeof(ModelTrieNode));
    ++(*modelSize);
    
    if(a->var == NOT_SOLN){
        newNode->var = b->var;
        a_neg = &END_NOT_SOLN_NODE;
        b_neg = b->neg;
        b = b->pos;

    } else if(b->var == NOT_SOLN) {
        newNode->var = a->var;
        b_neg = &END_NOT_SOLN_NODE;
        a_neg = a->neg;
        a = a->pos;
        
    } else {
        
        /* set new node to be variable that comes first */
        if(a->var < b->var){
            newNode->var = a->var;
        } else {
            newNode->var = b->var;
        }

        /* determine which children to union */
        if(a->var == newNode->var){
            a_neg = a->neg;
            a = a->pos;
        } else {
            a_neg = a;
        } if(b->var == newNode->var){
            b_neg = b->neg;
            b = b->pos;
        } else {
            b_neg = b;
        }
    }
    
    /* recusively union children */
    newNode->pos = auxUnionModels(a,b, modelSize);
    newNode->neg = auxUnionModels(a_neg, b_neg, modelSize);

    // TODO: finish this!

    return newNode;
}


ModelTrieNode* auxIntersectModels(ModelTrieNode* a, ModelTrieNode* b, size_t* modelSize);
void intersectModels(Model* dest, Model* src, ModelCompressionMap* cmap){
    /* TODO: this could possibly be made more efficient by merging this
             with the compressModel function and memoizing calls
    */
    ModelTrieNode* newModel;
    size_t newModelSize;

    newModelSize = 0;
    newModel = auxIntersectModels(dest->root, src->root, &newModelSize);

    /* destroy and replace old model with intersection model */
    destroyModel(dest);
    dest->root = newModel;
    dest->numNodes = newModelSize;

    compressModel(dest, cmap);
}

ModelTrieNode* auxIntersectModels(ModelTrieNode* a, ModelTrieNode* b, size_t* modelSize){
    ModelTrieNode *newNode, *a_neg, *b_neg;

    assert(a != NULL);
    assert(b != NULL);

    if((a->var == NOT_SOLN) || (b->var == NOT_SOLN)){
        /* FALSE base case for OR operation */
        return &END_NOT_SOLN_NODE;

    } else if(a->var == SOLN && b->var == SOLN){
        /* TRUE base case for OR operation */
        return &END_SOLN_NODE;

    }

    /* recursively AND submodels */
    newNode = malloc(sizeof(ModelTrieNode));
    ++(*modelSize);
    
    if(a->var == SOLN){
        newNode->var = b->var;
        a_neg = &END_NOT_SOLN_NODE;
        b_neg = b->neg;
        b = b->pos;

    } else if(b->var == SOLN) {
        newNode->var = a->var;
        b_neg = &END_NOT_SOLN_NODE;
        a_neg = a->neg;
        a = a->pos;
        
    } else {
        
        /* set new node to be variable that comes first */
        if(a->var < b->var){
            newNode->var = a->var;
        } else {
            newNode->var = b->var;
        }

        /* determine which children to intersect */
        if(a->var == newNode->var){
            a_neg = a->neg;
            a = a->pos;
        } else {
            a_neg = a;
        } if(b->var == newNode->var){
            b_neg = b->neg;
            b = b->pos;
        } else {
            b_neg = b;
        }
    }
    
    /* recusively intersect children */
    newNode->pos = auxIntersectModels(a,b, modelSize);
    newNode->neg = auxIntersectModels(a_neg, b_neg, modelSize);

    return newNode;
}

void auxPrintModel(ModelTrieNode* trie, int* history, int maxNumVariables, FILE* out);
void printModel(Model* model, int maxNumVariables, FILE* out){

    int* history;
    
    history = malloc((maxNumVariables+2)*sizeof(int));

    auxPrintModel(model->root, history, 0, out);

    free(history);
}

void auxPrintModel(ModelTrieNode* trie, int* history, int hInd, FILE* out){

    if(trie->var == SOLN){
        fputs("[", out);
        for(int i = 0; i < hInd; ++i){
            fprintf(out, " %3d", history[i]);
        }
        fputs(" ]\n", out);
    } else if(trie->var != NOT_SOLN){
        history[hInd] = trie->var;
        auxPrintModel(trie->pos, history, hInd+1, out);
        
        history[hInd] *= -1;
        auxPrintModel(trie->neg, history, hInd+1, out);
    }
}