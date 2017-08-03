#pragma once

#include "eggecutor.hpp"  // header

#include "checkers.hpp"
#include "string_utils.hpp"

#include <cmath>
#include <sstream>
#include <string>

namespace tumbletest {

Path BaseBinaryPath() {
    return Path(Path::default_path + "/tumbletest/src");
}

Path ProgrammingLanguage::base_binary_path = BaseBinaryPath();

ProgrammingLanguage::Library language_library({{"cpp", new ProgrammingLanguage::CPP()},
                                               {"cc", new ProgrammingLanguage::CPP()},
                                               {"java", new ProgrammingLanguage::Java()},
                                               {"py", new ProgrammingLanguage::Python()}});

ProgrammingLanguage::Base* Compile(const Path& source) {
    ProgrammingLanguage::Base *language = language_library.objects[source.Extension()];
    if (language == nullptr) {
        Error(StrCat("Unknown extension for file:\t", source.to_string()));
    }

    language->Compile(source);
    return language;
}

void EnsureChecker(const Path& checker) {
    if (not os.ValidFile(checker)) {
        if (checker.File() == "checker.cpp") {
            os.RunBashCommand(StrCat("cp /usr/local/include/default_batch_checker.cpp ", checker));
            Info("No checker file found. Using default checker.");
            Info("checker.cpp file was created with default code.");
        } else {
            Error(
                "Checker file not found. Checker name is not checker.cpp so default checker fallback is not "
                "available.");
        }
    }
}

EggecutorProfile EggecutorProfile::Debug() {
    return EggecutorProfile(true, 100.0, true, true);
}

EggecutorProfile EggecutorProfile::Deployment(const double& time_limit) {
    return EggecutorProfile(false, time_limit, false, false);
}

EggecutorProfile EggecutorProfile::Testing(const double& time_limit) {
    return EggecutorProfile(false, time_limit, true, false);
}

EggecutorProfile::EggecutorProfile(const bool& show_time, const double& time_limit, const bool& show_status,
                                   const bool& print_errors_to_stdout)
      : show_time(show_time),
        time_limit(time_limit),
        show_status(show_status),
        print_errors_to_stdout(print_errors_to_stdout) {
}

EggResult::EggResult(const Path& source, const std::string& stdin, const std::string& stdout, const std::string& stderr,
                     const ExecutionRunProfile& run_summary)
      : source(source), stdin(stdin), stdout(stdout), stderr(stderr), run_summary(run_summary) {
}

EggResult Eggecutor::Run(const Path& source, const std::string& input_data) const {
    Path stdin = os.TmpFile();
    Path stdout = os.TmpFile();
    Path stderr = os.TmpFile();

    os.WriteFile(stdin, input_data);

    auto language = Compile(source);
    std::string run_command =
        StrCat(language->RunCommand(source), " ", "<", stdin, " ", "1>", stdout, " ", "2>", stderr);

    if (profile.show_status) {
        std::cerr << GetBloatware() << "\t\t" << Colored(Color::green, "> ") <<  Colored(Color::magenta, source.File()) << '\t';
    }

    auto run_profile = ExecuteCommand(run_command);

    std::string output = os.ReadFile(stdout);
    std::string errors = os.ReadFile(stderr);

    if (run_profile.exit_code != 0) {
        std::cerr << '\n';
        Error(StrCat("Non zero exit status (", run_profile.exit_code, ")", "\n", "Source:\t", source));
    } else if (profile.show_status) {
         char buff[32];
        snprintf(buff, 32, "%.3lf", run_profile.real);
        std::cerr << "done\t" <<  Colored(Color::green, buff) << '\t';
    }

    tumbletest_cache.ClearTmp();
    return EggResult(source, input_data, output, errors, run_profile);
}

std::pair<EggResult, CheckerResult> Eggecutor::RunInteractive(const Path& source, const Path& checker,
                                                              const std::string& input_data) const {
    Path stdin = os.TmpFile();
    Path stdout = os.TmpFile();
    Path stderr = os.TmpFile();
    Path checker_stderr = os.TmpFile();

    os.WriteFile(stdin, input_data);

    auto language = Compile(source);
    auto checker_lang = Compile(checker);

    os.RunBashCommand(StrCat("rm ", stdin));
    os.RunBashCommand(StrCat("rm ", stdout));
    os.RunBashCommand(StrCat("mkfifo ", stdin));
    os.RunBashCommand(StrCat("mkfifo ", stdout));
 
    os.WriteFile("input.txt", input_data);

    std::string run_command =
        StrCat(language->RunCommand(source), " ", "<", stdin, " ", "1>", stdout, " ", "2>", stderr, " & ",
               checker_lang->RunCommand(checker), " 1>", stdin, " <", stdout, " 2>", checker_stderr, " ; wait $! ");

    if (profile.show_status) {
        std::cerr << GetBloatware() << "\t\t" << Colored(Color::green, "> ") <<  Colored(Color::magenta, source.File()) << '\t';
    }
    auto run_profile = ExecuteCommand(run_command);

    if (run_profile.exit_code != 0) {
        std::cerr << '\n';
        Error(StrCat("Non zero exit status (", run_profile.exit_code, ")", "\n", "Source:\t", source));
    } else if (profile.show_status) {
         char buff[32];
        snprintf(buff, 32, "%.3lf", run_profile.real);
        std::cerr << "done\t" <<  Colored(Color::green, buff) << '\t';
    }

    std::string output = "";
    std::string errors = os.ReadFile(stderr);

    std::string logger = os.ReadFile("logger.txt");
    std::string results = os.ReadFile("result.txt");

    os.RunBashCommand("rm logger.txt");
    os.RunBashCommand("rm result.txt");
    os.RunBashCommand("rm input.txt");

    std::stringstream R(results);
    bool passed = false;
    R >> passed;
    R.get();
    char _message[1024];
    bzero(_message, sizeof(_message));
    R.read(_message, sizeof(_message) - 1);
    std::string message(_message);
    while (message.size() and message.back() == '\n') {
        message.pop_back();
    }

    tumbletest_cache.ClearTmp();
    return {EggResult(source, input_data, output, errors, run_profile), CheckerResult({passed, message, logger})};
}

std::pair<EggResult, CheckerResult> Eggecutor::RunChecker(const Path& checker, const std::string& in,
                                                          const std::string& ok, const std::string& out) const {
    Path in_f = os.TmpFile();
    Path ok_f = os.TmpFile();
    Path out_f = os.TmpFile();

    os.WriteFile(in_f, in);
    os.WriteFile(ok_f, ok);
    os.WriteFile(out_f, out);

    EnsureChecker(checker);

    auto language = language_library.objects[checker.Extension()];
    if (language == nullptr) {
        Error(StrCat("Unknown extension for file:\t", checker.to_string()));
    }

    language->Compile(checker);
    std::string run_command = StrCat(language->RunCommand(checker), " ", "in_file=", in_f, " ", "ok_file=", ok_f, " ",
                                     "out_file=", out_f, " 1>/dev/null 2>/dev/null");

    if (profile.show_status) {
        Info("Run checker:\t", Colored(Color::magenta, checker.File()));
    }

    auto run_profile = ExecuteCommand(run_command);

    if (profile.show_status) {
        char buff[32];
        snprintf(buff, 32, "%.3lf", run_profile.real);
        Info("Finished running in ", Colored(Color::green, buff), "\tExit code:",
             Colored(Color::green, StrCat(run_profile.exit_code)));
    }

    if (profile.show_status) {
        Info("Run interactive - source:\t", Colored(Color::magenta, checker.File()));
    }

    std::string results = os.ReadFile("result.txt");
    os.RunBashCommand("rm result.txt");

    std::stringstream R(results);
    bool passed = false;
    R >> passed;
    R.get();
    char _message[1024];
    bzero(_message, sizeof(_message));
    R.read(_message, sizeof(_message) - 1);
    std::string message(_message);
    while (message.size() and message.back() == '\n') {
        message.pop_back();
    }

    tumbletest_cache.ClearTmp();
    return {EggResult(checker, "", "", "", run_profile), CheckerResult({passed, message, ""})};
}

ExecutionRunProfile Eggecutor::ExecuteCommand(const std::string& command) {
    std::string bash_output = "";

#ifdef __APPLE__
    auto exec_command = "(time -p " + command + ") 2>&1" + "; echo \"exit $?\"";
    bash_output = os.RunBashCommand(exec_command);
#elif __linux__
    auto tmp_file = os.TmpFile();
    auto exec_command = StrCat("time -p -o ", tmp_file, " ", command, "; echo exit $? >>", tmp_file);
    os.RunBashCommand(exec_command);
    bash_output = os.ReadFile(tmp_file);
#else
#error "gtfo windows"
#endif

    auto lines = Split(bash_output, '\n');
    auto GetValueAfterTab = [](const std::string& txt) -> double {
        int position = txt.find(" ");
        return std::stod(txt.substr(position + 1, txt.size() - position - 1).c_str());
    };

    auto n = lines.size() - 1;

    return ExecutionRunProfile(GetValueAfterTab(lines[n - 4]), GetValueAfterTab(lines[n - 3]),
                               GetValueAfterTab(lines[n - 2]), int(std::round(GetValueAfterTab(lines[n - 1]))));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Programming languages
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// c++
Path ProgrammingLanguage::CPP::BinaryFile(const Path& source) {
    std::string dir = base_binary_path + "/" + source.md5();
    os.RunBashCommand("mkdir -p " + dir);
    return dir + "/" + "_" + source.ExtensionLess();
}

bool ProgrammingLanguage::CPP::Compile(const Path& source) {
    if (os.ValidFile(this->BinaryFile(source))) {
        return true;
    }

    std::cerr << GetBloatware() << Colored(4, 1, 1, "\tCompiling") << '\t' << source << '\n';
    auto err = os.TmpFile();
    auto result = Eggecutor::ExecuteCommand("g++ -std=c++14 -O2 -Wall " + source + " -o " + this->BinaryFile(source) +
                                            " 2>" + err);
    if (result.exit_code != 0) {
        Error(StrCat("compile errors for", "\t", source, "\n", os.ReadFile(err.to_string())));
        return false;
    }

    return result.exit_code == 0;
}

std::string ProgrammingLanguage::CPP::RunCommand(const Path& source) {
    return BinaryFile(source);
}

// java
Path ProgrammingLanguage::Java::BinaryFile(const Path& source) {
    std::string dir = base_binary_path + "/" + source.md5();
    os.RunBashCommand("mkdir -p " + dir);
    return dir + "/" + source.File();
}

bool ProgrammingLanguage::Java::Compile(const Path& source) {
    if (os.ValidFile(this->BinaryFile(source))) {
        return true;
    }

    std::cerr << GetBloatware() << Colored(4, 1, 1, "\tCompiling") << '\t' << source << '\n';
    os.RunBashCommand("cp " + source.to_string() + " " + BinaryFile(source));
    auto err = os.TmpFile();
    auto result = Eggecutor::ExecuteCommand("javac -Xlint " + BinaryFile(source) + " 2>" + err.to_string());

    if (result.exit_code != 0) {
        Error(StrCat("compile errors for", "\t", source, "\n", os.ReadFile(err.to_string())));
        return false;
    }

    return result.exit_code == 0;
}

std::string ProgrammingLanguage::Java::RunCommand(const Path& source) {
    return "java " + BinaryFile(source).ExtensionLess();
}

// python
Path ProgrammingLanguage::Python::BinaryFile(const Path& source) {
    std::string dir = base_binary_path + "/" + source.md5();
    os.RunBashCommand("mkdir -p " + dir);
    return dir + "/" + source.File();
}

bool ProgrammingLanguage::Python::Compile(const Path& source) {
    os.RunBashCommand("cp " + source + " " + BinaryFile(source));
    return true;
}

std::string ProgrammingLanguage::Python::RunCommand(const Path& source) {
    return "python3 " + BinaryFile(source);
}

}  // namespace tumbletest
