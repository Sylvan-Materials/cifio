
all: LDFLAGS= -fuse-ld=gold -L../ -L../cpp_modules/lemon/dist/lib -L$(JAVA_HOME)/lib -L$(JAVA_HOME)/lib/server -L$(LUA_HOME)/install/lib -L../cpp_modules/urlcpp -L../cpp_modules/antlr4/runtime/Cpp/run/lib -L../cpp_modules/zlib/dist/lib -L../cpp_modules/openssl/openssl/lib64 -L../cpp_modules/openssl/openssl/lib -L../../libpng/dist/lib -L../../json-thresher -L../../xml-thresher -llua -lcifio -lantlr4-runtime -lemon -lurlcpp -lz -ldl -pthread -lgomp -lssl -lcrypto -lpng -ljsonthresher -lxmlthresher -Wl,--allow-multiple-definition
all:  test
	$(CXX) $(LDFLAGS) -o test $(wildcard build/src/*.o) $(LUA_HOME)/install/lib/liblua.a 
	
test: build/src/matching_test.o build/src/symmetry_test.o build/src/dictionary_test.o build/src/write_test.o build/src/type_test.o build/src/bipartite_test.o build/src/standard_protons_test.o build/src/spatial_exploration_test.o build/src/accessible_test.o build/src/optimization_test.o build/src/sequencing_test.o build/src/construction_test.o build/src/matrix_test.o build/src/crystallography_test.o build/src/lattice_test.o build/src/test.o

test: CXXFLAGS= -DNDEBUG -O3 -pthread -fopenmp -std=c++26 -I../include -I../cpp_modules/lemon/dist/include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src -I../cpp_modules/graph-v2/include -I../cpp_modules/json-thresher/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -Wl,--allow-multiple-definition -MMD
build/src/test.o: src/test.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/test.o src/test.cpp

build/src/matching_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fopenmp -std=c++26 -I../include -I../cpp_modules/lemon/dist/include -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src  -I../cpp_modules/fmt/dist/include -I../cpp_modules/graph-v2/include -I../cpp_modules/json-thresher/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/matching_test.o: src/matching_test.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/matching_test.o src/matching_test.cpp

build/src/symmetry_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fcoroutines -fopenmp -std=c++26 -fext-numeric-literals -I../include -I../cpp_modules/lemon/dist/include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../cpp_modules/graph-v2/include -I../src -I../cpp_modules/json-thresher/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/symmetry_test.o: src/symmetry_test.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/symmetry_test.o src/symmetry_test.cpp

build/src/dictionary_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fopenmp -std=c++26 -I../include -I../cpp_modules/lemon/dist/include -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src  -I../cpp_modules/fmt/dist/include -I../cpp_modules/graph-v2/include -I../cpp_modules/json-thresher/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/dictionary_test.o: src/dictionary_test.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/dictionary_test.o src/dictionary_test.cpp

build/src/write_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fopenmp -std=c++26 -I../include -I../cpp_modules/lemon/dist/include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src -I../cpp_modules/fmt/include  -I../cpp_modules/graph-v2/include  -I../cpp_modules/json-thresher/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/write_test.o: src/write_test.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/write_test.o src/write_test.cpp

build/src/type_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fopenmp -std=c++26 -I../include -I../cpp_modules/lemon/dist/include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include -I../src -I../cpp_modules/fmt/dist/include -I../cpp_modules/graph-v2/include -I../cpp_modules/json-thresher/include -I../cpp_modules/xml-thresher/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/xml-thresher/src  -MMD
build/src/type_test.o: src/type_test.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/type_test.o src/type_test.cpp

build/src/bipartite_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fopenmp -std=c++26 -I../include -I../include -I../src -I../src -I../cpp_modules/lemon/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/graph-v2/include -I../cpp_modules/json-thresher/include -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/doctest -MMD
build/src/bipartite_test.o: src/bipartite_test.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/bipartite_test.o src/bipartite_test.cpp

build/src/standard_protons_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fopenmp -std=c++26 -I../include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../include -I../include -I../src -I../src -I../cpp_modules/lemon/dist/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/doctest -MMD
build/src/standard_protons_test.o: src/standard_protons_test.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/standard_protons_test.o src/standard_protons_test.cpp

build/src/spatial_exploration_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fopenmp -std=c++26 -I../include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../include -I../include -I../src -I../src -I../cpp_modules/lemon/dist/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/doctest -MMD
build/src/spatial_exploration_test.o: src/spatial_exploration_test.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/spatial_exploration_test.o src/spatial_exploration_test.cpp

build/src/accessible_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fopenmp -std=c++26  -I../include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../include -I../src -I../src -I../cpp_modules/lemon/dist/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/graph-v2/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/doctest -MMD
build/src/accessible_test.o: src/accessible_test.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/accessible_test.o src/accessible_test.cpp

build/src/optimization_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fopenmp -std=c++26  -I../include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../include -I../src -I../src -I../cpp_modules/lemon/dist/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/graph-v2/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/doctest -MMD
build/src/optimization_test.o: src/optimization_test.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/optimization_test.o src/optimization_test.cpp

build/src/sequencing_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fopenmp -std=c++26  -I../include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../include -I../src -I../src -I../cpp_modules/lemon/dist/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/doctest -MMD
build/src/sequencing_test.o: src/sequencing_test.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/sequencing_test.o src/sequencing_test.cpp

build/src/construction_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fopenmp -std=c++26  -I../include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../include -I../src -I../src -I../cpp_modules/lemon/dist/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/graph-v2/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/doctest -MMD
build/src/construction_test.o: src/construction_test.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/construction_test.o src/construction_test.cpp

build/src/lattice_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fopenmp -std=c++26  -I../include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I$(LUA_HOME)/src -I../../julia/usr/include/julia -I../include -I../src -I../src -I../cpp_modules/lemon/dist/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../../libpng/dist/include -I../cpp_modules/doctest -MMD
build/src/lattice_test.o: src/lattice_test.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/lattice_test.o src/lattice_test.cpp

build/src/crystallography_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fopenmp -std=c++26  -I../include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I$(LUA_HOME)/src -I../../julia/usr/include/julia -I../include -I../src -I../src -I../cpp_modules/lemon/dist/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../../libpng/dist/include -I../cpp_modules/doctest -MMD
build/src/crystallography_test.o: src/crystallography_test.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/crystallography_test.o src/crystallography_test.cpp

build/src/matrix_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -fopenmp -std=c++26  -I../include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../../julia/usr/include/julia -I../include -I../src -I../src -I../cpp_modules/lemon/dist/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/doctest -MMD
build/src/matrix_test.o: src/matrix_test.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/matrix_test.o src/matrix_test.cpp

clean:
	rm -rf ./build/src

