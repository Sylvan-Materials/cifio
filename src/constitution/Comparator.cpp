
#include "constitution/Comparator.h"

#include "linear/Vector.h"

namespace sylvanmats::constitution {
    double Comparator::compareCentroids(sylvanmats::constitution::Graph& graphA, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subgraphA, sylvanmats::constitution::Graph& graphB, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subgraphB){
        sylvanmats::linear::Vector3d A = {0.0, 0.0, 0.0};
        sylvanmats::linear::Vector3d B = {0.0, 0.0, 0.0};
        unsigned count=0;
        for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(subgraphA); nSiteA!=lemon::INVALID; ++nSiteA){
            if(graphA.atomSites[nSiteA].type_symbol.compare("H")!=0){
                A+=sylvanmats::linear::Vector3d(graphA.atomSites[nSiteA].Cartn_x, graphA.atomSites[nSiteA].Cartn_y, graphA.atomSites[nSiteA].Cartn_z);
                count++;
            }
        }
        if(count>0)A/=count;
        count=0;
        for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteB(subgraphB); nSiteB!=lemon::INVALID; ++nSiteB){
            if(graphB.atomSites[nSiteB].type_symbol.compare("H")!=0){
                B+=sylvanmats::linear::Vector3d(graphB.atomSites[nSiteB].Cartn_x, graphB.atomSites[nSiteB].Cartn_y, graphB.atomSites[nSiteB].Cartn_z);
                count++;
            }
        }
        if(count>0)B/=count;
        
        return (A-B).norm();
    }

}
