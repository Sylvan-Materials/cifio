#include <sstream>

#include "forcefield/OpenFF.h"
#include "PeriodicTable.h"
#include "linear/Vector.h"

#include "lemon/list_graph.h"
#include <lemon/bfs.h>
#include "lemon/vf2.h"

namespace sylvanmats{
    namespace reading {

    bool operator==(const std::vector<sylvanmats::reading::atom_primitive>& ltp, const sylvanmats::constitution::_atom_site<double>& rtp){
        if((ltp.front().map_class>0 && rtp.map_class==0) || (ltp.front().map_class==0 && rtp.map_class>0)) return false;
        sylvanmats::PeriodicTable* periodicTable=sylvanmats::PeriodicTable::getInstance();
        sylvanmats::element ele=periodicTable->index((std::string&)rtp.type_symbol);
        bool an=false;
        bool rs=false;
        bool fc=false;
        bool connectivity=false;
        for(std::vector<sylvanmats::reading::atom_primitive>::const_iterator it=ltp.begin();(!an || !rs || !fc || !connectivity) && it!=ltp.end();it++){
            if((*it).wildcard || (*it).atomic_number==ele.atomic_number)an=true;
            if((*it).wildcard || (*it).ring_size==0 || (*it).ring_size==rtp.sssr_ring)rs=true;
            if((*it).wildcard || (*it).formal_charge==0 || (*it).formal_charge==rtp.pdbx_formal_charge)fc=true;
            if((*it).wildcard || (*it).connectivity==0 || (*it).connectivity==rtp.connectivity)connectivity=true;
        }
        return an && (rs) && (connectivity) && (fc);
    };
}
    namespace forcefield{
        
    OpenFF::OpenFF(sylvanmats::constitution::Graph& graph) : graph (graph), smirksPatterns(sylvanmats::reading::SMIRKSPatterns()), g(sylvanmats::linear::ArrayXd(3*lemon::countNodes(graph))) {}
    
    void OpenFF::operator()(){
        sylvanmats::PeriodicTable* periodicTable=sylvanmats::PeriodicTable::getInstance();
        Ebond=0.0;
        Eangle=0.0;
        Eproper=0.0;
        Eimproper=0.0;
        Eelectrostatics=0.0;
        Evdw=0.0;
        size_t nodeCount=0;
        size_t edgeCount=0;
        size_t incidentCount=0;
        size_t properCount=0;
        size_t improperCount=0;
        size_t vdwCount=0;
        lemon::ListGraph::NodeMap<bool> patternNodes(graph, false);
        lemon::ListGraph::EdgeMap<bool> patternEdges(graph, false);
        lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> patternGraph(graph, patternNodes, patternEdges);
        std::stringstream ssPropers;
        for (lemon::ListGraph::EdgeIt eSiteB(graph); eSiteB!=lemon::INVALID; ++eSiteB){
            lemon::ListGraph::Node nSiteB=graph.u(eSiteB);
            lemon::ListGraph::Node nSiteC=graph.v(eSiteB);
            patternGraph.enable(nSiteB);
            patternGraph.enable(nSiteC);
            patternGraph.enable(eSiteB);
            sylvanmats::element eleA=periodicTable->index(graph.atomSites[nSiteB].type_symbol);
            sylvanmats::element eleB=periodicTable->index(graph.atomSites[nSiteC].type_symbol);
            graph.atomSites[nSiteB].map_class=1;
            graph.atomSites[nSiteC].map_class=2;
            graph.atomSites[nSiteB].connectivity=lemon::countIncEdges(graph, nSiteB);
            graph.atomSites[nSiteC].connectivity=lemon::countIncEdges(graph, nSiteC);
            bool hit=false;
            smirksPatterns(eleA.atomic_number, graph.atomSites[nSiteB].connectivity, graph.compBond[eSiteB].type, graph.atomSites[nSiteC].connectivity, eleB.atomic_number, [&](double length, double k, sylvanmats::reading::smirks_pattern& smirksPattern)->bool{
                bool enabledBuffer=false;
                if(lemon::countNodes(patternGraph)<lemon::countNodes(smirksPattern.smirksGraph)){
                    enabledBuffer=true;
                    for (lemon::ListGraph::IncEdgeIt eB(graph, nSiteB); eB!=lemon::INVALID; ++eB){
                        patternGraph.enable(eB);
                        if(graph.runningNode(eB)!=nSiteC){
                            patternGraph.enable(graph.runningNode(eB));
                        }
                    }
                    for (lemon::ListGraph::IncEdgeIt eC(graph, nSiteC); eC!=lemon::INVALID; ++eC){
                        patternGraph.enable(eC);
                        if(graph.runningNode(eC)!=nSiteB){
                            patternGraph.enable(graph.runningNode(eC));
                        }
                    }
                }
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
                if(enabledBuffer){
                    for (lemon::ListGraph::IncEdgeIt eB(graph, nSiteB); eB!=lemon::INVALID; ++eB){
                        if(graph.runningNode(eB)!=nSiteC){
                            patternGraph.disable(eB);
                            patternGraph.disable(graph.runningNode(eB));
                        }
                    }
                    for (lemon::ListGraph::IncEdgeIt eC(graph, nSiteC); eC!=lemon::INVALID; ++eC){
                        if(graph.runningNode(eC)!=nSiteB){
                            patternGraph.disable(eC);
                            patternGraph.disable(graph.runningNode(eC));
                        }
                    }
                }
                return ret;
            });
            if(!hit)std::cout<<"missed \t"<<graph.atomSites[nSiteB].label_atom_id<<" "<<graph.compBond[eSiteB].type<<"("<<static_cast<int>(graph.compBond[eSiteB].value_order)<<") "<<graph.atomSites[nSiteC].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<std::endl;
            for (lemon::ListGraph::IncEdgeIt eSiteA(graph, nSiteB); eSiteA!=lemon::INVALID; ++eSiteA){
                if(graph.runningNode(eSiteA)!=nSiteC){
                lemon::ListGraph::Node nSiteA=graph.runningNode(eSiteA);
                    patternGraph.enable(eSiteA);
                    for (lemon::ListGraph::IncEdgeIt eSiteC(graph, nSiteC); eSiteC!=lemon::INVALID; ++eSiteC){
                        if(graph.runningNode(eSiteC)!=nSiteB){
                            patternGraph.enable(eSiteC);
                            lemon::ListGraph::Node nSiteD=graph.runningNode(eSiteC);
                            if(nSiteA!=nSiteC && nSiteD!=nSiteB){
                            patternGraph.enable(nSiteA);
                            patternGraph.enable(nSiteD);
                            graph.atomSites[nSiteA].map_class=1;
                            graph.atomSites[nSiteB].map_class=2;
                            graph.atomSites[nSiteC].map_class=3;
                            graph.atomSites[nSiteD].map_class=4;
                            sylvanmats::element eleA=periodicTable->index(graph.atomSites[nSiteA].type_symbol);
                            sylvanmats::element eleB=periodicTable->index(graph.atomSites[nSiteB].type_symbol);
                            sylvanmats::element eleC=periodicTable->index(graph.atomSites[nSiteC].type_symbol);
                            sylvanmats::element eleD=periodicTable->index(graph.atomSites[nSiteD].type_symbol);
                            graph.atomSites[nSiteA].connectivity=lemon::countIncEdges(graph, nSiteA);
                            graph.atomSites[nSiteD].connectivity=lemon::countIncEdges(graph, nSiteD);
                            bool hit=false;
                            smirksPatterns(eleA.atomic_number, graph.atomSites[nSiteA].connectivity, graph.compBond[eSiteA].type, graph.atomSites[nSiteB].connectivity, eleB.atomic_number, graph.compBond[eSiteB].type, graph.atomSites[nSiteC].connectivity, eleC.atomic_number, graph.compBond[eSiteC].type, graph.atomSites[nSiteD].connectivity, eleD.atomic_number, [&](sylvanmats::reading::smirks_torsions_pattern& smirksPattern)->bool{
                                lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<lemon::ListGraph::Node> bijectionMap(patternGraph);
//                                if(smirksPattern.smirks.compare("[*:1]-[#6X4:2]-[#8X2:3]-[#1:4]")==0){
//                                    ssPropers<<smirksPattern.smirks<<" "<<lemon::countNodes(patternGraph)<<" "<<lemon::countEdges(patternGraph)<<" "<<eleA.atomic_number<<" "<<static_cast<int>(graph.atomSites[nSiteA].connectivity)<<" "<<graph.compBond[eSiteA].type<<" "<<static_cast<int>(graph.atomSites[nSiteB].connectivity)<<" "<<eleB.atomic_number<<" "<<graph.compBond[eSiteB].type<<" "<<static_cast<int>(graph.atomSites[nSiteC].connectivity)<<" "<<eleC.atomic_number<<" "<<graph.compBond[eSiteC].type<<" "<<static_cast<int>(graph.atomSites[nSiteD].connectivity)<<" "<<eleD.atomic_number<<std::endl;
//                                    for (lemon::ListGraph::EdgeIt eStd(smirksPattern.smirksGraph); eStd!=lemon::INVALID; ++eStd){
//                                        lemon::ListGraph::Node nuStd=smirksPattern.smirksGraph.u(eStd);
//                                        lemon::ListGraph::Node nvStd=smirksPattern.smirksGraph.v(eStd);
//                                        ssPropers<<"\t"<<(lemon::countNodes(smirksPattern.smirksGraph))<<" "<<(lemon::countEdges(smirksPattern.smirksGraph))<<" wild? "<<smirksPattern.atomicPrimitives[nuStd].front().wildcard<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nuStd].front().connectivity)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nuStd].front().atomic_number)<<" "<<smirksPattern.bondPrimitives[eStd].type<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nvStd].front().atomic_number)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nvStd].front().connectivity)<<std::endl;
//                                    }
//
//                                }
                                bool ret = lemon::vf2(smirksPattern.smirksGraph, patternGraph).induced().mapping(bijectionMap).nodeLabels<lemon::ListGraph::NodeMap<std::vector<sylvanmats::reading::atom_primitive>>, lemon::ListGraph::NodeMap<sylvanmats::constitution::_atom_site<double>>>(smirksPattern.atomicPrimitives, graph.atomSites).run();
                                if(ret){
                                    sylvanmats::linear::Vector3d pointA(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z);
                                    sylvanmats::linear::Vector3d pointB(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z);
                                    sylvanmats::linear::Vector3d pointC(graph.atomSites[nSiteC].Cartn_x, graph.atomSites[nSiteC].Cartn_y, graph.atomSites[nSiteC].Cartn_z);
                                    sylvanmats::linear::Vector3d pointD(graph.atomSites[nSiteD].Cartn_x, graph.atomSites[nSiteD].Cartn_y, graph.atomSites[nSiteD].Cartn_z);
                                    double dihedral=findDihedralBetween(pointA, pointB, pointC, pointD);

                                    double et=(smirksPattern.k1Value*(1.0+std::cos(smirksPattern.periodicity1Value*dihedral-smirksPattern.phase1Value))+smirksPattern.k2Value*(1.0+std::cos(smirksPattern.periodicity2Value*dihedral-smirksPattern.phase2Value))+smirksPattern.k3Value*(1.0+std::cos(smirksPattern.periodicity3Value*dihedral-smirksPattern.phase3Value))+smirksPattern.k4Value*(1.0+std::cos(smirksPattern.periodicity4Value*dihedral-smirksPattern.phase4Value)))/2.0;
                                    double det=0.0;//k*(norm-angle);
    //////
                                    Eproper+=et;
                                ssPropers<<"\t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteA].value_order)<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteB].value_order)<<" "<<graph.atomSites[nSiteC].label_atom_id<<" "<<graph.atomSites[nSiteD].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<" "<<eleC.atomic_number<<" "<<eleD.atomic_number<<" "<<dihedral<<" "<<smirksPattern.phase1Value<<" "<<smirksPattern.k1Value<<" "<<et<<" "<<det<<std::endl;
                                properCount++;
                                hit=true;
                                }
                                return ret;
                            });
                            if(!hit)ssPropers<<"missed \t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteA].value_order)<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteB].value_order)<<" "<<graph.atomSites[nSiteC].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteC].value_order)<<" "<<graph.atomSites[nSiteD].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<" "<<eleC.atomic_number<<" "<<eleD.atomic_number<<" con "<<static_cast<int>(graph.atomSites[nSiteA].connectivity)<<" "<<static_cast<int>(graph.atomSites[nSiteB].connectivity)<<" "<<static_cast<int>(graph.atomSites[nSiteC].connectivity)<<" "<<static_cast<int>(graph.atomSites[nSiteD].connectivity)<<" sssr "<<static_cast<int>(graph.atomSites[nSiteA].sssr_ring)<<" "<<static_cast<int>(graph.atomSites[nSiteB].sssr_ring)<<" "<<static_cast<int>(graph.atomSites[nSiteC].sssr_ring)<<std::endl;

                            graph.atomSites[nSiteA].map_class=0;
                            graph.atomSites[nSiteB].map_class=1;
                            graph.atomSites[nSiteC].map_class=2;
                            graph.atomSites[nSiteD].map_class=0;
                            patternGraph.disable(nSiteD);
                            patternGraph.disable(nSiteA);
                            }
                            patternGraph.disable(eSiteC);
                        }
                    }
                    patternGraph.disable(eSiteA);
                }
            }
            graph.atomSites[nSiteB].map_class=0;
            graph.atomSites[nSiteC].map_class=0;
            patternGraph.disable(eSiteB);
            patternGraph.disable(nSiteB);
            patternGraph.disable(nSiteC);
            
        }
        std::cout<<"Propers:\n"<<ssPropers.str()<<std::endl;
        std::cout<<std::endl;
        std::stringstream ssImpropers;
        for (lemon::ListGraph::NodeIt nSiteB(graph); nSiteB!=lemon::INVALID; ++nSiteB){
            patternGraph.enable(nSiteB);
            graph.atomSites[nSiteB].map_class=2;
            incidentCount+=lemon::countIncEdges(graph, nSiteB)*(lemon::countIncEdges(graph, nSiteB)-1)/2;
            for (lemon::ListGraph::IncEdgeIt eSiteA(graph, nSiteB); eSiteA!=lemon::INVALID; ++eSiteA){
                lemon::ListGraph::Node nSiteA=graph.runningNode(eSiteA);
                patternGraph.enable(nSiteA);
                patternGraph.enable(eSiteA);
                graph.atomSites[nSiteA].map_class=1;
                bool unique=false;
                for (lemon::ListGraph::IncEdgeIt eSiteB(graph, nSiteB); eSiteB!=lemon::INVALID; ++eSiteB){
                    lemon::ListGraph::Node nSiteC=graph.runningNode(eSiteB);
                    if(nSiteA==nSiteC){
                        unique=true;
                    }
                    else if(unique && nSiteA!=nSiteC){
                        patternGraph.enable(nSiteC);
                        patternGraph.enable(eSiteB);
                        graph.atomSites[nSiteC].map_class=3;
//                        std::cout<<"\t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<graph.atomSites[nSiteC].label_atom_id<<std::endl;
                    sylvanmats::element eleA=periodicTable->index(graph.atomSites[nSiteA].type_symbol);
                    sylvanmats::element eleB=periodicTable->index(graph.atomSites[nSiteB].type_symbol);
                    sylvanmats::element eleC=periodicTable->index(graph.atomSites[nSiteC].type_symbol);
                    bool hit=false;
                    smirksPatterns(eleA.atomic_number, graph.atomSites[nSiteA].connectivity, graph.compBond[eSiteA].type, graph.atomSites[nSiteB].connectivity, eleB.atomic_number, graph.compBond[eSiteB].type, graph.atomSites[nSiteC].connectivity, eleC.atomic_number, [&](double angle, double k, sylvanmats::reading::smirks_pattern& smirksPattern)->bool{
                        bool enabledBuffer=false;
                        if(lemon::countNodes(patternGraph)<lemon::countNodes(smirksPattern.smirksGraph)){
                            enabledBuffer=true;
                            for (lemon::ListGraph::IncEdgeIt eA(graph, nSiteA); eA!=lemon::INVALID; ++eA){
                                patternGraph.enable(eA);
                                if(graph.runningNode(eA)!=nSiteB){
                                    patternGraph.enable(graph.runningNode(eA));
                                }
                            }
                            for (lemon::ListGraph::IncEdgeIt eB(graph, nSiteB); eB!=lemon::INVALID; ++eB){
                                patternGraph.enable(eB);
                                if(graph.runningNode(eB)!=nSiteC && graph.runningNode(eB)!=nSiteA){
                                    patternGraph.enable(graph.runningNode(eB));
                                }
                            }
                            for (lemon::ListGraph::IncEdgeIt eC(graph, nSiteC); eC!=lemon::INVALID; ++eC){
                                patternGraph.enable(eC);
                                if(graph.runningNode(eC)!=nSiteB){
                                    patternGraph.enable(graph.runningNode(eC));
                                }
                            }
                        }
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
                        std::cout<<"\t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteA].value_order)<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteB].value_order)<<" "<<graph.atomSites[nSiteC].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<" "<<eleC.atomic_number<<" "<<norm<<" "<<angle<<" "<<k<<" "<<eb<<" "<<deb<<" "<<enabledBuffer<<std::endl;
                        nodeCount++;
                        hit=true;
                        }
                        if(enabledBuffer){
                            for (lemon::ListGraph::IncEdgeIt eA(graph, nSiteA); eA!=lemon::INVALID; ++eA){
                                if(graph.runningNode(eA)!=nSiteB){
                                    patternGraph.disable(eA);
                                    patternGraph.disable(graph.runningNode(eA));
                                }
                            }
                            for (lemon::ListGraph::IncEdgeIt eB(graph, nSiteB); eB!=lemon::INVALID; ++eB){
                                if(graph.runningNode(eB)!=nSiteC && graph.runningNode(eB)!=nSiteA){
                                    patternGraph.disable(eB);
                                    patternGraph.disable(graph.runningNode(eB));
                                }
                            }
                            for (lemon::ListGraph::IncEdgeIt eC(graph, nSiteC); eC!=lemon::INVALID; ++eC){
                                if(graph.runningNode(eC)!=nSiteB){
                                    patternGraph.disable(eC);
                                    patternGraph.disable(graph.runningNode(eC));
                                }
                            }
                        }
                        return ret;
                    });
                    if(!hit)std::cout<<"missed \t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteA].value_order)<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteB].value_order)<<" "<<graph.atomSites[nSiteC].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<" "<<eleC.atomic_number<<" "<<static_cast<int>(graph.atomSites[nSiteA].sssr_ring)<<" "<<static_cast<int>(graph.atomSites[nSiteB].sssr_ring)<<" "<<static_cast<int>(graph.atomSites[nSiteC].sssr_ring)<<std::endl;
                        graph.atomSites[nSiteC].map_class=0;
                        patternGraph.disable(nSiteC);
                        patternGraph.disable(eSiteB);
                        
                    }
                }
                graph.atomSites[nSiteA].map_class=0;
                patternGraph.disable(nSiteA);
                patternGraph.disable(eSiteA);
                
            }
            if(lemon::countIncEdges(patternGraph, nSiteB)==3){
                lemon::ListGraph::Node nSiteA=lemon::INVALID;
                lemon::ListGraph::Node nSiteC=lemon::INVALID;
                lemon::ListGraph::Node nSiteD=lemon::INVALID;
                lemon::ListGraph::Edge eSiteA=lemon::INVALID;
                lemon::ListGraph::Edge eSiteC=lemon::INVALID;
                lemon::ListGraph::Edge eSiteD=lemon::INVALID;
                for (lemon::ListGraph::IncEdgeIt e(graph, nSiteB); e!=lemon::INVALID; ++e){
                    patternGraph.enable(e);
                    lemon::ListGraph::Node n=graph.runningNode(e);
                    patternGraph.enable(n);
                    if(nSiteA==lemon::INVALID)nSiteA=n;
                    else if(nSiteC==lemon::INVALID)nSiteC=n;
                    else if(nSiteD==lemon::INVALID)nSiteD=n;
                    if(eSiteA==lemon::INVALID)eSiteA=e;
                    else if(eSiteC==lemon::INVALID)eSiteC=e;
                    else if(eSiteD==lemon::INVALID)eSiteD=e;
                }
                            graph.atomSites[nSiteA].map_class=1;
                            graph.atomSites[nSiteB].map_class=2;
                            graph.atomSites[nSiteC].map_class=3;
                            graph.atomSites[nSiteD].map_class=4;
                            sylvanmats::element eleA=periodicTable->index(graph.atomSites[nSiteA].type_symbol);
                            sylvanmats::element eleB=periodicTable->index(graph.atomSites[nSiteB].type_symbol);
                            sylvanmats::element eleC=periodicTable->index(graph.atomSites[nSiteC].type_symbol);
                            sylvanmats::element eleD=periodicTable->index(graph.atomSites[nSiteD].type_symbol);
                            graph.atomSites[nSiteA].connectivity=lemon::countIncEdges(graph, nSiteA);
                            graph.atomSites[nSiteA].connectivity=lemon::countIncEdges(graph, nSiteC);
                            graph.atomSites[nSiteD].connectivity=lemon::countIncEdges(graph, nSiteD);
                            bool hit=false;
                            smirksPatterns(eleA.atomic_number, graph.atomSites[nSiteA].connectivity, graph.compBond[eSiteA].type, graph.atomSites[nSiteB].connectivity, eleB.atomic_number, graph.compBond[eSiteC].type, graph.atomSites[nSiteC].connectivity, eleC.atomic_number, graph.compBond[eSiteD].type, graph.atomSites[nSiteD].connectivity, eleD.atomic_number, [&](double k1Value, sylvanmats::reading::smirks_impropers_pattern& smirksPattern)->bool{
                                lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<lemon::ListGraph::Node> bijectionMap(patternGraph);
//                                if(smirksPattern.smirks.compare("[*:1]-[#6X4:2]-[#8X2:3]-[#1:4]")==0){
//                                    ssPropers<<smirksPattern.smirks<<" "<<lemon::countNodes(patternGraph)<<" "<<lemon::countEdges(patternGraph)<<" "<<eleA.atomic_number<<" "<<static_cast<int>(graph.atomSites[nSiteA].connectivity)<<" "<<graph.compBond[eSiteA].type<<" "<<static_cast<int>(graph.atomSites[nSiteB].connectivity)<<" "<<eleB.atomic_number<<" "<<graph.compBond[eSiteB].type<<" "<<static_cast<int>(graph.atomSites[nSiteC].connectivity)<<" "<<eleC.atomic_number<<" "<<graph.compBond[eSiteC].type<<" "<<static_cast<int>(graph.atomSites[nSiteD].connectivity)<<" "<<eleD.atomic_number<<std::endl;
//                                    for (lemon::ListGraph::EdgeIt eStd(smirksPattern.smirksGraph); eStd!=lemon::INVALID; ++eStd){
//                                        lemon::ListGraph::Node nuStd=smirksPattern.smirksGraph.u(eStd);
//                                        lemon::ListGraph::Node nvStd=smirksPattern.smirksGraph.v(eStd);
//                                        ssPropers<<"\t"<<(lemon::countNodes(smirksPattern.smirksGraph))<<" "<<(lemon::countEdges(smirksPattern.smirksGraph))<<" wild? "<<smirksPattern.atomicPrimitives[nuStd].front().wildcard<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nuStd].front().connectivity)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nuStd].front().atomic_number)<<" "<<smirksPattern.bondPrimitives[eStd].type<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nvStd].front().atomic_number)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nvStd].front().connectivity)<<std::endl;
//                                    }
//
//                                }
                                bool ret = lemon::vf2(smirksPattern.smirksGraph, patternGraph).induced().mapping(bijectionMap).nodeLabels<lemon::ListGraph::NodeMap<std::vector<sylvanmats::reading::atom_primitive>>, lemon::ListGraph::NodeMap<sylvanmats::constitution::_atom_site<double>>>(smirksPattern.atomicPrimitives, graph.atomSites).run();
                                if(ret){
                                    sylvanmats::linear::Vector3d pointA(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z);
                                    sylvanmats::linear::Vector3d pointB(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z);
                                    sylvanmats::linear::Vector3d pointC(graph.atomSites[nSiteC].Cartn_x, graph.atomSites[nSiteC].Cartn_y, graph.atomSites[nSiteC].Cartn_z);
                                    sylvanmats::linear::Vector3d pointD(graph.atomSites[nSiteD].Cartn_x, graph.atomSites[nSiteD].Cartn_y, graph.atomSites[nSiteD].Cartn_z);
                                    double dihedral=0.0;//findDihedralBetween(pointA, pointB, pointC, pointD);

                                    double et=0.0;//(smirksPattern.k1Value*(1.0+std::cos(smirksPattern.periodicity1Value*dihedral-smirksPattern.phase1Value))+smirksPattern.k2Value*(1.0+std::cos(smirksPattern.periodicity2Value*dihedral-smirksPattern.phase2Value))+smirksPattern.k3Value*(1.0+std::cos(smirksPattern.periodicity3Value*dihedral-smirksPattern.phase3Value))+smirksPattern.k4Value*(1.0+std::cos(smirksPattern.periodicity4Value*dihedral-smirksPattern.phase4Value)))/2.0;
                                    double det=0.0;//k*(norm-angle);
    //////
                                    Eimproper+=et;
                                    improperCount++;
                                ssImpropers<<"\t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteA].value_order)<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteC].value_order)<<" "<<graph.atomSites[nSiteC].label_atom_id<<" "<<graph.atomSites[nSiteD].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<" "<<eleC.atomic_number<<" "<<eleD.atomic_number<<" "<<dihedral<<" "<<smirksPattern.phase1Value<<" "<<smirksPattern.k1Value<<" "<<et<<" "<<det<<std::endl;
                                properCount++;
                                hit=true;
                                }
                                return ret;
                            });
                            if(!hit)ssImpropers<<"missed \t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteA].value_order)<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteC].value_order)<<" "<<graph.atomSites[nSiteC].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteD].value_order)<<" "<<graph.atomSites[nSiteD].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<" "<<eleC.atomic_number<<" "<<eleD.atomic_number<<" con "<<static_cast<int>(graph.atomSites[nSiteA].connectivity)<<" "<<static_cast<int>(graph.atomSites[nSiteB].connectivity)<<" "<<static_cast<int>(graph.atomSites[nSiteC].connectivity)<<" "<<static_cast<int>(graph.atomSites[nSiteD].connectivity)<<" sssr "<<static_cast<int>(graph.atomSites[nSiteA].sssr_ring)<<" "<<static_cast<int>(graph.atomSites[nSiteB].sssr_ring)<<" "<<static_cast<int>(graph.atomSites[nSiteC].sssr_ring)<<std::endl;

                            graph.atomSites[nSiteA].map_class=0;
                            graph.atomSites[nSiteB].map_class=2;
                            graph.atomSites[nSiteC].map_class=0;
                            graph.atomSites[nSiteD].map_class=0;
                for (lemon::ListGraph::IncEdgeIt eSiteA(graph, nSiteB); eSiteA!=lemon::INVALID; ++eSiteA){
                    patternGraph.disable(eSiteA);
                    lemon::ListGraph::Node n=graph.runningNode(eSiteA);
                    patternGraph.disable(n);
                }
                
            }
            graph.atomSites[nSiteB].map_class=0;
            patternGraph.disable(nSiteB);
            
        }
        std::cout<<"Impropers:\n"<<ssImpropers.str()<<std::endl;
        std::cout<<std::endl;
            lemon::ListGraph::NodeMap<vdw> vdwMap(graph);
        
        for (lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
            patternGraph.enable(nSiteA);
            graph.atomSites[nSiteA].map_class=1;
            sylvanmats::element eleA=periodicTable->index(graph.atomSites[nSiteA].type_symbol);
            bool hit=false;
            smirksPatterns(eleA.atomic_number, graph.atomSites[nSiteA].connectivity, [&](sylvanmats::reading::smirks_vdw_pattern& smirksPattern)->bool{
                lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<lemon::ListGraph::Node> bijectionMap(patternGraph);
//                                if(smirksPattern.smirks.compare("[*:1]-[#6X4:2]-[#8X2:3]-[#1:4]")==0){
                                    std::cout<<smirksPattern.smirks<<" "<<lemon::countNodes(patternGraph)<<" "<<lemon::countEdges(patternGraph)<<" "<<eleA.atomic_number<<" "<<static_cast<int>(graph.atomSites[nSiteA].connectivity)<<std::endl;
//                                    for (lemon::ListGraph::EdgeIt eStd(smirksPattern.smirksGraph); eStd!=lemon::INVALID; ++eStd){
//                                        lemon::ListGraph::Node nuStd=smirksPattern.smirksGraph.u(eStd);
//                                        lemon::ListGraph::Node nvStd=smirksPattern.smirksGraph.v(eStd);
//                                        ssPropers<<"\t"<<(lemon::countNodes(smirksPattern.smirksGraph))<<" "<<(lemon::countEdges(smirksPattern.smirksGraph))<<" wild? "<<smirksPattern.atomicPrimitives[nuStd].front().wildcard<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nuStd].front().connectivity)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nuStd].front().atomic_number)<<" "<<smirksPattern.bondPrimitives[eStd].type<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nvStd].front().atomic_number)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nvStd].front().connectivity)<<std::endl;
//                                    }
//
//                                }
                bool ret = lemon::vf2(smirksPattern.smirksGraph, patternGraph).induced().mapping(bijectionMap).nodeLabels<lemon::ListGraph::NodeMap<std::vector<sylvanmats::reading::atom_primitive>>, lemon::ListGraph::NodeMap<sylvanmats::constitution::_atom_site<double>>>(smirksPattern.atomicPrimitives, graph.atomSites).run();
                if(ret){
                    vdwMap[nSiteA].epsilon=smirksPattern.epsilon;
                    vdwMap[nSiteA].rmin_half=smirksPattern.rmin_half;
                    hit=true;
                }
                return ret;
            });
            if(!hit)ssPropers<<"missed \t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<eleA.atomic_number<<" con "<<static_cast<int>(graph.atomSites[nSiteA].connectivity)<<" sssr "<<static_cast<int>(graph.atomSites[nSiteA].sssr_ring)<<std::endl;
            graph.atomSites[nSiteA].map_class=0;
            patternGraph.disable(nSiteA);
        }
        for (lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
            lemon::Bfs<lemon::ListGraph> bfs(graph);
            bfs.init();
            bfs.addSource(nSiteA);
            while(!bfs.emptyQueue()){
                lemon::ListGraph::Node nn=bfs.processNextNode();
                if(bfs.dist(nn)>3){break;}
                patternGraph.enable(nn);
                lemon::Path<lemon::FilterEdges<lemon::ListGraph>> surroundingPath=bfs.path(nn);
                for(int index=0;index<surroundingPath.length();index++)
                {
                   patternGraph.enable(surroundingPath.nth(index));
                   patternGraph.enable(graph.source(surroundingPath.nth(index)));
                   patternGraph.enable(graph.target(surroundingPath.nth(index)));
                }
            }
            bool unique=false;
            for (lemon::ListGraph::NodeIt nSiteB(graph); nSiteB!=lemon::INVALID; ++nSiteB){
                if(nSiteA==nSiteB)unique=true;
                else{
                    if(unique && !patternGraph.status(nSiteB)){
                        sylvanmats::linear::Vector3d pointA(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z);
                        sylvanmats::linear::Vector3d pointB(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z);
                        double norm=(pointA-pointB).norm();
                        if(norm<=smirksPatterns.getElectrostatics().cutoff){
                            double D=1.0;
                            Eelectrostatics+=(graph.atomSites[nSiteA].partial_charge*graph.atomSites[nSiteB].partial_charge)/(4.0*std::numbers::pi*D*norm);
                            double εij=std::sqrt(vdwMap[nSiteA].epsilon*vdwMap[nSiteB].epsilon);
                            double Rmin=(vdwMap[nSiteA].rmin_half+vdwMap[nSiteB].rmin_half)/2.0;
                            Evdw+=εij*(std::pow(Rmin/norm, 12)-2.0*std::pow(Rmin/norm, 6));
                            vdwCount++;
                        }
                    }
                }
            }
        for (lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA)
            patternGraph.disable(nSiteA);
        }
        std::cout<<"Found angles: "<<nodeCount<<" of "<<incidentCount<<"("<<lemon::countNodes(graph)<<")"<<std::endl;
        std::cout<<"Found edges: "<<edgeCount<<" of "<<lemon::countEdges(graph)<<std::endl;
        std::cout<<"Found propers: "<<properCount<<" of "<<lemon::countEdges(graph)<<std::endl;
        std::cout<<"Found impropers: "<<improperCount<<" of "<<lemon::countNodes(graph)<<std::endl;
        std::cout<<"Found vdws: "<<vdwCount<<" of "<<(lemon::countNodes(graph)*(lemon::countNodes(graph)-1)/2)<<std::endl;
        std::cout<<"Ebond "<<Ebond<<" kcal/mol"<<std::endl;
        std::cout<<"Eangle "<<Eangle<<" kcal/mol"<<std::endl;
        std::cout<<"Eproper "<<Eproper<<" kcal/mol"<<std::endl;
        std::cout<<"Eimproper "<<Eimproper<<" kcal/mol"<<std::endl;
        std::cout<<"Eelectrostatics "<<Eelectrostatics<<" kcal/mol"<<std::endl;
        std::cout<<"Evdw "<<Evdw<<" kcal/mol"<<std::endl;
      
    }

    }
}
