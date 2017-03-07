Tumbletest - Generating Testcases for Competitive Programming, Simplified
=========================================================================

Overview
--------
Framework to easily develop testcases mostly for competive programming tasks. 
Tumbletest is focused on 
- managing your testcases
- running the sources on the testcase - `c++`, `java` and `python` are supported at the moment
- keeping your solutions binaries up to date - compiles them when it's needed
- managing the determinism of the generator - each run produces the same testcases
- creating the interaction for interactive problems

And most of all, it's focused on making an easy and simplified workflow for the users.

Design goals
------------
- Make the user do only the things that he needs to do. No more bash scripts that will run the official source on all sources or md5ing to see if the outputs are the same.
- Intuitive API. The whole framework exposes only 1 API. `addTest(code)`. The code will be run everytime the input of that test is needed. The code can be a string or a function call.
- Powerfull and scalable. You can just take the generator file and modify the library so it generates only some testcases, it runs sources on an external server for example without needing to change the "user code". In this way the main piece of code can be used in many ways.

Installation
------------
```sh
git clone https://github.com/establishment/tumbletest
cd tumbletest
make install
```

Why Tumbletest?
---------------
- You only need to write the code that actually makes sense. No more extra scripts or useless steps needed.
- It's easy to test multiple implementations against the same testcases. If one of the source fails, tumbletest will tell you what function spawned the test and what was the tests seed so you can reproduce the exact test that failed that specific source with one line of code.
- The way the API works is very natural. You want to write a function that will generate a string which will be a testcase. You only need to do that.

Examples
--------

```cpp
#include <tumbletest>
#include <generator_utils>
using namespace std;

string Random(int max_val = 1e9) {
    return StrCat(
        Rand(1, max_val + 1), ' ',
        Rand(1, max_val + 1), '\n'
        );
}

int main() {
    addTest(Random(10));                // 2 random numbers in range [1, 10]
    addTest(Random(20)).Seed(1000);     // this test will be created with a fixed seed of 1000
    addTest(Random());                  // 2 random numbers in range [1, 1e9]

    RunTumbletest();    /// create testcases

    // to test other sources to see if the outputs are ok, use
    TestSources(200, "other_cool_implementation.cpp");

    // or to test multiple sources in paralel
    TestSources(200, {"a.cpp", "b.cpp"});
    return 0;
}
```

Default values
--------------
- the official solution is `official.cpp` and can be changed by calling `SetOfficialSource(path)`
- the tests location is 'tests/` and can be changed by calling `SetTestsLocation(path)`
- when comparing 2 sources, a checker is used to determine if the output is correct knowing the input and the ok from the official source. The name of the checker is `checker.cpp`. If this file is not present, a default checker will be used which will check if the files are the same, excluding whitespaces
