#include "tumbletest.hpp"
#include <string>
using namespace std;

std::string f(int a) {
    return std::to_string(a) + " 123";
}

int main() {
    SetOfficialSource("test.cpp");
    AddExample(f(666013));
    RunTumbletest();
}