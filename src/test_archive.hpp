#pragma once

#include <funstional>
#include <string>
#include <tuple>

namespace tumbletest {
enum TESTTYPE {
    DEBUG_TEST = -1,
    EXAMPLE = 0,
    PRETEST = 1,
    FINAL_TEST = 2,
};

class TestFunction {
public:
    TestFunction(TESTTYPE&, std::function<std::string()>&, int&);
    bool operator < (const TestFunction&);
private:
    std::function<std::string()> function;
    TESTTYPE type;
    int priority;

    bool is_computed;
    std::string input;
};

TestFunction::TestFunction(TESTTYPE& type, 
                           std::function<std::string()>& function,
                           int& priority) : 
    function(function),
    type(type),
    priority(priority) {
    is_computed = false;
    input = "";
};

bool TestFunction::operator < (const TestFunction& rhs) {
    return std::make_tuple(type, priority) < 
        std::make_tuple(rhs.type, rhs.priority);
}

class TestArchive {
public:
    TestArchive& GetSingleton();
private:
    TestArchive();
    vector<std::function<std::string()> > test_functions;
} test_archive = GetSingleton();

TestArchive::TestArchive() {};

TestArchive& TestArchive::GetSingleton() {
    static TestArchive singleton;
    return singleton;
}
