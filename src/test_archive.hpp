#pragma once

#include <functional>
#include <string>
#include <vector>
#include <map>

namespace tumbletest {

enum TestType {
    DEBUG_TEST = -1,
    EXAMPLE = 0,
    PRETEST = 1,
    FINAL_TEST = 2,
};

class TestCase {
    // external API
  public:
    TestCase& Seed(const unsigned& seed);

    TestCase& Type(const TestType& type);

    // backwards compatibility
    TestCase& SetSeed(const unsigned& seed);

    TestCase& SetType(const TestType& type);

    // global external API
  public:
    static void AddSeedFunction(std::function<void(const unsigned&)> function);

    // internal API
  public:
    const std::string& Input();
    std::string Input(const unsigned& seed);
    const TestType& Type() const;

    // debug API
  public:
    std::string Details(bool show_seed);

    // construrs and such
  public:
    TestCase(const std::function<std::string()>& function, const std::string& function_call_string);

  protected:
    // user added seed functions
    static std::vector<std::function<void(unsigned)> > seed_functions;

    // used for counting how many testcases were added
    static int added_testcases;

    static std::map<int, int> hash_counter;

  public:
    // sort by type, and in case of equality, based on the time it was inserted
    bool operator<(const TestCase& rhs) const;

  public:
    TestType type;
    std::function<std::string()> function;
    unsigned official_seed;

    // debug purpose
    int initial_test_number;
    std::string function_call_string;

    unsigned last_gen_seed;

    // caching purpose
    bool is_computed;
    std::string official_input;
};

/*
 * Bulk of testcases.
 * Generates ok files and checks if 2 or more sources are correct
 */
class TestArchive {
  public:
    TestCase& AddTest(const TestCase& testcase);
    std::vector<TestCase>& AllTestcases();

  private:
    std::vector<TestCase> testcases;
};

extern TestArchive& test_archive; 

}  // namespace tumbletest
