#pragma once

#include <cassert>
#include <ctime>

#include <set>

#include "os.hpp"

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