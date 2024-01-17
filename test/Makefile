
all: LDFLAGS= -fuse-ld=gold -L../ -L../cpp_modules/lemon/dist/lib -L$(JAVA_HOME)/lib -L$(JAVA_HOME)/lib/server -L$(LUA_HOME)/install/lib -L../cpp_modules/urlcpp -L../cpp_modules/antlr4/runtime/Cpp/run/lib -L../cpp_modules/zlib/dist/lib -L../cpp_modules/openssl/openssl/lib -L../../libpng/dist/lib -L../../json-thresher -L../../xml-thresher -llua -lcifio -lantlr4-runtime -lemon -lurlcpp -lz -ldl -pthread -lssl -lcrypto -lpng -ljsonthresher -lxmlthresher -Wl,--allow-multiple-definition
all: build/src/matching_test.o build/src/symmetry_test.o build/src/dictionary_test.o build/src/write_test.o build/src/type_test.o build/src/bipartite_test.o build/src/standard_protons_test.o build/src/accessible_test.o build/src/optimization_test.o build/src/matrix_test.o build/src/lattice_test.o test
	$(CXX) $(LDFLAGS) -o test $(wildcard build/src/*.o) $(LUA_HOME)/install/lib/liblua.a 


build/src/matching_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++23 -I../include -I../cpp_modules/lemon/dist/include -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src  -I../cpp_modules/fmt/dist/include -I../cpp_modules/json-thresher/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/matching_test.o: src/matching_test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/matching_test.o src/matching_test.cpp

build/src/symmetry_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fcoroutines -std=c++23 -fext-numeric-literals -I../include -I./../cpp_modules/lemon/dist/include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src -I../cpp_modules/json-thresher/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/symmetry_test.o: src/symmetry_test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/symmetry_test.o src/symmetry_test.cpp

build/src/dictionary_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++23 -I../include -I./../cpp_modules/lemon/dist/include -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src  -I../cpp_modules/fmt/dist/include -I../cpp_modules/json-thresher/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/dictionary_test.o: src/dictionary_test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/dictionary_test.o src/dictionary_test.cpp

build/src/write_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++23 -I../include -I./../cpp_modules/lemon/dist/include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src -I../cpp_modules/json-thresher/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/write_test.o: src/write_test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/write_test.o src/write_test.cpp

build/src/type_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++23 -I../include -I./../cpp_modules/lemon/dist/include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include -I../cpp_modules/fmt/dist/include -I../src -I../cpp_modules/json-thresher/include -I../cpp_modules/xml-thresher/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/type_test.o: src/type_test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/type_test.o src/type_test.cpp

build/src/bipartite_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++23 -I../include -I../include -I../src -I../src -I./../cpp_modules/lemon/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/json-thresher/include -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/doctest -MMD
build/src/bipartite_test.o: src/bipartite_test.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/bipartite_test.o src/bipartite_test.cpp

build/src/standard_protons_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++23 -I../include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../include -I../include -I../src -I../src -I./../cpp_modules/lemon/dist/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/doctest -MMD
build/src/standard_protons_test.o: src/standard_protons_test.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/standard_protons_test.o src/standard_protons_test.cpp

build/src/accessible_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++23  -I../include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../include -I../src -I../src -I./../cpp_modules/lemon/dist/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/doctest -MMD
build/src/accessible_test.o: src/accessible_test.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/accessible_test.o src/accessible_test.cpp

build/src/optimization_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++23  -I../include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../include -I../src -I../src -I./../cpp_modules/lemon/dist/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/doctest -MMD
build/src/optimization_test.o: src/optimization_test.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/optimization_test.o src/optimization_test.cpp

build/src/lattice_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++23  -I../include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I$(LUA_HOME)/src -I../../julia/usr/include/julia -I../include -I../src -I../src -I./../cpp_modules/lemon/dist/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../../libpng/dist/include -I../cpp_modules/doctest -MMD
build/src/lattice_test.o: src/lattice_test.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/lattice_test.o src/lattice_test.cpp

build/src/matrix_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++23  -I../include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../../julia/usr/include/julia -I../include -I../src -I../src -I./../cpp_modules/lemon/dist/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/doctest -MMD
build/src/matrix_test.o: src/matrix_test.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/matrix_test.o src/matrix_test.cpp

test: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++23 -I../include -I./../cpp_modules/lemon/dist/include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src -I../cpp_modules/json-thresher/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD -Wl,--allow-multiple-definition
test: LDFLAGS= 

test: src/test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/test.o src/test.cpp 

clean:
	rm ./build/*
	#cd cpp_modules/zlib && $(MAKE) clean
	#cd cpp_modules/openssl && $(MAKE) clean

