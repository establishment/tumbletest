#pragma once

#include "logging.hpp"
#include "os.hpp"

#include <functional>
#include <sstream>
#include <string>

namespace tumbletest {

/*
 * Checkers for problems.
 * used for validating if output is good or not.
 */

class Checker {
  public:
    bool is_function;
    std::function<bool(const std::string& in, const std::string& ok, const std::string& out)> function;
    Path checker_path;

    Checker(const std::function<bool(const std::string& in, const std::string& ok, const std::string& out)>& function)
            : is_function(true), function(function), checker_path("/dev/null") { }

    Checker(const Path& path)
            : is_function(false),
              function([=](const std::string&, const std::string, const std::string&) -> bool {
                  Error("function for checker is not set.");
                  return false;
              }),
              checker_path(path) { }

    bool Accepted(const std::string& in, const std::string& ok, const std::string& out) {
        if (is_function) {
            return function(in, ok, out);
        } else {
            // TODO(@VELEA) add this feature
            Error("Feature not available.");
            return false;
        }
    }
};

// TODO(@velea) add checkers for double

Checker default_checker = Checker(
        [](const std::string& in, const std::string& ok, const std::string& out) -> bool {
            std::stringstream a;
            std::stringstream b;
            a << ok;
            b << out;
            std::string A, B;

            while (a >> A) {
                if (b >> B) {
                } else {
                    return false;
                }

                if (A != B) {
                    return false;
                }
            }

            if (b >> B) {
                return false;
            }

            return true;
        });

}  // namespace tumbletest
