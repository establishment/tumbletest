#pragma once

#include "test_archive.hpp" // header include

#include "logging.hpp"
#include "os.hpp"
#include "string_utils.hpp"

#include <algorithm>
#include <functional>
#include <vector>
#include <string>

namespace tumbletest {

bool is_interactive = false;

std::vector<std::function<void(unsigned)>> TestCase::seed_functions = {srand};
int TestCase::added_testcases = 0;
std::map<int, int> TestCase::hash_counter = {};

int DummyHash(const std::string& txt) {
    static const int kMod = 1e9 + 7, kSigma = 131;
    long long result = 0;
    for (int i = 0; i < (int)txt.size(); i += 1) {
        result *= kSigma;
        result += txt[i];
        result %= kMod;
    }

    return result;
}

TestCase::TestCase(std::function<std::string()> function, const std::string& function_call_string)
      : type(TestType::FINAL_TEST),
        function(function),
        seed(0),
        initial_test_number(added_testcases++),
        function_call_string(function_call_string) {
    int call_string_hash = DummyHash(function_call_string);
    this->seed = call_string_hash + (hash_counter[call_string_hash]++);
};

TestCase& TestCase::Seed(const unsigned& seed) {
    this->seed = seed;
    is_computed = false;
    input = "";
    return *this;
}

TestCase& TestCase::Type(const TestType& type) {
    this->type = type;
    return *this;
}

TestCase& TestCase::SetSeed(const unsigned& seed) {
    return this->Seed(seed);
}

TestCase& TestCase::SetType(const TestType& type) {
    return this->Type(type);
}

TestCase& TestCase::Example() {
    return this->Type(EXAMPLE);
}

bool TestCase::operator<(const TestCase& rhs) const {
    return type < rhs.type;
}

std::string TestCase::Input(const unsigned& seed) const {
    for (auto& itr : TestCase::seed_functions) {
        itr(seed);
    }

    return function();
}

std::string TestCase::Input() const {
    return Input(seed);
}

std::string TestCase::Details(bool show_seed = true) {
    return StrCat("Number:", "\t", this->initial_test_number, "\n",
                  (show_seed) ? StrCat("Seed:\t", this->seed, "\n") : "", "Command:", "\t", this->function_call_string);
}

std::string TestCase::DetailsWithoutSeed() {
    return this->Details(false);
}

std::string TestCase::DetailsWithSeed() {
    return this->Details(true);
}

const TestType& TestCase::Type() const {
    return this->type;
}

TestArchive& test_archive = TestArchive::GetSingleton();

TestArchive::TestArchive()
      : official_source("official.cpp"),
        tests_location("tests/"),
        test_prefix("test-"),
        deploy_eggecutor(EggecutorProfile::Testing(0.0)),
        debug_eggecutor(EggecutorProfile::Debug()),
        checker("checker.cpp") {
}

TestCase& TestArchive::AddTest(TestCase testcase) {
    testcases.push_back(testcase);
    // bad practice
    return testcases.back();
}

void TestArchive::TestsLocation(const std::string& tests_location) {
    this->tests_location = Path(tests_location);
}

void TestArchive::ArchiveOption(bool option) {
    this->archive_tests = option;
}

void TestArchive::OfficialSource(const std::string& source) {
    this->official_source = Path(source);
}

void TestArchive::Run() {
    Compile(official_source);
    if (is_interactive) {
        Compile(checker);
    }
    stable_sort(testcases.begin(), testcases.end());
    int test_number = 0;
    os.RunBashCommand("mkdir -p " + tests_location);

    for (auto itr : testcases) {
        if (itr.Type() == DEBUG_TEST) {
            continue;
        }
        std::cerr << GetBloatware() << "\t" << Colored(4, 2, 3, "Generating") << " input for test "
                  << Colored(5, 1, 1, Allign(StrCat("#", test_number), 3)) << '\n';

        std::string input = itr.Input();
        // write input before generating so in case something goes wrong
        // the user has the input that triggered that bug / crash
        os.WriteFile(tests_location + test_prefix + std::to_string(test_number) + ".in", input);
//        Info("Computed .in file and wrote it to\t", test_prefix + std::to_string(test_number));

        if (not is_interactive) {
            EggResult test_result = deploy_eggecutor.Run(official_source, input);
            std::cerr << '\n';
            os.WriteFile(tests_location + test_prefix + std::to_string(test_number) + ".ok", test_result.stdout);
        } else {
            auto all_results = deploy_eggecutor.RunInteractive(official_source, checker, input);
            if (itr.Type() == EXAMPLE) {
                os.WriteFile(tests_location + test_prefix + std::to_string(test_number) + ".json",
                             all_results.second.logger);
            }
            std::cerr << "Checker: " << (all_results.second.passed ? Colored(Color::green, "Passed!") : Colored(Color::red, "Not passed")) << 
                 "\t" << Colored(Color::light_magenta, all_results.second.message) << '\n';
        }

        test_number += 1;
    }
}

void TestArchive::TestSources(int num_runs, std::vector<Path> other_sources) {
    if (is_interactive) {
        other_sources.push_back(official_source);
    }
    EnsureChecker(checker);
    Compile(checker);
    Compile(official_source);
    for (auto itr : other_sources) {
        Compile(itr);
    }
    stable_sort(testcases.begin(), testcases.end());
    srand(time(NULL));
    rand();
    srand(rand());
    rand();
    srand(rand());
    rand();
    srand(rand());
    int seed = rand();
    for (int run_number = 0; run_number <= num_runs; run_number += 1) {
        int test_number = 0;
        for (auto testcase : testcases) {
            std::cerr << GetBloatware() << "\t" << Colored(4, 2, 3, "Generating") << " input for test "
                      << Colored(5, 1, 2, Allign(StrCat("#", test_number), 3)) << '\t' << "run\t"
                      << Colored(5, 1, 1, Allign(StrCat("#", run_number), 3)) << '\n';
            seed -= 1;
            std::string input;
            if (run_number == 0) {
                input = testcase.Input();
            } else {
                input = testcase.Input(seed);
            }

            os.WriteFile(Path::default_path + "/tumbletest/in.txt", input);

            if (not is_interactive) {
                EggResult official_result = debug_eggecutor.Run(official_source, input); std::cerr << '\n';

                for (auto itr : other_sources) {
                    EggResult other_result = debug_eggecutor.Run(itr, input); 

                    auto checker_results = deploy_eggecutor.RunChecker(checker, official_result.stdin,
                                                                       official_result.stdout, other_result.stdout);

                    std::cerr << "Checker: " << (checker_results.second.passed ? Colored(Color::green, "Passed!")
                                                                               : Colored(Color::red, "Not passed"))
                              << "\t" << Colored(Color::light_magenta, checker_results.second.message) << '\n';

                    if (not checker_results.second.passed) {
                        std::cerr << Colored(Color::red, "[Failed]") << '\n';
                        std::cerr << "Checker message:\"" << checker_results.second.message << '\n';
                        std::cerr << Colored(Color::light_magenta, "Test information\n");
                        std::cerr << testcase.Details() << '\n';
                        std::cerr << "Input Ok and Output have been written to tumbletest/{in,ok,out}.txt\n";

                        os.WriteFile(Path::default_path + "/tumbletest/in.txt", official_result.stdin);
                        os.WriteFile(Path::default_path + "/tumbletest/ok.txt", official_result.stdout);
                        os.WriteFile(Path::default_path + "/tumbletest/out.txt", other_result.stdout);
                        
                        exit(0);
                    }
                }
            } else {
                os.WriteFile(Path::default_path + "/tumbletest/in.txt", input);
                for (auto itr : other_sources) {
                    auto all_results = deploy_eggecutor.RunInteractive(itr, checker, input);

                    std::cerr << "Checker: " << (all_results.second.passed ? Colored(Color::green, "Passed!")
                                                                               : Colored(Color::red, "Not passed"))
                              << "\t" << Colored(Color::light_magenta, all_results.second.message) << '\n';

                    if (not all_results.second.passed) {
                        std::cerr << Colored(Color::red, "[Failed]") << '\n';
                        std::cerr << "Checker message:\"" << all_results.second.message << '\n';
                        std::cerr << Colored(Color::light_magenta, "Test information\n");
                        std::cerr << testcase.Details() << '\n';
                        std::cerr << "Input Ok and Output have been written to tumbletest/{in,ok,out}.txt\n";

                        exit(0);
                    }
                }
            }

            test_number += 1;
        }
    }
}

}  // namespace tumbletest
