#include "tumbletest.hpp"
#include "strcat.hpp"

#include <iostream>
#include <string>
using namespace std;

string Stringify(pair<int, int> input) {
    return StrCat(input.first, " ", input.second);
};

pair<int, int> Random() {
    return {rand(), rand()};
};

int main() {
    
    SetOfficialSource("ab.cpp");
    SetTestsLocation("tests/ab_tests/");

//    for (int i = 0; i < 100; i += 1) {
//        AddFinalTest(Stringify(Random()));
//    }

    addTest(Stringify(Random()));
    /*
    addTest(Stringify(Random())).setSeed(123).setType(FinalTest);
    addTest(Stringify(Random()));
    addCTest(
            int x;
            int n = x * 2;
            return Stringify(Random(n));
    );
    */

    TestSources(200, "ab.cpp");
    RunTumbletest();
    return 0;
}
