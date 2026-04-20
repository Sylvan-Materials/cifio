#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <iostream>
#include <sstream>
#include <regex>
#include <iterator>

#define protected public

#include "reading/tcp/TCPReader.h"
#include "io/json/Binder.h"

TEST_SUITE("search"){

TEST_CASE("find pyrophosphate entries in rcsb"){
    std::string search_request = R"({
  "query": {
    "type": "terminal",
    "service": "full_text",
    "parameters": {
      "value": "pyrophosphatase"
    }
  },
  "return_type": "entry"
})";
    std::string url = "https://search.rcsb.org/rcsbsearch/v2/query?json="+search_request+"";
    sylvanmats::reading::TCPReader tcpReader;
    CHECK(tcpReader(url, [&search_request](std::istream& is){
        std::string content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        CHECK(!content.empty());
      sylvanmats::io::json::Binder jsonBinder;
      jsonBinder(content);
      CHECK_EQ(graph::num_vertices(jsonBinder.dagGraph), 71);
      CHECK_EQ(graph::num_edges(jsonBinder.dagGraph), 70);
    sylvanmats::io::json::Path jpName;
    jpName["result_set"]["*"]["*"]["identifier"];
    std::vector<std::string_view> val{};
    jsonBinder(jpName, [&](std::any& v){
        //if(key.compare("identifier")==0){
                   val.push_back(std::any_cast<std::string_view>(v));
                   std::cout<<"identifier"<<" "<<val.back()<<std::endl;
        //}
    });
    CHECK_EQ(val.size(), 10);


    }));
}

}