

#include "constitution/Graph.h"
#include "constitution/Selection.h"

namespace sylvanmats::constitution {
    void Graph::identifyFusedSystems(){
        std::cout<<"identifyFusedSystems "<<std::endl;
        Selection selection(*this);
        selection(154, [](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph){
            lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<int> compMap(selectionGraph);
            int compCount=lemon::biEdgeConnectedComponents(selectionGraph, compMap);
            std::cout<<"compCount "<<compCount<<" "<<lemon::countNodes(selectionGraph)<<" "<<lemon::countEdges(selectionGraph)<<std::endl;
        });

    };

}