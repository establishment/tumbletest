#include <tumbletest>
#include <generator_utils>

#include <algorithm>
#include <iostream>
#include <string>
using namespace std;

void Asserts(int a, int b) {
    assert(1 <= a and a <= b and b <= 1e5);
}

string Stringify(int a, int b) {
    Asserts(a, b);

    string result = StrCat(a, ' ', b, '\n');

    return result;
}

int main() {
    addTest(Stringify(10, 20));    
    
    RunTumbletest();
    return 0;
}
