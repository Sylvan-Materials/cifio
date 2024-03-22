#include "linear/Array.h"
#include "linear/Vector.h"
#include "dynamics/Particles.h"
#include "constitution/Selection.h"

namespace sylvanmats::dynamics{
    
    Particles::Particles(sylvanmats::constitution::Graph& graph): graph (graph), selectionNodes(graph, true), selectionEdges(graph, true), subgraph (graph, selectionNodes, selectionEdges),
        particlesMap (particlesGraph), particlesEdgeMap (particlesGraph), sitesMap (graph){
        init(graph, subgraph);
    };
    Particles::Particles(sylvanmats::constitution::Graph& graph, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subgraph): graph (graph), selectionNodes(graph, true), selectionEdges(graph, true), subgraph (subgraph),
            particlesMap (particlesGraph), particlesEdgeMap (particlesGraph), sitesMap (graph){
        init(graph, subgraph);
    }
    void Particles::init(sylvanmats::constitution::Graph& graph, lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subgraph){
        std::cout<<"init "<<lemon::countNodes(subgraph)<<" "<<lemon::countNodes(subgraph)<<std::endl;
        lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::EdgeMap<bool> edgeFusedFilter(subgraph, false);
        lemon::FilterEdges<lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>> filterFusedEdges(subgraph, edgeFusedFilter);
        graph.identifyFusedSystems(subgraph, [&graph, &filterFusedEdges](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subSelectionGraph){
            graph.identifyRings(subSelectionGraph);
            for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::EdgeIt e(subSelectionGraph);e!=lemon::INVALID;++e){
                filterFusedEdges.enable(e);
            }
        });
        lemon::FilterEdges<lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>>::NodeMap<unsigned int> islandMap(filterFusedEdges);
        lemon::connectedComponents(filterFusedEdges, islandMap);
        sylvanmats::linear::ArrayXi islandSize = sylvanmats::linear::ArrayXi::Zero(lemon::countConnectedComponents(filterFusedEdges));
        std::cout<<"counts "<<lemon::countNodes(filterFusedEdges)<<" "<<lemon::countConnectedComponents(filterFusedEdges)<<" "<<islandSize.rows()<<" "<<islandSize.cols()<<" "<<islandSize.size()<<std::endl;
        for (lemon::FilterEdges<lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>>::NodeIt nI(filterFusedEdges); nI!=lemon::INVALID; ++nI)
        {
            islandSize[islandMap[nI]]++;
        }
        std::cout<<"connectComponent "<<islandSize<<std::endl;
        for (unsigned int islandIndex = 0; islandIndex<lemon::countConnectedComponents(filterFusedEdges); islandIndex++)
        {
            lemon::ListGraph::Node p=particlesGraph.addNode();
            particlesMap[p]=islandIndex;
            for (lemon::ListGraph::NodeIt nI(graph); nI!=lemon::INVALID; ++nI)
            {
                if (islandMap[nI]!=islandIndex)continue;
                        sitesMap.set(nI, p);
            }
        }
        for(lemon::ListGraph::NodeIt pA(particlesGraph);pA!=lemon::INVALID; ++pA){
            for(lemon::ListGraph::NodeIt pB(particlesGraph);pB!=lemon::INVALID; ++pB){
                if(pA!=pB && lemon::findEdge(particlesGraph, pA, pB)==lemon::INVALID){
                    bool hit=false;
                    for(lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt itU(sitesMap, pA);!hit && itU!=lemon::INVALID; ++itU){
                        for(lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt itV(sitesMap, pB);!hit && itV!=lemon::INVALID; ++itV){
                            if(itU!=itV && lemon::findEdge(subgraph, itU, itV)!=lemon::INVALID){
                                lemon::ListGraph::Edge ePP=particlesGraph.addEdge(pA, pB);
                                particlesEdgeMap[ePP].u=itU;
                                particlesEdgeMap[ePP].v=itV;
//                                particlesEdgeMap[ePP].uIsland=islandMap[itU];
//                                particlesEdgeMap[ePP].vIsland=islandMap[itV];
                                hit=true;
                            }
                        }
                        
                    }
                    
                }
            }
        }
//    selection.compliment(selectionGraph, [](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& complimentGraph){
//    });
            
    }
    
    void Particles::operator()(std::function<void(lemon::ListGraph& particlesGraph, lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>& sitesMap)> apply){
        apply(particlesGraph, sitesMap);
    }
    
    void Particles::attachOperation(std::function<sylvanmats::linear::ArrayXd(lemon::ListGraph::Node& a, lemon::ListGraph::Node& b, lemon::ListGraph::Node& c, lemon::ListGraph::Node& d)> apply){
        sylvanmats::linear::Vector3d t = {0.0, 0.0, 0.0};
        for(lemon::ListGraph::EdgeIt eP(particlesGraph);eP!=lemon::INVALID;++eP){
            std::vector<double> redundants;
            for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::IncEdgeIt eU(subgraph, particlesEdgeMap[eP].u);eU!=lemon::INVALID;++eU){
                for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::IncEdgeIt eV(subgraph,particlesEdgeMap[eP].v);eV!=lemon::INVALID;++eV){
                    if(eU!=eV && subgraph.runningNode(eU)!=particlesEdgeMap[eP].v && subgraph.runningNode(eV)!=particlesEdgeMap[eP].u){
                        lemon::ListGraph::Node a=subgraph.runningNode(eU);
                        lemon::ListGraph::Node d=subgraph.runningNode(eV);
                        if(graph.atomSites[a].type_symbol.compare("H")!=0 && graph.atomSites[d].type_symbol.compare("H")!=0){
                            sylvanmats::linear::ArrayXd&& torsions=apply(a,  particlesEdgeMap[eP].u,  particlesEdgeMap[eP].v, d);
                            if(torsions.size()>0){
                                for(size_t torsionIndex=0;torsionIndex<torsions.size();torsionIndex++){
                                    if(redundants.empty() || std::none_of(redundants.begin(), redundants.end(), [&torsions, &torsionIndex](double t){return t==torsions[torsionIndex];})){
                                        double torsion=torsions[torsionIndex];
                                        if(torsion!=0.0 && torsion!=2.0*std::numbers::pi){
                                            sylvanmats::linear::Vector3d pA(graph.atomSites[particlesEdgeMap[eP].u].Cartn_x, graph.atomSites[particlesEdgeMap[eP].u].Cartn_y, graph.atomSites[particlesEdgeMap[eP].u].Cartn_z);
                                            sylvanmats::linear::Vector3d pB(graph.atomSites[particlesEdgeMap[eP].v].Cartn_x, graph.atomSites[particlesEdgeMap[eP].v].Cartn_y, graph.atomSites[particlesEdgeMap[eP].v].Cartn_z);
                                            sylvanmats::linear::Vector3d rotationVector=pB-pA;
                                            particlesEdgeMap[eP].Ms.push_back(sylvanmats::algebra::geometric::Motord(t, rotationVector.normalized(), torsion));
                                            redundants.push_back(torsion);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void Particles::resetOriginalState(){
        for(lemon::ListGraph::EdgeIt e(particlesGraph);e!=lemon::INVALID;++e){
            particlesEdgeMap[e].i=0;
        }
    }
    
    void Particles::feed(std::function<bool(sylvanmats::constitution::Graph& graph)> apply){
        size_t count=0;
        lemon::ListGraph::Node largestParticle=lemon::INVALID;
        for(lemon::ListGraph::NodeIt n(particlesGraph);n!=lemon::INVALID;++n){
            size_t individualCount=0;
            for(lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt it(sitesMap, n);it!=lemon::INVALID;++it){
                individualCount++;
            }
            if(individualCount>count){
                largestParticle=n;
                count=individualCount;
            }
        }
        if(largestParticle==lemon::INVALID){
            count=0;
            sylvanmats::linear::ArrayX3d coordinates(graph.getNumberOfAtomSites());
            lemon::ListGraph::NodeMap<size_t> coordinatesMap(graph);
            for(lemon::ListGraph::NodeIt nSitesA(graph);nSitesA!=lemon::INVALID;++nSitesA){
                coordinatesMap[nSitesA]=count;
                sylvanmats::linear::Vector3d pA(graph.atomSites[nSitesA].Cartn_x, graph.atomSites[nSitesA].Cartn_y, graph.atomSites[nSitesA].Cartn_z);
                coordinates[std::slice(count, coordinates.cols(), coordinates.rows())]=pA;
                count++;
            }
            sylvanmats::linear::ArrayXXi combinatoricIndices=sylvanmats::linear::ArrayXXi::Zero(getCombinatoricSize(), getNumberOfCombinatorics());
            lemon::Bfs <lemon::ListGraph> bfs(particlesGraph);
            size_t columnIndex=0;
            size_t segmentSize=combinatoricIndices.rows();
            bfs.init();
            bfs.addSource(largestParticle);
            while(!bfs.emptyQueue()){
                lemon::ListGraph::Node nn=bfs.processNextNode();
                lemon::Path<lemon::ListGraph> bfsPath=bfs.path(nn);
                if(bfsPath.length()>0){
                    lemon::ListGraph::Edge eP=bfsPath.nth(bfsPath.length()-1);
                    if(particlesEdgeMap[eP].Ms.size()>0){
                        segmentSize=segmentSize/particlesEdgeMap[eP].Ms.size();
                        for(size_t motorIndex=0;motorIndex<combinatoricIndices.rows()/segmentSize;motorIndex++){
                            combinatoricIndices.block(motorIndex*segmentSize, columnIndex, segmentSize, 1)=motorIndex*particlesEdgeMap[eP].Ms.size();
                        }
                        columnIndex++;
                    }
                }
            }
            for(size_t index=0;index<combinatoricIndices.rows();index++){
                bfs.init();
                bfs.addSource(largestParticle);
                while(!bfs.emptyQueue()){
                    lemon::ListGraph::Node nn=bfs.processNextNode();
                    lemon::Path<lemon::ListGraph> bfsPath=bfs.path(nn);
                    if(bfsPath.length()>0){
                        lemon::ListGraph::Edge eP=bfsPath.nth(bfsPath.length()-1);
                        particlesEdgeMap[eP].i=(particlesEdgeMap[eP].Ms.size())?combinatoricIndices[index, columnIndex] : 0;
                        for(lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt it(sitesMap, nn);it!=lemon::INVALID;++it){
                            sylvanmats::linear::Vector3d atomVector;
                            atomVector=coordinates[std::slice(coordinatesMap[it], coordinates.cols(), coordinates.rows())];
                            graph.atomSites[it].Cartn_x=atomVector[0];
                            graph.atomSites[it].Cartn_y=atomVector[1];
                            graph.atomSites[it].Cartn_z=atomVector[2];
                            for(size_t pathIndex=bfsPath.length();pathIndex-- >0;){
                                lemon::ListGraph::Edge  ePi=bfsPath.nth(pathIndex);
                                sylvanmats::algebra::geometric::Motor mi=particlesEdgeMap[eP].Ms[particlesEdgeMap[eP].i];
//                                sylvanmats::linear::Vector3d connectAtomVectori=coordinates.row(particlesEdgeMap[eP].i);//.transpose();
                                sylvanmats::linear::Vector3d pA(graph.atomSites[it].Cartn_x, graph.atomSites[it].Cartn_y, graph.atomSites[it].Cartn_z);
                                pA=mi*(pA-coordinates.row(particlesEdgeMap[eP].i))+coordinates.row(particlesEdgeMap[eP].i);
                                graph.atomSites[it].Cartn_x=pA[0];
                                graph.atomSites[it].Cartn_y=pA[1];
                                graph.atomSites[it].Cartn_z=pA[2];
                            }
                        }
                        if(particlesEdgeMap[eP].Ms.size()>0)columnIndex++;
                    }
                }
                apply(graph);
            }
        }
    }
    
    size_t Particles::getNumberOfCombinatorics(){
        size_t count=0;
        for(lemon::ListGraph::EdgeIt e(particlesGraph);e!=lemon::INVALID;++e){
            if(particlesEdgeMap[e].Ms.size()>0)count++;
        }
        return count;
    }
    
    size_t Particles::getCombinatoricSize(){
        size_t count=0;
        bool hit=false;
        for(lemon::ListGraph::EdgeIt e(particlesGraph);e!=lemon::INVALID;++e){
            if(particlesEdgeMap[e].Ms.size()>0){
                count*=particlesEdgeMap[e].Ms.size();
                hit=true;
            }
        }
        return (hit) ? count : 0;
    }
}

