#pragma once

#include <stdlib.h>
#include <string>
#include <memory>
#include <tuple>

#include "lemon/list_graph.h"
#include <lemon/adaptors.h>

#include "constitution/Graph.h"

namespace sylvanmats::constitution {
    struct unique_component{
        std::string label_comp_id;
        std::string label_asym_id;
        std::string pdbx_PDB_ins_code="?";
        int auth_seq_id;
    };

    class Selection{
    protected:
        Graph& graph;
    public:
        Selection(Graph& graph) : graph (graph) {};
        Selection(const Selection& orig) = delete;
        virtual ~Selection() = default;

        void operator()(std::vector<unique_component>& uniqueComponents, std::function<void(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subGraph)> apply){
            lemon::ListGraph::NodeMap<bool> selectionNodes(graph, false);
            lemon::ListGraph::EdgeMap<bool> selectionEdges(graph, false);
            lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> selectionGraph(graph, selectionNodes, selectionEdges);
            bool firstSite=false;
            for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
                if(within(uniqueComponents, graph.atomSites[nSiteA].label_comp_id, graph.atomSites[nSiteA].pdbx_PDB_ins_code, graph.atomSites[nSiteA].auth_seq_id, graph.atomSites[nSiteA].label_asym_id) && graph.atomSites[nSiteA].type_symbol.compare("FE")!=0){
                    selectionGraph.enable(nSiteA);
                    for(lemon::ListGraph::IncEdgeIt eSiteA(graph, nSiteA); eSiteA!=lemon::INVALID; ++eSiteA){
//                    std::cout<<" "<<graph.atomSites[nSiteA].label_comp_id<<" "<<selectionGraph.status(nSiteA)<<" "<<selectionGraph.status(eSiteA)<<" "<<graph.atomSites[graph.oppositeNode(nSiteA, eSiteA)].auth_seq_id<<" "<<sequence<<std::endl;
                        if(!selectionGraph.status(eSiteA) && within(uniqueComponents, graph.atomSites[graph.oppositeNode(nSiteA, eSiteA)].label_comp_id, graph.atomSites[graph.oppositeNode(nSiteA, eSiteA)].pdbx_PDB_ins_code, graph.atomSites[graph.oppositeNode(nSiteA, eSiteA)].auth_seq_id, graph.atomSites[graph.oppositeNode(nSiteA, eSiteA)].label_asym_id) && graph.atomSites[nSiteA].type_symbol.compare("FE")!=0){
//std::cout<<"\t"<<graph.atomSites[nSiteA].label_comp_id<<" .. "<<graph.atomSites[graph.oppositeNode(nSiteA, eSiteA)].label_comp_id<<std::endl;
                            selectionGraph.enable(eSiteA);
                        }
                    }
                }
            }
            apply(selectionGraph);
        };

        inline bool within(std::vector<unique_component>& uniqueComponents, std::string& label_comp_id, std::string pdbx_PDB_ins_code, int auth_seq_id, std::string& label_asym_id){
            for(auto uc : uniqueComponents){
                if(uc.auth_seq_id!=auth_seq_id || uc.label_comp_id.compare(label_comp_id)!=0 || uc.label_asym_id.compare(label_asym_id)!=0 || uc.pdbx_PDB_ins_code.compare(pdbx_PDB_ins_code)!=0)return false;
            }
            return true;
        };
        
        void compliment(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subGraph, std::function<void(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> complimentGraph)> apply){
            lemon::ListGraph::NodeMap<bool> selectionNodes(graph, true);
            lemon::ListGraph::EdgeMap<bool> selectionEdges(graph, true);
            lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> selectionGraph(graph, selectionNodes, selectionEdges);
            bool firstSite=false;
            for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(subGraph); nSiteA!=lemon::INVALID; ++nSiteA){
                if(subGraph.status(nSiteA)){
                    selectionGraph.disable(nSiteA);
                    for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::IncEdgeIt eSiteA(subGraph, nSiteA); eSiteA!=lemon::INVALID; ++eSiteA){
                        if(selectionGraph.status(eSiteA) && subGraph.status(graph.oppositeNode(nSiteA, eSiteA))){
                            selectionGraph.disable(eSiteA);
                        }
                    }
                }
            }
            apply(selectionGraph);
        };

    };
}
