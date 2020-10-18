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
        JGFPublisher(sylvanmats::constitution::Graph& graph): graph (graph), selectionNodes(graph, true), selectionEdges(graph, true), subgraph (lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>(graph, selectionNodes, selectionEdges)) {std::cout<<"jgf construct "<<std::endl;};
        JGFPublisher(sylvanmats::constitution::Graph& graph, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subgraph): graph (graph), selectionNodes(graph, false), selectionEdges(graph, false), subgraph (subgraph) {};
        JGFPublisher(const JGFPublisher& orig) = delete;
        virtual ~JGFPublisher() = default;

        friend std::ostream& operator<<(std::ostream& os, const JGFPublisher& jgfp);
    };

    std::ostream& operator<<(std::ostream& os, const JGFPublisher& jgfp)
    {
        os << R"({
"graph" : {
    "directed": false,
    "type": "constitutional",
    "nodes": {
       )";
        std::vector<lemon::ListGraph::Node> nodes;
        for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(jgfp.subgraph); nSiteA!=lemon::INVALID; ++nSiteA){
           nodes.push_back(nSiteA);
        }
        unsigned long long count=0;
        for(lemon::ListGraph::Node n: nodes | std::views::reverse){
            os << R"(")"<<jgfp.graph.atomSites[n].id<<R"(":{"label": ")"+jgfp.graph.atomSites[n].label_atom_id+R"(", "metadata": {"comp_id":")"+jgfp.graph.atomSites[n].label_comp_id+R"(","seq_id":")"+std::to_string(jgfp.graph.atomSites[n].auth_seq_id)+R"(","type_symbol":")"+jgfp.graph.atomSites[n].type_symbol+R"("}})";
            if(count<nodes.size()-1) os << R"(,
       )";
            count++;
        }
os << R"(
  }
,
    "edges": [
               )";
       count=0;
       for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::EdgeIt e(jgfp.subgraph); e!=lemon::INVALID; ++e){
        os << R"(
          {
            "source": ")"+std::to_string(jgfp.graph.atomSites[jgfp.graph.u(e)].id)+R"(",
            "target": ")"+std::to_string(jgfp.graph.atomSites[jgfp.graph.v(e)].id)+R"(",
            "label": ")"+std::to_string(jgfp.graph.compBond[e].value_order)+R"(",
            "ring": )"+std::to_string(jgfp.graph.compBond[e].ring)+R"()";
        os << R"(
          })";
            if(count<lemon::countEdges(jgfp.subgraph)-1) os << R"(,
       )";
            count++;
       }
os << R"(
             ]
}
)";
        os << "}\n";
        return os;
    }
}
