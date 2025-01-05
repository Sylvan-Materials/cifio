#pragma once

#include <stdlib.h>
#include <string>
#include <memory>
#include <functional>

#include "lemon/list_graph.h"
#include "lemon/connectivity.h"
#include "lemon/maps.h"

#include "graph/container/dynamic_graph.hpp"

//#include "constitution/Selection.h"

namespace sylvanmats::surface {
    
    using biG = graph::container::dynamic_graph<size_t, lemon::ListGraph::Node>;

    class BipartiteSurface : public biG {
        protected:
        sylvanmats::constitution::Graph& graph;
        std::vector<sylvanmats::constitution::unique_component>& uniqueComponents;
        std::vector<lemon::ListGraph::Node> bpVertices;
        std::vector<std::tuple<graph::vertex_id_t<biG>, graph::vertex_id_t<biG>, int>> edges;
        public:
        //lemon::CrossRefMap<BipartiteSurface, lemon::ListBpGraph::Node, lemon::ListGraph::Node> constitutionRefMap;
        
        public:
        BipartiteSurface(sylvanmats::constitution::Graph& graph, std::vector<sylvanmats::constitution::unique_component>& uniqueComponents) : biG(), graph (graph), uniqueComponents (uniqueComponents) {};
        BipartiteSurface(const BipartiteSurface& orig) = delete;
        virtual ~BipartiteSurface() =  default;

        void operator()(){
            sylvanmats::constitution::Selection selection(graph);
            selection(uniqueComponents, [&](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph){
                graph.visibilityOff(selectionGraph);
                sylvanmats::surface::Accessible accessible(graph);
                //accessible([](std::string name, std::vector<sylvanmats::surface::circle<double>>& circles){});

                selection.compliment(selectionGraph, [&](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& complimentGraph){
                    
                    for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(selectionGraph); nSiteA!=lemon::INVALID; ++nSiteA){
                        //if(graph.atomSites[nSiteA].type_symbol.compare("O")==0){
                            for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteB(complimentGraph); nSiteB!=lemon::INVALID; ++nSiteB){
                                double d=std::sqrt(std::pow(graph.atomSites[nSiteA].Cartn_x-graph.atomSites[nSiteB].Cartn_x, 2)+std::pow(graph.atomSites[nSiteA].Cartn_y-graph.atomSites[nSiteB].Cartn_y, 2)+std::pow(graph.atomSites[nSiteA].Cartn_z-graph.atomSites[nSiteB].Cartn_z,2));
                                if(d<3.25){
                                    std::vector<lemon::ListGraph::Node>::iterator itA = find(bpVertices.begin(), bpVertices.end(), nSiteA);
                                    size_t indexA=(itA!=bpVertices.end()) ? itA-bpVertices.begin(): bpVertices.size();
                                    if(itA==bpVertices.end())bpVertices.push_back(nSiteA);
                                    std::vector<lemon::ListGraph::Node>::iterator itB = find(bpVertices.begin(), bpVertices.end(), nSiteB);
                                    size_t indexB=(itB!=bpVertices.end()) ? itB-bpVertices.begin(): bpVertices.size();
                                    if(itB==bpVertices.end())bpVertices.push_back(nSiteB);
                                    //std::cout<<" "<<bpVertices.size()<<" "<<indexA<<" "<<indexB<<std::endl;
                                    edges.push_back(std::make_tuple(indexA, indexB, 1));
                                    
                                }
                            }
                        //}
                    }
                });
            });
            std::sort(edges.begin(), edges.end(), [](std::tuple<graph::vertex_id_t<biG>, graph::vertex_id_t<biG>, int>& a, std::tuple<graph::vertex_id_t<biG>, graph::vertex_id_t<biG>, int>& b){return std::get<0>(a)<std::get<0>(b) || std::get<1>(a)<std::get<1>(b);});
            std::cout<<"bpVertices "<<bpVertices.size()<<" "<<edges.size()<<std::endl;
            using value = std::ranges::range_value_t<decltype(edges)>;
            graph::vertex_id_t<biG> N = static_cast<graph::vertex_id_t<biG>>(graph::size(graph::vertices(*this)));
            using edge_desc  = graph::edge_info<graph::vertex_id_t<biG>, true, void, int>;
            reserve_vertices(bpVertices.size());
            reserve_edges(edges.size());
            load_vertices(bpVertices, [&](lemon::ListGraph::Node& nm) {
                auto uid = static_cast<graph::vertex_id_t<biG>>(&nm - bpVertices.data());
                //std::cout<<"uid "<<uid<<std::endl;
                return graph::copyable_vertex_t<graph::vertex_id_t<biG>, lemon::ListGraph::Node>{uid, nm};
              });
            load_edges(edges, [](const value& val) -> edge_desc {
                    //std::cout<<"edge "<<std::get<0>(val)<<" "<<std::get<1>(val)<<" "<<std::get<2>(val)<<std::endl;
                return edge_desc{std::get<0>(val), std::get<1>(val), std::get<2>(val)};
              }, N);
              //size_t np=graph::num_partitions(bpGraph);
            //std::cout<<"num_partitions: "<<np<<std::endl;
        }
    };

}
