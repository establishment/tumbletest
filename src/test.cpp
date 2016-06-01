#define SETGET(type, name) \
private:\
    type name##_;\
public: \
    void name(const type& rhs) { name##_ = rhs; } \
    void name(const type&& rhs) { name##_(rhs); } \
    type name() { return name##_; }

#include <iostream>
#include <vector>
#include "vector_utils.hpp"
#include <map>
using namespace std;

map<int, int> x[int(1e6)];

int main() {
    vector<int> a = {1, 2, 3}, b({4, 5});
    vector<vector<int>> p;
    p += a;
    p += b;
    p += b + a;
    p += a + b;
    a += b;
    p += a + b;
    a += 10;
    p += a;
    cout << p << '\n';

    for (int i = 0; i < int(1e6); i += 1) {
        x[i][i] = i;
    }

    int _; cin >> _;
}

