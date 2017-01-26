install:
	sudo mkdir -p /usr/local/include/tumbletest_src/
	sudo cp src/*.hpp /usr/local/include/tumbletest_src/
	sudo cp src/tumbletest /usr/local/include/tumbletest

clean:
	sudo rm -rf /usr/local/include/tumbletest_src/
	sudo rm /usr/local/include/tumbletest
