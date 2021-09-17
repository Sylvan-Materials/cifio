#pragma once

#include <stdlib.h>
#include <string>
#include <memory>

#include "lemon/list_graph.h"
#include "lemon/connectivity.h"
#include "lemon/maps.h"

//#include "constitution/Selection.h"

namespace sylvanmats::surface {

    class BipartiteSurface : public lemon::ListBpGraph{
        protected:
        sylvanmats::constitution::Graph& graph;
        std::vector<sylvanmats::constitution::unique_component>& uniqueComponents;
        public:
        lemon::CrossRefMap<BipartiteSurface, lemon::ListBpGraph::Node, lemon::ListGraph::Node> constitutionRefMap;
        
        public:
        BipartiteSurface(sylvanmats::constitution::Graph& graph, std::vector<sylvanmats::constitution::unique_component>& uniqueComponents) : graph (graph), uniqueComponents (uniqueComponents), constitutionRefMap(*this) {};
        BipartiteSurface(const BipartiteSurface& orig) = delete;
        virtual ~BipartiteSurface() =  default;

        void operator()(){
            sylvanmats::constitution::Selection selection(graph);
            selection(uniqueComponents, [&](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph){
                //CHECK_EQ(lemon::countNodes(selectionGraph), 26);
                //CHECK_EQ(lemon::countEdges(selectionGraph), 25);
                graph.visibilityOff(selectionGraph);
                sylvanmats::surface::Accessible accessible(graph);
                //accessible([](std::string name, std::vector<sylvanmats::surface::circle<double>>& circles){});

                selection.compliment(selectionGraph, [&](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& complimentGraph){
                    //CHECK_EQ(lemon::countNodes(complimentGraph), 1726);
                    //CHECK_EQ(lemon::countEdges(complimentGraph), 1537);
                    
                    for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(selectionGraph); nSiteA!=lemon::INVALID; ++nSiteA){
                        //if(graph.atomSites[nSiteA].type_symbol.compare("O")==0){
                            for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteB(complimentGraph); nSiteB!=lemon::INVALID; ++nSiteB){
                                double d=std::sqrt(std::pow(graph.atomSites[nSiteA].Cartn_x-graph.atomSites[nSiteB].Cartn_x, 2)+std::pow(graph.atomSites[nSiteA].Cartn_y-graph.atomSites[nSiteB].Cartn_y, 2)+std::pow(graph.atomSites[nSiteA].Cartn_z-graph.atomSites[nSiteB].Cartn_z,2));
                                if(d<6.0){
                                    //std::cout<<"d: "<<d<<" "<<graph.getXPath(nSiteA)<<graph.atomSites[nSiteA].label_atom_id<<"---"<<graph.getXPath(nSiteB)<<graph.atomSites[nSiteB].label_atom_id<<std::endl;
                                    lemon::ListBpGraph::Node targetNode=(constitutionRefMap.count(nSiteB)==0) ? addRedNode() : constitutionRefMap((const lemon::ListGraph::Node)nSiteB);
                                    lemon::ListBpGraph::Node ligandNode=(constitutionRefMap.count(nSiteA)==0) ? addBlueNode() : constitutionRefMap((const lemon::ListGraph::Node)nSiteA);
                                    addEdge(asRedNode(targetNode), asBlueNode(ligandNode));
                                    
                                    if(constitutionRefMap.count(nSiteB)==0)constitutionRefMap.set(targetNode, nSiteB);
                                    if(constitutionRefMap.count(nSiteA)==0)constitutionRefMap.set(ligandNode, nSiteA);
                                    
                                }
                            }
                        //}
                    }
                });
            });
        }
    };

}
