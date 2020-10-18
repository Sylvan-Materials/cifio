//#include <format>

#include "constitution/Graph.h"

#include <lemon/suurballe.h>

namespace sylvanmats::constitution {
    void Graph::identifyFusedSystems(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph, std::function<void(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subSelectionGraph)> apply){
            lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<int> compMap(selectionGraph);
            int compCount=lemon::biEdgeConnectedComponents(selectionGraph, compMap);
            std::unique_ptr<int[]> comps(new int[compCount]);
            for(unsigned int i=0;i<compCount;i++)comps[i]=0;
            for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(selectionGraph); nSiteA!=lemon::INVALID; ++nSiteA){
                std::cout<<atomSites[nSiteA].label_atom_id<<" "<<compMap[nSiteA]<<std::endl;
                comps[compMap[nSiteA]]++;
            }
            for(unsigned int i=0;i<compCount;i++){
                if(comps[i]<=1)continue;
                lemon::ListGraph::NodeMap<bool> selectionNodes(*this, false);
                lemon::ListGraph::EdgeMap<bool> selectionEdges(*this, false);
                lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> subSelectionGraph(*this, selectionNodes, selectionEdges);
                for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(selectionGraph); nSiteA!=lemon::INVALID; ++nSiteA){
                    if(compMap[nSiteA]==i){
                        subSelectionGraph.enable(nSiteA);
                        for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::IncEdgeIt eSiteA(selectionGraph, nSiteA); eSiteA!=lemon::INVALID; ++eSiteA){
                            if(compMap[selectionGraph.oppositeNode(nSiteA, eSiteA)]==i){
                                subSelectionGraph.enable(eSiteA);
                            }
                        }
                    }
                }
                apply(subSelectionGraph);
            }
    };

    void Graph::identifyRings(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subGraph){\
        lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::EdgeMap<int> lengthMap(subGraph);
        lemon::Suurballe<lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::EdgeMap<int>> suurballe(subGraph, lengthMap);
        lemon::ListGraph::Node startNode=lemon::INVALID;
        for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(subGraph); startNode==lemon::INVALID && nSiteA!=lemon::INVALID; ++nSiteA){
            if(lemon::countIncEdges(subGraph, nSiteA)==2)startNode=nSiteA;
        }
        int currRing=1;
        lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::IncEdgeIt eSiteA(subGraph, startNode);
        suurballe.run(startNode, subGraph.oppositeNode(startNode, eSiteA));
        std::cout<<"suurballe.pathNum() "<<suurballe.pathNum()<<std::endl;
        for(unsigned int i=1;i<suurballe.pathNum();i++){
            std::cout<<"\tsuurballe.path("<<i<<") "<<suurballe.path(i).length()<<std::endl;
            compBond[eSiteA].ring=currRing;
            for(unsigned int j=0;j<suurballe.path(i).length();j++){
                compBond[suurballe.path(i).nth(j)].ring=currRing;
                std::cout<<" "<<atomSites[subGraph.source(suurballe.path(i).nth(j))].label_atom_id<<std::endl;
                if(j==suurballe.path(i).length()-1)std::cout<<" "<<atomSites[subGraph.target(suurballe.path(i).nth(j))].label_atom_id<<std::endl;
            }
            currRing++;
        }
        
    };

}