#pragma once

#include <string>
#include <vector>

/// add new functionality to std::to_string
namespace std {
inline std::string to_string(const std::string& t) {
    return t;
}

inline std::string to_string(const char& c) {
    return string(1, c);
}

inline std::string to_string(const char* t) {
    std::string result;
    while (*t) {
        result += *t;
        t++;
    }

    return result;
}
}  //  namespace std

/// optimise for multime recursive calls to StrCat if used like to_string(T) + StrCat(args...)
/// last element is apended n - 1 time
template <typename T>
void __StrCat(std::string& target_result, const T t) {
    target_result += std::to_string(t);
}

template <typename T, typename... Args>
void __StrCat(std::string& target_result, const T t, const Args... args) {
    target_result += std::to_string(t);
    __StrCat(target_result, args...);
}

/// Concatenates all args into a big string, calling to_string on them
/// see to_string added functionality above
template <typename T>
std::string StrCat(const T t) {
    return std::to_string(t);
}

template <typename T, typename... Args>
std::string StrCat(const T t, const Args... args) {
    std::string result = "";
    __StrCat(result, t, args...);
    return result;
}

std::vector<std::string> Split(const std::string& txt, char split_char) {
    std::string current_txt = "";
    std::vector<std::string> output;
    for (int i = 0; i < (int)txt.size(); i += 1) {
        if (split_char == txt[i]) {
            output.push_back(current_txt);
            current_txt = "";
        } else {
            current_txt += txt[i];
        }
    }

    output.push_back(current_txt);
    return output;
}
