#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <iterator>
#include <numbers>
#include <ranges>
#include <cmath>

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"
#include "reading/gz/GZReader.h"
#include "reading/tcp/TCPReader.h"
#include "constitution/Graph.h"
#include "constitution/Populator.h"
#include "constitution/Selection.h"
#include "publishing/jgf/JGFPublisher.h"

#include "surface/BipartiteSurface.h"

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
            sylvanmats::constitution::Selection selection(graph);
            selection(uniqueComponents, [&](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph){
                CHECK_EQ(lemon::countNodes(selectionGraph), 10);
                CHECK_EQ(lemon::countEdges(selectionGraph), 8);
                selection.compliment(selectionGraph, [&graph,&selectionGraph](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& complimentGraph){
                    CHECK_EQ(lemon::countNodes(complimentGraph), 4006);
                    CHECK_EQ(lemon::countEdges(complimentGraph), 3797);
                    sylvanmats::surface::BipartiteSurface bipartiteSurface;
                    for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(selectionGraph); nSiteA!=lemon::INVALID; ++nSiteA){
                        if(graph.atomSites[nSiteA].type_symbol.compare("O")==0){
                            for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteB(complimentGraph); nSiteB!=lemon::INVALID; ++nSiteB){
                                double d=std::sqrt(std::pow(graph.atomSites[nSiteA].Cartn_x-graph.atomSites[nSiteB].Cartn_x, 2)+std::pow(graph.atomSites[nSiteA].Cartn_y-graph.atomSites[nSiteB].Cartn_y, 2)+std::pow(graph.atomSites[nSiteA].Cartn_z-graph.atomSites[nSiteB].Cartn_z,2));
                                if(d<5.0){
                                    std::cout<<"d: "<<d<<" "<<graph.getXPath(nSiteA)<<graph.atomSites[nSiteA].label_atom_id<<"---"<<graph.getXPath(nSiteB)<<graph.atomSites[nSiteB].label_atom_id<<std::endl;
                                    //bipartiteSurface
                                }
                            }
                        }
                    }
                });
            });
       });
       CHECK_EQ(graph.getNumberOfAtomSites(), 4016);
       CHECK_EQ(lemon::countEdges(graph), 3805);
       CHECK_EQ(lemon::countNodes(graph.componentGraph), 893);
       CHECK_EQ(lemon::countEdges(graph.componentGraph), 603);
       
    });

}

}