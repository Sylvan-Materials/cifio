
all: LDFLAGS= -L../../lemon-main/dist/lib -L../../antlr4/runtime/Cpp/run/usr/local/lib -I../node_modules/zlib/dist/lib -lantlr4-runtime -lemon -lz -ldl -pthread
all: build/src/parsing/CIFLexer.o build/src/parsing/CIFParser.o test
	$(CXX) $(LDFLAGS) -o test $(wildcard build/src/*.o) $(wildcard build/src/parsing/*.o) $(wildcard build/src/constitution/*.o)

#build/src/constitution/graph.o: CXXFLAGS= -fmodules-ts -O3 -pthread -std=c++2a -I../../lemon-main/dist/include
#build/src/constitution/graph.o: ../src/constitution/graph.cpp
#	$(CXX) --version
#	$(CXX) $(CXXFLAGS) -c -o build/src/constitution/graph.o ../src/constitution/graph.cpp

build/src/parsing/CIFLexer.o: CXXFLAGS= -O3 -pthread -std=c++17 -I../include -I../src -I../node_modules/doctest -I../node_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime
build/src/parsing/CIFLexer.o: ../src/parsing/CIFLexer.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/CIFLexer.o ../src/parsing/CIFLexer.cpp

build/src/parsing/CIFParser.o: CXXFLAGS= -O3 -pthread -std=c++17 -I../include -I../src -I../node_modules/doctest -I../node_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime
build/src/parsing/CIFParser.o: ../src/parsing/CIFParser.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/CIFParser.o ../src/parsing/CIFParser.cpp

test: CXXFLAGS= -O3 -pthread -std=c++2a -I../include -I../../lemon-main/dist/include -I../src -I../node_modules/doctest -I../node_modules/zlib/dist/include -I../../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime

test: LDFLAGS= 

test: src/test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/test.o src/test.cpp

clean:
	cd node_modules/zlib && $(MAKE) clean
	cd node_modules/openssl && $(MAKE) clean

