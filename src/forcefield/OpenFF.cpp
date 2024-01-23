#include "forcefield/OpenFF.h"
#include "PeriodicTable.h"
#include "linear/Vector.h"

#include "lemon/vf2.h"

namespace sylvanmats::forcefield {

    bool operator==(const atom_primitive& ltp, const sylvanmats::constitution::_atom_site<double>& rtp){
        sylvanmats::PeriodicTable* periodicTable=sylvanmats::PeriodicTable::getInstance();
        sylvanmats::element ele=periodicTable->index((std::string&)rtp.type_symbol);
        return (ltp.atomic_number==ele.atomic_number||ltp.wildcard) && (ltp.ring_size==0 || ltp.ring_size==rtp.sssr_ring) && (ltp.connectivity==0 || ltp.connectivity==rtp.connectivity);
    };

    OpenFF::OpenFF() : smirksPatterns(SMIRKSPatterns()){
    }
    
    void OpenFF::operator()(sylvanmats::constitution::Graph& graph){
        sylvanmats::PeriodicTable* periodicTable=sylvanmats::PeriodicTable::getInstance();
        double Ebond=0.0;
        double Eangle=0.0;
        size_t nodeCount=0;
        size_t edgeCount=0;
        lemon::ListGraph::NodeMap<bool> patternNodes(graph, false);
        lemon::ListGraph::EdgeMap<bool> patternEdges(graph, false);
        lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> patternGraph(graph, patternNodes, patternEdges);
        for (lemon::ListGraph::EdgeIt eSiteA(graph); eSiteA!=lemon::INVALID; ++eSiteA){
            lemon::ListGraph::Node nSiteA=graph.u(eSiteA);
            lemon::ListGraph::Node nSiteB=graph.v(eSiteA);
            patternGraph.enable(nSiteA);
            patternGraph.enable(nSiteB);
            patternGraph.enable(eSiteA);
            sylvanmats::element eleA=periodicTable->index(graph.atomSites[nSiteA].type_symbol);
//            if(graph.compBond[eSiteA].ring>0)graph.compBond[eSiteA].type=sylvanmats::forcefield::BOND_ANY_RING;
//            sylvanmats::forcefield::BOND_TYPE type=sylvanmats::forcefield::BOND_ANY;
//            if(graph.compBond[eSiteA].value_order==3)type=sylvanmats::forcefield::BOND_TRIPLE;
//            else if(graph.compBond[eSiteA].value_order==2)type=sylvanmats::forcefield::BOND_DOUBLE;
//            else type=sylvanmats::forcefield::BOND_SINGLE;
            sylvanmats::element eleB=periodicTable->index(graph.atomSites[nSiteB].type_symbol);
            graph.atomSites[nSiteA].connectivity=lemon::countIncEdges(graph, nSiteA);
            graph.atomSites[nSiteB].connectivity=lemon::countIncEdges(graph, nSiteB);
            bool hit=false;
            smirksPatterns(eleA.atomic_number, graph.atomSites[nSiteA].connectivity, graph.compBond[eSiteA].type, graph.atomSites[nSiteB].connectivity, eleB.atomic_number, [&](double length, double k, smirks_pattern& smirksPattern)->bool{
                lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<lemon::ListGraph::Node> bijectionMap(patternGraph);
                bool ret = lemon::vf2(smirksPattern.smirksGraph, patternGraph).induced().mapping(bijectionMap).nodeLabels<lemon::ListGraph::NodeMap<atom_primitive>, lemon::ListGraph::NodeMap<sylvanmats::constitution::_atom_site<double>>>(smirksPattern.atomicPrimitives, graph.atomSites).run();
                if(ret){
                double norm=(sylvanmats::linear::Vector3d(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z)-sylvanmats::linear::Vector3d(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z)).norm();
                double eb=(k/2.0)*std::pow(norm-length, 2);
                double deb=k*(norm-length);
                Ebond+=eb;
                std::cout<<"\t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteA].value_order)<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<" "<<norm<<" "<<length<<" "<<k<<" "<<eb<<" "<<deb<<std::endl;
                edgeCount++;
                hit=true;
                }
                return ret;
            });
            if(!hit)std::cout<<"missed \t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.compBond[eSiteA].type<<"("<<static_cast<int>(graph.compBond[eSiteA].value_order)<<") "<<graph.atomSites[nSiteB].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<std::endl;
            patternGraph.disable(nSiteA);
            patternGraph.disable(nSiteB);
            patternGraph.disable(eSiteA);
            
        }
        std::cout<<std::endl;
        for (lemon::ListGraph::NodeIt nSiteB(graph); nSiteB!=lemon::INVALID; ++nSiteB){
            patternGraph.enable(nSiteB);
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
//                    sylvanmats::forcefield::BOND_TYPE typeA=sylvanmats::forcefield::BOND_ANY;
//                    if(graph.compBond[eSiteA].value_order==3)typeA=sylvanmats::forcefield::BOND_TRIPLE;
//                    else if(graph.compBond[eSiteA].value_order==2)typeA=sylvanmats::forcefield::BOND_DOUBLE;
//                    else typeA=sylvanmats::forcefield::BOND_SINGLE;
                    sylvanmats::element eleB=periodicTable->index(graph.atomSites[nSiteB].type_symbol);
//                    sylvanmats::forcefield::BOND_TYPE typeB=sylvanmats::forcefield::BOND_ANY;
//                    if(graph.compBond[eSiteB].value_order==3)typeB=sylvanmats::forcefield::BOND_TRIPLE;
//                    else if(graph.compBond[eSiteB].value_order==2)typeB=sylvanmats::forcefield::BOND_DOUBLE;
//                    else typeB=sylvanmats::forcefield::BOND_SINGLE;
                    sylvanmats::element eleC=periodicTable->index(graph.atomSites[nSiteC].type_symbol);
                    graph.atomSites[nSiteA].connectivity=lemon::countIncEdges(graph, nSiteA);
                    graph.atomSites[nSiteB].connectivity=lemon::countIncEdges(graph, nSiteB);
                    graph.atomSites[nSiteC].connectivity=lemon::countIncEdges(graph, nSiteC);
                    smirksPatterns(eleA.atomic_number, graph.atomSites[nSiteA].connectivity, graph.compBond[eSiteA].type, graph.atomSites[nSiteB].connectivity, eleB.atomic_number, graph.compBond[eSiteB].type, graph.atomSites[nSiteC].connectivity, eleC.atomic_number, [&](double angle, double k, smirks_pattern& smirksPattern)->bool{
                        lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<lemon::ListGraph::Node> bijectionMap(patternGraph);
                        bool ret = lemon::vf2(smirksPattern.smirksGraph, patternGraph).induced().mapping(bijectionMap).nodeLabels<lemon::ListGraph::NodeMap<atom_primitive>, lemon::ListGraph::NodeMap<sylvanmats::constitution::_atom_site<double>>>(smirksPattern.atomicPrimitives, graph.atomSites).run();
                        if(ret){
                        double norm=findAngleBetween(sylvanmats::linear::Vector3d(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z)-sylvanmats::linear::Vector3d(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z), sylvanmats::linear::Vector3d(graph.atomSites[nSiteC].Cartn_x, graph.atomSites[nSiteC].Cartn_y, graph.atomSites[nSiteC].Cartn_z)-sylvanmats::linear::Vector3d(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z));
                        double eb=(k/2.0)*std::pow(norm-angle, 2);
                        double deb=k*(norm-angle);
                        Eangle+=eb;
                        std::cout<<"\t"<<graph.atomSites[nSiteA].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteA].value_order)<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<static_cast<int>(graph.compBond[eSiteB].value_order)<<" "<<graph.atomSites[nSiteC].label_atom_id<<" "<<eleA.atomic_number<<" "<<eleB.atomic_number<<" "<<eleC.atomic_number<<" "<<norm<<" "<<angle<<" "<<k<<" "<<eb<<" "<<deb<<std::endl;
                        nodeCount++;
                        }
                        return ret;
                    });
                        patternGraph.disable(nSiteC);
                        patternGraph.disable(eSiteB);
                        
                    }
                }
                patternGraph.disable(nSiteA);
                patternGraph.disable(eSiteA);
                
            }
            patternGraph.disable(nSiteB);
            
        }
        std::cout<<"Found nodes: "<<nodeCount<<" of "<<lemon::countNodes(graph)<<std::endl;
        std::cout<<"Found edges: "<<edgeCount<<" of "<<lemon::countEdges(graph)<<std::endl;
        std::cout<<"Ebond "<<Ebond<<" kcal/mol"<<std::endl;
        std::cout<<"Eangle "<<Eangle<<" kcal/mol"<<std::endl;
      
    }

}
