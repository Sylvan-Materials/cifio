#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <iterator>
#include <utility>
#include <charconv>

#include "zlib.h"
#include "mio/mmap.hpp"
#include "io/json/Binder.h"

#include "antlr4-runtime.h"
#include "parsing/DICLexer.h"
#include "parsing/DICParser.h"
#include "primeval/MMCIFDictionary.h"

TEST_SUITE("dictionary"){

TEST_CASE("test dictionary parsing") {
    std::ifstream file("../dictionaries/cif_mm.dic");
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::stringstream sscontent(content);

    antlr4::ANTLRInputStream input(sscontent);
    sylvanmats::DICLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);

    /*sylvanmats::DICParser parser(&tokens);
    parser.setBuildParseTree(true);
    antlr4::tree::ParseTree* tree = parser.dic();*/
    

}

TEST_CASE("Compile time the mmcif dictionary"){
    static constexpr char dicBuffer[] ={
#embed "cif_mm.dic"
    };

    sylvanmats::primeval::MMCIFDictionary<sizeof(dicBuffer)> mmCIFDictionary{};
    //static constexpr auto gbnf=
    static constexpr auto astGraph=mmCIFDictionary(std::string_view(dicBuffer, sizeof(dicBuffer)));
    std::cout<<"v "<<std::get<0>(astGraph)<<" e "<<std::get<1>(astGraph)<<" "<<std::endl;
    CHECK_EQ(std::get<0>(astGraph), 100);
    CHECK_EQ(std::get<1>(astGraph), 101);
}

}
