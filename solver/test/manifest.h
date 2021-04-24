/* This manifest serves as the registry for 
   all unit tests in a project. To run all registered
   unit test groups simply make a call to: 
   
   runManifestTestGroups()
*/

#ifndef C_UNIT_TEST_MANIFEST_H
#define C_UNIT_TEST_MANIFEST_H

/* INCLUDED TESTGROUPS */
#include "testGroups/dpll/structures_test.h"
#include "testGroups/dpll/dpll_test.h"
#include "testGroups/model/model_structures_test.h"
#include "testGroups/klib/khash_int_2d_test.h"
#include "testGroups/stochastic/stochastic_structures_test.h"

static TestGroup* TEST_GROUP_MANIFEST[] = {
    
    /* list reference to all TestGroups here */
    &STRUCTURES_TESTS,
    &DPLL_TESTS,
    /* &MODEL_STRUCTURES_TESTS, */
    &KHASH_INT_2D_TESTS,
    &STOCHASTIC_STRUCTURES_TESTS,

    END_OF_MANIFEST
};


#endif /* C_UNIT_TEST_MANIFEST_H */