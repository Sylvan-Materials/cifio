#pragma once

#include <iostream>

#include "constitution/Graph.h"
#include "dynamics/Particles.h"

namespace sylvanmats::publishing{

    class SMILESPublisher{
    protected:
        sylvanmats::constitution::Graph& graph;
        sylvanmats::dynamics::Particles& particles;
    public:
        SMILESPublisher(sylvanmats::constitution::Graph& graph, sylvanmats::dynamics::Particles& particles): graph (graph), particles (particles) {};
        SMILESPublisher(const SMILESPublisher& orig) = delete;
        virtual ~SMILESPublisher() = default;

        friend std::ostream& operator<<(std::ostream& os, const SMILESPublisher& smilesp);
    };

    std::ostream& operator<<(std::ostream& os, const SMILESPublisher& smilesp);
}
