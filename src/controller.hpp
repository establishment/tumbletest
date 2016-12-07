#pragma once
#include <string>
#include <vector>

#include "test_archive.hpp"
#include "task_config.hpp"
#include "os.hpp"

namespace tumbletest {
class Controller {
  public:
    TestCase& AddTest(const TestCase& testcase);
    
    // set stuff
    void TestsLocation(const std::string& path);

    void ArchiveOption(const bool option);

    void OfficialSource(const std::string& source);

    void Checker(const std::string& checker);

    void Run();
    
    void TestSources(int num_runs, const std::vector<std::string>& test_sources);

  private:
    TestArchive archive;
    TaskConfig config;
};
}  // namespace tumbletest
