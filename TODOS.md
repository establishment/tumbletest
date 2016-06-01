#####Short time targets
1. Don't use temp files in /tmp , create a temporary folder that will contain the compiled sources and tmp input files.
2. Solve java problem when the source name should be the main class's name.
3. Add temp file class.
4. Support a mode only to create the .in files and empty .oks
5. Split testing the expected testcases and random testcases logic.

#####Long time targets
1. Integrate command line flags and logging from google's [base library](https://github.com/google/or-tools/tree/master/src/base).
2. Create a mode where tumbletest will create a .cpp file that will create all .ins and .oks so you have only one ultra portable file that will generate the testcases.
