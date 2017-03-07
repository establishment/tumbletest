#pragma once

#include "test_archive.hpp"

#include <functional>
#include <string>

#define addTest(...) \
    tumbletest::test_archive.AddTest(tumbletest::TestCase([=]() -> std::string { return __VA_ARGS__; }, #__VA_ARGS__))
#define addCTest(...) \
    tumbletest::test_archive.AddTest(tumbletest::TestCase([=]() -> std::string { __VA_ARGS__; }, #__VA_ARGS__))
