#include <doctest/doctest.h>

#include <string>
#include <iostream>
#include <utility>
#include <cstring>

#define protected public

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"
#include "reading/gz/GZReader.h"
#include "reading/tcp/TCPReader.h"

#include "constitution/Graph.h"
#include "constitution/Populator.h"
#include "alignment/SmithWatermanAligner.h"

TEST_SUITE("sequencing"){

TEST_CASE("test Smith-Waterman"){
    sylvanmats::alignment::SmithWatermanAligner smithWatermanAligner(sylvanmats::alignment::BLOSUM62);
    std::string sequenceA="TGTTACGG";
    std::string sequenceB="GGTTGACTA";
    auto&& [sequenceI, sequenceJ] =smithWatermanAligner(sequenceA, sequenceB);
    std::cout<<sequenceI<<std::endl;
    std::cout<<sequenceJ<<std::endl;
    CHECK_EQ(sequenceI, "GTT-AC");
    CHECK_EQ(sequenceJ, "GTTGAC");
}

TEST_CASE("test 8vo3"){
    std::string url = "https://files.rcsb.org/download/8vo3-sf.cif";
    sylvanmats::reading::TCPReader tcpSFReader;
    CHECK(tcpSFReader(url, [&](std::istream& is){
    }));
    url = "https://files.rcsb.org/download/8vo3.cif";
    sylvanmats::reading::TCPReader tcpReader;
    CHECK(tcpReader(url, [&](std::istream& is){
    sylvanmats::constitution::Graph graph;
    
    sylvanmats::constitution::Populator populator;
    populator(is, graph, [](sylvanmats::constitution::Graph& graph){
   CHECK_EQ(graph.getNumberOfAtomSites(), 1303);
   CHECK_EQ(graph.getCell().length_a, 48.767);
   CHECK_EQ(graph.getCell().length_b, 71.491);
   CHECK_EQ(graph.getCell().length_c,  92.265);
   CHECK_EQ(graph.getCell().angle_alpha, 90.000);
   CHECK_EQ(graph.getCell().angle_beta,  90.0);
   CHECK_EQ(graph.getCell().angle_gamma, 90.000);
   CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "C 2 2 21");
   CHECK_EQ(graph.getSymmetry().Int_Tables_number, 20);
   
   });
   CHECK_EQ(graph.getNumberOfAtomSites(), 1303);
   CHECK_EQ(lemon::countEdges(graph), 1187);
   CHECK_EQ(lemon::countNodes(graph.componentGraph), 274);
   CHECK_EQ(lemon::countEdges(graph.componentGraph), 156);
    
    //CHECK_EQ(graph.getNumberOfRings(), 3);
    //CHECK_EQ(graph.countFlexibles(), 6);
    std::unordered_map<std::string, std::string> oneLetter{{"A", "A"},{"G", "G"},{"T", "T"},{"U", "U"},
    {"GLY", "G"},
    {"ALA", "A"},
    {"VAL", "V"},
    {"LEU", "L"},
    {"ILE", "I"},
    {"THR", "T"},
    {"SER", "S"},
    {"MET", "M"},
    {"CYS", "C"},
    {"PRO", "P"},
    {"PHE", "F"},
    {"TYR", "Y"},
    {"TRP", "W"},
    {"HIS", "H"},
    {"LYS", "K"},
    {"ARG", "R"},
    {"ASP", "D"},
    {"GLU", "E"},
    {"ASN", "N"},
    {"GLN", "Q"}
    };
    std::string sequenceA{};
    for(std::vector<sylvanmats::constitution::_entity_poly_seq<double>>::iterator it=graph.getResidueSequence().begin();it!=graph.getResidueSequence().end();it++){
        std::string ol=oneLetter[(*it).mon_id];
        //std::cout<<" "<<(*it).mon_id<<" "<<ol<<std::endl;
        sequenceA.append(oneLetter[(*it).mon_id]);
    }
    std::string sequenceB{};
    for(lemon::ListGraph::NodeIt nR(graph.componentGraph); nR!=lemon::INVALID; ++nR){
        if(graph.componentProperties[nR].auth_mon_id.compare("?")!=0 && oneLetter.count(graph.componentProperties[nR].mon_id)>0){
        std::string ol=oneLetter[graph.componentProperties[nR].mon_id];
        //std::cout<<" "<<graph.componentProperties[nR].mon_id<<" "<<ol<<std::endl;
        sequenceB.insert(sequenceB.begin(), ol.begin(), ol.end());
        }
    }
    std::cout<<"A "<<sequenceA<<std::endl;
    std::cout<<"B "<<sequenceB<<std::endl;
    sylvanmats::alignment::SmithWatermanAligner smithWatermanAligner(sylvanmats::alignment::BLOSUM62);
    auto&& [sequenceI, sequenceJ] =smithWatermanAligner(sequenceA, sequenceB);
    std::cout<<sequenceI<<std::endl;
    std::cout<<sequenceJ<<std::endl;
    CHECK_EQ(sequenceJ, "LVGKLVTQLEVNCDADIFYKIVKHH---PNVIPHFFTGVQVTKGDGLVSGSIKEWNYVLEGKAMTAVEETTHADETRTLTHHITEGDAMKDYKKFDVIVETNPKPN--GSVVTYSIVYEKINEDSPAPFDYLKFFHQNIVDMSAHICSS");
    }));
}

}
