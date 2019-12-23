#ifndef C_UNIT_TEST_ASSERT_H
#define C_UNIT_TEST_ASSERT_H

#include <stdio.h>
#include "test_types.h"

#define __ASSERT_MSG_BUFFER_SIZE 2048
static char __ASSERT_MSG_BUFFER[__ASSERT_MSG_BUFFER_SIZE];
static char __ASSERT_MSG_SET = 0;
const char* __DEFAULT_ASSERT_MSG = "Test returned FAIL";

void failAssert(char* expr);
void failIntAssert(long long int a, long long int b, char* exprA, char* exprB, char* op);
void failUIntAssert(unsigned long long int a, unsigned long long int b, char* exprA, char* exprB, char* op);
void failFloatAssert(long double a, long double b, char* exprA, char* exprB, char* op);
void failStrAssert(char* a, char* b, char* exprA, char* exprB, char* op);

#define ASSERT(EXPR) if(!(EXPR)){ failAssert(#EXPR); return FAIL; }

#define ASSERT_INT_EQUAL(A, B) if(A != B){ failIntAssert((long long int) A, (long long int ) B, #A, #B, "=="); return FAIL; }
#define ASSERT_INT_LESS_THAN(A, B) if(A >= B){ failIntAssert((long long int) A, (long long int ) B, #A, #B, "<"); return FAIL; }
#define ASSERT_INT_LESS_EQUAL_TO(A, B) if(A > B){ failIntAssert((long long int) A, (long long int) B, #A, #B, "<="); return FAIL; }
#define ASSERT_INT_GREATER_THAN(A, B) if(A <= B){ failIntAssert((long long int) A, (long long int ) B, #A, #B, ">"); return FAIL; }
#define ASSERT_INT_GREATER_EQUAL_TO(A, B) if(A < B){ failIntAssert((long long int) A, (long long int) B, #A, #B, ">="); return FAIL; }

#define ASSERT_UINT_EQUAL(A, B) if(A != B){ failUIntAssert((long long int) A, (long long int ) B, #A, #B, "=="); return FAIL; }
#define ASSERT_UINT_LESS_THAN(A, B) if(A >= B){ failUIntAssert((long long int) A, (long long int ) B, #A, #B, "<"); return FAIL; }
#define ASSERT_UINT_LESS_EQUAL_TO(A, B) if(A > B){ failUIntAssert((long long int) A, (long long int) B, #A, #B, "<="); return FAIL; }
#define ASSERT_UINT_GREATER_THAN(A, B) if(A <= B){ failUIntAssert((long long int) A, (long long int ) B, #A, #B, ">"); return FAIL; }
#define ASSERT_UINT_GREATER_EQUAL_TO(A, B) if(A < B){ failUIntAssert((long long int) A, (long long int) B, #A, #B, ">="); return FAIL; }

#define ASSERT_FLOAT_EQUAL(A, B) if(A != B){ failFloatAssert((long double) A, (long double) B, #A, #B, "=="); return FAIL; }
#define ASSERT_FLOAT_LESS_THAN(A, B) if(A >= B){ failFloatAssert((long double) A, (long double) B, #A, #B, "<"); return FAIL; }
#define ASSERT_FLOAT_LESS_EQUAL_TO(A, B) if(A > B){ failFloatAssert((long double) A, (long double) B, #A, #B, "<="); return FAIL; }
#define ASSERT_FLOAT_GREATER_THAN(A, B) if(A <= B){ failFloatAssert((long double) A, (long double) B, #A, #B, ">"); return FAIL; }
#define ASSERT_FLOAT_GREATER_EQUAL_TO(A, B) if(A < B){ failFloatAssert((long double) A, (long double) B, #A, #B, ">="); return FAIL; }

#define ASSERT_STR_EQUAL(A, B) if(strcmp(A,B) != 0){ failStrAssert( A,  B, #A, #B, "=="); return FAIL; }
#define ASSERT_STR_LESS_THAN(A, B) if(strcmp(A,B) >= 0){ failStrAssert( A,  B, #A, #B, "<"); return FAIL; }
#define ASSERT_STR_LESS_EQUAL_TO(A, B) if(strcmp(A,B) > 0){ failStrAssert( A,  B, #A, #B, "<="); return FAIL; }
#define ASSERT_STR_GREATER_THAN(A, B) if(strcmp(A,B) <= 0){ failStrAssert( A,  B, #A, #B, ">"); return FAIL; }
#define ASSERT_STR_GREATER_EQUAL_TO(A, B) if(strcmp(A,B) < 0){ failStrAssert( A,  B, #A, #B, ">="); return FAIL; }

/* assertion message functions */

const char* getAssertMessage(){
    if(!__ASSERT_MSG_SET){
        return __DEFAULT_ASSERT_MSG;
    }
    __ASSERT_MSG_SET = 0;
    return __ASSERT_MSG_BUFFER;
}

void failAssert(char* expr){
    snprintf(__ASSERT_MSG_BUFFER, __ASSERT_MSG_BUFFER_SIZE,
    "Assertion failed: '%s'", expr);
    __ASSERT_MSG_SET = 1;
}

void failIntAssert(long long int a, long long int b, char* exprA, char* exprB, char* op){
    snprintf(__ASSERT_MSG_BUFFER, __ASSERT_MSG_BUFFER_SIZE,
    "Assertion failed: '%s' (%lli) %s '%s' (%lli)", exprA, a, op, exprB, b);
    __ASSERT_MSG_SET = 1;
}

void failUIntAssert(unsigned long long int a, unsigned long long int b, char* exprA, char* exprB, char* op){
    snprintf(__ASSERT_MSG_BUFFER, __ASSERT_MSG_BUFFER_SIZE,
    "Assertion failed: '%s' (%llu) %s '%s' (%llu)", exprA, a, op, exprB, b);
    __ASSERT_MSG_SET = 1;
}

void failFloatAssert(long double a, long double b, char* exprA, char* exprB, char* op){
    snprintf(__ASSERT_MSG_BUFFER, __ASSERT_MSG_BUFFER_SIZE,
    "Assertion failed: '%s' (%Lg) %s '%s' (%Lg)", exprA, a, op, exprB, b);
    __ASSERT_MSG_SET = 1;
}

void failStrAssert(char* a, char* b, char* exprA, char* exprB, char* op){
    snprintf(__ASSERT_MSG_BUFFER, __ASSERT_MSG_BUFFER_SIZE,
    "Assertion failed: '%s' (%s) %s '%s' (%s)", exprA, a, op, exprB, b);
    __ASSERT_MSG_SET = 1;
}

#endif /* C_UNIT_TEST_ASSERT_H */