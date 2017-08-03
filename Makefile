install:
	sudo mkdir -p /usr/local/include/tumbletest_src/
	sudo cp src/*.* /usr/local/include/tumbletest_src/
	sudo cp src/tumbletest /usr/local/include/tumbletest
	sudo cp src/default_batch_checker_include.hpp /usr/local/include/batch.hpp
	sudo cp src/default_batch_checker.cpp /usr/local/include/default_batch_checker.cpp
	$(MAKE) install-submodules
	
install-submodules:
	git submodule update --init --recursive
	$(MAKE) -C ./third_party/generator-utils install

clean:
	sudo rm -rf /usr/local/include/tumbletest_src/
	sudo rm /usr/local/include/tumbletest
	sudo rm /usr/local/include/batch.hpp
	sudo rm /usr/local/include/default_batch_checker.cpp
	$(MAKE) clean-submodules

clean-submodules:
	$(MAKE) -C ./third_party/generator-utils clean
