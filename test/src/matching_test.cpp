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
#include "density/ccp4/MapInput.h"
#include "publishing/jgf/JGFPublisher.h"

#include "lemon/vf2.h"

TEST_CASE("test 6jvm"){
    std::string url = "https://files.rcsb.org/download/6jvm.cif";
    sylvanmats::reading::TCPReader tcpReader;
    CHECK(tcpReader(url, [&](std::istream& is){
    sylvanmats::constitution::Graph graph;
    
    sylvanmats::constitution::Populator populator;
    populator(is, graph, [](sylvanmats::constitution::Graph& graph){
   CHECK_EQ(graph.getNumberOfAtomSites(), 2710);
   CHECK_EQ(graph.getCell().length_a, 59.097);
   CHECK_EQ(graph.getCell().length_b, 67.702);
   CHECK_EQ(graph.getCell().length_c, 79.872);
   CHECK_EQ(graph.getCell().angle_alpha, 90.000);
   CHECK_EQ(graph.getCell().angle_beta,  90.0);
   CHECK_EQ(graph.getCell().angle_gamma, 90.000);
   CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 21 21 21");
   CHECK_EQ(graph.getSymmetry().Int_Tables_number, 19);
   
   });
   CHECK_EQ(graph.getNumberOfAtomSites(), 2710);
   CHECK_EQ(lemon::countEdges(graph), 2561);
   CHECK_EQ(lemon::countNodes(graph.componentGraph), 528);
   CHECK_EQ(lemon::countEdges(graph.componentGraph), 308);
    
    std::vector<sylvanmats::constitution::unique_component> uniqueComponents = {{.label_comp_id="CGX", .label_asym_id="C", .auth_seq_id=200}};
    sylvanmats::constitution::Selection selection(graph);
    selection(uniqueComponents, [&](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph){
        graph.identifyFusedSystems(selectionGraph, [&graph](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> subSelectionGraph){
            //CHECK_EQ(lemon::countNodes(subSelectionGraph), 24);
            //CHECK_EQ(lemon::countEdges(subSelectionGraph), 28);
            graph.identifyRings(subSelectionGraph);
        });
	std::filesystem::path filePath="6jvm.json";
	sylvanmats::publishing::JGFPublisher jgfPublisher(graph, selectionGraph);   
	std::ofstream ofs(filePath);
	ofs<<" "<<jgfPublisher<<std::endl;
	ofs.close();
        
       lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::Node> symmetryMap(selectionGraph);
       //lemon::Vf2<lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::Node>> vf2(selectionGraph, selectionGraph, symmetryMap);
       bool iso = lemon::vf2(selectionGraph, selectionGraph).iso().run();
       CHECK(iso);
       /*lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<sylvanmats::constitution::symmetry_labels> selectionMap(selectionGraph);
        for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(selectionGraph); nSiteA!=lemon::INVALID; ++nSiteA){
             selectionMap[nSiteA].type_symbol=graph.atomSites[nSiteA].type_symbol;
        }
	lemon::Vf2<lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::Node>> symVf2(selectionGraph, selectionGraph, symmetryMap);
	symVf2.mappingType(lemon::ISOMORPH);
	if(symVf2.find())
	while(symVf2.find()){
          //process the current mapping symmetryMap
          //std::cout<<"next "<<std::endl;
            for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(selectionGraph); nSiteA!=lemon::INVALID; ++nSiteA){
                 if(symmetryMap[nSiteA]!=lemon::INVALID && nSiteA!=symmetryMap[nSiteA]){
                     //std::cout<<"\t"<<graph.atomSites[nSiteA].auth_atom_id<<"<--> "<<graph.atomSites[symmetryMap[nSiteA]].auth_atom_id<<std::endl;
                 }
            }

        }*/
       
   });
    CHECK_EQ(graph.getNumberOfRings(), 3);
    //CHECK_EQ(graph.countFlexibles(), 6);
    }));

}


