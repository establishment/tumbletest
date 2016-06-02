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
#include "tumbletest.hpp"

...

string RandomTest(int max_value) {
  return to_string(rand() % max_value) + " " + to_string(rand() % max_value);
};

int main() {
  SetOfficialSource("aplusb.cpp");
  SetTestsLocation("aplusb_tests/");
  
  addTest(RandomTest(10)).SetType(EXAMPLE);
  addTest(RandomTest(100)).SetType(PRETEST).SetSeed(123);
  addTest(RandomTest(1e9)).SetType(FINAL_TEST);
  
  RunTumbletest();
  return 0;
}
```
