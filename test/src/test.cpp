
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

#include "standards/ComponentStandards.h"
#include "standards/AminoStandards.h"
#include "constitution/Graph.h"
#include "constitution/Populator.h"
#include "constitution/Selection.h"
#include "density/Populator.h"
#include "lattice/Populator.h"
#include "publishing/jgf/JGFPublisher.h"

TEST_CASE("test tcp for a cif.gz"){
    std::string comp_id="3sgs";
    std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
    sylvanmats::reading::TCPReader tcpReader;
    tcpReader(url, [&comp_id](std::istream& is){
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
    CHECK(tcpReader(url, [&](std::istream& is){
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
        CHECK(tcpReader(url, [&](std::istream& is){
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
//        CHECK(tcpReader(hklUrl, [&](std::istream& is){
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
    CHECK(tcpReader(url, [&comp_id](std::istream& is){
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
CHECK_EQ(aminoStandards.getNumberOfEntries(), 218);
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
    
       /*for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
            std::cout<<" "<<graph.atomSites[nSiteA].auth_atom_id<<" "<<graph.atomSites[nSiteA].type_symbol<<" "<<graph.atomSites[nSiteA].proton_count<<std::endl;
        }*/
}

TEST_CASE("test 1ebc") {
    std::filesystem::path filePath="./examples/1ebc.cif.gz";
   
    sylvanmats::constitution::Graph graph;
    
    sylvanmats::constitution::Populator populator;
    populator(filePath, graph, [&filePath](sylvanmats::constitution::Graph& graph){
   CHECK_EQ(graph.getNumberOfAtomSites(), 1332);
   CHECK_EQ(graph.getCell().length_a, 64.87);
   CHECK_EQ(graph.getCell().length_b, 31.0);
   CHECK_EQ(graph.getCell().length_c, 35.29);
   CHECK_EQ(graph.getCell().angle_alpha, 90.000);
   CHECK_EQ(graph.getCell().angle_beta, 105.63);
   CHECK_EQ(graph.getCell().angle_gamma, 90.000);
   CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 1 21 1");
   CHECK_EQ(graph.getSymmetry().Int_Tables_number, 4);
   
   });
   CHECK_EQ(graph.getNumberOfAtomSites(), 1332);
   CHECK_EQ(lemon::countEdges(graph), 1298);
   CHECK_EQ(lemon::countNodes(graph.componentGraph), 222);  
   CHECK_EQ(lemon::countEdges(graph.componentGraph), 150);
    sylvanmats::constitution::Selection selection(graph);
    selection(154, [&](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph){
        graph.identifyFusedSystems(selectionGraph, [&graph, &filePath](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> subSelectionGraph){
            CHECK_EQ(lemon::countNodes(subSelectionGraph), 24);
            CHECK_EQ(lemon::countEdges(subSelectionGraph), 28);
            graph.identifyRings(subSelectionGraph);
            sylvanmats::publishing::JGFPublisher jgfPublisher(graph, subSelectionGraph);   
            filePath.replace_extension(".json");
            std::ofstream ofs(filePath);
            ofs<<" "<<jgfPublisher<<std::endl;
            ofs.close();
        });
    });
    
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
        std::regex r(R"(\n?data_(\S*))");

        nlohmann::json j;
        std::string previousDataId="";
        for(std::sregex_iterator it=std::sregex_iterator(content.begin(), content.end(), r);it!=std::sregex_iterator();++it)
        {
            //std::cout << sm.prefix().first << '\n';
            if((*it).size()>1){
                std::string s = (*it)[0];
                unsigned long p=(s.starts_with('\n')) ? (*it).position()+1 : (*it).position();
                j[(*it)[1]]["start"]=p;
                if(!previousDataId.empty())j[previousDataId]["end"]=p-1;
                previousDataId=(*it)[1];
            }
        }
        path.replace_extension(".json");
        std::ofstream ostrm(path, std::ios::trunc);
        ostrm<<j<<std::endl;
        ostrm.close();

        std::string current_comp_id="HEM";
        sylvanmats::standards::ComponentStandards componentStandards;
        bool ret=componentStandards(current_comp_id, [&](sylvanmats::standards::chem_comp_atom<double>& cca1, sylvanmats::standards::chem_comp_bond& ccb, sylvanmats::standards::chem_comp_atom<double>& cca2){
            std::cout<<" "<<ccb.atom_id_1<<" "<<ccb.atom_id_2<<std::endl;
        });
        CHECK(ret);

    }
    
}

