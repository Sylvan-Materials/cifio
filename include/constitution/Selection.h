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
        Selection(const Selection& orig) =  delete;
        virtual ~Selection() = default;

        void operator()(int sequence, std::function<void(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subGraph)> apply){
            lemon::ListGraph::NodeMap<bool> selectionNodes(graph, false);
            lemon::ListGraph::EdgeMap<bool> selectionEdges(graph, false);
            lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> selectionGraph(graph, selectionNodes, selectionEdges);
            bool firstSite=false;
            for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
                if(graph.atomSites[nSiteA].auth_seq_id==sequence && graph.atomSites[nSiteA].label_comp_id.compare("HEM")==0){
                    selectionGraph.enable(nSiteA);
                    std::cout<<" "<<graph.atomSites[nSiteA].label_comp_id<<" "<<selectionGraph.status(nSiteA)<<std::endl;
                    for(lemon::ListGraph::IncEdgeIt eSiteA(graph, nSiteA); eSiteA!=lemon::INVALID; ++eSiteA){
                        if(!selectionGraph.status(graph.oppositeNode(nSiteA, eSiteA)) && graph.atomSites[graph.oppositeNode(nSiteA, eSiteA)].auth_seq_id==sequence)selectionGraph.enable(eSiteA);
                    }
                }
            }
            apply(selectionGraph);
        };
    };
}
