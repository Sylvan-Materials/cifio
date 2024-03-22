#pragma once

#include "constitution/Graph.h"
#include "linear/Array.h"
#include "algebra/geometric/Motor.h"
#include <functional>

namespace sylvanmats::dynamics{
    struct particle_connection{
        lemon::ListGraph::Node u=lemon::INVALID;
        lemon::ListGraph::Node v=lemon::INVALID;
        size_t uIsland=0;
        size_t vIsland=0;
        std::vector<sylvanmats::algebra::geometric::Motord> Ms;
        size_t i=0;
    };
    class Particles{
    protected:
        sylvanmats::constitution::Graph& graph;
        lemon::ListGraph::NodeMap<bool> selectionNodes;
        lemon::ListGraph::EdgeMap<bool> selectionEdges;
        lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> subgraph;
        lemon::ListGraph particlesGraph;
        lemon::ListGraph::NodeMap<int> particlesMap;
        lemon::ListGraph::EdgeMap<particle_connection> particlesEdgeMap;
        lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node> sitesMap;
    public:
        Particles(sylvanmats::constitution::Graph& graph);
        Particles(sylvanmats::constitution::Graph& graph, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subgraph);
        Particles(const Particles& orig) = delete;
        virtual ~Particles() = default;
    private:
        void init(sylvanmats::constitution::Graph& graph, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subgraph);
    public:
        void operator()(std::function<void(lemon::ListGraph& particlesGraph, lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>& sitesMap)> apply);

        void attachOperation(std::function<sylvanmats::linear::ArrayXd(lemon::ListGraph::Node& a, lemon::ListGraph::Node& b, lemon::ListGraph::Node& c, lemon::ListGraph::Node& d)> apply);
        void resetOriginalState();
        void feed(std::function<bool(sylvanmats::constitution::Graph& graph)> apply);
        size_t getNumberOfCombinatorics();
        size_t getCombinatoricSize();
        size_t getNumberOfParticles(){return lemon::countNodes(particlesGraph);};
        size_t getNumberOfConnections(){return lemon::countEdges(particlesGraph);};
    };
    
}

