//#include <format>

#include "constitution/Graph.h"

#include <lemon/suurballe.h>

namespace sylvanmats::constitution {

    bool operator==(symmetry_labels& a, symmetry_labels& b){
        return (a.type_symbol.compare(b.type_symbol)==0);
    };

    void Graph::identifyFusedSystems(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph, std::function<void(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subSelectionGraph)> apply){
            lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<int> compMap(selectionGraph);
            int compCount=lemon::biEdgeConnectedComponents(selectionGraph, compMap);
            std::unique_ptr<int[]> comps(new int[compCount]);
            for(unsigned int i=0;i<compCount;i++)comps[i]=0;
            for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(selectionGraph); nSiteA!=lemon::INVALID; ++nSiteA){
                comps[compMap[nSiteA]]++;
            }
            for(unsigned int i=0;i<compCount;i++){
                if(comps[i]<=1){
                    //lemon::ListGraph::Edge flexibleEdge=lemon::INVALID;
                    for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(selectionGraph); nSiteA!=lemon::INVALID; ++nSiteA){
                        for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::IncEdgeIt eSiteA(selectionGraph, nSiteA); eSiteA!=lemon::INVALID; ++eSiteA){
                            if(compBond[eSiteA].value_order==1 && (lemon::countIncEdges(*this, selectionGraph.oppositeNode(nSiteA, eSiteA))+atomSites[selectionGraph.oppositeNode(nSiteA, eSiteA)].proton_count)>2){
                                //flexibleEdge=eSiteA;
                                compBond[eSiteA].flexible=true;
                            }
                        }
                    }
                    
                    continue;
                }
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
                            else if(compBond[eSiteA].value_order==1 && (lemon::countIncEdges(*this, selectionGraph.oppositeNode(nSiteA, eSiteA))+atomSites[selectionGraph.oppositeNode(nSiteA, eSiteA)].proton_count)>2)compBond[eSiteA].flexible=true;
                        }
                    }
                }
                apply(subSelectionGraph);
            }
    };

    void Graph::identifyRings(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subGraph){\
        lemon::ListGraph::NodeMap<bool> maskNodes(*this, false);
        lemon::ListGraph::EdgeMap<bool> maskEdges(*this, false);
        lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> maskGraph(*this, maskNodes, maskEdges);
        for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(subGraph); nSiteA!=lemon::INVALID; ++nSiteA){
            maskGraph.enable(nSiteA);
            for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::IncEdgeIt eSiteA(subGraph, nSiteA); eSiteA!=lemon::INVALID; ++eSiteA){
                    maskGraph.enable(eSiteA);
            }
        }
       lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::EdgeMap<int> lengthMap(maskGraph);
        lemon::Suurballe<lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::EdgeMap<int>> suurballe(maskGraph, lengthMap);

        lemon::ListGraph::Node startNode=lemon::INVALID;
        do{
            startNode=lemon::INVALID;
            for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(maskGraph); startNode==lemon::INVALID && nSiteA!=lemon::INVALID; ++nSiteA){
                if(lemon::countIncEdges(subGraph, nSiteA)==2)startNode=nSiteA;
            }
            if(startNode!=lemon::INVALID){
                lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::IncEdgeIt eSiteA(maskGraph, startNode);
                suurballe.run(startNode, subGraph.oppositeNode(startNode, eSiteA));
                for(unsigned int i=0;i<suurballe.pathNum();i++){
                    if(suurballe.path(i).length()<2 || suurballe.path(i).length()>8)continue;
                    currRing++;
                    compBond[eSiteA].ring=currRing;
                    //std::cout<<i<<" ";
                    for(unsigned int j=0;j<suurballe.path(i).length();j++){
                        compBond[suurballe.path(i).nth(j)].ring=currRing;
                        //std::cout<<atomSites[subGraph.source(suurballe.path(i).nth(j))].label_atom_id<<" ";
                        if(j==suurballe.path(i).length()-1)std::cout<<atomSites[subGraph.target(suurballe.path(i).nth(j))].label_atom_id<<std::endl;
                    }
                    //std::cout<<std::endl;
                 }
                maskGraph.disable(startNode);
                maskGraph.disable(eSiteA);
            }
        }while(startNode!=lemon::INVALID);
        
    };

}