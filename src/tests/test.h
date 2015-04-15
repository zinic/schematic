#ifndef test_h
#define test_h

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "types.h"

typedef struct TestResult TestResult;

struct TestResult {
    int retval;
    String *msg;
};


//
typedef TestResult * (*test_cb)();

TestResult * Result_new(int retval, char *msg);


//
typedef struct Test Test;
typedef struct TestSuite TestSuite;

struct TestSuite {
    String *name;
    List *tests;
};

struct Test {
    String *name;
    test_cb func;
};

TestSuite * TestSuite_new(String *name);
Test * Test_new(String *name, test_cb func);


#define OK Result_new(0, "OK")
#define FAIL(reason) return Result_new(1, reason)

#define assert_equal(a, b, msg)\
    if (a != b) {\
        FAIL(msg);\
    }\

#define test_init\
    List *__TESTS__ = List_new();\
    Node *__cursor__;\
    TestSuite *__suite__

#define tests_exec\
    __cursor__ = __TESTS__->head;\
    while (__cursor__ != EMPTY_NODE) {\
        TestSuite *suite_ref = (TestSuite *) Node_advance(&__cursor__);\
        printf("\nWhen %s\n", suite_ref->name->data);\
        Node *suite_cursor = suite_ref->tests->head;\
        while (suite_cursor != EMPTY_NODE) {\
            Test *test_ref = (Test *) Node_advance(&suite_cursor);\
            printf("\tSpec: %s", test_ref->name->data);\
            TestResult *result = test_ref->func();\
            if (result->retval != 0) {\
                printf("\t\t\t[FAILED]\n");\
                printf("\t\t--%s\n", result->msg->data);\
            } else {\
                printf("\t\t\t[PASSED]\n");\
            }\
        }\
    }
    
#define when(suite, ...)\
    __suite__ = TestSuite_new(AS_STRING(#suite));\
    void suite() __VA_ARGS__\
    suite();\
    List_append(__TESTS__, __suite__);

#define should(spec, ...)\
    TestResult * spec() {\
        __VA_ARGS__\
        return OK;\
    }\
    List_append(__suite__->tests, Test_new(AS_STRING(#spec), &spec));

void run_tests();

#ifdef __cplusplus
}
#endif
#endif
