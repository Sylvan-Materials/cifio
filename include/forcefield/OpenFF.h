#pragma once

#include "constitution/Graph.h"
#include "reading/smirnoff/SMIRKSPatterns.h"
#include "linear/Vector.h"

#include "lemon/list_graph.h"
#include "lemon/connectivity.h"

namespace sylvanmats{
    namespace reading {

    bool operator==(const std::vector<sylvanmats::reading::atom_primitive>& ltp, const sylvanmats::constitution::_atom_site<double>& rtp);
}
    namespace forcefield {
    class OpenFF{
    protected:
        sylvanmats::reading::SMIRKSPatterns smirksPatterns;
    public:
        OpenFF();
        OpenFF(const OpenFF& orig) = delete;
        virtual ~OpenFF() =  default;
        
        void operator()(sylvanmats::constitution::Graph& graph);
    private:
        inline double findAngleBetween(sylvanmats::linear::Vector3d v1, sylvanmats::linear::Vector3d v2){
            double dot=v1.dot(v2);
            return std::acos(dot/(v1.norm()*v2.norm()));
        }
        
    };
    }
}
