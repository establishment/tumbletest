#include "jailer.hpp"
#include "../os.hpp"
#include "../utils/logging.hpp"

#include "jailer_config.hpp"
#include "jailer_runstats.hpp"

namespace tumbletest {

Path Jailer::base_box_path = "/tmp/tumbletest";

Jailer::Jailer(int box_number) : box_path(StrCat(base_box_path, "/", box_number, "/box")) { 
    os.RunBashCommand(StrCat("mkdir -p ", box_path));
}

RunStats Jailer::RunCommand(const std::string& command, std::string input) {
    RunStats stats;
    stats.resultCode = RunStats::OK;

    os.RunBashCommand(StrCat("touch ", box_path, "/stdin"));
    if (input.size()) {
        os.WriteFile(StrCat(box_path, "/stdin"), input);
    }
    os.RunBashCommand(StrCat("cd ", box_path, " ; ", command, " <stdin 1>stdout 2>stderr"));
    
    return stats;
}

void Jailer::CopyToSandbox(const Path& local_path, std::string sandbox_path) {
    os.RunBashCommand(StrCat("cp ", local_path, " ", box_path, sandbox_path));
}

void Jailer::CopySandbox(const Path& path) {
    os.RunBashCommand(StrCat("cp -r ", box_path, "/.* ", path, "/"));
}

void Jailer::ClearSandbox() {
    os.RunBashCommand(StrCat("rm -rf ", box_path, "/*"));
}

std::string Jailer::stdout() {
    return os.ReadFile(StrCat(box_path, "/stdout"));
}

std::string Jailer::stderr() {
    return os.ReadFile(StrCat(box_path, "/stderr"));
}

Jailer run_jailer(1);
Jailer compile_jailer(2);

}
