
all: LDFLAGS= -fuse-ld=gold -L../../lemon-main/dist/lib -L../cpp_modules/urlcpp -L../../antlr4/runtime/Cpp/run/usr/local/lib -L../cpp_modules/zlib/dist/lib -L../cpp_modules/openssl/openssl/lib -lantlr4-runtime -lemon -lurlcpp -lz -ldl -pthread -lssl -lcrypto
all: build/src/parsing/CIFLexer.o build/src/parsing/CIFParser.o build/src/parsing/DICLexer.o build/src/parsing/DICParser.o build/src/constitution/graph.o build/src/constitution/Populator.o build/src/density/Populator.o build/src/lattice/Populator.o build/src/reading/tcp/TCPReader.o build/src/publishing/jgf/JGFPublisher.o build/src/matching_test.o build/src/dictionary_test.o test
	$(CXX) $(LDFLAGS) -o test $(wildcard build/src/*.o) $(wildcard build/src/parsing/*.o) $(wildcard build/src/constitution/*.o) $(wildcard build/src/density/*.o) $(wildcard build/src/lattice/*.o) $(wildcard build/src/reading/tcp/*.o) $(wildcard build/src/publishing/jgf/*.o) 

build/src/constitution/Populator.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++2a -I../include -I../src -I../../lemon-main/dist/include -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include -I../cpp_modules/json/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/constitution/Populator.o: ../src/constitution/Populator.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/constitution/Populator.o ../src/constitution/Populator.cpp
 
build/src/density/Populator.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++2a -I../include -I../src -I../../lemon-main/dist/include -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/density/Populator.o: ../src/density/Populator.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/density/Populator.o ../src/density/Populator.cpp

build/src/lattice/Populator.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++2a -I../include -I../src -I../../lemon-main/dist/include -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/lattice/Populator.o: ../src/lattice/Populator.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/lattice/Populator.o ../src/lattice/Populator.cpp

build/src/publishing/jgf/JGFPublisher.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++2a -I../include -I../src -I../../lemon-main/dist/include -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/publishing/jgf/JGFPublisher.o: ../src/publishing/jgf/JGFPublisher.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/jgf/JGFPublisher.o ../src/publishing/jgf/JGFPublisher.cpp

build/src/reading/tcp/TCPReader.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++2a -I../include -I../src -I../../lemon-main/dist/include -I../cpp_modules/urlcpp -I../cpp_modules/openssl/openssl/include -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/reading/tcp/TCPReader.o: ../src/reading/tcp/TCPReader.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/reading/tcp/TCPReader.o ../src/reading/tcp/TCPReader.cpp

build/src/constitution/graph.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++2a -I../include -I../../lemon-main/dist/include -MMD
build/src/constitution/graph.o: ../src/constitution/graph.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/constitution/graph.o ../src/constitution/graph.cpp

build/src/parsing/CIFLexer.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++17 -I../include -I../src -I../cpp_modules/doctest -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/parsing/CIFLexer.o: ../src/parsing/CIFLexer.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/CIFLexer.o ../src/parsing/CIFLexer.cpp

build/src/parsing/CIFParser.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++17 -I../include -I../src -I../cpp_modules/doctest -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/parsing/CIFParser.o: ../src/parsing/CIFParser.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/CIFParser.o ../src/parsing/CIFParser.cpp

build/src/parsing/DICLexer.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++17 -I../include -I../src -I../cpp_modules/doctest -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/parsing/DICLexer.o: ../src/parsing/DICLexer.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/DICLexer.o ../src/parsing/DICLexer.cpp

build/src/parsing/DICParser.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++17 -I../include -I../src -I../cpp_modules/doctest -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/parsing/DICParser.o: ../src/parsing/DICParser.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/DICParser.o ../src/parsing/DICParser.cpp

build/src/matching_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++2a -I../include -I../../lemon-main/dist/include -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src -I../cpp_modules/json/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/matching_test.o: src/matching_test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/matching_test.o src/matching_test.cpp

build/src/dictionary_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++2a -I../include -I../../lemon-main/dist/include -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src -I../cpp_modules/json/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/dictionary_test.o: src/dictionary_test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/matching_test.o src/matching_test.cpp

test: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++2a -I../include -I../../lemon-main/dist/include -I../cpp_modules/brigand/include -I../cpp_modules/multi_index/include  -I../src -I../cpp_modules/json/include -I../cpp_modules/urlcpp -I../cpp_modules/doctest -I../cpp_modules/openssl/openssl/include -I../cpp_modules/mio/include -I../cpp_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD

test: LDFLAGS= 

test: src/test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/test.o src/test.cpp 

clean:
	cd cpp_modules/zlib && $(MAKE) clean
	cd cpp_modules/openssl && $(MAKE) clean

