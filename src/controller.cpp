#include "controller.hpp"
#include "utils/string.hpp"
#include "utils/logging.hpp"

namespace tumbletest {

TestCase& Controller::AddTest(const TestCase& testcase) {
    return archive.AddTest(testcase); 
}

void Controller::TestsLocation(const std::string& tests_location) {
    this->config.tests_location = Path(tests_location);
}

void Controller::ArchiveOption(bool option) {
    this->config.archive_tests = option;
}

void Controller::OfficialSource(const std::string& source) {
    this->config.official_source = Path(source);
}

void Controller::Checker(const std::string& checker) {
    this->config.checker = Path(checker);
}

void Controller::Run() {
    int test_number = 0;
    os.RunBashCommand(StrCat("mkdir -p ", config.tests_location));

    std::vector<Path> files_location;

    Info("[Start] generating testcases");

    for (auto itr : archive.AllTestcases()) {
        Info("[Start] running on test #\t", test_number, "\twith source:\t", config.official_source.File());

        // write input before generating so in case something goes wrong
        // the user has the input that triggered that bug / crash
        os.WriteFile(StrCat(config.tests_location, config.test_prefix, test_number, ".in"), itr.Input());  
        Info("Computed .in file and wrote it to\t", config.test_prefix, test_number, ".in");
        
        EggResult test_result = deploy_eggec utor.Run(official_source, itr.Input());

        os.WriteFile(tests_location + test_prefix + std::to_string(test_number) + ".ok", test_result.stdout);

        test_number += 1;
    }
}

void Controller::TestSources(int num_runs, std::vector<Path> other_sources) {
/*    
    stable_sort(testcases.begin(), testcases.end());
    srand(time(NULL));
    int seed = rand();
    for (int run_number = 1; run_number <= num_runs; run_number += 1) {
        seed -= 1;
        int test_number = 0;
        for (auto& testcase : testcases) {
            Info("[Testing] testcase #", test_number, "\trun\t", run_number, "/", num_runs);
            
            auto input = testcase.Input(run_number == 1 ? testcase.official_seed : seed);
            Info("Input was written to /tumbletest/in.txt");
            os.WriteFile(Path::default_path + "/tumbletest/in.txt", input);


            Info("Getting .ok file for test #\t", test_number, "\twith source:\t", official_source.File());
            EggResult official_result = debug_eggecutor.Run(official_source, input);

            for (auto itr : other_sources) {
                Info("[Start] running on test #\t", test_number, "\twith source:\t", itr.File());
                EggResult other_result = debug_eggecutor.Run(itr, testcase.Input(seed));
                if (not checker.Accepted(official_result.stdin, official_result.stdout, other_result.stdout)) {
                    std::string error_message = StrCat(
                            "TestSources evaluation failed\n",
                            "Found a problem with source\n",
                            ">", other_result.source, "\n",
                            "<", official_result.source, "\n",
                            "Test information -------\n",
                            testcase.Details(), "\n",
                            "Input Ok and Output have been written to /tumbletest/{in,ok,out}.txt");

                    os.WriteFile(Path::default_path + "/tumbletest/in.txt", official_result.stdin);
                    os.WriteFile(Path::default_path + "/tumbletest/ok.txt", official_result.stdout);
                    os.WriteFile(Path::default_path + "/tumbletest/out.txt", other_result.stdout);

                    Error(error_message);
                }
            }

            test_number += 1;
        }
    }
*/
}

}  // namespace tumbletest
