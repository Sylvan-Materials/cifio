#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <iostream>
#include <sstream>
#include <regex>
#include <iterator>
#include <format>
#include <meta>

#define protected public

#include "reading/tcp/TCPReader.h"
#include "io/json/Binder.h"

TEST_SUITE("search"){

TEST_CASE("find pyrophosphate entries in rcsb"){
    std::string search_template = R"({{
  "query": {{
    "type": "terminal",
    "service": "full_text",
    "parameters": {{
      "value": "{}"
    }}
  }},
  "return_type": "{}"
}})";
    std::string search_request = std::format(std::dynamic_format(search_template), "pyrophosphate", "entry");
    std::string url = "https://search.rcsb.org/rcsbsearch/v2/query?json="+search_request+"";
    sylvanmats::reading::TCPReader tcpReader;
    if(getenv("SSL_CERT_FILEPATH")!=nullptr){
        tcpReader.setSSLCertificatePath(getenv("SSL_CERT_FILEPATH"));
    }
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
    jsonBinder(jpName, [&](const sylvanmats::io::json::JsonValue& v){
        //if(key.compare("identifier")==0){
        if (auto pVal = std::get_if<std::string_view>(&v)) {
            val.push_back(*pVal);
            std::cout<<"identifier"<<" "<<val.back()<<std::endl;
        }
        //}
    });
    CHECK_EQ(val.size(), 10);

    if(val.size()>0){
     for(auto id : val){
       std::string dataUri= "https://data.rcsb.org/rest/v1/core/entry/"+std::string(id);
        sylvanmats::reading::TCPReader tcpDataReader;
        CHECK(tcpDataReader(dataUri, [&id](std::istream& is){
            std::string content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
            CHECK(!content.empty());
            std::cout<<"data for "<<id<<" "<<content<<std::endl;
        }));
      }
    }
    }));
}

TEST_CASE("find crossref for chem"){
    std::string id = "10.1002/chem.202200942";
    try{
       std::string doiUri= "https://api.crossref.org/works/"+std::string(id);
       url::Url url(doiUri);
        sylvanmats::reading::TCPReader tcpDataReader;
        CHECK(tcpDataReader(doiUri, [&id](std::istream& is){
            std::string content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
            CHECK(!content.empty());
            std::cout<<"data for "<<id<<" "<<content<<std::endl;
            sylvanmats::io::json::Binder jsonBinder;
            jsonBinder(content);
            CHECK_EQ(graph::num_vertices(jsonBinder.dagGraph), 988);
            CHECK_EQ(graph::num_edges(jsonBinder.dagGraph), 987);
            for(auto&& [vid, v] : graph::views::vertexlist(jsonBinder.dagGraph)){
                auto& vv=graph::vertex_value(jsonBinder.dagGraph, v);
                if(vv.obj_type==sylvanmats::io::json::JSON_STRING){
                    std::string_view sv=substr_view(jsonBinder.jsonContent, vv.start, vv.end);
                    std::cout<<"string literal "<<sv<<std::endl;
                }
            }
            sylvanmats::io::json::Path jpTitle="/message/title/*"_jp;
            std::vector<std::string_view> valTitle{};
            jsonBinder(jpTitle, [&](const sylvanmats::io::json::JsonValue& v){
                if (auto pVal = std::get_if<std::string_view>(&v)) {
                    valTitle.push_back(*pVal);
                    std::cout<<"title"<<" "<<valTitle.back()<<std::endl;
                    CHECK_EQ(valTitle.back(), "A New Class of N-Heterocyclic Carbenes: Stable, Highly Catalytically Active, and Structurally Tunable Mesoionic Carbenes");
                }
            });
            CHECK_EQ(valTitle.size(), 1);
    sylvanmats::io::json::Path jpRefCount="/message/reference-count"_jp;
//    jpRefCount["message"]["reference-count"];
    std::vector<int> valRefCount{};
    jsonBinder(jpRefCount, [&](const sylvanmats::io::json::JsonValue& v){
        if (auto pVal = std::get_if<int>(&v)) {
            valRefCount.push_back(*pVal);
            std::cout<<"reference-count"<<" "<<valRefCount.back()<<std::endl;
            CHECK_EQ(valRefCount.back(), 10);
        }
    });
    CHECK_EQ(valRefCount.size(), 1);
    sylvanmats::io::json::Path jpName;
    jpName["message"]["reference"]["*"]["*"]["DOI"];
    std::vector<std::string_view> val{};
    jsonBinder(jpName, [&](const sylvanmats::io::json::JsonValue& v){
        //if(key.compare("identifier")==0){
        if (auto pVal = std::get_if<std::string_view>(&v)) {
            val.push_back(*pVal);
            std::cout<<"DOI"<<" "<<val.back()<<std::endl;
        }
        //}
    });
    CHECK_EQ(val.size(), 10);

        }));
    }catch(const std::exception& e){
        std::cerr<<"Error fetching data for "<<id<<" "<<e.what()<<std::endl;
    }

}

}