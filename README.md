###Tumbletest
Framework to easily develop testcases mostly for competive programming tasks. It does not come with some cool graph generators or flux matrices, that will be another project. Tumbletest is focused on creating the files and testing other sources with the advantage that the generator will be easy to use, with a single run and it will be deterministic.

The name come from the ideea that the framework creates testcases that will tumble magicly into a archive for usage.

####Why use tumbletest?
If you are tired of writing bash hacks that run magicly the source and generator to create the sources and managing seeds for tests, than tumbletest is for you.

####Pros
The testcases are deterministic. You can run the framework multiple times expecting the same output.

It's easy to test multiple implementations against the same testcases. If one of the source fails, tumbletest will tell you what function spawned the test and what was the tests seed so you can reproduce the exact test that failed that specific source with one line of code.

It can generate different archives with the input files that are examples, pretests and final_tests where is the case.

####Usage

```
#include <tumbletest>
#include <string_utils>
#include <vector_utils>

#include <generator_utils>

#include <algorithm>
#include <iostream>
#include <string>
using namespace std;

string Random(int max_val=1e9) {
    return StrCat(Rand(1, max_val + 1), ' ', Rand(1, max_val + 1), '\n');
}

int main() {
/// official source is "official.cpp"
/// default tests location is "/tests"
/// use these functions to change the defaults
/// SetOfficialSource(path);
/// SetTestsLocation(path);

    addTest(Random(10));
    addTest(Random(20)).Seed(1000);  /// this test will be run with seed 1000 every time
    addTest(Random());
    RunTumbletest();    /// creates testcases
    
/// to test other sources for bugs / strange behaviours
/// runs all testcases 200 times with different seeds.
/// in case one of them is wrong, prints the .in .ok .out
/// and the seed to reproduce the test
/// TestSources(200, "other_cool_implementation.cpp");
    return 0;
}
```
