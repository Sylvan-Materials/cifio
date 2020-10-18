#pragma once

#include <stdlib.h>
#include <string>
#include <memory>

#include "lemon/list_graph.h"
#include <lemon/adaptors.h>

#include "constitution/Graph.h"

namespace sylvanmats::constitution {
    class Selection{
    protected:
        Graph& graph;
    public:
        Selection(Graph& graph) : graph (graph) {};
        Selection(const Selection& orig) = delete;
        virtual ~Selection() = default;

        void operator()(int sequence, std::function<void(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subGraph)> apply){
            lemon::ListGraph::NodeMap<bool> selectionNodes(graph, false);
            lemon::ListGraph::EdgeMap<bool> selectionEdges(graph, false);
            lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> selectionGraph(graph, selectionNodes, selectionEdges);
            bool firstSite=false;
            for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
                if(graph.atomSites[nSiteA].auth_seq_id==sequence && graph.atomSites[nSiteA].label_comp_id.compare("HEM")==0 && graph.atomSites[nSiteA].type_symbol.compare("FE")!=0){
                    selectionGraph.enable(nSiteA);
                    for(lemon::ListGraph::IncEdgeIt eSiteA(graph, nSiteA); eSiteA!=lemon::INVALID; ++eSiteA){
//                    std::cout<<" "<<graph.atomSites[nSiteA].label_comp_id<<" "<<selectionGraph.status(nSiteA)<<" "<<selectionGraph.status(eSiteA)<<" "<<graph.atomSites[graph.oppositeNode(nSiteA, eSiteA)].auth_seq_id<<" "<<sequence<<std::endl;
                        if(!selectionGraph.status(eSiteA) && graph.atomSites[graph.oppositeNode(nSiteA, eSiteA)].auth_seq_id==sequence && graph.atomSites[nSiteA].type_symbol.compare("FE")!=0){
//std::cout<<"\t"<<graph.atomSites[nSiteA].label_comp_id<<" .. "<<graph.atomSites[graph.oppositeNode(nSiteA, eSiteA)].label_comp_id<<std::endl;
                            selectionGraph.enable(eSiteA);
                        }
                    }
                }
            }
            apply(selectionGraph);
        };
    };
}
