/* This manifest serves as the registry for 
   all unit tests in a project. To run all registered
   unit test groups simply make a call to: 
   
   runManifestTestGroups()
*/

#ifndef C_UNIT_TEST_MANIFEST_H
#define C_UNIT_TEST_MANIFEST_H

#include "testGroups/structures_test.h"

static TestGroup* TEST_GROUP_MANIFEST[] = {
    
    /* list reference to all TestGroups here */
    &STRUCTURES_TESTS,

    END_OF_MANIFEST
};


#endif /* C_UNIT_TEST_MANIFEST_H */