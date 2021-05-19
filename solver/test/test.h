/*
    This testing suite template is from the C-unit-tests
    repository at github:

    https://github.com/cburdine/C-unit-tests
*/


#ifndef C_UNIT_TEST_H
#define C_UNIT_TEST_H


#include <stdio.h>
#include "manifest.h"
#include "test_types.h"
#include "test_assert.h"

int runTestGroup(TestGroup* tGroup, FILE* testLog){
    int tind, result, numPassed;

    if(tGroup == NULL){
        fputs("Test Group undefined; tests cannot be run.", testLog);
        return FAIL;
    }

    numPassed = 0;
    result = PASS;

    fprintf(testLog, "Running testgroup \'%s\':\n", tGroup->groupName);

    for(tind = 0; 
        tGroup->testCases[tind].name != 0 || 
        tGroup->testCases[tind].testCase != 0;
        ++tind){

        result = tGroup->testCases[tind].testCase(testLog);

        fprintf(testLog, "[%s] %s::%s\n", 
            (result == PASS)? "PASS" : "FAIL",
             *(tGroup->groupName)? tGroup->groupName : "NULL", 
             *(tGroup->testCases[tind].name)? tGroup->testCases[tind].name : "UNNAMED"
        );

        if(result){ 
            ++numPassed; 
        } else {
            fprintf(testLog,"    %s\n", getAssertMessage());
        }
    }
    fprintf(testLog, "Testgroup \'%s\' finished; [%d/%d] tests passed.\n\n",
        tGroup->groupName, numPassed, tind);

    return (numPassed == tind)? PASS : FAIL;
}

int runManifestTestGroups(FILE* testLog){
    int tgInd, allPassed, result, numTestGroupsFailed;
    allPassed = PASS;
    numTestGroupsFailed = 0;

    for(tgInd = 0; TEST_GROUP_MANIFEST[tgInd] != END_OF_MANIFEST; ++tgInd){
        result = runTestGroup(TEST_GROUP_MANIFEST[tgInd], testLog);
        allPassed &= result;
        numTestGroupsFailed += (result? 0 : 1);
    }

    if(allPassed){
        fputs("----- ALL TESTGROUPS PASSED -----\n",testLog);
    } else {
        fprintf(testLog,"----- %d TESTGROUP%s FAILED -----\n", 
            numTestGroupsFailed, (numTestGroupsFailed==1)? "" : "S"
        );
    }

    return allPassed;
}

int runSpecifiedManifestTestGroups(int groupNameCount, char** groupNames, FILE* testLog){
    int tgInd, gnInd, allPassed, result, numTestGroupsFailed, numTestGroupsNotFound;
    char foundMatch;

    allPassed = PASS;
    for(gnInd = 0; gnInd < groupNameCount; ++gnInd){
        foundMatch = 0;
        for(tgInd = 0; TEST_GROUP_MANIFEST[tgInd] != END_OF_MANIFEST; ++tgInd){
            if(TEST_GROUP_MANIFEST[tgInd]->groupName == groupNames[gnInd]){
                result = runTestGroup(TEST_GROUP_MANIFEST[tgInd], testLog);
                allPassed &= result;
                if(!result){ ++numTestGroupsFailed; }

                foundMatch = 1;
                break;
            }
        }
        
        if(!foundMatch){
            fprintf(testLog, 
                "ERROR- found no match for test group name '%s'\n\n", 
                groupNames[gnInd]
            );
            ++numTestGroupsNotFound;
        }
    }

    if(allPassed){
        fputs("----- ALL TESTGROUPS PASSED -----\n",testLog);
    } else {
        fprintf(testLog,"----- %d TESTGROUP%s FAILED -----\n", 
            numTestGroupsFailed, (numTestGroupsFailed==1)? "" : "S"
        );
    }

    if(numTestGroupsNotFound > 0){
        fprintf(testLog,"---- %d TESTGROUP%s NOT FOUND ----\n", 
            numTestGroupsNotFound, (numTestGroupsNotFound==1)? "" : "S"
        );
    }

    return allPassed;
}

#endif /* C_UNIT_TEST_H */
