#pragma once

#include <map>
#include <string>

#include "../os.hpp"
#include "jailer_runstats.hpp"

namespace tumbletest {

namespace ProgrammingLanguage {
/*
 * Base class for every programming language
 */
    class Generic {
      public:
        virtual ~Generic() { }
        virtual RunStats Compile(const Path& source) = 0;
        virtual std::string RunCommand(const Path& source) = 0;
    };

    class CPP : public Generic {
      public:
        RunStats Compile(const Path& source);
        std::string RunCommand(const Path& source);
    };

    class Java : public Generic {
      public:
        RunStats Compile(const Path& source);
        std::string RunCommand(const Path& source);
    };

    class Python : public Generic {
      public:
        RunStats Compile(const Path& source);
        std::string RunCommand(const Path& source);
    };

    Generic* GetLanguage(const Path& source);
};

class Source {
  public:
    Source(const Path& source); 
    ~Source();

    static Path compile_root;

    Path CompileFolder();
    RunStats Compile();
    std::string RunCommand();

  protected:
    std::string content;
    std::string hash;
    ProgrammingLanguage::Generic* lang;
  
  public:
    Path path;
};

bool InitBuilder();

}  // namespace tumbletest
