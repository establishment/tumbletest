#pragma once

#include <ostream>
#include <string>
#include <vector>

template<typename T>
std::vector<T> operator+(const std::vector<T>& lhs, const std::vector<T>& rhs) {
    std::vector<T> result;
    result.reserve(lhs.size() + rhs.size());
    result.insert(result.end(), lhs.begin(), lhs.end());
    result.insert(result.end(), rhs.begin(), rhs.end());
    return result;
}

template<typename T>
std::vector<T>& operator+=(std::vector<T>& lhs, const std::vector<T>& rhs) {
    if (lhs.size() + rhs.size() > lhs.capacity()) {
        lhs.reserve(2 * std::max(lhs.size(), rhs.size()));
    }

    lhs.insert(lhs.end(), rhs.begin(), rhs.end());
    return lhs;
}

template<typename T>
std::vector<T>& operator+=(std::vector<T>& lhs, const T& rhs) {
    lhs.push_back(rhs);

    return lhs;
}

template<typename T>
std::ostream& operator<<(std::ostream& cout, std::vector<T>& rhs) {
    for (int i = 0 ; i < int(rhs.size()) ; i += 1) {
        if (i != 0) {
            cout << ' ';
        }
        cout << rhs[i];
    }
    return cout;
}

template<typename T>
std::ostream& operator<<(std::ostream& cout, std::vector<std::vector<T>>& rhs) {
    for (int i = 0 ; i < int(rhs.size()) ; i += 1) {
        if (i != 0) {
            cout << '\n';
        }
        cout << rhs[i];
    }
    return cout;
}
