#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <iterator>
#include <numbers>
#include <valarray>
#include <ranges>
#include <cmath>

#include "reading/gz/GZReader.h"
#include "reading/tcp/TCPReader.h"
#include "constitution/Graph.h"
#include "constitution/Populator.h"
#include "constitution/Selection.h"
#include "PeriodicTable.h"
#include "publishing/jgf/JGFPublisher.h"

#include "surface/Accessible.h"
#include "surface/BipartiteSurface.h"

#include "io/json/Binder.h"
#include "graph/views/vertexlist.hpp"
#include "graph/views/edgelist.hpp"

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"

TEST_SUITE("bipartite"){

TEST_CASE("test bipartite of 5aej.cif.gz"){
    std::string comp_id="5aej";
    std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
    sylvanmats::constitution::Graph graph;
    std::filesystem::path  filePath="./"+comp_id+".cif.gz";
    sylvanmats::reading::TCPReader tcpReader;
    tcpReader(url, [&graph, &filePath, &comp_id](std::istream& is){

        sylvanmats::constitution::Populator populator;
        populator(is, graph, [&filePath](sylvanmats::constitution::Graph& graph){
        sylvanmats::publishing::JGFPublisher jgfPublisher(graph);
       CHECK_EQ(graph.getNumberOfAtomSites(), 4016);
       CHECK_EQ(graph.getCell().length_a, 86.816);
       CHECK_EQ(graph.getCell().length_b, 106.135);
       CHECK_EQ(graph.getCell().length_c, 78.521);
       CHECK_EQ(graph.getCell().angle_alpha, 90.000);
       CHECK_EQ(graph.getCell().angle_beta, 121.25);
       CHECK_EQ(graph.getCell().angle_gamma, 90.000);
       CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "C 1 2 1");
       CHECK_EQ(graph.getSymmetry().Int_Tables_number, 5);
        filePath.replace_extension(".json");
        std::ofstream ofs(filePath);
        ofs<<" "<<jgfPublisher<<std::endl;
        ofs.close();

            std::vector<sylvanmats::constitution::unique_component> uniqueComponents = {{.label_comp_id="SO4", .label_asym_id="E", .auth_seq_id=1185},{.label_comp_id="SO4", .label_asym_id="F", .auth_seq_id=1185}};
            sylvanmats::surface::BipartiteSurface bipartiteSurface(graph, uniqueComponents);
            bipartiteSurface();
            for (auto&& [uid, u] : graph::views::vertexlist(bipartiteSurface)) {
                auto uValue=graph::vertex_value(bipartiteSurface, *graph::find_vertex(bipartiteSurface, uid));
                //std::cout<<"L: "<<graph.getXPath(uValue)<<std::endl;
            }
       });
       CHECK_EQ(graph.getNumberOfAtomSites(), 4016);
       CHECK_EQ(lemon::countEdges(graph), 3805);
       CHECK_EQ(lemon::countNodes(graph.componentGraph), 893);
       CHECK_EQ(lemon::countEdges(graph.componentGraph), 603);
       
    });

}

TEST_CASE("test bipartite of 1a30.cif.gz"){
    std::string comp_id="1a30";
    std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
    sylvanmats::constitution::Graph graph;
    std::filesystem::path  filePath="./"+comp_id+".cif.gz";
    sylvanmats::reading::TCPReader tcpReader;
    tcpReader(url, [&graph, &filePath, &comp_id](std::istream& is){

        sylvanmats::constitution::Populator populator;
        populator(is, graph, [&filePath](sylvanmats::constitution::Graph& graph){
        sylvanmats::publishing::JGFPublisher jgfPublisher(graph);
       CHECK_EQ(graph.getNumberOfAtomSites(), 1752);
       CHECK_EQ(graph.getCell().length_a, 58.470);
       CHECK_EQ(graph.getCell().length_b, 86.440);
       CHECK_EQ(graph.getCell().length_c, 45.810);
       CHECK_EQ(graph.getCell().angle_alpha, 90.000);
       CHECK_EQ(graph.getCell().angle_beta, 90.000);
       CHECK_EQ(graph.getCell().angle_gamma, 90.000);
       CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 21 21 2");
       CHECK_EQ(graph.getSymmetry().Int_Tables_number, 18);
        filePath.replace_extension(".json");
        std::ofstream ofs(filePath);
        ofs<<" "<<jgfPublisher<<std::endl;
        ofs.close();

            std::vector<sylvanmats::constitution::unique_component> uniqueComponents = {{.label_comp_id="GLU", .label_asym_id="C", .auth_seq_id=506},{.label_comp_id="ASP", .label_asym_id="C", .auth_seq_id=507},{.label_comp_id="LEU", .label_asym_id="C", .auth_seq_id=508}};
            sylvanmats::surface::BipartiteSurface bipartiteSurface(graph, uniqueComponents);
            bipartiteSurface();
            sylvanmats::PeriodicTable* periodicTable=sylvanmats::PeriodicTable::getInstance();
            
            for (auto&& [uid, u] : graph::views::vertexlist(bipartiteSurface)) {
                auto nSiteA=graph::vertex_value(bipartiteSurface, *graph::find_vertex(bipartiteSurface, uid));
                sylvanmats::element ele=periodicTable->index(graph.atomSites[nSiteA].type_symbol);
                //std::cout<<"L: "<<graph.getXPath(nSiteA)<<" "<<graph.atomSites[nSiteA].type_symbol<<" "<<ele.mass<<std::endl;
            }
//            std::cout<<"BP: "<<lemon::countBlueNodes(bipartiteSurface)<<" "<<lemon::countRedNodes(bipartiteSurface)<<" "<<lemon::countEdges(bipartiteSurface)<<std::endl;
            std::valarray<unsigned int> distanceBin(0u, 100);
            for(auto& [uid, vid, uv]: graph::views::edgelist(bipartiteSurface)){
                lemon::ListGraph::Node nSiteA=graph::vertex_value(bipartiteSurface, *graph::find_vertex(bipartiteSurface, uid));
                lemon::ListGraph::Node nSiteB=graph::vertex_value(bipartiteSurface, *graph::find_vertex(bipartiteSurface, uid));
                sylvanmats::linear::Vector3d pointA(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z);
                sylvanmats::linear::Vector3d pointB(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z);
                double d=(pointB-pointA).norm();
                unsigned int bin=(unsigned int)(d*100.0/6.0);
                distanceBin[bin]++;
            }
//            std::cout<<"Bin: ";
//            for(unsigned int binVal : distanceBin)std::cout<<" "<<binVal;
//            std::cout<<std::endl;
       });
       CHECK_EQ(graph.getNumberOfAtomSites(), 1752);
       CHECK_EQ(lemon::countEdges(graph), 1563);
       CHECK_EQ(lemon::countNodes(graph.componentGraph), 417);
       CHECK_EQ(lemon::countEdges(graph.componentGraph), 197);
       
    });

}

}