#pragma once

#include "checkers.hpp"
#include "eggecutor.hpp"
#include "good_defines.hpp"
#include "os.hpp"

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace tumbletest {

enum TestType {
    DEBUG_TEST = -1,
    EXAMPLE = 0,
    PRETEST = 1,
    FINAL_TEST = 2,
};

class TestCase {
  public:
    static void AddSeedFunction(std::function<void(const unsigned&)> function);

  protected:
    // user added seed functions
    static std::vector<std::function<void(unsigned)>> seed_functions;

    // used for counting how many testcases were added
    static int added_testcases;

    static std::map<int, int> hash_counter;

  public:
    TestCase(std::function<std::string()> function, const std::string& function_call_string);

    virtual ~TestCase() = default;

    // user interaction
    TestCase& Seed(const unsigned& seed);

    TestCase& Type(const TestType& type);

    // backwards compatibility
    TestCase& SetSeed(const unsigned& seed);

    TestCase& SetType(const TestType& type);

    TestCase& Example();
    
    TestCase& Output(const std::string& output);

    // sort by type, and in case of equality, based on the time it was inserted
    bool operator<(const TestCase& rhs) const;

    // use
    std::string Details(unsigned seed);

    std::string Input() const;

    std::string Input(const unsigned& seed) const;

    const TestType& Type() const;

    unsigned GetSeed() const;

  protected:
    TestType type;
    std::function<std::string()> function;
    unsigned seed;

    // debug purpose
    int initial_test_number;
    std::string function_call_string;

    // caching purpose
    bool is_computed;
    std::string input;

  public:
    bool has_output;
    std::string output;
};

/*
 * Bulk of testcases.
 * Generates ok files and checks if 2 or more sources are correct
 */
class TestArchive {
    PermanentSingleton(TestArchive)

        public : TestCase& AddTest(TestCase testcase);

    void Run();

    // for debug purpose
    void TestSources(int num_runs, std::vector<Path> other_sources);

    /*
     * Set stuff required for the generator
     */
    void TestsLocation(const std::string& path);

    void ArchiveOption(const bool option);

    void OfficialSource(const std::string& source);

  private:
    std::vector<TestCase> testcases;

    Path official_source;
    Path tests_location;
    std::string test_prefix;  // test-$i or sth

    bool archive_tests;  // create a archive with all testcases after generating .ok files or not

    Eggecutor deploy_eggecutor;  // mode used for Run
    Eggecutor debug_eggecutor;   // mode used for TestSources

    Path checker;  // checker used for the problem
};

}  // namespace tumbletest

#include "test_archive_implementation.hpp"
