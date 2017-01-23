#copy the entire library into /urs/local/include
#so you the user can just #include <tumbletest>

install:
	mkdir -p /usr/local/include/tumbletest_src/
	cp src/*.hpp /usr/local/include/tumbletest_src/
	cp src/tumbletest /usr/local/include/tumbletest

clean:
	rm -rf /usr/local/include/tumbletest_src/
	rm /usr/local/include/tumbletest
