#pragma once

#include <iostream>

#include "constitution/Graph.h"

namespace sylvanmats::publishing{

    /**Json Graph
    **/
    class JGFPublisher{
    protected:
        sylvanmats::constitution::Graph& graph;
        lemon::ListGraph::NodeMap<bool> selectionNodes;
        lemon::ListGraph::EdgeMap<bool> selectionEdges;
        const lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> subgraph;
    public:
        JGFPublisher(sylvanmats::constitution::Graph& graph): graph (graph), selectionNodes(graph, true), selectionEdges(graph, true), subgraph (lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>(graph, selectionNodes, selectionEdges)) {};
        JGFPublisher(sylvanmats::constitution::Graph& graph, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subgraph): graph (graph), selectionNodes(graph, false), selectionEdges(graph, false), subgraph (subgraph) {};
        JGFPublisher(const JGFPublisher& orig) = delete;
        virtual ~JGFPublisher() = default;

        friend std::ostream& operator<<(std::ostream& os, const JGFPublisher& jgfp);
    };

    std::ostream& operator<<(std::ostream& os, const JGFPublisher& jgfp);
}
