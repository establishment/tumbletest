#pragma once

#include <string>

#define __AddTest(type, function, priority) tumbletest::test_archive.AddTest(\
        tumbletest::TestFunction(\
            type,\
            [=]() -> std::string { return function; }, \
            priority))

#define AddExample(x) __AddTest(tumbletest::EXAMPLE, x, 0)
