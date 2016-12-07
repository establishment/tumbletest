#include "eval/builder.cpp"
#include "os.cpp"
#include "eval/jailer.cpp"
#include "controller.cpp"

#include <iostream>

using namespace std;
using namespace tumbletest;

int main() {
    Source a("test2.cpp");
    a.Compile();
    cout << a.RunCommand(); 
    return 0;
}
