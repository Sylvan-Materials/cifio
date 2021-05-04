#pragma once

#include <stdlib.h>
#include <string>
#include <memory>

#include "constitution/Graph.h"

#include "linear/Vector.h"
#include "algebra/geometric/Motor.h"

namespace sylvanmats::standards {

    class Protons{
        public:
        Protons(){};
        Protons(const Protons& orig) = delete;
        virtual ~Protons() = default;
        void operator()(sylvanmats::constitution::Graph& graph){
            sylvanmats::linear::Vector3d t = {0.0, 0.0, 0.0};
            for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
                if(graph.atomSites[nSiteA].proton_count>0){
                    int incA=lemon::countIncEdges(graph, nSiteA);
                    if(graph.atomSites[nSiteA].auth_seq_id==1 && graph.atomSites[nSiteA].type_symbol.compare("N")==0)std::cout<<graph.atomSites[nSiteA].label_atom_id<<" "<<incA<<" "<<graph.atomSites[nSiteA].proton_count<<std::endl;
                    if(graph.atomSites[nSiteA].proton_count==1 && incA==2){
                        sylvanmats::linear::Vector3d aveHeavy=sylvanmats::linear::Vector3d::Zero();
                        for(lemon::ListGraph::IncEdgeIt eSiteA(graph, nSiteA); eSiteA!=lemon::INVALID; ++eSiteA){
                            lemon::ListGraph::Node nSiteB=graph.oppositeNode(nSiteA, eSiteA);
                            aveHeavy+=(sylvanmats::linear::Vector3d(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y,graph.atomSites[nSiteA].Cartn_z)-sylvanmats::linear::Vector3d(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y,graph.atomSites[nSiteB].Cartn_z));
                        }
                        aveHeavy/=incA;
                        //std::cout<<"vH "<<std::endl;
                        sylvanmats::linear::Vector3d vH=1.1*aveHeavy.normalized()+sylvanmats::linear::Vector3d(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y,graph.atomSites[nSiteA].Cartn_z);
                        addHydrogen(graph, nSiteA, vH);
                    }
                    else if(graph.atomSites[nSiteA].proton_count==2 && incA==2){
                        lemon::ListGraph::IncEdgeIt eSiteA(graph, nSiteA);
                        lemon::ListGraph::Node nSiteB=graph.oppositeNode(nSiteA, eSiteA);
                        sylvanmats::linear::Vector3d aveHeavy=sylvanmats::linear::Vector3d(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y,graph.atomSites[nSiteB].Cartn_z);
                        sylvanmats::linear::Vector3d vTilt=sylvanmats::linear::Vector3d(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y,graph.atomSites[nSiteB].Cartn_z);
                        ++eSiteA;
                        nSiteB=graph.oppositeNode(nSiteA, eSiteA);
                        aveHeavy+=sylvanmats::linear::Vector3d(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y,graph.atomSites[nSiteB].Cartn_z);
                        vTilt-=sylvanmats::linear::Vector3d(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y,graph.atomSites[nSiteB].Cartn_z);
                        aveHeavy/=incA;
                        sylvanmats::linear::Vector3d vH=1.1*(sylvanmats::linear::Vector3d(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y,graph.atomSites[nSiteA].Cartn_z)-aveHeavy).normalized();
                        sylvanmats::algebra::geometric::Motord quat1(t, vTilt.normalized(), std::numbers::pi/4.0);
                        sylvanmats::algebra::geometric::Motord quat2(t, vTilt.normalized(), -std::numbers::pi/4.0);
                        sylvanmats::linear::Vector3d vH1=quat1*vH+sylvanmats::linear::Vector3d(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y,graph.atomSites[nSiteA].Cartn_z);
                        addHydrogen(graph, nSiteA, vH1);
                        sylvanmats::linear::Vector3d vH2=quat2*vH+sylvanmats::linear::Vector3d(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y,graph.atomSites[nSiteA].Cartn_z);
                        addHydrogen(graph, nSiteA, vH2);
                    }
                    else if(incA==1){
                        lemon::ListGraph::IncEdgeIt eSiteA(graph, nSiteA);
                        lemon::ListGraph::Node nSiteB=graph.oppositeNode(nSiteA, eSiteA);
                        lemon::ListGraph::Node nSiteC=lemon::INVALID;
                        short sumOfBondOrder=0;
                        short sumOfHeavies=0;
                        short sumOfProtons=0;
                        sylvanmats::linear::Vector3d aveHeavy=sylvanmats::linear::Vector3d::Zero();
                        for(lemon::ListGraph::IncEdgeIt eSiteB(graph, nSiteB); eSiteB!=lemon::INVALID; ++eSiteB){
                            lemon::ListGraph::Node nSiteO=graph.oppositeNode(nSiteB, eSiteB);
                            if(nSiteO!=nSiteA){
                                nSiteC=nSiteO;
                                sumOfBondOrder+=graph.compBond[eSiteB].value_order;
                                if(graph.atomSites[nSiteO].type_symbol.compare("H")!=0){
                                    aveHeavy+=(sylvanmats::linear::Vector3d(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y,graph.atomSites[nSiteB].Cartn_z)-sylvanmats::linear::Vector3d(graph.atomSites[nSiteO].Cartn_x, graph.atomSites[nSiteO].Cartn_y,graph.atomSites[nSiteO].Cartn_z));
                                    sumOfHeavies++;
                                }
                                else sumOfProtons++;
                            }
                        }
                    if(graph.atomSites[nSiteA].auth_seq_id==1 && graph.atomSites[nSiteA].type_symbol.compare("N")==0)std::cout<<graph.atomSites[nSiteA].label_atom_id<<" "<<incA<<" "<<sumOfHeavies<<" "<<graph.atomSites[nSiteA].proton_count<<std::endl;
                         if(nSiteC!=lemon::INVALID){
                            if(graph.atomSites[nSiteA].proton_count==1 && sumOfHeavies>1){
                                //aveHeavy+=sylvanmats::linear::Vector3d(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y,graph.atomSites[nSiteA].Cartn_z);
                                aveHeavy/=(sumOfHeavies);
                                sylvanmats::linear::Vector3d vH=aveHeavy.normalized()+sylvanmats::linear::Vector3d(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y,graph.atomSites[nSiteA].Cartn_z);
                                addHydrogen(graph, nSiteA, vH);
                            }
                            else if(graph.atomSites[nSiteA].proton_count>=2 && sumOfHeavies>=1){
                                sylvanmats::linear::Vector3d vAB(graph.atomSites[nSiteA].Cartn_x-graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteA].Cartn_y-graph.atomSites[nSiteB].Cartn_y,graph.atomSites[nSiteA].Cartn_z-graph.atomSites[nSiteB].Cartn_z);
                                sylvanmats::linear::Vector3d vCB(graph.atomSites[nSiteC].Cartn_x-graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteC].Cartn_y-graph.atomSites[nSiteB].Cartn_y,graph.atomSites[nSiteC].Cartn_z-graph.atomSites[nSiteB].Cartn_z);
                                sylvanmats::linear::Vector3d vH=-1.1*vCB.normalized();
                                sylvanmats::linear::Vector3d vH1=vH+sylvanmats::linear::Vector3d(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y,graph.atomSites[nSiteA].Cartn_z);
                                addHydrogen(graph, nSiteA, vH1);
                                sylvanmats::algebra::geometric::Motord quat(t, vAB.normalized(), 2.0*std::numbers::pi/3.0);
                                sylvanmats::linear::Vector3d vH2=quat*vH+sylvanmats::linear::Vector3d(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y,graph.atomSites[nSiteA].Cartn_z);
                                addHydrogen(graph, nSiteA, vH2);
                                if(graph.atomSites[nSiteA].proton_count=3){
                                    sylvanmats::algebra::geometric::Motord quat2(t, vAB.normalized(), 4.0*std::numbers::pi/3.0);
                                    sylvanmats::linear::Vector3d vH3=quat2*vH+sylvanmats::linear::Vector3d(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y,graph.atomSites[nSiteA].Cartn_z);
                                    addHydrogen(graph, nSiteA, vH3);
                                }
                            }
                            else{
                                /*sylvanmats::linear::Vector3d vAB(graph.atomSites[nSiteA].Cartn_x-graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteA].Cartn_y-graph.atomSites[nSiteB].Cartn_y,graph.atomSites[nSiteA].Cartn_z-graph.atomSites[nSiteB].Cartn_z);
                                sylvanmats::linear::Vector3d vBC(graph.atomSites[nSiteC].Cartn_x-graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteC].Cartn_y-graph.atomSites[nSiteB].Cartn_y,graph.atomSites[nSiteC].Cartn_z-graph.atomSites[nSiteB].Cartn_z);
                                double abcAngle=std::acos(vAB.dot(vBC)/(vAB.norm()*vBC.norm()));
                                sylvanmats::linear::Vector3d vCross=vAB.cross(vBC);
                                sylvanmats::linear::Vector3d vH=-1.1*vBC.normalized();
                                //Eigen::Quaterniond quatTilt(Eigen::AngleAxisd(-abcAngle, vCross.normalized()));
                                double angle=2.0*std::numbers::pi/graph.atomSites[nSiteA].proton_count;
                                Eigen::Quaterniond quat(Eigen::AngleAxisd(angle, vAB.normalized()));
                                vH=vH+sylvanmats::linear::Vector3d(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y,graph.atomSites[nSiteA].Cartn_z);
                                addHydrogen(graph, nSiteA, vH);*/
                            }
                        }
                    }
                    graph.atomSites[nSiteA].proton_count=0;
                }
            }
        };

        inline void addHydrogen(sylvanmats::constitution::Graph& graph, lemon::ListGraph::Node nSiteA, sylvanmats::linear::Vector3d& vH){
            lemon::ListGraph::Node nH=graph.addNode();
            graph.atomSites[nH].group_PDB=graph.atomSites[nSiteA].group_PDB;
            graph.atomSites[nH].id=lemon::countNodes(graph);
            graph.atomSites[nH].type_symbol="H";
            graph.atomSites[nH].label_atom_id="H"; 
            graph.atomSites[nH].label_alt_id=graph.atomSites[nSiteA].label_alt_id; 
            graph.atomSites[nH].label_comp_id=graph.atomSites[nSiteA].label_comp_id; 
            graph.atomSites[nH].label_asym_id=graph.atomSites[nSiteA].label_asym_id; 
            graph.atomSites[nH].label_entity_id=graph.atomSites[nSiteA].label_entity_id;
            graph.atomSites[nH].label_seq_id=graph.atomSites[nSiteA].label_seq_id;
            graph.atomSites[nH].pdbx_PDB_ins_code=graph.atomSites[nSiteA].pdbx_PDB_ins_code;
            graph.atomSites[nH].Cartn_x=vH[0];
            graph.atomSites[nH].Cartn_y=vH[1];
            graph.atomSites[nH].Cartn_z=vH[2];
            graph.atomSites[nH].occupancy=1;
            graph.atomSites[nH].B_iso_or_equiv=1.0;
            graph.atomSites[nH].pdbx_formal_charge=0;
            graph.atomSites[nH].auth_seq_id=graph.atomSites[nSiteA].auth_seq_id;
            graph.atomSites[nH].auth_comp_id=graph.atomSites[nSiteA].auth_comp_id;
            graph.atomSites[nH].auth_asym_id=graph.atomSites[nSiteA].auth_asym_id;
            graph.atomSites[nH].auth_atom_id=graph.atomSites[nSiteA].auth_atom_id;
            graph.atomSites[nH].pdbx_PDB_model_num=graph.atomSites[nSiteA].pdbx_PDB_model_num;
            lemon::ListGraph::Edge eH=graph.addEdge(nSiteA, nH);
            graph.compBond[eH].comp_id=graph.atomSites[nSiteA].label_comp_id;
            graph.compBond[eH].atom_id_1=graph.atomSites[nSiteA].label_atom_id;
            graph.compBond[eH].atom_id_2=graph.atomSites[nH].label_atom_id;
            graph.compBond[eH].value_order=1;
            graph.compBond[eH].pdbx_aromatic_flag=false;
            graph.compBond[eH].pdbx_stereo_config=false;
            graph.compBond[eH].pdbx_ordinal=lemon::countEdges(graph);
        };
    };

}

