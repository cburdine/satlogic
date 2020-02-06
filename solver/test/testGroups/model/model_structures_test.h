#include <stdio.h>
#include "../../test.h"
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

    int orderedClause[10] = {1, -2, 3, -4, 5, -6, 7, -8, 9, 10};

    Model model;
    ModelCompressionMap cmap;

    initModelCompressionMap(&cmap, 10);
    initModelFromClause(&model, orderedClause, 10, &cmap);

    printModel(&model, testLog);

    compressModel(&model);

    printModel(&model, testLog);

    destroyModel(&model);
    destroyModelCompressionMap(&cmap);

    return PASS;
}