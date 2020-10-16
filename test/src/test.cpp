
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <iterator>

#include "zlib.h"
#include "mio/mmap.hpp"
#include "nlohmann/json.hpp"

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"
#include "reading/gz/GZReader.h"
#include "reading/tcp/TCPReader.h"

#include "standards/AminoStandards.h"
#include "constitution/Graph.h"
#include "constitution/Populator.h"
#include "density/Populator.h"
#include "lattice/Populator.h"
#include "publishing/jgf/JGFPublisher.h"

TEST_CASE("test tcp for a cif.gz"){
    std::string comp_id="3sgs";
    std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
    sylvanmats::reading::TCPReader tcpReader;
    tcpReader("128.6.244.95", url, [&comp_id](std::istream& is){
        std::string content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        CHECK_EQ(content.size(), 26817);
        std::string path="./examples/"+comp_id+".cif.gz";
        gzFile file=gzopen(path.c_str(), "wb");
        int uncomprLen=1024;
        char buf[uncomprLen+1];
        std::memset (buf, 0, uncomprLen+1);
        bool eofHit=false;
        unsigned long long count=0;
        while(!eofHit && uncomprLen>0){
            std::memset (buf, 0, uncomprLen+1);
            std:strncpy(buf, &content[count], uncomprLen);
            buf[uncomprLen+1]=0;
            int ret=gzputs(file, (char*)buf);
            if(ret<=0)eofHit=true;
            else count+=ret;
            if(uncomprLen>content.size()-count)uncomprLen=content.size()-count;
            //std::cout<<ret<<" "<<eofHit<<" "<<count<<" "<<content.size()<<" "<<uncomprLen<<std::endl;
        }
//        count++;
//        gzseek(file, 1L, SEEK_CUR);
        CHECK_EQ(count, 26817);
        gzclose(file);
        
    });

}

TEST_CASE("test tcp for COD hydroxyapatite"){
    std::string url = "http://www.crystallography.net/cod/result*?text=hydroxyapatite&has_fobs&format=urls&el1=Ca&nel1=Na&nel2=C";
    sylvanmats::reading::TCPReader tcpReader;
    CHECK(tcpReader("193.219.81.210", url, [&](std::istream& is){
        std::string content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        std::vector<std::string> urls;
        std::string delim="\n";
        char *token = strtok(const_cast<char*>(content.c_str()), delim.c_str());
        while (token != nullptr)
        {
                urls.push_back(std::string(token));
                token = strtok(nullptr, delim.c_str());
        }
        CHECK_EQ(urls.size(), 28);
        url=urls[5];
        std::string hklUrl=url.substr(0, url.size()-4)+".hkl";
        CHECK(tcpReader("193.219.81.210", url, [&](std::istream& is){
            std::string cifContent((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
            CHECK_EQ(cifContent.size(), 4766);
            std::string path="./examples/hydroxyapatite.cif.gz";
            gzFile file=gzopen(path.c_str(), "wb");
            int uncomprLen=1024;
            char buf[uncomprLen+1];
            std::memset (buf, 0, uncomprLen+1);
            bool eofHit=false;
            unsigned long long count=0;
            while(!eofHit && uncomprLen>0){
                std::memset (buf, 0, uncomprLen+1);
                std:strncpy(buf, &cifContent[count], uncomprLen);
                buf[uncomprLen+1]=0;
                int ret=gzputs(file, (char*)buf);
                if(ret<=0)eofHit=true;
                else count+=ret;
                if(uncomprLen>cifContent.size()-count)uncomprLen=cifContent.size()-count;
                //std::cout<<ret<<" "<<eofHit<<" "<<count<<" "<<cifContent.size()<<" "<<uncomprLen<<std::endl;
            }
    //        count++;
    //        gzseek(file, 1L, SEEK_CUR);
            CHECK_EQ(count, 4766);
            gzclose(file);

        }));
//        CHECK(tcpReader("193.219.81.210", hklUrl, [&](std::istream& is){
//            std::string cifContent((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
//            std::cout<<"cifContent: "<<cifContent<<std::endl;
//            CHECK_EQ(cifContent.size(), 4766);
//            std::string path="./examples/hydroxyapatite.hkl.cif.gz";
//            gzFile file=gzopen(path.c_str(), "wb");
//            int uncomprLen=1024;
//            char buf[uncomprLen+1];
//            std::memset (buf, 0, uncomprLen+1);
//            bool eofHit=false;
//            unsigned long long count=0;
//            while(!eofHit && uncomprLen>0){
//                std::memset (buf, 0, uncomprLen+1);
//                std:strncpy(buf, &cifContent[count], uncomprLen);
//                buf[uncomprLen+1]=0;
//                int ret=gzputs(file, (char*)buf);
//                if(ret<=0)eofHit=true;
//                else count+=ret;
//                if(uncomprLen>cifContent.size()-count)uncomprLen=cifContent.size()-count;
//                //std::cout<<ret<<" "<<eofHit<<" "<<count<<" "<<cifContent.size()<<" "<<uncomprLen<<std::endl;
//            }
//    //        count++;
//    //        gzseek(file, 1L, SEEK_CUR);
//            CHECK_EQ(count, 4766);
//            gzclose(file);
//
//        }));
//        
    }));

}

TEST_CASE("test retrieving structure factors for 3sgs"){
    std::string comp_id="3sgs";
    std::string url = "https://files.rcsb.org/download/"+comp_id+"-sf.cif";
    sylvanmats::reading::TCPReader tcpReader;
    CHECK(tcpReader("128.6.244.95", url, [&comp_id](std::istream& is){
        std::string content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        CHECK_EQ(content.size(), 30174);
        std::string path="./examples/"+comp_id+"-sf.cif.gz";
        gzFile file=gzopen(path.c_str(), "wb");
        int uncomprLen=1024;
        char buf[uncomprLen+1];
        std::memset (buf, 0, uncomprLen+1);
        bool eofHit=false;
        unsigned long long count=0;
        while(!eofHit && uncomprLen>0){
            std::memset (buf, 0, uncomprLen+1);
            std:strncpy(buf, &content[count], uncomprLen);
            buf[uncomprLen+1]=0;
            int ret=gzputs(file, (char*)buf);
            if(ret<=0)eofHit=true;
            else count+=ret;
            if(uncomprLen>content.size()-count)uncomprLen=content.size()-count;
            //std::cout<<ret<<" "<<eofHit<<" "<<count<<" "<<content.size()<<" "<<uncomprLen<<std::endl;
        }
//        count++;
//        gzseek(file, 1L, SEEK_CUR);
        CHECK_EQ(count, 30174);
        gzclose(file);
        
    }));

}

TEST_CASE("test decompression of aa-variants-v1.cif.gz") {
    std::vector<sylvanmats::standards::chem_comp_bond> standardChemCompBond;
    standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="N", .atom_id_2="CA", .value_order=1});
    standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="N", .atom_id_2="H", .value_order=1});
    standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="N", .atom_id_2="HN2", .value_order=1});
    standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="CA", .atom_id_2="C", .value_order=1});
    standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="CA", .atom_id_2="CB", .value_order=1});
    standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="CA", .atom_id_2="HA", .value_order=1});
    standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="C", .atom_id_2="O", .value_order=2});
    standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="C", .atom_id_2="OXT", .value_order=1});
    standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="CB", .atom_id_2="HB1", .value_order=1});
    standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="CB", .atom_id_2="HB2", .value_order=1});
    standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="CB", .atom_id_2="HB3", .value_order=1});
    standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="OXT", .atom_id_2="HXT", .value_order=1});
 
    sylvanmats::standards::AminoStandards aminoStandards;
   std::string comp_id="ALA";
   unsigned int counter=0;
   CHECK(aminoStandards(comp_id, [&](sylvanmats::standards::chem_comp_atom<double>& cca1, sylvanmats::standards::chem_comp_bond ccb, sylvanmats::standards::chem_comp_atom<double>& cca2){
       CHECK_EQ(standardChemCompBond[counter].comp_id, ccb.comp_id);
       CHECK_EQ(standardChemCompBond[counter].atom_id_1, ccb.atom_id_1);
       CHECK_EQ(standardChemCompBond[counter].atom_id_2, ccb.atom_id_2);
       CHECK_EQ(standardChemCompBond[counter++].value_order, ccb.value_order);
   }));

   comp_id="UNK";
   counter=0;
   REQUIRE_FALSE(aminoStandards(comp_id, [&](sylvanmats::standards::chem_comp_atom<double>& cca1, sylvanmats::standards::chem_comp_bond ccb, sylvanmats::standards::chem_comp_atom<double>& cca2){
       std::cout<<"unk? "<<ccb.comp_id<<" "<<ccb.atom_id_1<<" "<<ccb.atom_id_2<<" "<<ccb.value_order<<" "<<std::endl;
   }));
   
}

TEST_CASE("test 3sgs") {
    std::filesystem::path  filePath="./examples/3sgs.cif.gz";
   
    sylvanmats::constitution::Graph graph;
    
    sylvanmats::constitution::Populator populator;
    populator(filePath, graph, [&filePath](sylvanmats::constitution::Graph& graph){
    sylvanmats::publishing::JGFPublisher jgfPublisher(graph);   
   CHECK_EQ(graph.getNumberOfAtomSites(), 46);
   CHECK_EQ(graph.getCell().length_a, 4.821);
   CHECK_EQ(graph.getCell().length_b, 19.500);
   CHECK_EQ(graph.getCell().length_c, 21.004);
   CHECK_EQ(graph.getCell().angle_alpha, 90.000);
   CHECK_EQ(graph.getCell().angle_beta, 94.230);
   CHECK_EQ(graph.getCell().angle_gamma, 90.000);
   CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 1 21 1");
   CHECK_EQ(graph.getSymmetry().Int_Tables_number, 4);
    filePath.replace_extension(".json");
    std::ofstream ofs(filePath);
    ofs<<" "<<jgfPublisher<<std::endl;
    ofs.close();
   
   });
   CHECK_EQ(graph.getNumberOfAtomSites(), 46);
   CHECK_EQ(lemon::countEdges(graph), 43);
   CHECK_EQ(lemon::countNodes(graph.componentGraph), 8);
   CHECK_EQ(lemon::countEdges(graph.componentGraph), 4);
    
       for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
            std::cout<<" "<<graph.atomSites[nSiteA].auth_atom_id<<" "<<graph.atomSites[nSiteA].type_symbol<<" "<<graph.atomSites[nSiteA].proton_count<<std::endl;
        }
}

TEST_CASE("test 1ebc") {
    std::filesystem::path filePath="./examples/1ebc.cif.gz";
   
    sylvanmats::constitution::Graph graph;
    
    sylvanmats::constitution::Populator populator;
    populator(filePath, graph, [&filePath](sylvanmats::constitution::Graph& graph){
    sylvanmats::publishing::JGFPublisher jgfPublisher(graph);   
   CHECK_EQ(graph.getNumberOfAtomSites(), 1332);
   CHECK_EQ(graph.getCell().length_a, 64.87);
   CHECK_EQ(graph.getCell().length_b, 31.0);
   CHECK_EQ(graph.getCell().length_c, 35.29);
   CHECK_EQ(graph.getCell().angle_alpha, 90.000);
   CHECK_EQ(graph.getCell().angle_beta, 105.63);
   CHECK_EQ(graph.getCell().angle_gamma, 90.000);
   CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 1 21 1");
   CHECK_EQ(graph.getSymmetry().Int_Tables_number, 4);
    filePath.replace_extension(".json");
    std::ofstream ofs(filePath);
    ofs<<" "<<jgfPublisher<<std::endl;
    ofs.close();
   
   });
   CHECK_EQ(graph.getNumberOfAtomSites(), 1332);
   CHECK_EQ(lemon::countEdges(graph), 1244);
   CHECK_EQ(lemon::countNodes(graph.componentGraph), 222);  
   CHECK_EQ(lemon::countEdges(graph.componentGraph), 150);
    graph.identifyFusedSystems();
    
       /*for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
            std::cout<<" "<<graph.atomSites[nSiteA].auth_atom_id<<" "<<graph.atomSites[nSiteA].type_symbol<<" "<<graph.atomSites[nSiteA].proton_count<<std::endl;
       }*/
}

TEST_CASE("test 3sgs-sf") {
    std::filesystem::path filePath="./examples/3sgs-sf.cif.gz";
   
    sylvanmats::density::Populator populator;
    populator(filePath);//, graph, [](sylvanmats::constitution::Graph& graph){
        CHECK_EQ(populator.getStructureFactors().size(), 437);
  
   //});
}

TEST_CASE("test hydroxyapatite to lattice") {
    std::filesystem::path filePath="./examples/hydroxyapatite.cif.gz";
   
    sylvanmats::lattice::Graph graph;

    sylvanmats::lattice::Populator populator;
    populator(filePath, graph, [](sylvanmats::lattice::Graph& graph){
        CHECK_EQ(lemon::countNodes(graph), 7);
  
   });
}
    
TEST_CASE("test component db") {
    std::filesystem::path path="../db/components.cif";
    CHECK(std::filesystem::exists(path));
    if(std::filesystem::exists(path)){
        mio::mmap_source mmap(path.string());
        std::string content(mmap.begin(), mmap.end());
        mmap.unmap();
        std::regex r(R"(data_(\S*))");

        nlohmann::json j;
        std::string previousDataId="";
        for(std::sregex_iterator it=std::sregex_iterator(content.begin(), content.end(), r);it!=std::sregex_iterator();++it)
        {
            //std::cout << sm.prefix().first << '\n';
            if((*it).size()>1){
                j[(*it)[1]]["start"]=(*it).position();
                if(!previousDataId.empty())j[previousDataId]["end"]=(*it).position()-1;
                previousDataId=(*it)[1];
            }
        }
        path.replace_extension(".json");
        std::ofstream ostrm(path, std::ios::trunc);
        ostrm<<j<<std::endl;
        ostrm.close();

        std::ifstream file(path);
        std::string jsonContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        nlohmann::json jin=nlohmann::json::parse(jsonContent);
        file.close();

        unsigned int start=jin["/ZZY/start"_json_pointer];
        unsigned int end=jin["/ZZY/end"_json_pointer];
        CHECK_EQ(start, 312821721);
        CHECK_EQ(end, 312829779);
        path.replace_extension(".cif");
        mio::mmap_source mmap2nd(path.string(), start, end-start+1);
        content=std::string(mmap2nd.begin(), mmap2nd.end());
        mmap2nd.unmap();
        std::shared_ptr<antlr4::ANTLRInputStream> input=std::make_shared<antlr4::ANTLRInputStream>(content);
        std::shared_ptr<sylvanmats::CIFLexer> lexer=std::make_shared<sylvanmats::CIFLexer>(input.get());
        std::shared_ptr<antlr4::CommonTokenStream> tokens=std::make_shared<antlr4::CommonTokenStream>(lexer.get());

        std::shared_ptr<sylvanmats::CIFParser> parser=std::make_shared<sylvanmats::CIFParser>(tokens.get());
        //parser->setBuildParseTree(true);
        std::cout<<"parser "<<std::endl;
        antlr4::tree::ParseTree* tree = parser->cif();

        //std::cout << tree->toStringTree(&parser) << std::endl << std::endl;

        const std::string thePath="/cif/dataBlock";
        std::shared_ptr<antlr4::tree::xpath::XPath> xPath=std::make_shared<antlr4::tree::xpath::XPath>(parser.get(), thePath);
        std::vector<antlr4::tree::ParseTree*> dataBlock=xPath->evaluate(tree);

    }
    
}

