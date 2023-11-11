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

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"
#include "reading/gz/GZReader.h"
#include "reading/tcp/TCPReader.h"

#include "PeriodicTable.h"
#include "standards/ComponentStandards.h"
#include "standards/AminoStandards.h"
#include "standards/Fragments.h"
#include "constitution/Graph.h"
#include "constitution/Populator.h"
#include "constitution/GraphmlPopulator.h"
#include "constitution/Selection.h"
#include "density/Populator.h"
#include "lattice/Populator.h"
#include "density/ccp4/MapInput.h"
#include "publishing/jgf/JGFPublisher.h"

#include "io/json/Binder.h"

#include "lemon/vf2.h"

TEST_SUITE("matching"){

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
   CHECK_EQ(lemon::countEdges(graph.componentGraph), 309);
    
    std::vector<sylvanmats::constitution::unique_component> uniqueComponents = {{.label_comp_id="CGX", .label_asym_id="C", .auth_seq_id=200}};
    sylvanmats::constitution::Selection selection(graph);
    selection(uniqueComponents, [&](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph){
        graph.identifyFusedSystems(selectionGraph, [&graph, &selectionGraph](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> subSelectionGraph){
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
    sylvanmats::constitution::Graph fragmentGraph;
    sylvanmats::constitution::GraphmlPopulator populator;
    populator(filePath, fragmentGraph, [&graph, &selectionGraph, &filePath](sylvanmats::constitution::Graph& fragmentGraph){
    CHECK_EQ(fragmentGraph.getNumberOfAtomSites(), 4);
    CHECK_EQ(fragmentGraph.getNumberOfConnections(), 3);
    
        sylvanmats::PeriodicTable* periodicTable=sylvanmats::PeriodicTable::getInstance();
    lemon::ListGraph::NodeMap<sylvanmats::standards::type_properties> typeMapA(fragmentGraph);
    for (lemon::ListGraph::NodeIt nSiteA(fragmentGraph); nSiteA!=lemon::INVALID; ++nSiteA)
    {
        typeMapA[nSiteA].atomic_number = periodicTable->index(fragmentGraph.atomSites[nSiteA].type_symbol).atomic_number;
    }
    lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<sylvanmats::standards::type_properties> typeMapB(selectionGraph);
    for (lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(selectionGraph); nSiteA!=lemon::INVALID; ++nSiteA)
    {
        typeMapB[nSiteA].atomic_number = periodicTable->index(graph.atomSites[nSiteA].type_symbol).atomic_number;
    }
    lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<lemon::ListGraph::Node> bijectionMap(selectionGraph);
    bool ret = lemon::vf2(fragmentGraph, selectionGraph).induced().mapping(bijectionMap).nodeLabels<lemon::ListGraph::NodeMap<sylvanmats::standards::type_properties>, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<sylvanmats::standards::type_properties>>(typeMapA, typeMapB).run();
    CHECK(!ret);
       
    });
   });
    CHECK_EQ(graph.getNumberOfRings(), 3);
    //CHECK_EQ(graph.countFlexibles(), 6);
    }));

}

TEST_CASE("test 1q8h"){
    std::string url = "https://files.rcsb.org/download/1q8h.cif";
    sylvanmats::reading::TCPReader tcpReader;
    CHECK(tcpReader(url, [&](std::istream& is){
    sylvanmats::constitution::Graph graph;
    
    sylvanmats::constitution::Populator populator;
    populator(is, graph, [](sylvanmats::constitution::Graph& graph){
   CHECK_EQ(graph.getNumberOfAtomSites(), 378);
   CHECK_EQ(graph.getCell().length_a, 51.491);
   CHECK_EQ(graph.getCell().length_b, 51.491);
   CHECK_EQ(graph.getCell().length_c, 35.389);
   CHECK_EQ(graph.getCell().angle_alpha, 90.000);
   CHECK_EQ(graph.getCell().angle_beta,  90.0);
   CHECK_EQ(graph.getCell().angle_gamma, 120.000);
   CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 31 2 1");
   CHECK_EQ(graph.getSymmetry().Int_Tables_number, 152);
   
   });
   CHECK_EQ(graph.getNumberOfAtomSites(), 378);
   CHECK_EQ(lemon::countEdges(graph), 316);
   CHECK_EQ(lemon::countNodes(graph.componentGraph), 113);
   CHECK_EQ(lemon::countEdges(graph.componentGraph), 47);
    
    std::vector<sylvanmats::constitution::unique_component> uniqueComponents = {{.label_comp_id="CGU", .label_asym_id="A", .auth_seq_id=17}};
    sylvanmats::constitution::Selection selection(graph);
    selection(uniqueComponents, [&](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph){
        graph.identifyFusedSystems(selectionGraph, [&graph, &selectionGraph](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> subSelectionGraph){
            //CHECK_EQ(lemon::countNodes(subSelectionGraph), 24);
            //CHECK_EQ(lemon::countEdges(subSelectionGraph), 28);
            graph.identifyRings(subSelectionGraph);
        });
	std::filesystem::path filePath="1q8h.json";
	sylvanmats::publishing::JGFPublisher jgfPublisher(graph, selectionGraph);   
	std::ofstream ofs(filePath);
	ofs<<" "<<jgfPublisher<<std::endl;
	ofs.close();
        
       lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::Node> symmetryMap(selectionGraph);
       //lemon::Vf2<lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::Node>> vf2(selectionGraph, selectionGraph, symmetryMap);
       bool iso = lemon::vf2(selectionGraph, selectionGraph).iso().run();
       CHECK(iso);
    sylvanmats::constitution::Graph fragmentGraph;
    sylvanmats::constitution::GraphmlPopulator populator;
    populator(filePath, fragmentGraph, [&graph, &selectionGraph, &filePath](sylvanmats::constitution::Graph& fragmentGraph){
    CHECK_EQ(fragmentGraph.getNumberOfAtomSites(), 4);
    CHECK_EQ(fragmentGraph.getNumberOfConnections(), 3);
    
        sylvanmats::PeriodicTable* periodicTable=sylvanmats::PeriodicTable::getInstance();
    lemon::ListGraph::NodeMap<sylvanmats::standards::type_properties> typeMapA(fragmentGraph);
    for (lemon::ListGraph::NodeIt nSiteA(fragmentGraph); nSiteA!=lemon::INVALID; ++nSiteA)
    {
        typeMapA[nSiteA].atomic_number = periodicTable->index(fragmentGraph.atomSites[nSiteA].type_symbol).atomic_number;
    }
    lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<sylvanmats::standards::type_properties> typeMapB(selectionGraph);
    for (lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(selectionGraph); nSiteA!=lemon::INVALID; ++nSiteA)
    {
        typeMapB[nSiteA].atomic_number = periodicTable->index(graph.atomSites[nSiteA].type_symbol).atomic_number;
    }
    lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<lemon::ListGraph::Node> bijectionMap(selectionGraph);
    bool ret = lemon::vf2(fragmentGraph, selectionGraph).induced().mapping(bijectionMap).nodeLabels<lemon::ListGraph::NodeMap<sylvanmats::standards::type_properties>, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<sylvanmats::standards::type_properties>>(typeMapA, typeMapB).run();
    CHECK(ret);
    if(ret){
        for (lemon::ListGraph::NodeIt nSiteA(fragmentGraph); nSiteA!=lemon::INVALID; ++nSiteA){
            std::cout<<static_cast<int>(typeMapA[nSiteA].atomic_number)<<" "<<fragmentGraph.atomSites[nSiteA].type_symbol<<" "<<graph.atomSites[bijectionMap[nSiteA]].type_symbol<<" "<<graph.atomSites[bijectionMap[nSiteA]].label_atom_id<<" "<<graph.atomSites[bijectionMap[nSiteA]].Cartn_x<<" "<<graph.atomSites[bijectionMap[nSiteA]].Cartn_y<<" "<<graph.atomSites[bijectionMap[nSiteA]].Cartn_z<<std::endl;
        }
        
    }
       
    });
   });
    CHECK_EQ(graph.getNumberOfRings(), 0);
    //CHECK_EQ(graph.countFlexibles(), 6);
    }));

}

}
