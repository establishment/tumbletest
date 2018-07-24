This examples showcases what happens when stress-testing bad solutions.

Using the command `TestSources(200, "bad.cpp");` the solution `bad.cpp` will be tested 200 times with all the test cases added up to this point.

Testing multiple sources can be done using `TestSources(200, {"bad.cpp", "bad2.cpp");`

In case a test cases was found on which one of the sources did not pass the `checker` the following message will be printed:

![Screenshot](photos/found-failed-testcase.png)
