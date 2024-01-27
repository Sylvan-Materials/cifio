#include "forcefield/OpenFF.h"
#include "PeriodicTable.h"
#include "linear/Vector.h"

#include "lemon/list_graph.h"
#include "lemon/vf2.h"

namespace sylvanmats{
    namespace reading {

    bool operator==(const std::vector<sylvanmats::reading::atom_primitive>& ltp, const sylvanmats::constitution::_atom_site<double>& rtp){
        sylvanmats::PeriodicTable* periodicTable=sylvanmats::PeriodicTable::getInstance();
        sylvanmats::element ele=periodicTable->index((std::string&)rtp.type_symbol);
        bool an=false;
        bool rs=false;
        bool fc=false;
        bool connectivity=false;
        for(std::vector<sylvanmats::reading::atom_primitive>::const_iterator it=ltp.begin();(!an || !rs || !connectivity) && it!=ltp.end();it++){
            if((*it).wildcard || (*it).atomic_number==ele.atomic_number)an=true;
            if((*it).ring_size==0 || (*it).ring_size==rtp.sssr_ring)rs=true;
            if((*it).formal_charge==0)fc=true;
            if((*it).connectivity==0 || (*it).connectivity==rtp.connectivity)connectivity=true;
        }
        return an && (rs) && (connectivity) && (fc /*ltp.front().formal_charge==0 || ltp.front().formal_charge==rtp.sssr_ring*/);
    };
}
    namespace forcefield{
        
    OpenFF::OpenFF(sylvanmats::constitution::Graph& graph) : graph (graph), smirksPatterns(sylvanmats::reading::SMIRKSPatterns()), g(sylvanmats::linear::ArrayXd(3*lemon::countNodes(graph))) {}
    
    void OpenFF::operator()(){
        sylvanmats::PeriodicTable* periodicTable=sylvanmats::PeriodicTable::getInstance();
        Ebond=0.0;
        Eangle=0.0;
        size_t nodeCount=0;
        size_t edgeCount=0;
        size_t incidentCount=0;
        size_t properCount=0;
        lemon::ListGraph::NodeMap<bool> patternNodes(graph, false);
        lemon::ListGraph::EdgeMap<bool> patternEdges(graph, false);
        lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> patternGraph(graph, patternNodes, patternEdges);
        for (lemon::ListGraph::EdgeIt eSiteB(graph); eSiteB!=lemon::INVALID; ++eSiteB){
            lemon::ListGraph::Node nSiteB=graph.u(eSiteB);
            lemon::ListGraph::Node nSiteC=graph.v(eSiteB);
            patternGraph.enable(nSiteB);
            patternGraph.enable(nSiteC);
            patternGraph.enable(eSiteB);
            sylvanmats::element eleA=periodicTable->index(graph.atomSites[nSiteB].type_symbol);
//            if(graph.compBond[eSiteB].ring>0)graph.compBond[eSiteB].type=sylvanmats::forcefield::BOND_ANY_RING;
//            sylvanmats::forcefield::BOND_TYPE type=sylvanmats::forcefield::BOND_ANY;
//            if(graph.compBond[eSiteB].value_order==3)type=sylvanmats::forcefield::BOND_TRIPLE;
//            else if(graph.compBond[eSiteB].value_order==2)type=sylvanmats::forcefield::BOND_DOUBLE;
//            else type=sylvanmats::forcefield::BOND_SINGLE;
            sylvanmats::element eleB=periodicTable->index(graph.atomSites[nSiteC].type_symbol);
            graph.atomSites[nSiteB].connectivity=lemon::countIncEdges(graph, nSiteB);
            graph.atomSites[nSiteC].connectivity=lemon::countIncEdges(graph, nSiteC);
            bool hit=false;
            smirksPatterns(eleA.atomic_number, graph.atomSites[nSiteB].connectivity, graph.compBond[eSiteB].type, graph.atomSites[nSiteC].connectivity, eleB.atomic_number, [&](double length, double k, sylvanmats::reading::smirks_pattern& smirksPattern)->bool{
                lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<lemon::ListGraph::Node> bijectionMap(patternGraph);
//                std::cout<<smirksPattern.smirks<<" "<<lemon::countNodes(smirksPattern.smirksGraph)<<" "<<lemon::countEdges(smirksPattern.smirksGraph)<<std::endl;
                bool ret = lemon::vf2(smirksPattern.smirksGraph, patternGraph).induced().mapping(bijectionMap).nodeLabels<lemon::ListGraph::NodeMap<std::vector<sylvanmats::reading::atom_primitive>>, lemon::ListGraph::NodeMap<sylvanmats::constitution::_atom_site<double>>>(smirksPattern.atomicPrimitives, graph.atomSites).run();
                if(ret){
                    sylvanmats::linear::Vector3d pointB(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z);
                    sylvanmats::linear::Vector3d pointC(graph.atomSites[nSiteC].Cartn_x, graph.atomSites[nSiteC].Cartn_y, graph.atomSites[nSiteC].Cartn_z);
                double norm=(pointB-pointC).norm();
                double eb=(k/2.0)*std::pow(norm-length, 2);
                double deb=k*(norm-length);
                g[std::slice(3*graph.id(nSiteB), 3, 1)] += -deb*(pointB-pointC)/norm;
                g[std::slice(3*graph.id(nSiteC), 3, 1)] += deb*(pointB-pointC)/norm;
                Ebond+=eb;
                std::cout<<"\t"<<graph.atomSites[nSiteB].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteB].value_order)<<" "<<graph.atomSites[nSiteC].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<" "<<norm<<" "<<length<<" "<<k<<" "<<eb<<" "<<deb<<" "<<smirksPattern.smirks<<std::endl;
                edgeCount++;
                hit=true;
                }
                return ret;
            });
            if(!hit)std::cout<<"missed \t"<<graph.atomSites[nSiteB].label_atom_id<<" "<<graph.compBond[eSiteB].type<<"("<<static_cast<int>(graph.compBond[eSiteB].value_order)<<") "<<graph.atomSites[nSiteC].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<std::endl;
            for (lemon::ListGraph::IncEdgeIt eSiteA(graph, nSiteB); eSiteA!=lemon::INVALID; ++eSiteA){
                patternGraph.enable(eSiteA);
                for (lemon::ListGraph::IncEdgeIt eSiteC(graph, nSiteC); eSiteC!=lemon::INVALID; ++eSiteC){
                    patternGraph.enable(eSiteC);
                    if(graph.runningNode(eSiteA)!=nSiteC && graph.runningNode(eSiteC)!=nSiteB){
                        lemon::ListGraph::Node nSiteA=graph.runningNode(eSiteA);
                        lemon::ListGraph::Node nSiteD=graph.runningNode(eSiteC);
                        if(nSiteA!=nSiteC && nSiteD!=nSiteB){
                        patternGraph.enable(nSiteA);
                        patternGraph.enable(nSiteD);
                        sylvanmats::element eleA=periodicTable->index(graph.atomSites[nSiteA].type_symbol);
                        sylvanmats::element eleB=periodicTable->index(graph.atomSites[nSiteB].type_symbol);
                        sylvanmats::element eleC=periodicTable->index(graph.atomSites[nSiteC].type_symbol);
                        sylvanmats::element eleD=periodicTable->index(graph.atomSites[nSiteD].type_symbol);
                        graph.atomSites[nSiteA].connectivity=lemon::countIncEdges(graph, nSiteA);
                        graph.atomSites[nSiteD].connectivity=lemon::countIncEdges(graph, nSiteD);
                        bool hit=false;
                        smirksPatterns(eleA.atomic_number, graph.atomSites[nSiteA].connectivity, graph.compBond[eSiteA].type, graph.atomSites[nSiteB].connectivity, eleB.atomic_number, graph.compBond[eSiteB].type, graph.atomSites[nSiteC].connectivity, eleC.atomic_number, graph.compBond[eSiteC].type, graph.atomSites[nSiteD].connectivity, eleD.atomic_number, [&](sylvanmats::reading::smirks_torsions_pattern& smirksPattern)->bool{
                            lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<lemon::ListGraph::Node> bijectionMap(patternGraph);
//                            std::cout<<smirksPattern.smirks<<" "<<lemon::countNodes(smirksPattern.smirksGraph)<<" "<<lemon::countEdges(smirksPattern.smirksGraph)<<std::endl;
                            bool ret = lemon::vf2(smirksPattern.smirksGraph, patternGraph).induced().mapping(bijectionMap).nodeLabels<lemon::ListGraph::NodeMap<std::vector<sylvanmats::reading::atom_primitive>>, lemon::ListGraph::NodeMap<sylvanmats::constitution::_atom_site<double>>>(smirksPattern.atomicPrimitives, graph.atomSites).run();
                            if(ret){
                                sylvanmats::linear::Vector3d pointA(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z);
                                sylvanmats::linear::Vector3d pointB(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z);
                                sylvanmats::linear::Vector3d pointC(graph.atomSites[nSiteC].Cartn_x, graph.atomSites[nSiteC].Cartn_y, graph.atomSites[nSiteC].Cartn_z);
                                sylvanmats::linear::Vector3d pointD(graph.atomSites[nSiteD].Cartn_x, graph.atomSites[nSiteD].Cartn_y, graph.atomSites[nSiteD].Cartn_z);
                                double dihedral=findDihedralBetween(pointA, pointB, pointC, pointD);
                                
                                double et=smirksPattern.k1Value*(1.0+std::cos(smirksPattern.periodicity1Value*dihedral-smirksPattern.phase1Value))+smirksPattern.k2Value*(1.0+std::cos(smirksPattern.periodicity2Value*dihedral-smirksPattern.phase2Value))+smirksPattern.k3Value*(1.0+std::cos(smirksPattern.periodicity3Value*dihedral-smirksPattern.phase3Value))+smirksPattern.k4Value*(1.0+std::cos(smirksPattern.periodicity4Value*dihedral-smirksPattern.phase4Value));
//////                            double deb=k*(norm-angle);
//////
                                Eproper+=et;
////                            std::cout<<"\t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteA].value_order)<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteB].value_order)<<" "<<graph.atomSites[nSiteC].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<" "<<eleC.atomic_number<<" "<<norm<<" "<<angle<<" "<<k<<" "<<eb<<" "<<deb<<std::endl;
                            properCount++;
                            hit=true;
                            }
                            return ret;
                        });
                        if(!hit)std::cout<<"missed \t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteA].value_order)<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteB].value_order)<<" "<<graph.atomSites[nSiteC].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteC].value_order)<<" "<<graph.atomSites[nSiteD].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<" "<<eleC.atomic_number<<" "<<static_cast<int>(graph.atomSites[nSiteA].sssr_ring)<<" "<<static_cast<int>(graph.atomSites[nSiteB].sssr_ring)<<" "<<static_cast<int>(graph.atomSites[nSiteC].sssr_ring)<<std::endl;
                        
                        patternGraph.disable(nSiteD);
                        patternGraph.disable(nSiteA);
                        }
                    }
                    patternGraph.disable(eSiteC);
                }
                patternGraph.disable(eSiteA);
            }
            patternGraph.disable(nSiteB);
            patternGraph.disable(nSiteC);
            patternGraph.disable(eSiteB);
            
        }
        std::cout<<std::endl;
        for (lemon::ListGraph::NodeIt nSiteB(graph); nSiteB!=lemon::INVALID; ++nSiteB){
            patternGraph.enable(nSiteB);
            incidentCount+=lemon::countIncEdges(graph, nSiteB)*(lemon::countIncEdges(graph, nSiteB)-1)/2;
            for (lemon::ListGraph::IncEdgeIt eSiteA(graph, nSiteB); eSiteA!=lemon::INVALID; ++eSiteA){
                lemon::ListGraph::Node nSiteA=graph.runningNode(eSiteA);
                patternGraph.enable(nSiteA);
                patternGraph.enable(eSiteA);
                bool unique=false;
                for (lemon::ListGraph::IncEdgeIt eSiteB(graph, nSiteB); eSiteB!=lemon::INVALID; ++eSiteB){
                    lemon::ListGraph::Node nSiteC=graph.runningNode(eSiteB);
                    if(nSiteA==nSiteC){
                        unique=true;
                    }
                    else if(unique && nSiteA!=nSiteC){
                        patternGraph.enable(nSiteC);
                        patternGraph.enable(eSiteB);
//                        std::cout<<"\t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<graph.atomSites[nSiteC].label_atom_id<<std::endl;
                    sylvanmats::element eleA=periodicTable->index(graph.atomSites[nSiteA].type_symbol);
                    sylvanmats::element eleB=periodicTable->index(graph.atomSites[nSiteB].type_symbol);
                    sylvanmats::element eleC=periodicTable->index(graph.atomSites[nSiteC].type_symbol);
                    bool hit=false;
                    smirksPatterns(eleA.atomic_number, graph.atomSites[nSiteA].connectivity, graph.compBond[eSiteA].type, graph.atomSites[nSiteB].connectivity, eleB.atomic_number, graph.compBond[eSiteB].type, graph.atomSites[nSiteC].connectivity, eleC.atomic_number, [&](double angle, double k, sylvanmats::reading::smirks_pattern& smirksPattern)->bool{
                        lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<lemon::ListGraph::Node> bijectionMap(patternGraph);
                        bool ret = lemon::vf2(smirksPattern.smirksGraph, patternGraph).induced().mapping(bijectionMap).nodeLabels<lemon::ListGraph::NodeMap<std::vector<sylvanmats::reading::atom_primitive>>, lemon::ListGraph::NodeMap<sylvanmats::constitution::_atom_site<double>>>(smirksPattern.atomicPrimitives, graph.atomSites).run();
                        if(ret){
                            sylvanmats::linear::Vector3d pointA(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z);
                            sylvanmats::linear::Vector3d pointB(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z);
                            sylvanmats::linear::Vector3d pointC(graph.atomSites[nSiteC].Cartn_x, graph.atomSites[nSiteC].Cartn_y, graph.atomSites[nSiteC].Cartn_z);
                            sylvanmats::linear::Vector3d vectorAB=pointA-pointB;
                            sylvanmats::linear::Vector3d vectorCB=pointC-pointB;
                        double norm=findAngleBetween(vectorAB, vectorCB);
                        double eb=(k/2.0)*std::pow(norm-angle, 2);
                        double deb=k*(norm-angle);
                        g[3*graph.id(nSiteB)] += (vectorAB.cross(vectorAB.cross(vectorCB))).norm()/vectorAB.norm();
                        g[3*graph.id(nSiteB)+2] += (vectorCB.cross(vectorCB.cross(vectorAB))).norm()/vectorCB.norm();
                        g[3*graph.id(nSiteB)+1] -= (g[3*graph.id(nSiteB)]+g[3*graph.id(nSiteB)+2])/2.0;
                        Eangle+=eb;
                        std::cout<<"\t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteA].value_order)<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteB].value_order)<<" "<<graph.atomSites[nSiteC].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<" "<<eleC.atomic_number<<" "<<norm<<" "<<angle<<" "<<k<<" "<<eb<<" "<<deb<<std::endl;
                        nodeCount++;
                        hit=true;
                        }
                        return ret;
                    });
                    if(!hit)std::cout<<"missed \t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteA].value_order)<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteB].value_order)<<" "<<graph.atomSites[nSiteC].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<" "<<eleC.atomic_number<<" "<<static_cast<int>(graph.atomSites[nSiteA].sssr_ring)<<" "<<static_cast<int>(graph.atomSites[nSiteB].sssr_ring)<<" "<<static_cast<int>(graph.atomSites[nSiteC].sssr_ring)<<std::endl;
                        patternGraph.disable(nSiteC);
                        patternGraph.disable(eSiteB);
                        
                    }
                }
                patternGraph.disable(nSiteA);
                patternGraph.disable(eSiteA);
                
            }
            patternGraph.disable(nSiteB);
            
        }
        std::cout<<"Found nodes: "<<nodeCount<<" of "<<incidentCount<<"("<<lemon::countNodes(graph)<<")"<<std::endl;
        std::cout<<"Found edges: "<<edgeCount<<" of "<<lemon::countEdges(graph)<<std::endl;
        std::cout<<"Found propers: "<<properCount<<" of "<<lemon::countEdges(graph)<<std::endl;
        std::cout<<"Ebond "<<Ebond<<" kcal/mol"<<std::endl;
        std::cout<<"Eangle "<<Eangle<<" kcal/mol"<<std::endl;
        std::cout<<"Eproper "<<Eproper<<" kcal/mol"<<std::endl;
        std::cout<<"Eimproper "<<Eimproper<<" kcal/mol"<<std::endl;
      
    }

    }
}
