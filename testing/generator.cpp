#include "../src/tumbletest"
using namespace std;

int main() {
    SetOfficialSource("a1.cpp");
    SetTestsLocation("tests/");
    addTest("10 20\n");
    RunTumbletest();
    return 0;   
}
