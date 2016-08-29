#pragma once

#include "logging.hpp"
#include "os.hpp"

#include <map>
#include <string>

namespace tumbletest {

/*
 * Profiles for running testcases.
 * TL;DR specifies the vorbose level for logging.
 * TODO(@velea) actually use this.
 */
class EggecutorProfile {
  public:
    EggecutorProfile(const bool& show_time,
                     const double& time_limit,
                     const bool& show_status,
                     const bool& print_errors_to_stdout);

    static EggecutorProfile Debug();

    static EggecutorProfile Deployment(const double& time_limit);

    static EggecutorProfile Testing(const double& time_limit);

    const bool show_time;
    const double time_limit;
    const bool show_status;
    const bool print_errors_to_stdout;
};

/*
 * Information about a run.
 * Used for Eggecutor::ExecuteCommand
 */
class ExecutionRunProfile {
  public:
    ExecutionRunProfile(const double& real, const double& user, const double& sys, const int& exit_code)
            : real(real), user(user), sys(sys), cpu(user + sys), exit_code(exit_code) { }

    const double real, user, sys, cpu;
    int exit_code;
};

/*
 * Information about a test run.
 * in, out, err and run_summary.
 */

class EggResult {
  public:
    EggResult(const Path& source, const std::string& stdin, const std::string& stdout, const std::string& stderr,
              const ExecutionRunProfile& run_summary);

    const Path source;
    const std::string stdin;
    const std::string stdout;
    const std::string stderr;
    const ExecutionRunProfile run_summary;
};

/*
 * List of supported programming languages.
 * Each language supports it's own compilation and run command
 */

class ProgrammingLanguage {
  public:
    static Path base_binary_path;

/*
 * Base class for every programming language
 */
    class Base {
      public:
        virtual Path BinaryFile(const Path& source) = 0;

        virtual bool Compile(const Path& source) = 0;

        virtual std::string RunCommand(const Path& source) = 0;
    };

    class CPP : public Base {
      public:
        Path BinaryFile(const Path& source);

        bool Compile(const Path& source);

        std::string RunCommand(const Path& source);
    };

    class Java : public Base {
      public:
        Path BinaryFile(const Path& source);

        bool Compile(const Path& source);

        std::string RunCommand(const Path& source);
    };

    class Python : public Base {
      public:
        Path BinaryFile(const Path& source);

        bool Compile(const Path& source);

        std::string RunCommand(const Path& source);
    };

/*
 * An object from all programming languages is hashed here.
 */
    class Library {
      public:
        std::map<std::string, ProgrammingLanguage::Base*> objects;

        Library(std::map<std::string, ProgrammingLanguage::Base*> objects)
                : objects(objects) { }
    };
};

/*
 * Executes stuff
 * Runs testcases, watches over compilation and shit.
 * Also runs random commands
 */
class Eggecutor {
  public:
    Eggecutor(const EggecutorProfile& profile) : profile(profile) { }

    EggResult Run(const Path& source, const std::string& input_data) const;

  private:
    const EggecutorProfile profile;

  public:
    static ExecutionRunProfile ExecuteCommand(const std::string& command);
};

}  // namespace tumbletest

#include "eggecutor_implementation.hpp"
