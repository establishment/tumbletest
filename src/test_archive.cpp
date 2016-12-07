#include "test_archive.hpp" // header include

#include "os.hpp"
#include "utils/logging.hpp"
#include "utils/string.hpp"

#include <algorithm>
#include <functional>
#include <vector>
#include <string>

namespace tumbletest {

int DummyHash(const std::string& txt) {
    static const int kMod = 1e9+7, kSigma = 131;
    long long result = 0;
    for (int i = 0; i < (int)txt.size(); i += 1) {
        result *= kSigma;
        result += txt[i];
        result %= kMod;
    }

    return result;
}

// ~~~~~~~~~~~~~~~~~~~~~~
// TestCase
// ~~~~~~~~~~~~~~~~~~~~~~

/// external API
TestCase& TestCase::Seed(const unsigned& seed) {
    this->official_seed = seed;
    is_computed = false;
    official_input = "";
    return *this;
}

TestCase& TestCase::Type(const TestType& type) {
    this->type = type;
    return *this;
}

// backwards compatibility
TestCase& TestCase::SetSeed(const unsigned& seed) {
    return this->Seed(seed);
}

TestCase& TestCase::SetType(const TestType& type) {
    return this->Type(type);
}

// global external API
void TestCase::AddSeedFunction(std::function<void(const unsigned&)> function) {
   seed_functions.push_back(function); 
}

// internal API
const std::string& TestCase::Input() {
    if (not is_computed) {
        for (auto& itr : seed_functions) {
            itr(this->official_seed);
        }

        is_computed = true;
        official_input = function();
    }

    return official_input;
}

std::string TestCase::Input(const unsigned& seed) {
    this->last_gen_seed = seed;
    for (auto& itr : TestCase::seed_functions) {
        itr(seed);
    }

    return function();
}

const TestType& TestCase::Type() const {
    return this->type;
}

// debug API
std::string TestCase::Details(bool show_seed=true) {
    return StrCat(
            "Type:", "\t", this->type, "\n",
            "test-num(0 based)", "\t", this->initial_test_number, "\n",
            "Command:", "\t", this->function_call_string, "\n",
            (show_seed) ? ("Seed:\t" + std::to_string(this->last_gen_seed)) : ("")
        );
}

// constructor
TestCase::TestCase(const std::function<std::string()>& function, const std::string& function_call_string)
        : type(TestType::FINAL_TEST), 
          function(function), 
          official_seed(0),
          initial_test_number(added_testcases++), 
          function_call_string(function_call_string),
          last_gen_seed(0),
          is_computed(false),
          official_input("") {
    int call_string_hash = DummyHash(function_call_string);
    this->official_seed = call_string_hash + (hash_counter[call_string_hash]++);
};

std::vector<std::function<void(unsigned)> > TestCase::seed_functions = {srand};
int TestCase::added_testcases = 0;
std::map<int, int> TestCase::hash_counter = {};


bool TestCase::operator<(const TestCase& rhs) const {
    return type < rhs.type;
}

TestCase& TestArchive::AddTest(const TestCase& testcase) {
    testcases.push_back(testcase);  
    return testcases.back();
}

std::vector<TestCase>& TestArchive::AllTestcases() {
    stable_sort(testcases.begin(), testcases.end());
    return testcases;
}

}  // namespace tumbletest
