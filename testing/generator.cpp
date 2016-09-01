#include <tumbletest>
using namespace std;

int main() {
    SetOfficialSource("a1.cpp");
    addTest("10 20\n");
    TestSources(100, "a2.cpp");
    return 0;   
}
