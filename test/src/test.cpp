
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <iterator>
#include <utility>
#include <charconv>
#include <array>
#include <typeinfo>
#include <cxxabi.h>

#include "zlib.h"
#include "mio/mmap.hpp"

#include "antlr4-runtime.h"
#include "reading/gz/GZReader.h"
#include "reading/tcp/TCPReader.h"

#include "standards/ComponentStandards.h"
#include "standards/AminoStandards.h"
#include "constitution/Graph.h"
#include "constitution/Populator.h"
#include "constitution/MOL2Populator.h"
#include "constitution/Selection.h"
#include "density/Populator.h"
#include "lattice/Populator.h"
#include "density/ccp4/MapInput.h"
#include "publishing/jgf/JGFPublisher.h"
#include "publishing/st/CIFPublisher.h"
#include "publishing/gz/CIFCompressor.h"
#include "PeriodicTable.h"

#include "io/json/Binder.h"

#include "lemon/vf2.h"

#include "utils/JVMSingleton.h"

TEST_SUITE("main"){


TEST_CASE("test jvm singleton") {

    sylvanmats::utils::JVMSingleton* jvmSingleton=sylvanmats::utils::JVMSingleton::getInstance();
    JNIEnv *jniEnv=jvmSingleton->getEnv();
    CHECK_NE(jniEnv, nullptr);
    jclass jcls = jniEnv->FindClass("org/stringtemplate/v4/ST");
    CHECK_NE(jcls, nullptr);
    if (jcls == nullptr) {
       jniEnv->ExceptionDescribe();
    }
    if (jcls != nullptr) {
       jmethodID constructorId = jniEnv->GetMethodID(jcls, "<init>", "(Ljava/lang/String;)V");
        CHECK_NE(constructorId, nullptr);
       if (constructorId == nullptr) {
          jniEnv->ExceptionDescribe();
       }
    }
    std::filesystem::path path="../templates/cif";
    sylvanmats::publishing::st::CIFPublisher cifPublisher(path);
    cifPublisher.add("entry_id", "3SGS");
    std::string&& content = cifPublisher.render();
    CHECK(!content.empty());
}

TEST_CASE("test periodic table") {
        sylvanmats::PeriodicTable* periodicTable=sylvanmats::PeriodicTable::getInstance();

        std::string h{"H"};
        sylvanmats::element ele=periodicTable->index(h);
        CHECK_EQ(ele.atomic_number, 1);
        CHECK_EQ(ele.name, "Hydrogen");
        CHECK_EQ(ele.symbol, h);
        std::string c{"C"};
        sylvanmats::element eleC=periodicTable->index(c);
        CHECK_EQ(eleC.atomic_number, 6);
        CHECK_EQ(eleC.name, "Carbon");
        CHECK_EQ(eleC.symbol, c);
}

TEST_CASE("test component db") {
    std::filesystem::path path="../db/components.cif";
    CHECK(std::filesystem::exists(path));
    if(std::filesystem::exists(path)){
        mio::mmap_source mmap(path.string());
        std::string content(mmap.begin(), mmap.end());
        mmap.unmap();
        std::regex r(R"(\n?data_(\S*))");

        sylvanmats::io::json::Binder jsonBinder;
        sylvanmats::io::json::Path jp;
        std::string previousDataId="";
        for(std::sregex_iterator it=std::sregex_iterator(content.begin(), content.end(), r);it!=std::sregex_iterator();++it)
        {
            //std::cout << sm.prefix().first << '\n';
            if((*it).size()>1){
                std::string s = (*it)[0];
                unsigned long p=(s.starts_with('\n')) ? (*it).position()+1 : (*it).position();
                jsonBinder(jp, std::string_view((*it)[1].str()), sylvanmats::io::json::object());
                sylvanmats::io::json::Path jpStart((*it)[1].str().c_str());
                jsonBinder(jpStart, "start",p);
                if(!previousDataId.empty()){
                    sylvanmats::io::json::Path jpEnd(previousDataId.c_str());
                    jsonBinder(jpEnd, "end",p-1);
                }
                previousDataId=(*it)[1];
            }
        }
        path.replace_extension(".json");
        std::ofstream ostrm(path, std::ios::trunc);
        ostrm<<jsonBinder<<std::endl;
        ostrm.close();

        std::vector<std::string> comp_ids;
        comp_ids.push_back("HEM");
        sylvanmats::standards::ComponentStandards componentStandards;
        bool ret=componentStandards(comp_ids, [&](sylvanmats::standards::chem_comp_atom<double>& cca1, sylvanmats::standards::chem_comp_bond& ccb, sylvanmats::standards::chem_comp_atom<double>& cca2){
            //std::cout<<" "<<ccb.atom_id_1<<" "<<ccb.atom_id_2<<std::endl;
        });
        CHECK(ret);

    }
    
}
    
TEST_CASE("test tcp for 3sgs.cif.gz"){
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

TEST_CASE("test tcp for 6yzq.cif.gz"){
    std::string comp_id="6yzq";
    std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
    sylvanmats::constitution::Graph graph;
    std::filesystem::path  filePath="./"+comp_id+".cif.gz";
    sylvanmats::reading::TCPReader tcpReader;
    tcpReader(url, [&graph, &filePath, &comp_id](std::istream& is){

        sylvanmats::constitution::Populator populator;
        populator(is, graph, [&filePath](sylvanmats::constitution::Graph& graph){
        sylvanmats::publishing::JGFPublisher jgfPublisher(graph);
       CHECK_EQ(graph.getNumberOfAtomSites(), 2487);
       CHECK_EQ(graph.getCell().length_a, 42.114);
       CHECK_EQ(graph.getCell().length_b, 41.394);
       CHECK_EQ(graph.getCell().length_c, 72.157);
       CHECK_EQ(graph.getCell().angle_alpha, 90.000);
       CHECK_EQ(graph.getCell().angle_beta, 104.43);
       CHECK_EQ(graph.getCell().angle_gamma, 90.000);
       CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 1 21 1");
       CHECK_EQ(graph.getSymmetry().Int_Tables_number, 4);
        filePath.replace_extension(".json");
        std::ofstream ofs(filePath);
        ofs<<" "<<jgfPublisher<<std::endl;
        ofs.close();

       });
       CHECK_EQ(graph.getNumberOfAtomSites(), 2487);
       CHECK_EQ(lemon::countEdges(graph), 2063);
       CHECK_EQ(lemon::countNodes(graph.componentGraph), 592);
       CHECK_EQ(lemon::countEdges(graph.componentGraph), 258);
       
    });

}

TEST_CASE("test tcp for 6lp0.cif.gz"){
    std::string comp_id="6lp0";
    std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
    sylvanmats::constitution::Graph graph;
    std::filesystem::path  filePath="./"+comp_id+".cif.gz";
    sylvanmats::reading::TCPReader tcpReader;
    tcpReader(url, [&graph, &filePath, &comp_id](std::istream& is){
    //std::filesystem::path  filePathTmp="~/Downloads/6lp0.cif";

        sylvanmats::constitution::Populator populator;
        populator(is, graph, [&filePath](sylvanmats::constitution::Graph& graph){
        sylvanmats::publishing::JGFPublisher jgfPublisher(graph);
       CHECK_EQ(graph.getNumberOfAtomSites(), 2171);
       CHECK_EQ(graph.getCell().length_a, 131.347);
       CHECK_EQ(graph.getCell().length_b, 131.347);
       CHECK_EQ(graph.getCell().length_c,  37.297);
       CHECK_EQ(graph.getCell().angle_alpha, 90.000);
       CHECK_EQ(graph.getCell().angle_beta, 90.0);
       CHECK_EQ(graph.getCell().angle_gamma, 120.0);
       CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "H 3");
       CHECK_EQ(graph.getSymmetry().Int_Tables_number, 146);
        filePath.replace_extension(".json");
        std::ofstream ofs(filePath);
        ofs<<" "<<jgfPublisher<<std::endl;
        ofs.close();

       });
       CHECK_EQ(graph.getNumberOfAtomSites(), 2171);
       CHECK_EQ(lemon::countEdges(graph), 1994);
       CHECK_EQ(lemon::countNodes(graph.componentGraph), 490);
       CHECK_EQ(lemon::countEdges(graph.componentGraph), 284);
       
    });

}

TEST_CASE("test tcp for 4k7t.cif.gz"){
    std::string comp_id="4k7t";
    std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
    sylvanmats::constitution::Graph graph;
    std::filesystem::path  filePath="./"+comp_id+".cif.gz";
    sylvanmats::reading::TCPReader tcpReader;
    tcpReader(url, [&graph, &filePath, &comp_id](std::istream& is){

        sylvanmats::constitution::Populator populator;
        populator(is, graph, [&filePath](sylvanmats::constitution::Graph& graph){
        sylvanmats::publishing::JGFPublisher jgfPublisher(graph);
       CHECK_EQ(graph.getNumberOfAtomSites(), 260);
       CHECK_EQ(graph.getCell().length_a, 35.98);
       CHECK_EQ(graph.getCell().length_b, 35.98);
       CHECK_EQ(graph.getCell().length_c, 16.12);
       CHECK_EQ(graph.getCell().angle_alpha, 90.000);
       CHECK_EQ(graph.getCell().angle_beta, 90.0);
       CHECK_EQ(graph.getCell().angle_gamma, 120.0);
       CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 62");
       CHECK_EQ(graph.getSymmetry().Int_Tables_number, 171);
        filePath.replace_extension(".json");
        std::ofstream ofs(filePath);
        ofs<<" "<<jgfPublisher<<std::endl;
        ofs.close();

       });
       CHECK_EQ(graph.getNumberOfAtomSites(), 260);
       CHECK_EQ(lemon::countEdges(graph), 229);
       CHECK_EQ(lemon::countNodes(graph.componentGraph), 35);
       CHECK_EQ(lemon::countEdges(graph.componentGraph), 10);
       
    });

}

TEST_CASE("test tcp for 6u6j.cif.gz"){
    std::string comp_id="6u6j";
    std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
    sylvanmats::constitution::Graph graph;
    std::filesystem::path  filePath="./"+comp_id+".cif.gz";
    sylvanmats::reading::TCPReader tcpReader;
    tcpReader(url, [&graph, &filePath, &comp_id](std::istream& is){

        sylvanmats::constitution::Populator populator;
        populator(is, graph, [&filePath](sylvanmats::constitution::Graph& graph){
        sylvanmats::publishing::JGFPublisher jgfPublisher(graph);
       CHECK_EQ(graph.getNumberOfAtomSites(), 745);
       CHECK_EQ(graph.getCell().length_a, 43.304);
       CHECK_EQ(graph.getCell().length_b, 43.304);
       CHECK_EQ(graph.getCell().length_c, 84.303);
       CHECK_EQ(graph.getCell().angle_alpha, 90.000);
       CHECK_EQ(graph.getCell().angle_beta, 90.0);
       CHECK_EQ(graph.getCell().angle_gamma, 120.0);
       CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 3 2 1");
       CHECK_EQ(graph.getSymmetry().Int_Tables_number, 150);
        filePath.replace_extension(".json");
        std::ofstream ofs(filePath);
        ofs<<" "<<jgfPublisher<<std::endl;
        ofs.close();

            std::array<unsigned int, 8> terminals{3, 3, 2, 0, 0, 0, 0, 1};
            unsigned int index=0;
            for(lemon::ListGraph::NodeIt nCompA(graph.componentGraph); nCompA!=lemon::INVALID; ++nCompA){
                //std::cout<<graph.componentProperties[nCompA].auth_mon_id<<" "<<graph.componentProperties[nCompA].termination<<std::endl;
                //CHECK_EQ(graph.componentProperties[nCompA].termination, terminals[index++]);
            }    
       });
       CHECK_EQ(graph.getNumberOfAtomSites(), 745);
       CHECK_EQ(lemon::countEdges(graph), 668);
       CHECK_EQ(lemon::countNodes(graph.componentGraph), 67);
       CHECK_EQ(lemon::countEdges(graph.componentGraph), 27);
       
    });

}

TEST_CASE("test tcp for 1q8h.cif.gz"){
    std::string comp_id="1q8h";
    std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
    sylvanmats::constitution::Graph graph;
    std::filesystem::path  filePath="./"+comp_id+".cif.gz";
    sylvanmats::reading::TCPReader tcpReader;
//        std::cout<<"tcpReader "<<std::endl;
    tcpReader(url, [&graph, &filePath, &comp_id](std::istream& is){
//        std::cout<<"tcpReader 2 "<<std::endl;

        sylvanmats::constitution::Populator populator;
//        std::cout<<"populator "<<std::endl;
        populator(is, graph, [&filePath](sylvanmats::constitution::Graph& graph){
        sylvanmats::publishing::JGFPublisher jgfPublisher(graph);
        CHECK_EQ(graph.getNumberOfAtomSites(), 378);
        CHECK_EQ(graph.getCell().length_a, 51.491);
        CHECK_EQ(graph.getCell().length_b, 51.491);
        CHECK_EQ(graph.getCell().length_c, 35.389);
        CHECK_EQ(graph.getCell().angle_alpha, 90.000);
        CHECK_EQ(graph.getCell().angle_beta, 90.0);
        CHECK_EQ(graph.getCell().angle_gamma, 120.0);
        CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 31 2 1");
        CHECK_EQ(graph.getSymmetry().Int_Tables_number, 152);
        CHECK_EQ(graph.getOperationList().size(), 2);
        if(graph.getOperationList().size()==2){
            CHECK_EQ(graph.getOperationList()[1].id, 2);
            CHECK_EQ(graph.getOperationList()[1].type, "'crystal symmetry operation'");
            CHECK_EQ(graph.getOperationList()[1].name, "4_557");
            CHECK_EQ(graph.getOperationList()[1].symmetry_operation, "y,x,-z+2");
            CHECK(graph.getOperationList()[1].matrix[0][0] == doctest::Approx(-0.5));
            CHECK(graph.getOperationList()[1].matrix[0][1] == doctest::Approx(0.8660254));
            CHECK(graph.getOperationList()[1].matrix[0][2] == doctest::Approx(0.0));
            CHECK(graph.getOperationList()[1].matrix[1][0] == doctest::Approx(0.8660254));
            CHECK(graph.getOperationList()[1].matrix[1][1] == doctest::Approx(0.5));
            CHECK(graph.getOperationList()[1].matrix[1][2] == doctest::Approx(0.0));
            CHECK(graph.getOperationList()[1].matrix[2][0] == doctest::Approx(0.0));
            CHECK(graph.getOperationList()[1].matrix[2][1] == doctest::Approx(0.0));
            CHECK(graph.getOperationList()[1].matrix[2][2] == doctest::Approx(-1.0));
            CHECK_EQ(graph.getOperationList()[1].vector[0], 0);
            CHECK_EQ(graph.getOperationList()[1].vector[1], 0);
            CHECK(graph.getOperationList()[1].vector[2] == doctest::Approx(70.778));
        }
        filePath.replace_extension(".json");
        std::ofstream ofs(filePath);
        ofs<<" "<<jgfPublisher<<std::endl;
        ofs.close();

            std::array<unsigned int, 8> terminals{3, 3, 2, 0, 0, 0, 0, 1};
            unsigned int index=0;
            for(lemon::ListGraph::NodeIt nCompA(graph.componentGraph); nCompA!=lemon::INVALID; ++nCompA){
                //std::cout<<graph.componentProperties[nCompA].auth_mon_id<<" "<<graph.componentProperties[nCompA].termination<<std::endl;
                //CHECK_EQ(graph.componentProperties[nCompA].termination, terminals[index++]);
            }
       });
       CHECK_EQ(graph.getStructureConnections().size(), 30);
       CHECK_EQ(graph.getNumberOfAtomSites(), 378);
       CHECK_EQ(lemon::countEdges(graph), 316);
       CHECK_EQ(lemon::countNodes(graph.componentGraph), 113);
       CHECK_EQ(lemon::countEdges(graph.componentGraph), 47);
       sylvanmats::publishing::CIFCompressor cifCompressor;
       cifCompressor(comp_id, is);

    });

}

TEST_CASE("test tcp for 4ac0.cif.gz"){
    std::string comp_id="4ac0";
    std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
    sylvanmats::constitution::Graph graph;
    std::filesystem::path  filePath="./"+comp_id+".cif.gz";
    sylvanmats::reading::TCPReader tcpReader;
     tcpReader(url, [&graph, &filePath, &comp_id](std::istream& is){

        sylvanmats::constitution::Populator populator;
//        std::cout<<"populator "<<std::endl;
        populator(is, graph, [&filePath](sylvanmats::constitution::Graph& graph){
        sylvanmats::publishing::JGFPublisher jgfPublisher(graph);
       CHECK_EQ(graph.getNumberOfAtomSites(), 1623);
       CHECK_EQ(graph.getCell().length_a, 71.88);
       CHECK_EQ(graph.getCell().length_b, 71.88);
       CHECK_EQ(graph.getCell().length_c, 93.43);
       CHECK_EQ(graph.getCell().angle_alpha, 90.0);
       CHECK_EQ(graph.getCell().angle_beta, 90.0);
       CHECK_EQ(graph.getCell().angle_gamma, 90.0);
       CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 41 21 2");
       CHECK_EQ(graph.getSymmetry().Int_Tables_number, 92);
        filePath.replace_extension(".json");
        std::ofstream ofs(filePath);
        ofs<<" "<<jgfPublisher<<std::endl;
        ofs.close();

            std::array<unsigned int, 8> terminals{3, 3, 2, 0, 0, 0, 0, 1};
            unsigned int index=0;
            for(lemon::ListGraph::NodeIt nCompA(graph.componentGraph); nCompA!=lemon::INVALID; ++nCompA){
                //std::cout<<graph.componentProperties[nCompA].auth_mon_id<<" "<<graph.componentProperties[nCompA].termination<<std::endl;
                //CHECK_EQ(graph.componentProperties[nCompA].termination, terminals[index++]);
            }
       });
       CHECK_EQ(graph.getStructureConnections().size(), 6);
       CHECK_EQ(graph.getNumberOfAtomSites(), 1623);
       CHECK_EQ(lemon::countEdges(graph), 1626);
       CHECK_EQ(lemon::countNodes(graph.componentGraph), 230);
       CHECK_EQ(lemon::countEdges(graph.componentGraph), 200);
       sylvanmats::publishing::CIFCompressor cifCompressor;
       cifCompressor(comp_id, is);

    });

}

TEST_CASE("test tcp for COD hydroxyapatite"){
    std::string url = "http://www.crystallography.net/cod/result*?text=hydroxyapatite&format=urls&el1=Ca&nel1=Na&nel2=C";
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
        std::string hklUrl=url.substr(0, url.size()-4)+".hkl";
//std::cout<<"hklUrl "<<hklUrl<<std::endl;
        /*CHECK(tcpReader(hklUrl, [&](std::istream& is){
            std::string cifContent((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
            std::cout<<"cifContent: "<<cifContent<<std::endl;
            CHECK_EQ(cifContent.size(), 4766);
            std::string path="./examples/hydroxyapatite.hkl.cif.gz";
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

        }));*/
        
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
//       std::cout<<"unk? "<<ccb.comp_id<<" "<<ccb.atom_id_1<<" "<<ccb.atom_id_2<<" "<<ccb.value_order<<" "<<std::endl;
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
   CHECK_EQ(graph.getOperationList().size(), 1);
    if(graph.getOperationList().size()==1){
        CHECK_EQ(graph.getOperationList()[0].id, 1);
        CHECK_EQ(graph.getOperationList()[0].type, "identity operation");
        CHECK_EQ(graph.getOperationList()[0].name, "1_555");
        CHECK_EQ(graph.getOperationList()[0].symmetry_operation, "x,y,z");
        CHECK_EQ(graph.getOperationList()[0].vector[0], 0);
        CHECK_EQ(graph.getOperationList()[0].vector[1], 0);
        CHECK_EQ(graph.getOperationList()[0].vector[2], 0);
    }
    filePath.replace_extension(".json");
    std::ofstream ofs(filePath);
    ofs<<" "<<jgfPublisher<<std::endl;
    ofs.close();

    std::array<unsigned int, 8> terminals{3, 3, 2, 0, 0, 0, 0, 1};
    unsigned int index=0;
    for(lemon::ListGraph::NodeIt nCompA(graph.componentGraph); nCompA!=lemon::INVALID; ++nCompA){
        CHECK_EQ(graph.componentProperties[nCompA].termination, terminals[index++]);
    }    
   });
   CHECK_EQ(graph.getNumberOfAtomSites(), 46);
   CHECK_EQ(lemon::countEdges(graph), 43);
   CHECK_EQ(lemon::countNodes(graph.componentGraph), 8);
   CHECK_EQ(lemon::countEdges(graph.componentGraph), 4);
    
    std::vector<sylvanmats::constitution::unique_component> uniqueComponents = {};
    sylvanmats::constitution::Selection selection(graph);
    selection(uniqueComponents, [&](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph){
        graph.identifyFusedSystems(selectionGraph, [&graph, &filePath](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> subSelectionGraph){
            CHECK_EQ(lemon::countNodes(subSelectionGraph), 24);
            CHECK_EQ(lemon::countEdges(subSelectionGraph), 28);
            graph.identifyRings(subSelectionGraph);
        });
    });
    CHECK_EQ(graph.getNumberOfRings(), 0);
    CHECK_EQ(graph.countFlexibles(), 35);
    std::filesystem::path path="../templates/cif";
    sylvanmats::publishing::st::CIFPublisher cifPublisher(path);
    cifPublisher.add("entry_id", "3SGS");
    std::vector<std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>> atomSitesLoop;
    for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
        atomSitesLoop.insert(atomSitesLoop.begin(), std::make_tuple(graph.atomSites[nSiteA].group_PDB, graph.atomSites[nSiteA].id, graph.atomSites[nSiteA].type_symbol, graph.atomSites[nSiteA].label_atom_id, graph.atomSites[nSiteA].label_alt_id, graph.atomSites[nSiteA].label_comp_id, graph.atomSites[nSiteA].label_asym_id, graph.atomSites[nSiteA].label_entity_id, graph.atomSites[nSiteA].label_seq_id, graph.atomSites[nSiteA].pdbx_PDB_ins_code, graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z, graph.atomSites[nSiteA].occupancy, graph.atomSites[nSiteA].B_iso_or_equiv, graph.atomSites[nSiteA].pdbx_formal_charge, graph.atomSites[nSiteA].auth_seq_id, graph.atomSites[nSiteA].auth_comp_id, graph.atomSites[nSiteA].auth_asym_id, graph.atomSites[nSiteA].auth_atom_id, graph.atomSites[nSiteA].pdbx_PDB_model_num));
    }
    cifPublisher.add("atom_sites", atomSitesLoop);
    std::vector<std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>> pdxPolySchemesLoop;
    std::vector<std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string>> pdxNonpolySchemesLoop;
    for(lemon::ListGraph::NodeIt nComponentA(graph.componentGraph); nComponentA!=lemon::INVALID; ++nComponentA){
        if(graph.componentProperties[nComponentA].termination==sylvanmats::constitution::MONOMER)pdxNonpolySchemesLoop.insert(pdxNonpolySchemesLoop.begin(), std::make_tuple(graph.componentProperties[nComponentA].asym_id, graph.componentProperties[nComponentA].entity_id, graph.componentProperties[nComponentA].seq_id, graph.componentProperties[nComponentA].mon_id, graph.componentProperties[nComponentA].ndb_seq_num, graph.componentProperties[nComponentA].pdb_seq_num, graph.componentProperties[nComponentA].auth_seq_num, graph.componentProperties[nComponentA].pdb_mon_id, graph.componentProperties[nComponentA].auth_mon_id, graph.componentProperties[nComponentA].pdb_strand_id, graph.componentProperties[nComponentA].pdb_ins_code));
        else pdxPolySchemesLoop.insert(pdxPolySchemesLoop.begin(), std::make_tuple(graph.componentProperties[nComponentA].asym_id, graph.componentProperties[nComponentA].entity_id, graph.componentProperties[nComponentA].seq_id, graph.componentProperties[nComponentA].mon_id, graph.componentProperties[nComponentA].ndb_seq_num, graph.componentProperties[nComponentA].pdb_seq_num, graph.componentProperties[nComponentA].auth_seq_num, graph.componentProperties[nComponentA].pdb_mon_id, graph.componentProperties[nComponentA].auth_mon_id, graph.componentProperties[nComponentA].pdb_strand_id, graph.componentProperties[nComponentA].pdb_ins_code, graph.componentProperties[nComponentA].hetero));
    }
    cifPublisher.add("pdbx_poly_seq_schemes", pdxPolySchemesLoop);
    cifPublisher.add("pdbx_nonpoly_schemes", pdxNonpolySchemesLoop);
    CHECK_EQ(pdxPolySchemesLoop.size(), 6);
    CHECK_EQ(pdxNonpolySchemesLoop.size(), 2);
    std::string&& content = cifPublisher.render();
    //std::cout<<"content "<<content<<std::endl;
    CHECK_EQ(content.size(), 5591);
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
   CHECK_EQ(lemon::countEdges(graph.componentGraph), 151);
    std::vector<sylvanmats::constitution::unique_component> uniqueComponents = {{.label_comp_id="HEM", .label_asym_id="D", .auth_seq_id=154}};
    sylvanmats::constitution::Selection selection(graph);
    selection(uniqueComponents, [&](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph){
        graph.identifyFusedSystems(selectionGraph, [&graph, &filePath](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> subSelectionGraph){
            CHECK_EQ(lemon::countNodes(subSelectionGraph), 25);
            CHECK_EQ(lemon::countEdges(subSelectionGraph), 32);
            graph.identifyRings(subSelectionGraph);
            sylvanmats::publishing::JGFPublisher jgfPublisher(graph, subSelectionGraph);   
            filePath.replace_extension(".json");
            std::ofstream ofs(filePath);
            ofs<<" "<<jgfPublisher<<std::endl;
            ofs.close();
        });
        selection.compliment(selectionGraph, [](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& complimentGraph){
            CHECK_EQ(lemon::countNodes(complimentGraph), 1289);
            CHECK_EQ(lemon::countEdges(complimentGraph), 1248);
        });
    });
    CHECK_EQ(graph.getNumberOfRings(), 4);
    CHECK_EQ(graph.countFlexibles(), 35);

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
    
TEST_CASE("test ccp4 map input") {
    std::filesystem::path path="./examples/emd_8194.map";

    CHECK(std::filesystem::exists(path));
    if(std::filesystem::exists(path)){
        sylvanmats::density::ccp4::MapInput mapInput;
        mapInput(path, [](sylvanmats::density::ccp4::ccp4_header& ccp4Header, unsigned int sectionIndex, float* slice){});
        CHECK_EQ(mapInput.getHeader().NC, 234);
        CHECK_EQ(mapInput.getHeader().NR, 234);
        CHECK_EQ(mapInput.getHeader().NS, 234);
        CHECK_EQ(mapInput.getHeader().MODE, 2);
        CHECK(mapInput.getHeader().X_length == doctest::Approx(149.058f));
        CHECK(mapInput.getHeader().Y_length == doctest::Approx(149.058f));
        CHECK(mapInput.getHeader().Z_length == doctest::Approx(149.058f));
        //CHECK(mapInput.getHeader().Alpha == doctest::Approx(3.0f));
        //CHECK(mapInput.getHeader().Beta == doctest::Approx(3.0f));
        //CHECK(mapInput.getHeader().Gamma == doctest::Approx(3.0f));
        CHECK(mapInput.getHeader().AMIN == doctest::Approx(-0.01429f));
        CHECK(mapInput.getHeader().AMAX == doctest::Approx(0.03361f));
        CHECK(mapInput.getHeader().AMEAN == doctest::Approx(-0.00011f));
        CHECK_EQ(mapInput.getHeader().ISPG, 1);
        CHECK_EQ(mapInput.getHeader().NSYMBT, 0);
        CHECK_EQ(mapInput.getHeader().LSKFLG, 0);
        CHECK_EQ(mapInput.getHeader().NVERSION, 0);
         CHECK(mapInput.getHeader().ARMS == doctest::Approx(0.0029f));
       CHECK_EQ(mapInput.getHeader().NLABL, 1);
        CHECK_EQ(mapInput.getHeader().LABEL[0], "::::EMDATABANK.org::::EMD-8194::::                                              ");
        
    }
    
}
    
TEST_CASE("test QD0 ligand from mol2") {
   std::filesystem::path filePath="../../CASF-2016/coreset/4bkt/4bkt_ligand.mol2";
   
    sylvanmats::constitution::Graph graph;
    
    sylvanmats::constitution::MOL2Populator populator;
    populator(filePath, graph, [&filePath](sylvanmats::constitution::Graph& graph){
   CHECK_EQ(graph.getNumberOfAtomSites(), 36);
   
   });
   CHECK_EQ(graph.getNumberOfAtomSites(), 36);
   CHECK_EQ(lemon::countEdges(graph), 37);
   CHECK_EQ(lemon::countNodes(graph.componentGraph), 1);  
   CHECK_EQ(lemon::countEdges(graph.componentGraph), 0);
    std::vector<sylvanmats::constitution::unique_component> uniqueComponents = {{.label_comp_id="QD0", .label_asym_id="*", .pdbx_PDB_ins_code="*", .auth_seq_id=1}};
    sylvanmats::constitution::Selection selection(graph);
    selection(uniqueComponents, [&](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph){
        graph.identifyFusedSystems(selectionGraph, [&graph, &filePath](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subSelectionGraph){
            CHECK_EQ(lemon::countNodes(subSelectionGraph), 5);
            CHECK_EQ(lemon::countEdges(subSelectionGraph), 5);
            graph.identifyRings(subSelectionGraph);
            sylvanmats::publishing::JGFPublisher jgfPublisher(graph, subSelectionGraph);   
            filePath.replace_extension(".json");
            std::ofstream ofs(filePath);
            ofs<<" "<<jgfPublisher<<std::endl;
            ofs.close();
        });
        selection.compliment(selectionGraph, [](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& complimentGraph){
            CHECK_EQ(lemon::countNodes(complimentGraph), 0);
            CHECK_EQ(lemon::countEdges(complimentGraph), 0);
        });
    });
    CHECK_EQ(graph.getNumberOfRings(), 2);
    CHECK_EQ(graph.countFlexibles(), 31);
    
}

}