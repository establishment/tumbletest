#pragma once

#include "logging.hpp"
#include "os.hpp"

#include <functional>
#include <sstream>
#include <string>

namespace tumbletest {

class CheckerResult {
  public:
    bool passed;
    std::string message;
    std::string logger;
};
}  // namespace tumbletest
