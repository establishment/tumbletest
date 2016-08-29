#pragma once

#include "eggecutor.hpp" // header

#include "string_utils.hpp"

#include <cmath>

#include <string>

namespace tumbletest {

Path BaseBinaryPath() {
    return Path(Path::default_path + "/tumbletest/src");
}

Path ProgrammingLanguage::base_binary_path = BaseBinaryPath();

ProgrammingLanguage::Library language_library(
        {
                {"cpp",  new ProgrammingLanguage::CPP()},
                {"cc",   new ProgrammingLanguage::CPP()},
                {"java", new ProgrammingLanguage::Java()},
                {"py",   new ProgrammingLanguage::Python()}
        });

EggecutorProfile EggecutorProfile::Debug() {
    return EggecutorProfile(true, 100.0, true, true);
}

EggecutorProfile EggecutorProfile::Deployment(const double& time_limit) {
    return EggecutorProfile(false, time_limit, false, false);
}

EggecutorProfile EggecutorProfile::Testing(const double& time_limit) {
    return EggecutorProfile(false, time_limit, true, false);
}

EggecutorProfile::EggecutorProfile(const bool& show_time,
                                   const double& time_limit,
                                   const bool& show_status,
                                   const bool& print_errors_to_stdout)
        : show_time(show_time),
          time_limit(time_limit),
          show_status(show_status),
          print_errors_to_stdout(print_errors_to_stdout) { }

EggResult::EggResult(const Path& source, const std::string& stdin, const std::string& stdout, const std::string& stderr,
                     const ExecutionRunProfile& run_summary)
        : source(source), stdin(stdin), stdout(stdout), stderr(stderr), run_summary(run_summary) { }

EggResult Eggecutor::Run(const Path& source, const std::string& input_data) const {
    Path stdin = os.TmpFile();
    Path stdout = os.TmpFile();
    Path stderr = os.TmpFile();

    os.WriteFile(stdin, input_data);

    auto language = language_library.objects[source.Extension()];
    if (language == nullptr) {
        Error(StrCat("Unknown extension for file:\t", source.to_string()));
    }

    language->Compile(source);
    std::string run_command = StrCat(
            language->RunCommand(source), " ",
            "<", stdin, " ",
            "1>", stdout, " ",
            "2>", stderr);

    if (profile.show_status) {
        Info("Run source:\t" + source.to_string());
    }

    auto run_profile = ExecuteCommand(run_command);

    if (profile.show_status) {
        Info("Finished running.");
    }

    std::string output = os.ReadFile(stdout);
    std::string errors = os.ReadFile(stderr);

    if (run_profile.exit_code != 0) {
        Error(StrCat("Non zero exit status (", run_profile.exit_code, ")", "\n",
                     "Source:\t", source));
    }

    tumbletest_cache.ClearTmp();

    return EggResult(source, input_data, output, errors, run_profile);
}


ExecutionRunProfile Eggecutor::ExecuteCommand(const std::string& command) {
    auto bash_output = os.RunBashCommand("(time -p " + command + ") 2>&1" + "; echo \"exit $?\"");
    auto lines = Split(bash_output, '\n');
    auto GetValueAfterTab = [](const std::string& txt) -> double {
        int position = txt.find(" ");
        return std::stod(txt.substr(position + 1, txt.size() - position - 1).c_str());
    };

    auto n = lines.size() - 1;

    return ExecutionRunProfile(
            GetValueAfterTab(lines[n - 4]),
            GetValueAfterTab(lines[n - 3]),
            GetValueAfterTab(lines[n - 2]),
            int(std::round(GetValueAfterTab(lines[n - 1])))
    );
}

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

    Info("Compiling", "\t", source);
    auto err = os.TmpFile();
    auto result = Eggecutor::ExecuteCommand(
            "g++ -std=c++14 -O2 -Wall " + source + " -o " + this->BinaryFile(source) + " 2>" + err);
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
    os.RunBashCommand("mkdir -r " + dir);
    return dir + "/" + source.ExtensionLess();
}

bool ProgrammingLanguage::Java::Compile(const Path& source) {
    if (os.ValidFile(this->BinaryFile(source))) {
        return true;
    }

    Info("Compiling", "\t", source);
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
    return "java " + BinaryFile(source);
}

// python
Path ProgrammingLanguage::Python::BinaryFile(const Path& source) {
    std::string dir = base_binary_path + "/" + source.md5();
    os.RunBashCommand("mkdir -r " + dir);
    return dir + "/" + source.File();
}

bool ProgrammingLanguage::Python::Compile(const Path& source) {
    os.RunBashCommand("cp " + source + " " + BinaryFile(source));
    return true;
}

std::string ProgrammingLanguage::Python::RunCommand(const Path& source) {
    return "python " + BinaryFile(source);
}

}  // namespace tumbletest
