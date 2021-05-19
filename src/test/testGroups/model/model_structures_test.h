#include <stdio.h>
#include "../../test_types.h"
#include "../../test_assert.h"

#include "../../../model/model_structures.h"


int testPrintModel(FILE* testLog);

static TestCase MODEL_STRUCTURES_TEST_CASE_ARR[] = {
    {"Test Print Model", testPrintModel},
    END_OF_TEST_CASES
};

static TestGroup MODEL_STRUCTURES_TESTS = {
    "Model Structures Tests",
    MODEL_STRUCTURES_TEST_CASE_ARR
};


int testPrintModel(FILE* testLog){

    int orderedClause1[10] = {1, -2, 3, -4, 5, -6, 7, -8, 9, 10};
    int orderedClause2[8] = {-2, -4, -6, -8, 10, 11, 12, 13};

    Model model1, model2;
    ModelCompressionMap cmap;

    initModelCompressionMap(&cmap, 13, 2);
    initModelFromClause(&model1, orderedClause1, 10, &cmap);
    initModelFromClause(&model2, orderedClause2, 8, &cmap);

    compressModel(&model1, &cmap);


    compressModel(&model2, &cmap);

    intersectModels(&model1, &model2, &cmap);

    destroyModel(&model1);
    destroyModel(&model2);

    destroyModelCompressionMap(&cmap);

    return PASS;
}