#pragma once

#include <iostream>

#include "constitution/Graph.h"

namespace sylvanmats::publishing{

    class JGFPublisher{
    protected:
        sylvanmats::constitution::Graph& graph;
    public:
        JGFPublisher(sylvanmats::constitution::Graph& graph): graph (graph) {};
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
        for(lemon::ListGraph::NodeIt nSiteA(jgfp.graph); nSiteA!=lemon::INVALID; ++nSiteA){
           nodes.push_back(nSiteA);
        }
        unsigned long long count=0;
        for(lemon::ListGraph::Node n: nodes | std::views::reverse){
            os << R"(")"<<jgfp.graph.atomSites[n].id<<R"(":{"label": ")"+jgfp.graph.atomSites[n].label_atom_id+R"(", "metadata": {"comp_id":")"+jgfp.graph.atomSites[n].label_comp_id+R"(","seq_id":")"+std::to_string(jgfp.graph.atomSites[n].auth_seq_id)+R"("}})";
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
       for(lemon::ListGraph::EdgeIt e(jgfp.graph); e!=lemon::INVALID; ++e){
        os << R"(
          {
            "source": ")"+std::to_string(jgfp.graph.atomSites[jgfp.graph.u(e)].id)+R"(",
            "target": ")"+std::to_string(jgfp.graph.atomSites[jgfp.graph.v(e)].id)+R"(",
            "label": ")"+std::to_string(jgfp.graph.compBond[e].value_order)+R"(")";
            if(count<lemon::countEdges(jgfp.graph)-1) os << R"(,
       )";
            count++;
        os << R"(
          }
)";
       }
os << R"(
             ]
}
)";
        os << "}\n";
        return os;
    }
}
