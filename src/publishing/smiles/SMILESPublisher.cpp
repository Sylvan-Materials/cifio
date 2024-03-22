#include <ranges>
#include "publishing/smiles/SMILESPublisher.h"

#include <lemon/bfs.h>

namespace sylvanmats::publishing{

    std::ostream& operator<<(std::ostream& os, const SMILESPublisher& smilesp)
    {
        smilesp.particles([&os, &smilesp](lemon::ListGraph& particlesGraph, lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>& sitesMap){
            unsigned int count=0;
            lemon::ListGraph::Node largestParticle=lemon::INVALID;
            lemon::ListGraph::NodeMap<unsigned int> particleSizeMap(particlesGraph);
            for(lemon::ListGraph::NodeIt pA(particlesGraph);pA!=lemon::INVALID; ++pA){
                unsigned int innerCount=0;
                for(lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt itU(sitesMap, pA);itU!=lemon::INVALID; ++itU){
                    if(smilesp.graph.atomSites[itU].type_symbol.compare("H")!=0)innerCount++;
                }
                particleSizeMap[pA]=count;
//                if(innerCount==1){
//                    lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt itU(sitesMap, pA);
//                    unsigned int connectivity=smilesp.graph.connectivity(itU);
//                    if(smilesp.graph.atomSites[itU].type_symbol.compare("C")==0 && connectivity==4)std::cout<<smilesp.graph.atomSites[itU].type_symbol;
//                    else if(smilesp.graph.atomSites[itU].type_symbol.compare("N")==0 && connectivity==3)std::cout<<smilesp.graph.atomSites[itU].type_symbol;
//                    else if(smilesp.graph.atomSites[itU].type_symbol.compare("O")==0 && connectivity==2)std::cout<<smilesp.graph.atomSites[itU].type_symbol;
//                    else if(smilesp.graph.atomSites[itU].type_symbol.length()==2)std::cout<<"["<<smilesp.graph.atomSites[itU].type_symbol<<"]";
//                    else{
//                        std::string type_symbol=smilesp.graph.atomSites[itU].type_symbol;
//                std::transform(type_symbol.cbegin(), type_symbol.cend(), type_symbol.begin(), [](unsigned char c) {return std::tolower(c);});
//                        std::cout<<type_symbol;
//                    }
//                }
                if(innerCount>count){
                    largestParticle=pA;
                    count=innerCount;
                }
            }
            std::cout<<" count "<<count<<" "<<std::endl;
            lemon::Dfs<lemon::ListGraph> dfs(particlesGraph);
            dfs.init();
            dfs.addSource(largestParticle);
            bool first=true;
            while(!dfs.emptyQueue()){
                lemon::ListGraph::Arc a=dfs.processNextArc();
//                if(dfs.dist(nn)>3){break;}
                lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt itV(sitesMap, particlesGraph.v(a));
                lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt itU(sitesMap, particlesGraph.u(a));
                    if(particleSizeMap[particlesGraph.v(a)]==1 || particleSizeMap[particlesGraph.u(a)]==1){
                        if(first && particleSizeMap[particlesGraph.u(a)]==1)std::cout<<smilesp.graph.atomSites[itV].type_symbol<<"\""<<smilesp.graph.atomSites[itV].label_atom_id<<"\"";
                        if(particleSizeMap[particlesGraph.v(a)]==1)std::cout<<smilesp.graph.atomSites[itU].type_symbol<<"\""<<smilesp.graph.atomSites[itU].label_atom_id<<"\" ";
//                        first=false;
                    }
                    else{ 
                        std::cout<<" p ";
                        first=true;
                    }
//                   patternGraph.enable(surroundingPath.nth(index));
//                   patternGraph.enable(particlesGraph.source(surroundingPath.nth(index)));
//                   patternGraph.enable(particlesGraph.target(surroundingPath.nth(index)));
            }
            std::cout<<std::endl;
        });
        
        return os;
    }

}

