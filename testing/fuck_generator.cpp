#include <tumbletest>
using namespace std;

int main() {
    SetOfficialSource("a1.cpp");
    SetTestsLocation("tests/");
    addTest("10 20\n");
    addTest("20 30\n");
    addTest("10 20\n");
    addTest("10 20\n");
    TestSources(2, "a2.cpp");
    RunTumbletest();
    return 0;   
}
