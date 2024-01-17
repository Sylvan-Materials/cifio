#include "forcefield/OpenFF.h"
#include "PeriodicTable.h"
#include "linear/Vector.h"

namespace sylvanmats::forcefield {

    OpenFF::OpenFF() : smirksPatterns(SMIRKSPatterns()){
    }
    
    void OpenFF::operator()(sylvanmats::constitution::Graph& graph){
        sylvanmats::PeriodicTable* periodicTable=sylvanmats::PeriodicTable::getInstance();
        double Ebond=0.0;
        double Eangle=0.0;
        for (lemon::ListGraph::EdgeIt eSiteA(graph); eSiteA!=lemon::INVALID; ++eSiteA){
            lemon::ListGraph::Node nSiteA=graph.u(eSiteA);
            lemon::ListGraph::Node nSiteB=graph.v(eSiteA);
            sylvanmats::element eleA=periodicTable->index(graph.atomSites[nSiteA].type_symbol);
            sylvanmats::forcefield::BOND_TYPE type=sylvanmats::forcefield::BOND_ANY;
            if(graph.compBond[eSiteA].value_order==3)type=sylvanmats::forcefield::BOND_TRIPLE;
            else if(graph.compBond[eSiteA].value_order==2)type=sylvanmats::forcefield::BOND_DOUBLE;
            else type=sylvanmats::forcefield::BOND_SINGLE;
            sylvanmats::element eleB=periodicTable->index(graph.atomSites[nSiteB].type_symbol);
            smirksPatterns(eleA.atomic_number, lemon::countIncEdges(graph, nSiteA), type, lemon::countIncEdges(graph, nSiteB), eleB.atomic_number, [&](double length, double k, smirks_pattern& smirksPattern){
                double norm=(sylvanmats::linear::Vector3d(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z)-sylvanmats::linear::Vector3d(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z)).norm();
                double eb=(k/2.0)*std::pow(norm-length, 2);
                double deb=eb/norm;
                Ebond+=eb;
                std::cout<<"\t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.compBond[eSiteA].value_order<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<" "<<norm<<" "<<length<<" "<<k<<" "<<eb<<" "<<deb<<std::endl;

            });
        }
        std::cout<<std::endl;
        for (lemon::ListGraph::NodeIt nSiteB(graph); nSiteB!=lemon::INVALID; ++nSiteB){
            for (lemon::ListGraph::IncEdgeIt eSiteA(graph, nSiteB); eSiteA!=lemon::INVALID; ++eSiteA){
                lemon::ListGraph::Node nSiteA=graph.runningNode(eSiteA);
                bool unique=false;
                for (lemon::ListGraph::IncEdgeIt eSiteB(graph, nSiteB); eSiteB!=lemon::INVALID; ++eSiteB){
                    lemon::ListGraph::Node nSiteC=graph.runningNode(eSiteB);
                    if(nSiteA==nSiteC){
                        unique=true;
                    }
                    else if(unique && nSiteA!=nSiteC){
//                        std::cout<<"\t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<graph.atomSites[nSiteC].label_atom_id<<std::endl;
                    sylvanmats::element eleA=periodicTable->index(graph.atomSites[nSiteA].type_symbol);
                    sylvanmats::forcefield::BOND_TYPE typeA=sylvanmats::forcefield::BOND_ANY;
                    if(graph.compBond[eSiteA].value_order==3)typeA=sylvanmats::forcefield::BOND_TRIPLE;
                    else if(graph.compBond[eSiteA].value_order==2)typeA=sylvanmats::forcefield::BOND_DOUBLE;
                    else typeA=sylvanmats::forcefield::BOND_SINGLE;
                    sylvanmats::element eleB=periodicTable->index(graph.atomSites[nSiteB].type_symbol);
                    sylvanmats::forcefield::BOND_TYPE typeB=sylvanmats::forcefield::BOND_ANY;
                    if(graph.compBond[eSiteB].value_order==3)typeB=sylvanmats::forcefield::BOND_TRIPLE;
                    else if(graph.compBond[eSiteB].value_order==2)typeB=sylvanmats::forcefield::BOND_DOUBLE;
                    else typeB=sylvanmats::forcefield::BOND_SINGLE;
                    sylvanmats::element eleC=periodicTable->index(graph.atomSites[nSiteC].type_symbol);
                    smirksPatterns(eleA.atomic_number, lemon::countIncEdges(graph, nSiteA), typeA, lemon::countIncEdges(graph, nSiteB), eleB.atomic_number, typeB, lemon::countIncEdges(graph, nSiteC), eleC.atomic_number, [&](double angle, double k, smirks_pattern& smirksPattern){
                        double norm=findAngleBetween(sylvanmats::linear::Vector3d(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z)-sylvanmats::linear::Vector3d(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z), sylvanmats::linear::Vector3d(graph.atomSites[nSiteC].Cartn_x, graph.atomSites[nSiteC].Cartn_y, graph.atomSites[nSiteC].Cartn_z)-sylvanmats::linear::Vector3d(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z));
                        double eb=(k/2.0)*std::pow(norm-angle, 2);
                        double deb=eb/norm;
                        Eangle+=eb;
                        std::cout<<"\t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.compBond[eSiteA].value_order<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<graph.compBond[eSiteB].value_order<<" "<<graph.atomSites[nSiteC].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<" "<<eleC.atomic_number<<" "<<norm<<" "<<angle<<" "<<k<<" "<<eb<<" "<<deb<<std::endl;

                    });
                        
                    }
                }
                
            }
            
        }
        std::cout<<"Ebond "<<Ebond<<" kcal/mol"<<std::endl;
        std::cout<<"Eangle "<<Eangle<<" kcal/mol"<<std::endl;
      
    }

}
