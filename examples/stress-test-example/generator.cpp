#include <tumbletest>
#include <generator_utils>

#include <algorithm>
#include <iostream>
#include <string>
using namespace std;

void Asserts(int a, int b) {
    assert(1 <= a and a <= 1e9);
    assert(1 <= b and b <= 1e9);
}

string Stringify(int a, int b) {
    Asserts(a, b);

    string result = StrCat(a, ' ', b, '\n');

    return result;
}

string Random(int max_val) {
    return Stringify(xRand(1, max_val), xRand(1, max_val));
}

int main() {
    // examples
    addTest(Stringify(3, 7));
    addTest(Stringify(5, 33));

    // small testcases
    addTest(Random(10));
    addTest(Random(15));


    addTest(Random(10)).Seed(1587590963); // testcase found by the stress-test
    // TestSources(200, "bad.cpp"); // test solution "bad.cpp" 200 times with the 4 testcases

    addTest(Random(1e2));
    addTest(Random(1e3));
    addTest(Random(1e4));
    addTest(Random(1e5));
    addTest(Random(1e6));
    addTest(Random(1e7));
    addTest(Random(1e8));
    addTest(Random(1e9));
    addTest(Stringify(1e9, 1e9));

    RunTumbletest();
    return 0;
}
