#pragma once

#include <stdlib.h>
#include <string>
#include <memory>
#include <ranges>
#include <numbers>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <tuple>
#include <thread>

#include "surface/AtomAreaExposure.h"

#include "constitution/Graph.h"

namespace sylvanmats::surface {
    
    template<typename T>
    struct accessible_surface{
        mutable T atom_site_volume;
        mutable T atom_site_area;
        mutable T area_dx;
        mutable T area_dy;
        mutable T area_dz;
    };

    /**
    * http://paulbourke.net/geometry/circlesphere/ "Intersection of two circles"
    **/
    class Accessible{
        protected:
        sylvanmats::constitution::Graph& graph;
        double probe_radius=0.0;
        std::unordered_map<std::string, double> radii={{"C", 2.0}, {"N", 2.0}, {"O", 1.0}, {"P", 4.0}, {"S", 6.0}};
        lemon::ListGraph::NodeMap<accessible_surface<double>> accessibles;

        public:
        Accessible(sylvanmats::constitution::Graph& graph) : graph (graph), accessibles (graph) {};
        Accessible(const Accessible& orig) = delete;
        virtual ~Accessible() = default;

        lemon::ListGraph::NodeMap<accessible_surface<double>>& getAccessibleSurface(){return accessibles;};

        void operator()(std::function<void(std::string name, std::vector<circle<double>>& circles)> apply){
            double V=0.0;
            double A=0.0;
            std::vector<std::shared_ptr<AtomAreaExposure>> atomAreaExposures;
            std::vector<std::jthread> threads;
            unsigned int totalThreadCount=1;
            unsigned int count=0;
            for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
                
                if(atomAreaExposures.size()<totalThreadCount){
                    double ri=(radii.contains(graph.atomSites[nSiteA].type_symbol)) ? radii[graph.atomSites[nSiteA].type_symbol]: 2.0;
                    atomAreaExposures.push_back(std::shared_ptr<AtomAreaExposure>(new AtomAreaExposure(graph, nSiteA, ri)));
                    threads.push_back(std::jthread(&AtomAreaExposure::operator(), atomAreaExposures.back().get()));
                    threads.back().detach();
                }
                else
                {
                    bool completedOne=false;
                    do{
                        for(auto aae : atomAreaExposures | std::views::filter([](std::shared_ptr<AtomAreaExposure>& a){ std::this_thread::yield();return a->checkStatus()==DATA_READY; })){
                                 accessibles[aae->getAtomSite()].atom_site_volume = aae->getVolume();
                                accessibles[aae->getAtomSite()].atom_site_area = aae->getArea();
                                V+=accessibles[aae->getAtomSite()].atom_site_volume;
                                A+=accessibles[aae->getAtomSite()].atom_site_area;
                                if(nSiteA!=lemon::INVALID){
                                    aae->setAtomSite(nSiteA);
                                    aae->notifyAndContinue();
                                }
                                else aae->notifyAndFinish();
                                completedOne=true;
                                count++;
                                break;
                        }
                        std::this_thread::yield();
                    }while(!completedOne);
                }
                //apply(graph.getUniqueName(nSiteA)+".svg", circles);
            }
            std::cout<<atomAreaExposures.size()<<" V "<<V<<" A "<<A<<" "<<count<<" "<<lemon::countNodes(graph)<<std::endl;
            do{
                for(auto aae : atomAreaExposures | std::views::filter([](std::shared_ptr<AtomAreaExposure>& a){ std::this_thread::yield();return a->checkStatus()==DATA_READY; })){
                         accessibles[aae->getAtomSite()].atom_site_volume = aae->getVolume();
                        accessibles[aae->getAtomSite()].atom_site_area = aae->getArea();
                        V+=accessibles[aae->getAtomSite()].atom_site_volume;
                        A+=accessibles[aae->getAtomSite()].atom_site_area;
                        aae->notifyAndFinish();
                        //std::vector<std::shared_ptr<AtomAreaExposure>>::iterator pos=std::find(atomAreaExposures.begin(), atomAreaExposures.end(), aae);
                        //if(pos!=atomAreaExposures.end())atomAreaExposures.erase(pos);
                        count++;
                        std::cout<<count<<" "<<lemon::countNodes(graph)<<std::endl;
                        break;
                }
                std::this_thread::yield();
            }while(count<lemon::countNodes(graph));
            std::cout<<"V "<<V<<" A "<<A<<std::endl;
        }

    };

}
