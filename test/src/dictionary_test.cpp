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
#include "nlohmann/json.hpp"

#include "antlr4-runtime.h"
#include "parsing/DICLexer.h"
#include "parsing/DICParser.h"


TEST_CASE("test dictionary parsing") {
    std::ifstream file("../dictionaries/cif_mm.dic");
    std::stringstream content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    antlr4::ANTLRInputStream input(content);
    sylvanmats::DICLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);

    sylvanmats::DICParser parser(&tokens);
    parser.setBuildParseTree(true);
    antlr4::tree::ParseTree* tree = parser.dic();
    

}


