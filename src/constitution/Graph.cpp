//#include <format>

#include "constitution/Graph.h"
#include "linear/Vector.h"

#include <lemon/suurballe.h>

namespace sylvanmats::constitution {

    bool operator==(symmetry_labels& a, symmetry_labels& b){
        return (a.type_symbol.compare(b.type_symbol)==0);
    };

    void Graph::visibilityOn(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subGraph){
        for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(subGraph); nSiteA!=lemon::INVALID; ++nSiteA){
            atomSites[nSiteA].visibility=true;
        }
    }

    void  Graph::visibilityOff(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subGraph){
        for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(subGraph); nSiteA!=lemon::INVALID; ++nSiteA){
            atomSites[nSiteA].visibility=false;
        }

    }

    void Graph::hydrogenVisibilityOn(){
        for(lemon::ListGraph::NodeIt nSiteA(*this); nSiteA!=lemon::INVALID; ++nSiteA){
            if(atomSites[nSiteA].type_symbol.compare("H")==0)atomSites[nSiteA].visibility=true;
        }
    }

    void  Graph::hydrogenVisibilityOff(){
        for(lemon::ListGraph::NodeIt nSiteA(*this); nSiteA!=lemon::INVALID; ++nSiteA){
            if(atomSites[nSiteA].type_symbol.compare("H")==0)atomSites[nSiteA].visibility=false;
        }

    }

    void Graph::flipVisibility(){
        for(lemon::ListGraph::NodeIt nSiteA(*this); nSiteA!=lemon::INVALID; ++nSiteA){
            atomSites[nSiteA].visibility=!atomSites[nSiteA].visibility;
        }
    }

    void Graph::identifyFusedSystems(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph, std::function<void(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subSelectionGraph)> apply){
            lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<int> compMap(selectionGraph);
            int compCount=lemon::biEdgeConnectedComponents(selectionGraph, compMap);
            std::unique_ptr<int[]> comps(new int[compCount]);
            for(unsigned int i=0;i<compCount;i++)comps[i]=0;
            for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(selectionGraph); nSiteA!=lemon::INVALID; ++nSiteA){
                comps[compMap[nSiteA]]++;
            }
            for(unsigned int i=0;i<compCount;i++){
                if(comps[i]<=1){
                    //lemon::ListGraph::Edge flexibleEdge=lemon::INVALID;
                    for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(selectionGraph); nSiteA!=lemon::INVALID; ++nSiteA){
                        for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::IncEdgeIt eSiteA(selectionGraph, nSiteA); eSiteA!=lemon::INVALID; ++eSiteA){
                            if(compBond[eSiteA].value_order==1 && (lemon::countIncEdges(*this, selectionGraph.oppositeNode(nSiteA, eSiteA))+atomSites[selectionGraph.oppositeNode(nSiteA, eSiteA)].proton_count)>2){
                                //flexibleEdge=eSiteA;
                                compBond[eSiteA].flexible=true;
                            }
                        }
                    }
                    
                    continue;
                }
                lemon::ListGraph::NodeMap<bool> selectionNodes(*this, false);
                lemon::ListGraph::EdgeMap<bool> selectionEdges(*this, false);
                lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> subSelectionGraph(*this, selectionNodes, selectionEdges);
                for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(selectionGraph); nSiteA!=lemon::INVALID; ++nSiteA){
                    if(compMap[nSiteA]==i){
                        subSelectionGraph.enable(nSiteA);
                        for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::IncEdgeIt eSiteA(selectionGraph, nSiteA); eSiteA!=lemon::INVALID; ++eSiteA){
                            if(compMap[selectionGraph.oppositeNode(nSiteA, eSiteA)]==i){
                                subSelectionGraph.enable(eSiteA);
                            }
                            else if(compBond[eSiteA].value_order==1 && (lemon::countIncEdges(*this, selectionGraph.oppositeNode(nSiteA, eSiteA))+atomSites[selectionGraph.oppositeNode(nSiteA, eSiteA)].proton_count)>2)compBond[eSiteA].flexible=true;
                        }
                    }
                }
                apply(subSelectionGraph);
            }
    };

    void Graph::identifyRings(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subGraph){
        lemon::ListGraph::NodeMap<bool> maskNodes(*this, false);
        lemon::ListGraph::EdgeMap<bool> maskEdges(*this, false);
        lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> maskGraph(*this, maskNodes, maskEdges);
        for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(subGraph); nSiteA!=lemon::INVALID; ++nSiteA){
            if(atomSites[nSiteA].type_symbol.compare("FE")!=0){
                maskGraph.enable(nSiteA);
                for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::IncEdgeIt eSiteA(subGraph, nSiteA); eSiteA!=lemon::INVALID; ++eSiteA){
                        if(atomSites[subGraph.oppositeNode(nSiteA, eSiteA)].type_symbol.compare("FE")!=0){
                            maskGraph.enable(eSiteA);
                        }
                }
            }
        }
        //std::cout<<"subGraph "<<lemon::countNodes(subGraph)<<" "<<lemon::countEdges(subGraph)<<std::endl;
        //std::cout<<"maskGraph "<<lemon::countNodes(maskGraph)<<" "<<lemon::countEdges(maskGraph)<<std::endl;
       lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::EdgeMap<int> lengthMap(maskGraph);
        lemon::Suurballe<lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::EdgeMap<int>> suurballe(maskGraph, lengthMap);

        lemon::ListGraph::Node startNode=lemon::INVALID;
        do{
            startNode=lemon::INVALID;
            for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(maskGraph); startNode==lemon::INVALID && nSiteA!=lemon::INVALID; ++nSiteA){
                if(lemon::countIncEdges(subGraph, nSiteA)==2)startNode=nSiteA;
            }
            if(startNode!=lemon::INVALID){
                lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::IncEdgeIt eSiteA(maskGraph, startNode);
                suurballe.run(startNode, subGraph.runningNode(eSiteA));
                for(unsigned int i=0;i<suurballe.pathNum();i++){
                    if(suurballe.path(i).length()<2 || suurballe.path(i).length()>8)continue;
                    currRing++;
                    compBond[eSiteA].ring=currRing;
                    if(atomSites[startNode].sssr_ring==0 || atomSites[startNode].sssr_ring>suurballe.path(i).length()+1)atomSites[startNode].sssr_ring=suurballe.path(i).length()+1;
                    //std::cout<<i<<" ";
                    for(unsigned int j=0;j<suurballe.path(i).length();j++){
                        compBond[suurballe.path(i).nth(j)].ring=currRing;
                        if(atomSites[subGraph.target(suurballe.path(i).nth(j))].sssr_ring==0 || atomSites[subGraph.target(suurballe.path(i).nth(j))].sssr_ring>suurballe.path(i).length()+1)atomSites[subGraph.target(suurballe.path(i).nth(j))].sssr_ring=suurballe.path(i).length()+1;
                        //std::cout<<atomSites[subGraph.source(suurballe.path(i).nth(j))].label_atom_id<<" ";
//                        if(j==suurballe.path(i).length()-1)std::cout<<atomSites[subGraph.target(suurballe.path(i).nth(j))].label_atom_id<<std::endl;
                    }
                    //std::cout<<std::endl;
                 }
                maskGraph.disable(startNode);
                maskGraph.disable(eSiteA);
            }
        }while(startNode!=lemon::INVALID);
        
    };

    std::string Graph::getComponentXPath(lemon::ListGraph::Node& n){
        return "/"+componentProperties[n].asym_id+"/"+componentProperties[n].pdb_mon_id+"/"+std::to_string(componentProperties[n].auth_seq_num)+"/"+((componentProperties[n].pdb_ins_code.compare("?")!=0 && componentProperties[n].pdb_ins_code.compare(".")!=0) ? componentProperties[n].pdb_ins_code: "")+"/";
    }
    
    unsigned int Graph::getNumberOfComponentAtoms(lemon::ListGraph::Node& n){
        unsigned int count=0;
        for(lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt itemIt(componentNavigation, n); itemIt!=lemon::INVALID; ++itemIt){
            count++;
        }
        return count;
    };

    unsigned int Graph::getNumberOfComponentNeutronicAtoms(lemon::ListGraph::Node& n, bool alts){
        unsigned int count=0;
        std::string firstAlternateId="";
        for(lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt itemIt(componentNavigation, n); itemIt!=lemon::INVALID; ++itemIt){
            bool singleAlt=(atomSites[itemIt].label_alt_id.empty() || atomSites[itemIt].label_alt_id.compare(".")==0 || atomSites[itemIt].label_alt_id.compare("?")==0);
            if(firstAlternateId.empty() && !singleAlt)firstAlternateId=atomSites[itemIt].label_alt_id;
            if(atomSites[itemIt].type_symbol.compare("H")!=0 && (!alts || singleAlt || atomSites[itemIt].label_alt_id.compare(firstAlternateId)==0))count++;
        }
        return count;
    };

    bool Graph::componentHasAlternateIds(lemon::ListGraph::Node& n){
        for(lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt itemItA(componentNavigation, n); itemItA!=lemon::INVALID; ++itemItA){
            if(!atomSites[itemItA].label_alt_id.empty() && atomSites[itemItA].label_alt_id.compare(".")!=0 && atomSites[itemItA].label_alt_id.compare("?")!=0)return true;
        }
        return false;
    }
    
    std::vector<std::string> Graph::getComponentAlternateIds(lemon::ListGraph::Node& n){
        std::vector<std::string> alternateIds;
        for(lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt itemItA(componentNavigation, n); itemItA!=lemon::INVALID; ++itemItA){
            if(!atomSites[itemItA].label_alt_id.empty() && atomSites[itemItA].label_alt_id.compare(".")!=0 && atomSites[itemItA].label_alt_id.compare("?")!=0 && std::find(alternateIds.begin(), alternateIds.end(), atomSites[itemItA].label_alt_id) == alternateIds.end())alternateIds.push_back(atomSites[itemItA].label_alt_id);
        }
        return alternateIds;
    }
    
    double Graph::getComponentMaximumDiameter(lemon::ListGraph::Node& n){
        double dia=componentProperties[n].maximum_diameter;
        if(dia!=0.0) return dia;
        for(lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt itemItA(componentNavigation, n); itemItA!=lemon::INVALID; ++itemItA){
            bool rightTriangle=false;
            for(lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt itemItB(componentNavigation, n); itemItB!=lemon::INVALID; ++itemItB){
                if(rightTriangle){
                    double norm=(sylvanmats::linear::Vector3d(atomSites[itemItA].Cartn_x, atomSites[itemItA].Cartn_y, atomSites[itemItA].Cartn_z)-sylvanmats::linear::Vector3d(atomSites[itemItB].Cartn_x, atomSites[itemItB].Cartn_y, atomSites[itemItB].Cartn_z)).norm();
                    if(dia<norm)dia=norm;
                }
                if(itemItA==itemItB)rightTriangle=true;
            }
        }
        componentProperties[n].maximum_diameter=dia;
        return dia;
    }

    sylvanmats::linear::Vector3d Graph::getComponentCentroid(lemon::ListGraph::Node& n){
        sylvanmats::linear::Vector3d centroid(0.0, 0.0, 0.0);//=componentProperties[n].maximum_diameter;
        //if(dia!=0.0) return dia;
        size_t count=0;
        for(lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt itemItA(componentNavigation, n); itemItA!=lemon::INVALID; ++itemItA){
            std::cout<<" "<<(itemItA!=lemon::INVALID)<<" "<<componentProperties[n].mon_id<<std::endl;
            centroid+=sylvanmats::linear::Vector3d(atomSites[itemItA].Cartn_x, atomSites[itemItA].Cartn_y, atomSites[itemItA].Cartn_z);
            count++;
        }
        if(count>0)centroid/=count;
        return centroid;
    }

    std::tuple<sylvanmats::linear::Vector3d, sylvanmats::linear::Vector3d> Graph::getLimitingCorners(){
        sylvanmats::linear::Vector3d tlf;
        sylvanmats::linear::Vector3d brb;
        bool firstTime=true;
        for(lemon::ListGraph::NodeIt nR(componentGraph); nR!=lemon::INVALID; ++nR){
            sylvanmats::linear::Vector3d&& centroid=getComponentCentroid(nR);
            if(firstTime){
                tlf=centroid;
                brb=centroid;
                firstTime=false;
            }
            else{
                if(tlf[0]>centroid[0])tlf[0]=centroid[0];
                if(tlf[1]>centroid[1])tlf[1]=centroid[1];
                if(tlf[2]>centroid[2])tlf[2]=centroid[2];
                if(brb[0]<centroid[0])brb[0]=centroid[0];
                if(brb[1]<centroid[1])brb[1]=centroid[1];
                if(brb[2]<centroid[2])brb[2]=centroid[2];
            }
        }
        return std::make_tuple(tlf, brb);
    }
}