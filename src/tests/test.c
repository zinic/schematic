#include <stdlib.h>
#include <stdio.h>

#include "test.h"

TestResult * Result_new(int retval, char *msg) {
    TestResult *result = (TestResult *) malloc(sizeof(TestResult));

    result->retval = retval;
    result->msg = String_wrap(msg);

    return result;
}

Test * Test_new(String *name, test_cb func) {
    Test *test = (Test *) malloc(sizeof(Test));
    
    test->name = name;
    test->func = func;
    
    return test;
}

TestSuite * TestSuite_new(String *name) {
    TestSuite *suite = (TestSuite *) malloc(sizeof(TestSuite));
    
    suite->name = name;
    suite->tests = SList_new();
    
    return suite;
}

void run_tests() {
    test_init;
    #include "specs/structures.c"

    tests_exec;
}
