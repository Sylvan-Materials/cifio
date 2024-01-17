#pragma once

#include "constitution/Graph.h"
#include "forcefield/SMIRKSPatterns.h"
#include "linear/Vector.h"

#include "lemon/list_graph.h"
#include "lemon/connectivity.h"

namespace sylvanmats::forcefield {
    
    class OpenFF{
    protected:
        SMIRKSPatterns smirksPatterns;
    public:
        OpenFF();
        OpenFF(const OpenFF& orig) = delete;
        virtual ~OpenFF() =  default;
        
        void operator()(sylvanmats::constitution::Graph& graph);
    private:
        inline double findAngleBetween(sylvanmats::linear::Vector3d v1, sylvanmats::linear::Vector3d v2){
            double dot=v1.dot(v2);
            //std::cout<<"dot "<<dot<<std::endl;
            return std::acos(dot/(v1.norm()*v2.norm()));
//            return (dot>=0) ? std::acos(dot/(v1.norm()*v2.norm())) : 2.0*std::numbers::pi-std::acos(dot/(v1.norm()*v2.norm()));
//            v2.normalize();
//            return (v2.y()>=0.0) ? std::atan2(v2.y(), v2.x()) : 2.0*std::numbers::pi + std::atan2(v2.y(), v2.x()); 
        }
        
    };
}
