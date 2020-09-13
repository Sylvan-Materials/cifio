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
    "nodes": {
)";
        for(lemon::ListGraph::NodeIt nSiteA(jgfp.graph); nSiteA!=lemon::INVALID; ++nSiteA){
        os << R"(")"<<jgfp.graph.atomSites[nSiteA].label_atom_id<<R"(":{})";
        lemon::ListGraph::NodeIt n=nSiteA;
        ++n;
        if(n!=lemon::INVALID) os << R"(,)";
        }
os << R"(
  }
}
)";
        os << "}\n";
        return os;
    }
}
