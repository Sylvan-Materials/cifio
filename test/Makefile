
all: LDFLAGS= -fuse-ld=gold -L../ -L../../lemon-main/dist/lib -L$(JAVA_HOME)/lib -L$(JAVA_HOME)/lib/server -L../cpp_modules/urlcpp -L../../antlr4/runtime/Cpp/run/usr/local/lib -L../cpp_modules/zlib/dist/lib -L../cpp_modules/openssl/openssl/lib -lcifio -lantlr4-runtime -lemon -lurlcpp -lz -ldl -pthread -lssl -lcrypto
all: build/src/matching_test.o build/src/dictionary_test.o build/src/write_test.o test
	$(CXX) $(LDFLAGS) -o test $(wildcard build/src/*.o)  


build/src/matching_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++2a -I../include -I../../lemon-main/dist/include -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src -I../cpp_modules/json/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/matching_test.o: src/matching_test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/matching_test.o src/matching_test.cpp

build/src/dictionary_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++2a -I../include -I../../lemon-main/dist/include -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src -I../cpp_modules/json/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/dictionary_test.o: src/dictionary_test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/dictionary_test.o src/dictionary_test.cpp

build/src/write_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++2a -I../include -I../../lemon-main/dist/include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src -I../cpp_modules/json/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/write_test.o: src/write_test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/write_test.o src/write_test.cpp

test: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++2a -I../include -I../../lemon-main/dist/include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src -I../cpp_modules/json/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD

test: LDFLAGS= 

test: src/test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/test.o src/test.cpp 

clean:
	cd cpp_modules/zlib && $(MAKE) clean
	cd cpp_modules/openssl && $(MAKE) clean

