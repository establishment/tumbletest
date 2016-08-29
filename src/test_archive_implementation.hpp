#pragma once

#include "test_archive.hpp"

#include "os.hpp"
#include "logging.hpp"
#include "string_utils.hpp"

#include <functional>
#include <vector>
#include <string>

namespace tumbletest {

std::vector<std::function<void(unsigned)> > TestCase::seed_functions = {srand};
int TestCase::added_testcases = 0;

TestCase::TestCase(std::function<std::string()> function, const std::string& function_call_string)
        : type(TestType::FINAL_TEST), function(function), seed(0),
          initial_test_number(added_testcases++), function_call_string(function_call_string),
          is_computed(false), input("") {
    this->seed = rand();
};

TestCase& TestCase::Seed(const unsigned& seed) {
    this->seed = seed;
    is_computed = false;
    input = "";
    return * this;
}

TestCase& TestCase::Type(const TestType& type) {
    this->type = type;
    return * this;
}


TestCase& TestCase::SetSeed(const unsigned& seed) {
    return this->Seed(seed);
}


TestCase& TestCase::SetType(const TestType& type) {
    return this->Type(type);
}

bool TestCase::operator<(const TestCase& rhs) const {
    return type < rhs.type;
}

const std::string& TestCase::Input() {
    if (not is_computed) {
        for (auto& itr : seed_functions) {
            itr(this->seed);
        }

        is_computed = true;
        input = function();
    }

    return input;
}

std::string TestCase::Input(const unsigned& seed) {
    for (auto& itr : TestCase::seed_functions) {
        itr(seed);
    }

    return function();
}


std::string TestCase::Details(bool show_seed) {
    return StrCat(
            "Type:", "\t", this->type, "\n"
                    "#", "\t", this->initial_test_number, "\n",
            "Command:", "\t", this->function_call_string, "\n",
            (show_seed) ? "Seed:\t" + std::to_string(this->seed) : "");
}

std::string TestCase::DetailsWithoutSeed() {
    return this->Details(false);
}

std::string TestCase::DetailsWithSeed() {
    return this->Details(true);
}

TestArchive& test_archive = TestArchive::GetSingleton();

TestArchive::TestArchive()
        : official_source("official.cpp"),
          tests_location("tests/"),
          test_prefix("test-"),
          deploy_eggecutor(EggecutorProfile::Testing(0.0)),
          debug_eggecutor(EggecutorProfile::Debug()),
          checker(default_checker) { }

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
/*
    stable_sort(testcases.begin(), testcases.end());
    int test_number = 0;
    os.RunBashCommand("mkdir -p " + tests_location);

    std::vector<Path> files_location;

    for (auto itr : testcases) {
        if (itr.Type() == DEBUG_TEST) {
            continue;
        }

        EggResult test_result = deploy_eggecutor.Run(official_source, itr.Input());

        os.WriteFile(tests_location + test_prefix + std::to_string(test_number) + ".in", test_result.stdin);
        os.WriteFile(tests_location + test_prefix + std::to_string(test_number) + ".ok", test_result.stdout);

        test_number += 1;
    }
*/
//  archive feature not available at the moment
//    if (archive_tests) {
//        for (auto test_type : {EXAMPLE, PRETEST, FINAL_TEST}) {
//            files_location.push_back(Path(test_prefix + std::string(test_number) + ".in"));
//            files_location.push_back(Path(test_prefix + std::string(test_number) + ".ok"));
//        }
//    }
}

void TestArchive::TestSources(int num_runs, std::vector<Path> other_sources) {
    stable_sort(testcases.begin(), testcases.end());
    srand(time(NULL));
    int seed = rand();
    while (num_runs--) {
        for (auto testcase : testcases) {
            seed -= 1;
            auto input = testcase.Input(seed);
            EggResult official_result = debug_eggecutor.Run(official_source, input);

            for (auto itr : other_sources) {
                EggResult other_result = debug_eggecutor.Run(itr, testcase.Input(seed));
                if (not checker.Accepted(official_result.stdin, official_result.stdout, other_result.stdout)) {

                    std::string error_message = StrCat(
                            "TestSources evaluation failed\n"
                                    "Found a problem with source\n",
                            ">", other_result.source, "\n"
                                    "<", official_result.source, "\n"
                                    "Test information -------\n",
                            testcase.DetailsWithoutSeed(), "\n",
                            "Input Ok and Output have been written to /tumbletest/{in,ok,out}.txt");

                    os.WriteFile(Path::default_path + "/tumbletest/in.txt", official_result.stdin);
                    os.WriteFile(Path::default_path + "/tumbletest/ok.txt", official_result.stdout);
                    os.WriteFile(Path::default_path + "/tumbletest/out.txt", other_result.stdout);

                    Error(error_message);
                }
            }
        }
    }
}

}  // namespace tumbletest
