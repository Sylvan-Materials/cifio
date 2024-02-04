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
        
        struct vdw{
        double epsilon=0.0;
        double rmin_half=0.0;
        };
    class OpenFF{
    protected:
        sylvanmats::constitution::Graph& graph;
        sylvanmats::reading::SMIRKSPatterns smirksPatterns;
        
        double Ebond=0.0;
        double Eangle=0.0;
        double Eproper=0.0;
        double Eimproper=0.0;
        double Evdw=0.0;
        sylvanmats::linear::ArrayXd g;
    public:
        OpenFF() = delete;
        OpenFF(sylvanmats::constitution::Graph& graph);
        OpenFF(const OpenFF& orig) = delete;
        virtual ~OpenFF() =  default;
        
        void operator()();
        
        double getEnergy(){return Ebond+Eangle+Eproper+Eimproper+Evdw;};
        sylvanmats::linear::ArrayXd& getGradients(){return g;};
        
    private:
        inline double findAngleBetween(sylvanmats::linear::Vector3d v1, sylvanmats::linear::Vector3d v2){
            double dot=v1.dot(v2);
            return std::acos(dot/(v1.norm()*v2.norm()));
        }
        
        inline double findDihedralBetween(sylvanmats::linear::Vector3d v1, sylvanmats::linear::Vector3d v2, sylvanmats::linear::Vector3d v3, sylvanmats::linear::Vector3d v4){
            sylvanmats::linear::Vector3d b1=v2-v1;
            sylvanmats::linear::Vector3d b2=v3-v2;
            sylvanmats::linear::Vector3d b3=v4-v3;
            sylvanmats::linear::Vector3d n1=b1.cross(b2).normalized();
            sylvanmats::linear::Vector3d n2=b2.cross(b3).normalized();
            double x=n1.dot(n2);
            double y=n1.cross(b2.normalized()).dot(n2);
            return std::atan2(y, x);
        }
        
    };
    }
}
