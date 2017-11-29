#include "tumbletest.hpp"
#include "vector_utils.hpp"
#include "string_utils.hpp"
using namespace std;

string Rand() {
    return to_string(rand() % 100);
}


int main() {
    addTest(Rand());
    TestSources(200, "b.cpp");
}

