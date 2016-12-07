#pragma once

#include <string>

#include "jailer_config.hpp"
#include "jailer_runstats.hpp"

#include "../os.hpp"

namespace tumbletest {
class Jailer {
  public:
    Jailer(int box_number);
    
    static Path base_box_path;
    Path box_path;
    
    RunStats RunCommand(const std::string& command, std::string input="");
    void CopyToSandbox(const Path& path, std::string sandbox_path);
    void CopySandbox(const Path& local_path);
    void ClearSandbox();

    std::string stdout();
    std::string stderr();
};

bool InitJailer();

extern Jailer run_jailer;
extern Jailer compile_jailer;
}  // namespace tumbletest
