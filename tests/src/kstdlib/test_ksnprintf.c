#include "tests/asserts.h"

#include <cut.h>

CUT_DEFINE_TEST(test_ksnprintf)
{

}

CUT_DEFINE_MODULE(module_ksnprintf)
    CUT_CALL_TEST(test_ksnprintf);
CUT_END_MODULE
