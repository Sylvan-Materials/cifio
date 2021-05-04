
all: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++2a  -Iinclude -I../lemon-main/dist/include -Icpp_modules/brigand/include -Icpp_modules/multi_index/include  -Isrc -Icpp_modules/json/include -Icpp_modules/urlcpp -Icpp_modules/doctest -Icpp_modules/openssl/openssl/include -Icpp_modules/mio/include -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
all: LDFLAGS= -shared  -fuse-ld=gold -L../lemon-main/dist/lib -L$(JAVA_HOME)/lib -L$(LUA_HOME)/install/lib -L$(JAVA_HOME)/lib/server -Lcpp_modules/urlcpp -L../antlr4/runtime/Cpp/run/usr/local/lib -Lcpp_modules/zlib/dist/lib -Lcpp_modules/openssl/openssl/lib -ljvm -llua -lantlr4-runtime -lemon -lurlcpp -lz -ldl -pthread -lssl -lcrypto
all: build/src/parsing/CIF2Lexer.o build/src/parsing/CIF2Parser.o build/src/parsing/CIFLexer.o build/src/parsing/CIFParser.o build/src/parsing/DICLexer.o build/src/parsing/DICParser.o build/src/constitution/graph.o build/src/constitution/Populator.o build/src/density/Populator.o build/src/standards/AminoStandards.o build/src/standards/ComponentStandards.o build/src/symmetry/LatticeSites.o build/src/lattice/Populator.o build/src/reading/tcp/TCPReader.o build/src/publishing/st/Publisher.o build/src/publishing/st/CIFPublisher.o build/src/publishing/st/OBJPublisher.o build/src/publishing/st/SVGPublisher.o build/src/publishing/st/MOL2Publisher.o build/src/publishing/jgf/JGFPublisher.o build/src/publishing/gz/CIFCompressor.o build/src/utils/JVMSingleton.o /home/roger/sylvanmats/lua-5.4.3/install/lib/liblua.a
	$(CXX) $(LDFLAGS) -o libcifio.so $(wildcard build/src/*.o) $(wildcard build/src/parsing/*.o) $(wildcard build/src/standards/*.o) $(wildcard build/src/constitution/*.o) $(wildcard build/src/density/*.o) $(wildcard build/src/lattice/*.o) $(wildcard build/src/symmetry/*.o) $(wildcard build/src/reading/tcp/*.o) $(wildcard build/src/publishing/jgf/*.o) $(wildcard build/src/publishing/gz/*.o) $(wildcard build/src/publishing/st/*.o) $(wildcard build/src/utils/*.o) 

build/src/standards/AminoStandards.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++2a -Iinclude -Isrc -I../lemon-main/dist/include -Icpp_modules/brigand/include -Icpp_modules/multi_index/include -Icpp_modules/json/include -Icpp_modules/mio/include -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/standards/AminoStandards.o: src/standards/AminoStandards.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/standards/AminoStandards.o src/standards/AminoStandards.cpp
 
build/src/standards/ComponentStandards.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++2a -Iinclude -Isrc -I../lemon-main/dist/include -Icpp_modules/brigand/include -Icpp_modules/multi_index/include -Icpp_modules/json/include -Icpp_modules/mio/include -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/standards/ComponentStandards.o: src/standards/ComponentStandards.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/standards/ComponentStandards.o src/standards/ComponentStandards.cpp
 
build/src/constitution/Populator.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++2a  -Iinclude -Isrc -I../lemon-main/dist/include -Icpp_modules/brigand/include -Icpp_modules/multi_index/include -Icpp_modules/json/include -Icpp_modules/mio/include -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/constitution/Populator.o: src/constitution/Populator.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/constitution/Populator.o src/constitution/Populator.cpp
 
build/src/density/Populator.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++2a -Iinclude -Isrc -I../lemon-main/dist/include -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/density/Populator.o: src/density/Populator.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/density/Populator.o src/density/Populator.cpp

build/src/lattice/Populator.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++2a -Iinclude -Isrc -I../lemon-main/dist/include -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/lattice/Populator.o: src/lattice/Populator.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/lattice/Populator.o src/lattice/Populator.cpp

build/src/symmetry/LatticeSites.o: CXXFLAGS= -DNDEBUG -O3 -fPIC  -pthread -std=c++2a  -Iinclude -I../include -I$(LUA_HOME)/src -MMD
build/src/symmetry/LatticeSites.o: src/symmetry/LatticeSites.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/symmetry/LatticeSites.o src/symmetry/LatticeSites.cpp

build/src/publishing/jgf/JGFPublisher.o: CXXFLAGS= -DNDEBUG -fPIC -O3 -pthread -std=c++2a -Iinclude -Isrc -I../lemon-main/dist/include -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/publishing/jgf/JGFPublisher.o: src/publishing/jgf/JGFPublisher.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/jgf/JGFPublisher.o src/publishing/jgf/JGFPublisher.cpp

build/src/publishing/gz/CIFCompressor.o: CXXFLAGS= -DNDEBUG -fPIC -O3 -pthread -std=c++2a -Iinclude -Isrc -I../lemon-main/dist/include -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/publishing/gz/CIFCompressor.o: src/publishing/gz/CIFCompressor.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/gz/CIFCompressor.o src/publishing/gz/CIFCompressor.cpp

build/src/publishing/st/Publisher.o: CXXFLAGS= -DNDEBUG -fPIC -O3 -pthread -std=c++2a -Iinclude -Isrc -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../lemon-main/dist/include -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/publishing/st/Publisher.o: src/publishing/st/Publisher.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/st/Publisher.o src/publishing/st/Publisher.cpp

build/src/publishing/st/CIFPublisher.o: CXXFLAGS= -DNDEBUG -fPIC -O3 -pthread -std=c++2a -Iinclude -Isrc -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../lemon-main/dist/include -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/publishing/st/CIFPublisher.o: src/publishing/st/CIFPublisher.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/st/CIFPublisher.o src/publishing/st/CIFPublisher.cpp

build/src/publishing/st/OBJPublisher.o: CXXFLAGS= -DNDEBUG -fPIC -O3 -pthread -std=c++2a -Iinclude -Isrc -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../lemon-main/dist/include -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/publishing/st/OBJPublisher.o: src/publishing/st/OBJPublisher.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/st/OBJPublisher.o src/publishing/st/OBJPublisher.cpp

build/src/publishing/st/SVGPublisher.o: CXXFLAGS= -DNDEBUG -fPIC -O3 -pthread -std=c++2a -Iinclude -Isrc -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../lemon-main/dist/include -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/publishing/st/SVGPublisher.o: src/publishing/st/SVGPublisher.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/st/SVGPublisher.o src/publishing/st/SVGPublisher.cpp

build/src/publishing/st/MOL2Publisher.o: CXXFLAGS= -DNDEBUG -fPIC -O3 -pthread -std=c++2a -Iinclude -Isrc -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../lemon-main/dist/include -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/publishing/st/MOL2Publisher.o: src/publishing/st/MOL2Publisher.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/st/MOL2Publisher.o src/publishing/st/MOL2Publisher.cpp

build/src/utils/JVMSingleton.o: CXXFLAGS= -DNDEBUG -fPIC -O3 -pthread -std=c++2a -Iinclude -Isrc -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I../lemon-main/dist/include -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/utils/JVMSingleton.o: src/utils/JVMSingleton.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/utils/JVMSingleton.o src/utils/JVMSingleton.cpp

build/src/reading/tcp/TCPReader.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++2a -Iinclude -Isrc -I../lemon-main/dist/include -Icpp_modules/urlcpp -Icpp_modules/openssl/openssl/include -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/reading/tcp/TCPReader.o: src/reading/tcp/TCPReader.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/reading/tcp/TCPReader.o src/reading/tcp/TCPReader.cpp

build/src/constitution/graph.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++2a  -Iinclude -I../lemon-main/dist/include -MMD
build/src/constitution/graph.o: src/constitution/graph.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/constitution/graph.o src/constitution/graph.cpp

build/src/parsing/CIFLexer.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++17 -Iinclude -Isrc -Icpp_modules/doctest -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/parsing/CIFLexer.o: src/parsing/CIFLexer.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/CIFLexer.o src/parsing/CIFLexer.cpp

build/src/parsing/CIFParser.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++17 -Iinclude -Isrc -Icpp_modules/doctest -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/parsing/CIFParser.o: src/parsing/CIFParser.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/CIFParser.o src/parsing/CIFParser.cpp

build/src/parsing/CIF2Lexer.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++17 -Iinclude -Isrc -Icpp_modules/doctest -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/parsing/CIF2Lexer.o: src/parsing/CIF2Lexer.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/CIF2Lexer.o src/parsing/CIF2Lexer.cpp

build/src/parsing/CIF2Parser.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++17 -Iinclude -Isrc -Icpp_modules/doctest -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/parsing/CIF2Parser.o: src/parsing/CIF2Parser.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/CIF2Parser.o src/parsing/CIF2Parser.cpp

build/src/parsing/DICLexer.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++17 -Iinclude -Isrc -Icpp_modules/doctest -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/parsing/DICLexer.o: src/parsing/DICLexer.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/DICLexer.o src/parsing/DICLexer.cpp

build/src/parsing/DICParser.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++17 -Iinclude -Isrc -Icpp_modules/doctest -Icpp_modules/zlib/dist/include -I../antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/parsing/DICParser.o: src/parsing/DICParser.cpp 
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/DICParser.o src/parsing/DICParser.cpp

clean:
	cd cpp_modules/zlib && $(MAKE) clean
	cd cpp_modules/openssl && $(MAKE) clean
