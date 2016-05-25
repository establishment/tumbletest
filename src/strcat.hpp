#pragma once

#include <string>

namespace std {
inline std::string to_string(const std::string t) {
    return t;
}

inline std::string to_string(const char *t) {
    std::string result;
    while (*t) {
        result += *t;
        t++;
    }

    return result;
}
} // namespace std

template <typename T>
std::string StrCat(const T t) {
    return std::to_string(t);
}

template <typename T, typename... Args>
std::string StrCat(const T t, const Args... args) {
    return std::to_string(t) + StrCat(args...);
}