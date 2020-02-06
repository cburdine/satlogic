
#include "model_structures.h"

#include <stdlib.h>
#include <assert.h>

/* const initializations */
static ModelTrieNode END_SOLN_NODE = { SOLN, SOLN, NULL, NULL };
static ModelTrieNode END_NOT_SOLN_NODE = { NOT_SOLN, NOT_SOLN, NULL, NULL };

void initModelCompressionMap(ModelCompressionMap* cmap, int maxNumVariables){

    int i, j;

    cmap->maxNumVariables = maxNumVariables;
    cmap->numCompressedNodes = 1;
    cmap->nodeChildren = malloc((maxNumVariables+2)*(maxNumVariables+2) * sizeof(int[3]));
    
    /* increment to allow '-1' indexing */
    ++(cmap->nodeChildren);

    /* set children of terminal nodes */
    cmap->nodeChildren[0][CHILD_POS_IND] = NOT_SOLN;
    cmap->nodeChildren[0][CHILD_NEG_IND] = NOT_SOLN;
    cmap->nodeChildren[0][NODE_VAR_IND] = NOT_SOLN;

    cmap->nodeChildren[-1][CHILD_POS_IND] = SOLN;
    cmap->nodeChildren[-1][CHILD_NEG_IND] = SOLN;
    cmap->nodeChildren[-1][NODE_VAR_IND] = SOLN;

    cmap->nodeIndexMap = malloc((maxNumVariables+1)*sizeof(int*));

    /* increment to allow '-1' indexing */
    ++(cmap->nodeIndexMap);

    for(i = -1; i < maxNumVariables; ++i){
        cmap->nodeIndexMap[i] = malloc((maxNumVariables+1)*sizeof(int));
        
        /* increment to allow '-1' indexing */
        ++(cmap->nodeIndexMap[i]);

        for(j = -1; j < maxNumVariables; ++j){
            cmap->nodeIndexMap[i][j] = 0;
        }
    }
}

void destroyModelCompressionMap(ModelCompressionMap* cmap){
    
    int i;

    /* decrement to shift array from '-1' index */
    --(cmap->nodeChildren);
    free(cmap->nodeChildren);

    for(i = -1; i < cmap->maxNumVariables; ++i){

        /* decrement to shift array from '-1' index */
        --(cmap->nodeIndexMap[i]);
        free(cmap->nodeIndexMap[i]);
    }

    /* decrement to shift array from '-1' index */
    --(cmap->nodeIndexMap);
    free(cmap->nodeIndexMap);
}

void initModelFromClause(Model* model, int* orderedClause, int clauseSize, ModelCompressionMap* cmap){

    ModelTrieNode *root, **current;
    int numNodes;

    assert(cmap != NULL);
    assert(clauseSize >= 0);

    root = NULL;
    current = &root;
    numNodes = 0;

    for(int i =0 ; i < clauseSize; ++i){

        assert(orderedClause[i] != 0);
        assert(abs(orderedClause[i]) <= cmap->maxNumVariables);

        ++numNodes;
        (*current) = malloc(sizeof(ModelTrieNode));
        (*current)->var = abs(orderedClause[i]);
        (*current)->serialIndex = numNodes;

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
    model->compressionMap = cmap;
    model->numNodes = numNodes;
}

void auxDestroyModel(ModelTrieNode* subtrie);

void destroyModel(Model* model){
    auxDestroyModel(model->root);
}

void auxDestroyModel(ModelTrieNode* subtrie){
    if(subtrie != NULL && subtrie->var > 0){
        auxDestroyModel(subtrie->pos);
        auxDestroyModel(subtrie->neg);

        free(subtrie);
    }
}

int auxCompressModel(ModelTrieNode* subtrie, ModelCompressionMap* cmap);
void compressModel(Model* model){
    
    int i, posInd, negInd, numNodes;
    ModelTrieNode** compressedNodes;
    ModelCompressionMap* cmap;

    assert(model->root != NULL);
    assert(model->compressionMap != NULL);

    /* find the structure of the compressed model */
    cmap = model->compressionMap;
    auxCompressModel(model->root, cmap);

    /* generate the compressed model */
    numNodes = model->compressionMap->numCompressedNodes;
    
    compressedNodes = malloc(numNodes*sizeof(ModelTrieNode*));
    for(i = 1; i < numNodes; ++i){

        posInd = cmap->nodeChildren[i][CHILD_POS_IND];
        negInd = cmap->nodeChildren[i][CHILD_NEG_IND];

        printf("%d, %d\n", posInd, negInd);

        /* clear compression map entry */
        cmap->nodeIndexMap[posInd][negInd] = 0;

        compressedNodes[i] = malloc(sizeof(ModelTrieNode));
        compressedNodes[i]->serialIndex = i;
        compressedNodes[i]->var = cmap->nodeChildren[i][NODE_VAR_IND];
        
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
    auxDestroyModel(model->root);

    /* replace old model with compresed model */
    model->root = compressedNodes[numNodes-1];
    model->numNodes = numNodes;

    /* free serial array */
    free(compressedNodes);

}

int auxCompressModel(ModelTrieNode* subtrie, ModelCompressionMap* cmap){

    int pos, neg, ind;

    if(subtrie->var <= 0){
        return subtrie->var;
        
    } else {
        
        pos = auxCompressModel(subtrie->pos, cmap);
        neg = auxCompressModel(subtrie->neg, cmap);
        
        if(pos == neg){
            return pos;
        } else {
            ind = cmap->nodeIndexMap[pos][neg];
            
            /* if index has not been put in compression map, add it */
            if(ind == 0){
                ind = cmap->numCompressedNodes;
                printf("adding: %d: %d, %d\n", ind, pos, neg);
                cmap->nodeIndexMap[pos][neg] = ind;
                cmap->nodeChildren[ind][CHILD_POS_IND] = pos;
                cmap->nodeChildren[ind][CHILD_NEG_IND] = neg;
                cmap->nodeChildren[ind][NODE_VAR_IND] = subtrie->var;
                ++(cmap->numCompressedNodes);
            }

            return ind;
        }
    }
}


ModelTrieNode* auxUnionModels(ModelTrieNode* a, ModelTrieNode* b, ModelTrieNode*** memTable);
void unionModels(Model* dest, Model* src){
     
}

ModelTrieNode* auxUnionModels(ModelTrieNode* a, ModelTrieNode* b, ModelTrieNode*** memTable){

}


void intersectModels(Model* dest, Model* src){
    
}

void auxPrintModel(ModelTrieNode* trie, int* history, int maxNumVariables, FILE* out);
void printModel(Model* model, FILE* out){

    int* history;
    int hInd;
    
    history = malloc((model->compressionMap->maxNumVariables+2)*sizeof(int));

    auxPrintModel(model->root, history, 0, out);

    free(history);
}

void auxPrintModel(ModelTrieNode* trie, int* history, int hInd, FILE* out){
    int i;
    
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