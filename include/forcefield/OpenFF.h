#pragma once

#include "constitution/Graph.h"
#include "reading/smirnoff/SMIRKSPatterns.h"
#include "linear/Vector.h"
#include "linear/Array.h"

#include "lemon/list_graph.h"
#include "lemon/connectivity.h"

namespace sylvanmats{
    namespace reading {

    bool operator==(const std::vector<sylvanmats::reading::atom_primitive>& ltp, const sylvanmats::constitution::_atom_site<double>& rtp);
}
    namespace forcefield {
    class OpenFF{
    protected:
        sylvanmats::constitution::Graph& graph;
        sylvanmats::reading::SMIRKSPatterns smirksPatterns;
        
        double Ebond=0.0;
        double Eangle=0.0;
        double Eproper=0.0;
        double Eimproper=0.0;
        sylvanmats::linear::ArrayXd g;
    public:
        OpenFF() = delete;
        OpenFF(sylvanmats::constitution::Graph& graph);
        OpenFF(const OpenFF& orig) = delete;
        virtual ~OpenFF() =  default;
        
        void operator()();
        
        double getEnergy(){return Ebond+Eangle;};
        sylvanmats::linear::ArrayXd& getGradients(){return g;};
        
    private:
        inline double findAngleBetween(sylvanmats::linear::Vector3d v1, sylvanmats::linear::Vector3d v2){
            double dot=v1.dot(v2);
            return std::acos(dot/(v1.norm()*v2.norm()));
        }
        
    };
    }
}
