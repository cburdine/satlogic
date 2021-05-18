#include <stdio.h>
#include "../../test_types.h"
#include "../../test_assert.h"

#include "../../../klib/khash_int_2d.h"

int testKhashInt2d(FILE*);

static TestCase KHASH_INT_2D_TEST_CASE_ARR[] = {
    {"KhashInt2d operations", testKhashInt2d},
    END_OF_TEST_CASES
};

static TestGroup KHASH_INT_2D_TESTS = {
    "KhashInt2d Tests",
    KHASH_INT_2D_TEST_CASE_ARR
};


int testKhashInt2d(FILE* testLog){
    
    KhashInt2d h;
    int i, j, hashOk, hashExists, hashVal, actualVal;

    initKhashInt2d(&h);

    for(i = 1; i < 20; ++i){
        for(j = 0; j < i; ++j){
            hashOk = hashPut(&h, 7*i, -1*j, (i*j)-41);
            ASSERT(hashOk);
        }
    }

    for(i = 0; i < 20; ++i){
        for(int j = 0; j < i; ++j){
            hashExists = checkHashExists(&h, 7*i, -1*j);
            ASSERT(hashExists);

            actualVal = ((i*j)-41);
            hashVal = hashGet(&h, 7*i, -1*j);
            ASSERT_INT_EQUAL(hashVal, actualVal);
        }
    }

    destroyKhashInt2d(&h);

    return PASS;
}