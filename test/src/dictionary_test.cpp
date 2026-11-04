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
#include "constitution/Populator.h"
#include "primeval/MMCIFDictionary.h"

// Preparing today for tomorrow's injection
template <size_t N>
struct struct_holder {
    // CURRENT C++26 WAY: Array of variants mapped by static schema indices
    std::array<std::variant<int, double, std::string_view, std::unique_ptr<struct_holder>>, N> values;

    // TOMORROW'S C++29 WAY (When P3138/P3289 lands in trunk):
    /*
    consteval {
        for (auto const& name : parse_atom_site_keys()) {
            // Pseudo-code representation of structural injection:
            inject_member(name, type_from_cif(name));
        }
    }
    */

    // Clean extraction interface ensuring code outside graph-v3 doesn't care
    template <cairns::fixed_string FieldName, auto SchemaPtr>
    constexpr decltype(auto) get() const {
        // Today: lookup index in the static descriptor pointer
        constexpr size_t idx = SchemaPtr->index_of(FieldName).value();
        return std::get<0>(values[idx]); // replace with actual variant type match
        
        // Tomorrow: return this.[: std::meta::find_member(FieldName) :];
    }
};

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
    REQUIRE(sylvanmats::constitution::globalSchema != nullptr);

    // sylvanmats::constitution::Populator populator;
    // sylvanmats::primeval::MMCIFDictionary<sizeof(dicBuffer)> mmCIFDictionary{};
    //static constexpr auto gbnf=
    // static constexpr auto astGraph=populator.bake(std::string_view(dicBuffer, sizeof(dicBuffer)));
    // std::cout<<"v "<<std::get<0>(astGraph).size()<<" e "<<std::get<1>(astGraph).size()<<std::endl;
    // CHECK_EQ(std::get<0>(astGraph).size(), 100);
    // CHECK_EQ(std::get<1>(astGraph).size(), 101);

    CHECK_EQ(std::get<0>(*sylvanmats::constitution::globalSchema).size(), 86007);
    CHECK_EQ(std::get<1>(*sylvanmats::constitution::globalSchema).size(), 86006);
    // 1. Get a direct reference to your compile-time stacks
    const auto& vertices = std::get<0>(*sylvanmats::constitution::globalSchema);
    
    // 2. Loop through the captured vertices to verify boundaries and text slices
    for (size_t i = 0; i < vertices.size(); i++) {
        const auto& node = std::get<1>(vertices[i]);
        
        // Reconstitute the string_view window using the primitive numeric offsets
        // Note: You will need to expose dicBuffer or its pointer to your test unit
        std::string_view token_text(&sylvanmats::constitution::dicBuffer[node.token_position], node.token_length);
        std::string_view value_text(&sylvanmats::constitution::dicBuffer[node.value_position], node.value_length);
        
        // Linear, single-line telemetry per entry
        std::cout << "[V:" << i << "] parent: " << node.parent << " Depth: " << node.depth
                << " | ColIdx: " << node.column_index 
                << " | Token: [" << token_text << "] "
                << " | Value: [" << value_text << "]\n";
        
        // Example assertion: Ensure the tokens aren't empty or leaping out of bounds
        if (i>0 && node.value_length > 0) {
            CHECK(node.value_position >= node.token_position);
        }
    }
    


}

}
