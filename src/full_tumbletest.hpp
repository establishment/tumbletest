#include <string>
#include <vector>


#include <functional>
#include <string>
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>
#include <unordered_map>

// os.hpp

#include <cstdio>

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace tumbletest {

class Path : public std::string {
  public:
    Path(const Path&) = default;
    Path(Path&&) = default;
    Path& operator=(const Path&) = default;
    Path& operator=(Path&&) = default;
    virtual ~Path() = default;

    Path(std::string location) : std::string(CheckIntegrity(location)), extension(GetExtension(location)) { }
    Path(const char* location) : std::string(CheckIntegrity(std::string(location))), extension(GetExtension(location)) { }

    std::string extension;

    Path ExtensionLess() const;

  private:
    static const std::string GetBasePath();

    std::string GetExtension(std::string current_path);
    std::string CheckIntegrity(std::string current_path);

    const static std::string default_path;
};

class OS {
  public:
    std::string RunBashCommand(const char *cmd);
    std::string RunBashCommand(const std::string cmd);
    std::string RunBashCommand(std::vector<std::string> cmds);

    void CreateArchive(std::vector<Path> files, Path archive);
    bool ValidFile(Path file);

    std::string ReadFile(Path file);
    void WriteFile(Path file, const std::string &content);

    std::string TmpFile();

    static OS& GetSingleton();

  private:
    OS();
} os = OS::GetSingleton();

const std::string Path::default_path = Path::GetBasePath();

const std::string Path::GetBasePath() {
    std::string path = os.RunBashCommand("pwd");
    path.pop_back();
    path += "/";
    return path;
}

std::string Path::CheckIntegrity(std::string current_path) {
    if (current_path[0] == '/' or current_path[0] == '~') {
        return current_path;
    } else {
        return default_path + current_path;
    }
}

std::string Path::GetExtension(std::string current_path) {
    std::string extension;
    bool ok = false;
    for (int index = int(current_path.size()) - 1; index >= 0; index -= 1) {
        if (current_path[index] == '/') {
            break;
        } else if (current_path[index] == '.') {
            ok = true;
            break;
        }
        extension += current_path[index];
    }

    if (ok) {
        reverse(extension.begin(), extension.end());
    } else {
        extension = "N/A";
    }

    return extension;
}

Path Path::ExtensionLess() const {
    std::string full_path = *this;
    while (full_path.back() != '.') {
        full_path.pop_back();
    }
    full_path.pop_back();
    return Path(full_path);
}

OS::OS() { }

OS& OS::GetSingleton() {
    static OS singleton;
    return singleton;
}

std::string OS::RunBashCommand(const char *cmd) {
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

std::string OS::RunBashCommand(const std::string cmd) {
    return RunBashCommand(cmd.c_str());
}

std::string OS::RunBashCommand(std::vector<std::string> cmds) {
    std::string result = "";
    std::string cmd = "";
    for (std::string &itr : cmds) {
        cmd += itr;
        cmd += ';';
    }
    return RunBashCommand(cmd);
}

void OS::CreateArchive(std::vector<Path> files, Path archive) {
    if (files.empty()) {
        return;
    }

    std::string zip_command = "";
    if (archive.find(".zip") == std::string::npos) {
        zip_command = "zip -j " + archive + ".zip";
    } else {
        zip_command = "zip -j " + archive;
    }

    std::string files_to_zip = "";
    for (auto file_path : files) {
        files_to_zip  += " " + file_path;
    }

    RunBashCommand(zip_command + files_to_zip);
    RunBashCommand("rm " + files_to_zip);
}

bool OS::ValidFile(Path file) {
    std::string answer = RunBashCommand("( test -e " + file + " ) && echo \"yes\" || echo \"no\"");
    return answer == "yes\n";
}

std::string OS::ReadFile(Path file) {
    std::ifstream fin(file, std::ios::in | std::ios::binary);

    fin.seekg(0, std::ios::end);
    std::string file_information;
    file_information.resize(fin.tellg());

    fin.seekg(0, std::ios::beg);
    fin.read(&file_information[0], file_information.size());
    fin.close();
    return file_information;
}

void OS::WriteFile(Path file, const std::string &content) {
    std::ofstream fout(file);
    fout << content;
    fout.close();
    return;
}

std::string OS::TmpFile() {
    char *tmpname = strdup("/tmp/tmpfileXXXXXX");
    mkstemp(tmpname);
    return std::string(tmpname);
}
}  // namespace tumbletest

// eggecutor.hpp

#include <cassert>
#include <ctime>

#include <set>

using std::cerr;

namespace tumbletest {

class EggecutorProfile {
  public:
    EggecutorProfile(const bool &show_time,
                     const double &time_limit,
                     const bool &show_status,
                     const bool &print_errors_to_stdout);

    static EggecutorProfile Debug();
    static EggecutorProfile Deployment(const double &time_limit);
    static EggecutorProfile Testing(const double &time_limit);

    const bool show_time;
    const double time_limit;
    const bool show_status;
    const bool print_errors_to_stdout;
};

EggecutorProfile::EggecutorProfile(const bool &show_time,
                                   const double &time_limit,
                                   const bool &show_status,
                                   const bool &print_errors_to_stdout)
        : show_time(show_time),
          time_limit(time_limit),
          show_status(show_status),
          print_errors_to_stdout(print_errors_to_stdout) { }

EggecutorProfile EggecutorProfile::Debug() {
    return EggecutorProfile(true, 100.0, true, true);
}

EggecutorProfile EggecutorProfile::Deployment(const double &time_limit) {
    return EggecutorProfile(false, time_limit, false, false);
}

EggecutorProfile EggecutorProfile::Testing(const double &time_limit) {
    return EggecutorProfile(false, time_limit, true, false);
}

class EggResult {
  public:
    EggResult(const Path &source, const std::string &stdin, const std::string &stdout, const std::string &stderr, const double &time);
    const Path source;
    const std::string stdin;
    const std::string stdout;
    const std::string stderr;
    const double time;
};

EggResult::EggResult(const Path &source, const std::string &stdin, const std::string &stdout, const std::string &stderr, const double &time)
        : source(source), stdin(stdin), stdout(stdout), stderr(stderr), time(time) { }

class Eggecutor {
  public:
    Eggecutor(const EggecutorProfile &profile);
    EggResult Run(const Path &source, const std::string &input_data="") const;

  private:
    const EggecutorProfile profile;
    mutable std::map<Path, std::string> compiled_sources;

    std::string GetCommand(const Path &source) const;
    double ExecuteSource(std::string command) const;

    std::string cpp_command(const Path &source) const;
    std::string py_command(const Path &source) const;
    std::string java_command(const Path &source) const;
};

Eggecutor::Eggecutor(const EggecutorProfile &profile) : profile(profile) { }

EggResult Eggecutor::Run(const Path &source, const std::string &input_data) const {

    Path stdin = os.TmpFile();
    Path stdout = os.TmpFile();
    Path stderr = os.TmpFile();

    os.WriteFile(stdin, input_data);

    std::string run_command = GetCommand(source);

    run_command = run_command +  " " +
                  "<" + stdin + " " +
                  "1>" + stdout + " " +
                  "2>" + stderr;

//    cerr << "Run command >\n" << run_command << "\nRun command<-\n";

    if (profile.show_status) { cerr << "Run source\n"; }
    double exec_time = ExecuteSource(run_command);
    if (profile.show_status) { cerr << "Finished running\n"; }


    std::string output = os.ReadFile(stdout);
    std::string errors = os.ReadFile(stderr);

    os.RunBashCommand("rm " + stdin);
    os.RunBashCommand("rm " + stdout);
    os.RunBashCommand("rm " + stderr);
    return EggResult(source, input_data, output, errors, exec_time);
}

double Eggecutor::ExecuteSource(std::string command) const {
    time_t start = clock();
    os.RunBashCommand(command);
    return 1.0 * (clock() - start) / CLOCKS_PER_SEC;
}

std::string Eggecutor::GetCommand(const Path &source) const {
    if (source.extension == "py") {
        return py_command(source);
    } else if (source.extension == "java") {
        return java_command(source);
    } else if (source.extension == "cc" or source.extension == "cpp") {
        return cpp_command(source);
    } else {
        assert(0);
    }
}

std::string Eggecutor::py_command(const Path &source) const {
    return "python3 " + source;
}

std::string Eggecutor::java_command(const Path &source) const {
    if (compiled_sources.find(source) == compiled_sources.end()) {
        if (profile.show_status) { cerr << "Compiling\t" << source << "\n"; }
        os.RunBashCommand("javac " + source);

        Path compiled = source.ExtensionLess();

        std::string run_command = "java " + compiled;
        compiled_sources[source] = run_command;
        if (profile.show_status) { cerr << "Finished compiling\n"; }
    }

    return compiled_sources[source];
}

std::string Eggecutor::cpp_command(const Path &source) const {
    if (compiled_sources.find(source) == compiled_sources.end()) {
        if (profile.show_status) { cerr << "Compiling\t" << source << '\n'; }
        Path compiled = source.ExtensionLess();

        os.RunBashCommand("g++ -std=c++14 -O2 -Wall " + source + " -o " + compiled + " -lm");

        std::string run_command = compiled;
        compiled_sources[source] = run_command;
        if (profile.show_status) { cerr << "Finished compiling\n"; }
    }

    return compiled_sources[source];
}
}  // namespace tumbletest

using std::cerr;

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

enum TESTTYPE {
    DEBUG_TEST = -1,
    EXAMPLE = 0,
    PRETEST = 1,
    FINAL_TEST = 2,
};

class TestCase {
  public:
//    TestCase&(const TestCase&) = default;
//    TestCase&(TestCase&&) = default;
//    TestCase& operator=(const TestCase&) = default;
//    TestCase& operator=(TestCase&&) = default;
    virtual ~TestCase() = default;

    TestCase(const TESTTYPE& type, std::function<std::string()> function, const int& priority, const std::string &function_call_string);
    bool operator<(const TestCase &rhs) const;

    const std::string& GetInput();

    void ChangeSeed(unsigned seed);
    void AddSeedFunction(std::function<void(unsigned)> function);
    void FailDebug();


  private:
    std::function<std::string()> function;

  public:
    TESTTYPE type;

  private:
    static std::vector<std::function<void(unsigned)> > seed_functions;
    static std::unordered_map<int, int> num_testcases;
    int priority;

    // debug purpose
    std::string function_call_string;
    // debug purpose
    int function_added_num;

    int seed;
    bool is_computed;
    std::string input;
};

std::vector<std::function<void(unsigned)> > TestCase::seed_functions = {srand};
std::unordered_map<int, int> TestCase::num_testcases = {};

TestCase::TestCase(const TESTTYPE& type, std::function<std::string()> function, const int& priority, const std::string &function_call_string)
        : function(function), type(type), priority(priority), function_call_string(function_call_string) {
    is_computed = false;
    function_added_num = ++num_testcases[type];
    seed = rand();
};

void TestCase::ChangeSeed(unsigned seed) {
    this->seed = seed;
    is_computed = false;
    input = "";
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
    void AddTest(TestCase testcase);
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

void TestArchive::AddTest(TestCase testcase) {
    testcases.push_back(testcase);
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
            testcase.ChangeSeed(rand());
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
                    std::exit(0);
                }
            }
        }
    }
}
}  // namespace tumbletest



// macros.hpp
#include <string>

#define __AddTest(type, function, priority) tumbletest::test_archive.AddTest(\
        tumbletest::TestCase(\
            type,\
            [=]() -> std::string { return function; }, \
            priority, \
            #function))\

#define AddFinalTest(x) __AddTest(tumbletest::FINAL_TEST, x, 0)
#define AddDebugTest(x) __AddTest(tumbletest::DEBUG_TEST, x, 0)

// tumbletest.hpp
void RunTumbletest() {
    tumbletest::test_archive.Run();
}

void SetTestsLocation(const std::string &location) {
    tumbletest::test_archive.SetTestsLocation(location);
}

void SetOfficialSource(std::string source) {
    tumbletest::test_archive.SetOfficialSource(source);
}

void TestSources(std::string source) {
    tumbletest::test_archive.TestSources(100, {tumbletest::Path(source)});
}

void TestSources(int runs, std::string source) {
    tumbletest::test_archive.TestSources(runs, {tumbletest::Path(source)});
}

void TestSources(std::vector<std::string> _sources) {
    std::vector<tumbletest::Path> sources = {};
    for (auto& itr : _sources) {
        sources.push_back(tumbletest::Path(itr));
    }

    tumbletest::test_archive.TestSources(100, sources);
}

void TestSources(int runs, std::vector<std::string> _sources) {
    std::vector<tumbletest::Path> sources = {};
    for (auto& itr : _sources) {
        sources.push_back(tumbletest::Path(itr));
    }

    tumbletest::test_archive.TestSources(runs, sources);
}

