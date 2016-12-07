#pragma once
#include "os.hpp"

namespace tumbletest {
enum TaskType {
    kBatch = 0,
    kInteractive = 1,
};

class TaskConfig {
  public:
    Path official_source;
    Path checker;
    Path tests_location;

    std::string test_prefix; // test-$i or sth

    bool archive_tests; // create a archive with all testcases after generating .ok files or not

    int time_limit_ms;
    int memory_limit_kb;
    
};
}  // namespace tumbletest
