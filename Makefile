
all: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26  -Iinclude  -Icpp_modules/fmt/dist/include -Icpp_modules/lemon/dist/include -Icpp_modules/brigand/include -Icpp_modules/multi_index/include  -Isrc -Icpp_modules/json-thresher/include -Icpp_modules/urlcpp -Icpp_modules/openssl/openssl/include -Icpp_modules/mio/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/usr/local/lib -MMD
all: LDFLAGS= -Wl,--allow-multiple-definition -shared  -fuse-ld=gold -Lcpp_modules/lemon/dist/lib -L$(LUA_HOME)/install/lib -Lcpp_modules/urlcpp -L./cpp_modules/antlr4/runtime/Cpp/run/lib -Lcpp_modules/zlib/dist/lib -Lcpp_modules/fmt/dist/lib -Lcpp_modules/openssl/openssl/lib -Lcpp_modules/openssl/openssl/lib64 -Lcpp_modules/json-thresher -Lcpp_modules/xml-thresher -llua -lantlr4-runtime -lemon -lurlcpp -lfmt -ljsonthresher -lxmlthresher -lz -ldl -pthread -lssl -lcrypto
all: build/src/PeriodicTable.o build/src/parsing/CIF2Lexer.o build/src/parsing/CIF2Parser.o build/src/parsing/CIFLexer.o build/src/parsing/CIFParser.o build/src/parsing/DICLexer.o build/src/parsing/DICParser.o build/src/parsing/MOL2Lexer.o build/src/parsing/MOL2Parser.o build/src/parsing/SMIRKSLexer.o build/src/parsing/SMIRKSParser.o build/src/constitution/Graph.o build/src/dynamics/Particles.o build/src/constitution/Comparator.o build/src/caching/FileSteward.o build/src/reading/smirnoff/SMIRKSPatterns.o build/src/forcefield/OpenFF.o build/src/constitution/Populator.o build/src/constitution/MOL2Populator.o build/src/constitution/GraphmlPopulator.o build/src/density/Populator.o build/src/standards/AminoStandards.o build/src/standards/ComponentStandards.o build/src/symmetry/LatticeSites.o build/src/lattice/Populator.o build/src/reading/tcp/TCPReader.o build/src/publishing/st/Publisher.o build/src/publishing/st/CIFPublisher.o build/src/publishing/st/OBJPublisher.o build/src/publishing/st/TIKZPublisher.o build/src/publishing/st/SVGPublisher.o build/src/publishing/st/MOL2Publisher.o build/src/publishing/jgf/JGFPublisher.o build/src/publishing/gz/CIFCompressor.o build/src/publishing/smiles/SMILESPublisher.o $(if $(findstring $(mode),extra), build/src/guile/guile-cifio.o) /home/roger/sylvanmats/lua-5.4.3/install/lib/liblua.a
	$(CXX) $(LDFLAGS) -o libcifio.so $(wildcard build/src/*.o) $(wildcard build/src/parsing/*.o) $(wildcard build/src/standards/*.o) $(wildcard build/src/constitution/*.o) $(wildcard build/src/caching/*.o) $(wildcard build/src/dynamics/*.o) $(wildcard build/src/forcefield/*.o) $(wildcard build/src/density/*.o) $(wildcard build/src/lattice/*.o) $(wildcard build/src/symmetry/*.o) $(wildcard build/src/reading/tcp/*.o) $(wildcard build/src/reading/smirnoff/*.o) $(wildcard build/src/publishing/jgf/*.o) $(wildcard build/src/publishing/gz/*.o) $(wildcard build/src/publishing/smiles/*.o) $(wildcard build/src/publishing/st/*.o) $(wildcard build/src/utils/*.o) 

build/src/PeriodicTable.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/fmt/include -Icpp_modules/graph-v2/include -Icpp_modules/json-thresher/include -Icpp_modules/brigand/include -Icpp_modules/multi_index/include -MMD
build/src/PeriodicTable.o: src/PeriodicTable.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/PeriodicTable.o src/PeriodicTable.cpp

build/src/standards/AminoStandards.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/brigand/include -Icpp_modules/multi_index/include -Icpp_modules/fmt/include -Icpp_modules/json-thresher/include -Icpp_modules/mio/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/standards/AminoStandards.o: src/standards/AminoStandards.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/standards/AminoStandards.o src/standards/AminoStandards.cpp
 
build/src/standards/ComponentStandards.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/brigand/include -Icpp_modules/multi_index/include -Icpp_modules/fmt/include -Icpp_modules/graph-v2/include -Icpp_modules/json-thresher/include -Icpp_modules/mio/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/standards/ComponentStandards.o: src/standards/ComponentStandards.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/standards/ComponentStandards.o src/standards/ComponentStandards.cpp
 
build/src/constitution/Populator.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26  -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/brigand/include -Icpp_modules/multi_index/include -Icpp_modules/fmt/include -Icpp_modules/graph-v2/include -Icpp_modules/json-thresher/include -Icpp_modules/mio/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/constitution/Populator.o: src/constitution/Populator.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/constitution/Populator.o src/constitution/Populator.cpp
 
build/src/constitution/MOL2Populator.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26  -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/brigand/include -Icpp_modules/multi_index/include -Icpp_modules/fmt/include -Icpp_modules/graph-v2/include -Icpp_modules/json-thresher/include -Icpp_modules/mio/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/constitution/MOL2Populator.o: src/constitution/MOL2Populator.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/constitution/MOL2Populator.o src/constitution/MOL2Populator.cpp
 
build/src/constitution/GraphmlPopulator.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26  -Iinclude -Isrc -Icpp_modules/xml-thresher/include -Icpp_modules/xml-thresher/src -Icpp_modules/fmt/dist/include -Icpp_modules/lemon/dist/include -Icpp_modules/brigand/include -Icpp_modules/multi_index/include -Icpp_modules/fmt/include -Icpp_modules/graph-v2/include -Icpp_modules/json-thresher/include -Icpp_modules/mio/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/constitution/GraphmlPopulator.o: src/constitution/GraphmlPopulator.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/constitution/GraphmlPopulator.o src/constitution/GraphmlPopulator.cpp
 
build/src/density/Populator.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/density/Populator.o: src/density/Populator.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/density/Populator.o src/density/Populator.cpp

build/src/lattice/Populator.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/lattice/Populator.o: src/lattice/Populator.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/lattice/Populator.o src/lattice/Populator.cpp

build/src/symmetry/LatticeSites.o: CXXFLAGS= -DNDEBUG -O3  -fPIC -pthread -fopenmp -std=c++26  -Iinclude -I../include -I$(LUA_HOME)/src -MMD
build/src/symmetry/LatticeSites.o: src/symmetry/LatticeSites.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/symmetry/LatticeSites.o src/symmetry/LatticeSites.cpp

build/src/publishing/jgf/JGFPublisher.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/publishing/jgf/JGFPublisher.o: src/publishing/jgf/JGFPublisher.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/jgf/JGFPublisher.o src/publishing/jgf/JGFPublisher.cpp

build/src/publishing/gz/CIFCompressor.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/publishing/gz/CIFCompressor.o: src/publishing/gz/CIFCompressor.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/gz/CIFCompressor.o src/publishing/gz/CIFCompressor.cpp

build/src/publishing/smiles/SMILESPublisher.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD -Wl,--allow-multiple-definition
build/src/publishing/smiles/SMILESPublisher.o: src/publishing/smiles/SMILESPublisher.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/smiles/SMILESPublisher.o src/publishing/smiles/SMILESPublisher.cpp

build/src/publishing/st/Publisher.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/fmt/dist/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/publishing/st/Publisher.o: src/publishing/st/Publisher.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/st/Publisher.o src/publishing/st/Publisher.cpp

build/src/publishing/st/CIFPublisher.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/fmt/dist/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/publishing/st/CIFPublisher.o: src/publishing/st/CIFPublisher.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/st/CIFPublisher.o src/publishing/st/CIFPublisher.cpp

build/src/publishing/st/OBJPublisher.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/fmt/dist/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/publishing/st/OBJPublisher.o: src/publishing/st/OBJPublisher.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/st/OBJPublisher.o src/publishing/st/OBJPublisher.cpp

build/src/publishing/st/TIKZPublisher.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/fmt/dist/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/publishing/st/TIKZPublisher.o: src/publishing/st/TIKZPublisher.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/st/TIKZPublisher.o src/publishing/st/TIKZPublisher.cpp

build/src/publishing/st/SVGPublisher.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/fmt/dist/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/publishing/st/SVGPublisher.o: src/publishing/st/SVGPublisher.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/st/SVGPublisher.o src/publishing/st/SVGPublisher.cpp

build/src/publishing/st/MOL2Publisher.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/fmt/dist/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/publishing/st/MOL2Publisher.o: src/publishing/st/MOL2Publisher.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/st/MOL2Publisher.o src/publishing/st/MOL2Publisher.cpp

build/src/reading/tcp/TCPReader.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/lemon/dist/include -Icpp_modules/urlcpp -Icpp_modules/openssl/openssl/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/reading/tcp/TCPReader.o: src/reading/tcp/TCPReader.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/reading/tcp/TCPReader.o src/reading/tcp/TCPReader.cpp

build/src/constitution/Graph.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26  -Iinclude -Icpp_modules/lemon/dist/include -MMD
build/src/constitution/Graph.o: src/constitution/Graph.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/constitution/Graph.o src/constitution/Graph.cpp

build/src/dynamics/Particles.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26  -Iinclude -Icpp_modules/lemon/dist/include -MMD  -Wl,--allow-multiple-definition
build/src/dynamics/Particles.o: src/dynamics/Particles.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/dynamics/Particles.o src/dynamics/Particles.cpp

build/src/reading/smirnoff/SMIRKSPatterns.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26  -Iinclude -Isrc -Icpp_modules/xml-thresher/include -Icpp_modules/xml-thresher/src -Icpp_modules/fmt/dist/include -Icpp_modules/lemon/dist/include -Icpp_modules/brigand/include -Icpp_modules/multi_index/include -Icpp_modules/fmt/include -Icpp_modules/graph-v2/include -Icpp_modules/json-thresher/include -Icpp_modules/mio/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/reading/smirnoff/SMIRKSPatterns.o: src/reading/smirnoff/SMIRKSPatterns.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/reading/smirnoff/SMIRKSPatterns.o src/reading/smirnoff/SMIRKSPatterns.cpp

build/src/forcefield/OpenFF.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26  -Iinclude -Isrc -Icpp_modules/xml-thresher/include -Icpp_modules/fmt/dist/include -Icpp_modules/lemon/dist/include -Icpp_modules/brigand/include -Icpp_modules/multi_index/include -Icpp_modules/fmt/include -Icpp_modules/graph-v2/include -Icpp_modules/json-thresher/include -Icpp_modules/mio/include -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/forcefield/OpenFF.o: src/forcefield/OpenFF.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/forcefield/OpenFF.o src/forcefield/OpenFF.cpp

build/src/constitution/Comparator.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26  -Iinclude -Icpp_modules/lemon/dist/include -MMD
build/src/constitution/Comparator.o: src/constitution/Comparator.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/constitution/Comparator.o src/constitution/Comparator.cpp

build/src/caching/FileSteward.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26  -Iinclude -Icpp_modules/lemon/dist/include -MMD
build/src/caching/FileSteward.o: src/caching/FileSteward.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/caching/FileSteward.o src/caching/FileSteward.cpp

build/src/parsing/CIFLexer.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/parsing/CIFLexer.o: src/parsing/CIFLexer.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/CIFLexer.o src/parsing/CIFLexer.cpp

build/src/parsing/CIFParser.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/parsing/CIFParser.o: src/parsing/CIFParser.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/CIFParser.o src/parsing/CIFParser.cpp

build/src/parsing/CIF2Lexer.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/parsing/CIF2Lexer.o: src/parsing/CIF2Lexer.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/CIF2Lexer.o src/parsing/CIF2Lexer.cpp

build/src/parsing/CIF2Parser.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/parsing/CIF2Parser.o: src/parsing/CIF2Parser.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/CIF2Parser.o src/parsing/CIF2Parser.cpp

build/src/parsing/DICLexer.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/parsing/DICLexer.o: src/parsing/DICLexer.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/DICLexer.o src/parsing/DICLexer.cpp

build/src/parsing/DICParser.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/parsing/DICParser.o: src/parsing/DICParser.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/DICParser.o src/parsing/DICParser.cpp

build/src/parsing/MOL2Lexer.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/parsing/MOL2Lexer.o: src/parsing/MOL2Lexer.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/MOL2Lexer.o src/parsing/MOL2Lexer.cpp

build/src/parsing/MOL2Parser.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/parsing/MOL2Parser.o: src/parsing/MOL2Parser.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/MOL2Parser.o src/parsing/MOL2Parser.cpp

build/src/parsing/SMIRKSLexer.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/parsing/SMIRKSLexer.o: src/parsing/SMIRKSLexer.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/SMIRKSLexer.o src/parsing/SMIRKSLexer.cpp

build/src/parsing/SMIRKSParser.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -fopenmp -std=c++26 -Iinclude -Isrc -Icpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/parsing/SMIRKSParser.o: src/parsing/SMIRKSParser.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/SMIRKSParser.o src/parsing/SMIRKSParser.cpp

build/src/guile/guile-cifio.o: LDFLAGS= -L. -lcifio
build/src/guile/guile-cifio.o: CXXFLAGS= `pkg-config --cflags guile-3.0` -DNDEBUG -O3 -fPIC -fopenmp -std=c++26 -Iinclude -Icpp_modules/lemon/dist/include
build/src/guile/guile-cifio.o: src/guile/guile-cifio.c
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c  -o build/src/guile/guile-cifio.o src/guile/guile-cifio.c  -MMD
	$(CXX) $(LDFLAGS) -shared -o libguile-cifio.so build/src/guile/guile-cifio.o

clean:
	rm -rf build/src
	#cd cpp_modules/zlib && $(MAKE) clean
	#cd cpp_modules/openssl && $(MAKE) clean
