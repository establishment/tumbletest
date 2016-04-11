#pragma once

#include <string>
#include "test_archive.hpp"

#define __AddTest(type, function, priority) tumbletest::test_archive.AddTest(\
        tumbletest::TestCase(\
            type,\
            [=]() -> std::string { return function; }, \
            priority, \
            #function))\

#define AddFinalTest(x) __AddTest(tumbletest::FINAL_TEST, x, 0)
#define AddDebugTest(x) __AddTest(tumbletest::DEBUG_TEST, x, 0)