#pragma once

#include "string_utils.hpp"
#include "text_modifiers.hpp"

#include <iostream>
#include <string>

namespace tumbletest {

enum LogType { INFO = 0, WARNING = 1, ERROR = 2 };
/*
#define Info(...) tumbletest::Log<tumbletest::LogType::INFO>(__FILE__, std::to_string(__LINE__), StrCat(__VA_ARGS__))
*/
#define Warning(...) \
    tumbletest::Log<tumbletest::LogType::WARNING>(__FILE__, std::to_string(__LINE__), StrCat(__VA_ARGS__))
#define Error(...) tumbletest::Log<tumbletest::LogType::ERROR>(__FILE__, std::to_string(__LINE__), StrCat(__VA_ARGS__))


#define Info(...)
// #define Warning(...)
// #define Error(...)

template <int>
void Log(const std::string& file, const std::string& line, const std::string& message);

std::string Allign(std::string text, int size) {
    while ((int)text.size() < size) {
        text += ' ';
    }
    return text;
}

/*
template <>
void Log<LogType::INFO>(const std::string& file, const std::string& line, const std::string& message) {
    std::string _file = file;
    if (file.find_last_of("/") != std::string::npos) {
        _file = file.substr(file.find_last_of("/"));
    }
    std::cerr << Allign(StrCat(Colored(Color::dark_gray, "[INFO]"), _file, ":", line), 50 + 12) << "> " << message
              << '\n';
}
*/

template <>
void Log<LogType::WARNING>(const std::string& file, const std::string& line, const std::string& message) {
    std::string _file = file;
    if (file.find_last_of("/") != std::string::npos) {
        _file = file.substr(file.find_last_of("/"));
    }
    std::cerr << Allign(StrCat(Colored(Color::yellow, "[WARNING]"), _file, ":", line), 45 + 12) << "> " << message
              << '\n';
}

template <>
void Log<LogType::ERROR>(const std::string& file, const std::string& line, const std::string& message) {
    std::string _file = file;
    if (file.find_last_of("/") != std::string::npos) {
        _file = file.substr(file.find_last_of("/"));
    }
    std::cerr << Allign(StrCat(Colored(Color::red, "[ERROR]"), _file, ":", line), 50 + 12) << "> " << message << '\n';
    exit(0);
}
}  // namespace tumbletest
