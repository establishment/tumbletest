#pragma once

#include "good_defines.hpp"
#include "eggecutor.hpp"
#include "os.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace tumbletest {

bool DefaultChecker(const std::string &in, const std::string &ok, const std::string &out) {
    std::stringstream a;
    std::stringstream b;
    a << ok;
    b << out;
    std::string A, B;
    while (a >> A) {
        if (b >> B) {
        } else {
            return false;
        }

        if (A != B) {
            return 0;
        }
    }
    if (b >> B) {
        return false;
    }

    return true;
}

enum TestType {
    DEBUG_TEST = -1,
    EXAMPLE = 0,
    PRETEST = 1,
    FINAL_TEST = 2,
};

class TestCase {
  public:
    TestCase(std::function<std::string()> function, const std::string& function_call_string);

    virtual ~TestCase() = default;
    
//    MakeMoveOnly(TestCase);

    // sort by type, and in case of equality, based on the time it was inserted
    bool operator<(const TestCase &rhs) const;

    // user interaction
    TestCase& SetSeed(const unsigned& seed);
    TestCase& SetPriority(const int& priority);
    TestCase& SetType(const TestType& type);

    static void AddSeedFunction(std::function<void(const unsigned&)> function);

    // use
    void FailDebug();
    const std::string& GetInput();
    const std::string& GetInput(unsigned seed);

    const int& GetPriority();
    const TestType& GetType();
    
    int seed;
    TestType type;

  protected:
    // user added seed functions
    static std::vector<std::function<void(unsigned)> > seed_functions;

    // used for counting how many testcases are for every type
    static int added_testcases;

    std::function<std::string()> function;

    int priority;

    // debug purpose
    int function_added_num;
    std::string function_call_string;

    // caching purpose
    bool is_computed;
    std::string input;
};

std::vector<std::function<void(unsigned)> > TestCase::seed_functions = {srand};
int TestCase::added_testcases = 0;

TestCase::TestCase(std::function<std::string()> function, const std::string& function_call_string)
        : type(FINAL_TEST), function(function), priority(TestCase::added_testcases++), function_call_string(function_call_string) {
    is_computed = false;
    function_added_num = ++added_testcases;
    this->seed = rand();
};

TestCase& TestCase::SetSeed(const unsigned& seed) {
    this->seed = seed;
    is_computed = false;
    input = "";
    return *this;
}

TestCase& TestCase::SetType(const TestType& type) {
    this->type = type;
    return *this;
}

bool TestCase::operator<(const TestCase &rhs) const {
    return std::make_tuple(type, priority) <
           std::make_tuple(rhs.type, rhs.priority);
}

const std::string& TestCase::GetInput() {
    if (not is_computed) {
        for (auto &itr : seed_functions) {
            itr(this->seed);
        }

        is_computed = true;
        input = function();
    }

    return input;
}

void TestCase::FailDebug() {
    cerr << "~~~~ TestCase\n";
    cerr << "Type\t" << this->type << "\t Was the #" << this->function_added_num << "\ttestcase added of it's type\n";
    cerr << "Spawned from the function call\t" << function_call_string << "\n";
    cerr << "Run with the seed\t" << seed << "\n";
    cerr << "input:\n" << input << '\n';
    cerr << "\n\n";
}

class TestArchive {
  public:
    static TestArchive &GetSingleton();
    TestCase& AddTest(TestCase testcase);
    void Run();
    void SetTestsLocation(const std::string &path);
    void SetArchiveOption(const bool option);
    void SetOfficialSource(const std::string &source);

  private:
    TestArchive();

    std::vector <TestCase> testcases;
    Path tests_location;
    std::string test_prefix;

    Path official_source;

    bool archive_tests;

    Eggecutor deploy_eggecutor;
    Eggecutor debug_eggecutor;
    std::function<bool(const std::string&, const std::string&, const std::string&)> checker;

  public:
    // for debug purpose
    void TestSources(int num_runs, std::vector<Path> other_sources);
} test_archive = TestArchive::GetSingleton();

TestArchive::TestArchive()
        : tests_location("tests/"),
          test_prefix("test-"),
          official_source("not_set"),
          deploy_eggecutor(EggecutorProfile::Testing(0.0)),
          debug_eggecutor(EggecutorProfile::Debug()),
          checker(DefaultChecker) { }

TestArchive &TestArchive::GetSingleton() {
    static TestArchive singleton;
    return singleton;
}

TestCase& TestArchive::AddTest(TestCase testcase) {
    testcases.push_back(testcase);
    // bad practice
    return testcases.back();
}

void TestArchive::SetTestsLocation(const std::string &tests_location) {
    this->tests_location = Path(tests_location);
}

void TestArchive::SetArchiveOption(bool option) {
    archive_tests = option;
}

void TestArchive::SetOfficialSource(const std::string &source) {
    this->official_source = Path(source);
}

void TestArchive::Run() {
//    cerr << "Hello\n";
//    cerr << this->tests_location << '\n';
    stable_sort(testcases.begin(), testcases.end());
    int test_number = 0;
    os.RunBashCommand("mkdir -p " + tests_location);

    std::vector<Path> files_location;

    for (auto itr : testcases) {
        if (itr.type == DEBUG_TEST) {
            continue;
        }

        EggResult test_result = deploy_eggecutor.Run(official_source, itr.GetInput());
//        cerr << "stdin: " << test_result.stdin << '\n';
//        cerr << "stdout: " << test_result.stdout << '\n';
//        cerr << "stderr: " << test_result.stderr << '\n';

        os.WriteFile(tests_location + test_prefix + std::to_string(test_number) + ".in", test_result.stdin);
        os.WriteFile(tests_location + test_prefix + std::to_string(test_number) + ".ok", test_result.stdout);

        test_number += 1;
    }

//  archive feature not available at the moment
//    if (archive_tests) {
//        for (auto test_type : {EXAMPLE, PRETEST, FINAL_TEST}) {
//            files_location.push_back(Path(test_prefix + std::string(test_number) + ".in"));
//            files_location.push_back(Path(test_prefix + std::string(test_number) + ".ok"));
//        }
//    }
}

void TestArchive::TestSources(int num_runs, std::vector<Path> other_sources) {
while (num_runs--) {
    for (auto testcase : testcases) {
        EggResult official_result = debug_eggecutor.Run(official_source, testcase.GetInput());
        for (auto itr : other_sources) {
            EggResult other_result = debug_eggecutor.Run(itr, testcase.GetInput());
            if (not checker(official_result.stdin, official_result.stdout, other_result.stdout)) {
                cerr << "Found a problem with source\n";
                cerr << other_result.source << '\n';
                cerr << '\n';
                cerr << "Run against " << official_result.source << '\n';
                cerr << "Generated by\n";
                testcase.FailDebug();
                cerr << "Expected|" << official_result.stdout << "|\n";
                cerr << "Got|" << other_result.stdout << "|\n";
                std::exit(0);
            }
        }
        testcase.SetSeed(rand());
    }
    srand(time(NULL));
}
}
}  // namespace tumbletest
