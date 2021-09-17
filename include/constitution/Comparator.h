#pragma once

#include <stdlib.h>
#include <string>
#include <memory>
#include <tuple>

#include "lemon/list_graph.h"
#include <lemon/adaptors.h>

#include "constitution/Graph.h"

namespace sylvanmats::constitution {

    class Comparator{
    protected:
    public:
        Comparator() = default;
        Comparator(const Comparator& orig) = delete;
        virtual ~Comparator() = default;
        void operator()(sylvanmats::constitution::Graph& graphA, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subgraphA, sylvanmats::constitution::Graph& graphB, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subgraphB){
            
        }
        
        double compareCentroids(sylvanmats::constitution::Graph& graphA, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subgraphA, sylvanmats::constitution::Graph& graphB, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subgraphB);
    };
        
}