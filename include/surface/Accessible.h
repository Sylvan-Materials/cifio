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
#include <chrono>

#include "surface/AtomAreaExposure.h"

#include "constitution/Graph.h"

using namespace std::chrono_literals;

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
        double probe_radius=1.4;
        std::unordered_map<std::string, double> radii={{"H", 1.2}, {"C", 1.7}, {"N", 1.55}, {"O", 1.52}, {"P", 1.8}, {"S", 1.8}, {"Cl", 1.75}, {"Cu", 1.4}};
        lemon::ListGraph::NodeMap<accessible_surface<double>> accessibles;

        public:
        Accessible() = delete;
        Accessible(sylvanmats::constitution::Graph& graph) : graph (graph), accessibles (graph) {};
        Accessible(const Accessible& orig) = delete;
        virtual ~Accessible() = default;

        lemon::ListGraph::NodeMap<accessible_surface<double>>& getAccessibleSurface(){return accessibles;};
        
        double getVolume(){
            double v=0.0;
            for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
                v+=getAccessibleSurface()[nSiteA].atom_site_volume;
            }
            return v;
        };
        
        double getArea(){
            double a=0.0;
            for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
                a+=getAccessibleSurface()[nSiteA].atom_site_area;
            }
            return a;
        };

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
                    atomAreaExposures.push_back(std::shared_ptr<AtomAreaExposure>(new AtomAreaExposure(graph, nSiteA, ri, radii, probe_radius)));
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
                                apply(graph.getUniqueName(aae->getAtomSite())+".svg", aae->getCircles());
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

            }
            std::cout<<atomAreaExposures.size()<<" V "<<V<<" A "<<A<<" count "<<count<<" "<<lemon::countNodes(graph)<<std::endl;
            do{
                for(auto& aae : atomAreaExposures | std::views::filter([](std::shared_ptr<AtomAreaExposure>& a){ std::this_thread::yield();return a->checkStatus()==DATA_READY; })){
                         accessibles[aae->getAtomSite()].atom_site_volume = aae->getVolume();
                        accessibles[aae->getAtomSite()].atom_site_area = aae->getArea();
                        V+=accessibles[aae->getAtomSite()].atom_site_volume;
                        A+=accessibles[aae->getAtomSite()].atom_site_area;
                        apply(graph.getUniqueName(aae->getAtomSite())+".svg", aae->getCircles());
                        aae->notifyAndFinish();
                        //std::vector<std::shared_ptr<AtomAreaExposure>>::iterator pos=std::find(atomAreaExposures.begin(), atomAreaExposures.end(), aae);
                        //if(pos!=atomAreaExposures.end())atomAreaExposures.erase(pos);
                        count++;
                        std::cout<<count<<" count "<<lemon::countNodes(graph)<<std::endl;
                        break;
                }
                std::this_thread::yield();
                if(count==lemon::countNodes(graph)){
                    int countFinished=0;
                    for(auto& aae : atomAreaExposures){
                        if(aae->checkStatus()!=FINISHED)std::cout<<"not finished "<<aae->checkStatus()<<" "<<FINISHED<<std::endl;
                        else {aae->notifyAndFinish();countFinished++;}
                        std::this_thread::sleep_for(400ms);
                    }
                    /*for(auto& t : threads){
                        std::cout<<"Requesting stop of sleepy worker "<<" "<<t.joinable()<<std::endl;
                        //t.request_stop();
                        //t.join();
                        std::cout<<"Sleepy worker joined"<<std::endl;
                    }*/
                    //std::cout<<countFinished<<" threads size "<<threads.size()<<" "<<atomAreaExposures.size()<<std::endl;
                }
            }while(count<lemon::countNodes(graph));
            std::cout<<"V: "<<V<<" A: "<<A<<std::endl;
        }

    };

}
