#pragma once

#include <string>
#include <vector>

namespace std {
inline std::string to_string(const std::string& t) {
    return t;
}

inline std::string to_string(const char& c) {
    return std::string(new char(c));
}

inline std::string to_string(const char* t) {
    std::string result;
    while (* t) {
        result += * t;
        t++;
    }

    return result;
}
} // namespace std

template<typename T>
std::string StrCat(const T t) {
    return std::to_string(t);
}

template<typename T, typename... Args>
std::string StrCat(const T t, const Args... args) {
    return std::to_string(t) + StrCat(args...);
}

std::vector<std::string> Split(const std::string& txt, char split_char) {
    std::string current_txt = "";
    std::vector<std::string> output;
    for (int i = 0 ; i < (int) txt.size() ; i += 1) {
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
