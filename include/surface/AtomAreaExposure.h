#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>

#include "constitution/Graph.h"
#include "linear/Array.h"
#include "linear/Vector.h"

#include "lemon/euler.h"
#include "lemon/bellman_ford.h"
#include <lemon/suurballe.h>

namespace sylvanmats::surface{

    enum STATUS{
        UNKNOWN,
        INITIALIZED,
        RUNNING,
        DATA_READY,
        CONTINUE,
        FINISHED
    };

    enum DIRECTION{
        COUNTER_CLOCKWISE,
        CLOCKWISE
    };

    template<typename T>
    struct circle{
        public:
        unsigned int id=0;
        mutable sylvanmats::linear::Vector<T, 2> center;
        mutable sylvanmats::linear::Vector<T, 3> center3d;
        mutable T r0;
        mutable T a;
        mutable T b;
        mutable T c;
        mutable T d;
        mutable DIRECTION direction=COUNTER_CLOCKWISE;
        mutable bool connected=false;
    };
    template<typename T>
    struct arc : public circle<T> {
        public:
//        using circle<T>::id;
//        using circle<T>::r0;
        mutable T α;
        mutable T β;
        mutable T length;
        mutable bool member_of_domain=false;
        arc(){};
        arc(T α, T β, unsigned int id, T t0, T s0, T r0, DIRECTION direction, T a, T b, T c, T d) : α (α), β (β) {
            this->id=id;
            this->center=sylvanmats::linear::Vector2d(t0, s0);
            this->r0=r0;
            this->a=a;
            this->b=b;
            this->c=c;
            this->d=d;
            this->direction=direction;
            length=(direction==COUNTER_CLOCKWISE) ? std::abs(β-α)*r0 : -std::abs(β-α)*r0;
        };
        arc(const arc& orig) = default;
        virtual ~arc() = default;
    };

    /**
    * http://paulbourke.net/geometry/circlesphere/ "Intersection of two circles"
    **/
    class AtomAreaExposure{
        protected:
        sylvanmats::constitution::Graph& graph;
        lemon::ListGraph::Node nSiteA;
        double ri;
        std::unordered_map<std::string, double>& radii;
        double probe_radius=1.4;
        std::atomic<STATUS> status;

        double siteV=0.0;
        double siteA=0.0;
        std::mutex m;
        std::condition_variable cv;
        double toleranceSquared=0.0001*0.0001;

        public:
        AtomAreaExposure(sylvanmats::constitution::Graph& graph, lemon::ListGraph::Node& nSiteA, double ri,std::unordered_map<std::string, double>& radii, double probe_radius=1.4) : graph (graph), nSiteA (nSiteA), ri (ri), radii (radii), probe_radius (probe_radius), status (UNKNOWN) {
        };
        AtomAreaExposure(const AtomAreaExposure& orig) = delete;
        virtual ~AtomAreaExposure() = default;

        void operator()(){
            try{
            do{
                status.store(RUNNING);
                siteV=0.0;
                siteA=0.0;
                if(graph.atomSites[nSiteA].visibility){
                bool engulfed=false;
                std::vector<circle<double>>&& circles=project(graph, nSiteA, ri, engulfed);
                if(adjustAnyTangentNorthPoles(graph, nSiteA, ri, circles))std::cout<<"hit at least one north pole "<<std::endl;
                if(circles.empty()){}
                else if(engulfed){
                }
                else{
                    lemon::ListDigraph projectedGraph;
                    lemon::ListDigraph::ArcMap<arc<double>> arcMap(projectedGraph);
                    auto [clockwiseCount, countOverlays, countIntersections, countFulls] = graphProjection(projectedGraph, circles, arcMap);
//                    std::cout<<"clockwiseCount "<<clockwiseCount<<" "<<circles.size()<<" "<<countOverlays<<" "<<countIntersections<<" "<<countFulls<<std::endl;
                    clockwiseCount=0;
                    auto [dV, dA] = integrateAlongDomainPath(countIntersections, countFulls, graph.atomSites[nSiteA].Cartn_z, ri, projectedGraph, circles, arcMap, clockwiseCount);
                    std::cout<<"domain connectivity nodes: "<<lemon::countNodes(projectedGraph)<<" edges: "<<lemon::countArcs(projectedGraph)<<" connected "<<lemon::countConnectedComponents(projectedGraph)<<" "<<lemon::countStronglyConnectedComponents(projectedGraph)<<" eulerian? "<<lemon::eulerian(projectedGraph)<<" loop? "<<lemon::loopFree(projectedGraph)<<" parallel? "<<lemon::parallelFree(projectedGraph)<<" "<<std::endl;
                    if(clockwiseCount==0 || circles.size()<=1){
                        dV+=(4.0*std::numbers::pi*std::pow(ri, 3)/3.0);
                        dA+=(4.0*std::numbers::pi*std::pow(ri, 2));
                    }
                    siteV+=dV;
                    siteA+=dA;
                }
                }
                
                if(status.load()==RUNNING)status.store(DATA_READY);
                std::unique_lock<std::mutex> lk(m);
                do{
                cv.wait(lk, [&]{return status.load()!=DATA_READY;});
                }while(status.load()==DATA_READY);
                std::cout<<"done waiting "<<std::endl;
            }while(status.load()!=FINISHED);
            //std::cout<<"aae finish finished "<<std::endl;
            } catch (const std::bad_alloc& e) {
                std::cout << "Allocation failed: " << e.what() << '\n';
            }
        };

        STATUS checkStatus(){return status.load();};
        void notifyAndContinue(){status.store(CONTINUE);std::this_thread::yield();cv.notify_one();};
        void notifyAndFinish(){status.store(FINISHED);std::this_thread::yield();cv.notify_one();};
        lemon::ListGraph::Node& getAtomSite(){return nSiteA;};
        void setAtomSite(lemon::ListGraph::Node& nSiteA){this->nSiteA = nSiteA;ri=(radii.contains(graph.atomSites[nSiteA].type_symbol)) ? radii[graph.atomSites[nSiteA].type_symbol]: 2.0;};
        double getVolume(){return siteV;};
        double getArea(){return siteA;};

    protected:
        std::vector<circle<double>> project(sylvanmats::constitution::Graph& graph, lemon::ListGraph::Node& nSiteA, double ri, bool& engulfed){
            std::vector<circle<double>> circles;
            sylvanmats::linear::Vector3d pointA(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z);
            for(lemon::ListGraph::NodeIt nSiteB(graph); nSiteB!=lemon::INVALID; ++nSiteB){
                if(graph.atomSites[nSiteB].visibility){
                sylvanmats::linear::Vector3d pointB(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z);
                double rj=(radii.contains(graph.atomSites[nSiteB].type_symbol)) ? radii[graph.atomSites[nSiteB].type_symbol]: 2.0;
                double d=(pointB-pointA).norm();
                if(nSiteA!=nSiteB && d<(ri+rj+probe_radius)){
                    if(d+ri<=rj){
                        circles.clear();
                        engulfed=true;
                        std::cout<<"engulfed "<<pointA<<" "<<pointB<<" ri="<<ri<<" rj="<<rj<<std::endl;
                        return circles;
                    }
                    else if(d+rj>ri){
                        double a=std::pow(graph.atomSites[nSiteA].Cartn_x-graph.atomSites[nSiteB].Cartn_x, 2)+std::pow(graph.atomSites[nSiteA].Cartn_y-graph.atomSites[nSiteB].Cartn_y, 2)+std::pow(graph.atomSites[nSiteA].Cartn_z+ri-graph.atomSites[nSiteB].Cartn_z, 2)-std::pow(rj, 2);
                        double b=8.0*std::pow(ri, 2)*(graph.atomSites[nSiteA].Cartn_x-graph.atomSites[nSiteB].Cartn_x);
                        double c=8.0*std::pow(ri, 2)*(graph.atomSites[nSiteA].Cartn_y-graph.atomSites[nSiteB].Cartn_y);
                        double d=4.0*std::pow(ri, 2)*(std::pow(graph.atomSites[nSiteA].Cartn_x-graph.atomSites[nSiteB].Cartn_x, 2)+std::pow(graph.atomSites[nSiteA].Cartn_y-graph.atomSites[nSiteB].Cartn_y, 2)+std::pow(graph.atomSites[nSiteA].Cartn_z-ri-graph.atomSites[nSiteB].Cartn_z, 2)-std::pow(rj, 2));
                        double r0=((std::pow(b, 2)+std::pow(c, 2)-4.0*a*d)<0.0) ? std::sqrt((std::pow(b, 2)+std::pow(c, 2)+4.0*a*d)/(4.0*std::pow(a, 2))) : std::sqrt((std::pow(b, 2)+std::pow(c, 2)-4.0*a*d)/(4.0*std::pow(a, 2)));
                         if(r0==0.0){
//                             std::cout<<"r0 "<<r0<<std::endl;
                             continue;
                         }
                        sylvanmats::linear::Vector2d center(-b/(2.0*a), -c/(2.0*a));
                        bool hit=false;
                        if(a<0.0){
                            hit=checkOutsideAtLeastOneCircle(circles, center, r0);
//                            std::cout<<hit<<" a<0 "<<a<<" "<<b<<" "<<c<<" "<<d<<" ri="<<ri<<" rj="<<rj<<std::endl;
                            if(!hit)eraseAnyPreviousCirclesOutside(circles, center, r0);
                        }
                        else{
                            hit=checkInsideAtLeastOneCircle(circles, center, r0);
//                            std::cout<<hit<<" a>=0 "<<a<<" "<<b<<" "<<c<<" "<<d<<" ri="<<ri<<" rj="<<rj<<std::endl;
                            if(!hit)eraseAnyPreviousCirclesInside(circles, center, r0);
                        }
                       if(!hit){
                            circles.push_back(circle<double>());
                            circles.back().id=circles.size();
                            circles.back().center=center;
                            circles.back().r0=r0;
                            circles.back().a=a;
                            circles.back().b=b;
                            circles.back().c=c;
                            circles.back().d=d;
                            circles.back().center3d=pointB;
                            if(a>=0.0)circles.back().direction=CLOCKWISE;
//                            std::cout<<pointA<<" "<<pointB<<" a: "<<a<<" b: "<<b<<" c: "<<c<<" d: "<<d<<" ground zero t0="<<circles.back().center[0]<<" s0="<<circles.back().center[1]<<" r0="<<circles.back().r0<<" ri="<<ri<<" rj="<<rj<<std::endl;
                        }
                    }
//                    else std::cout<<"neither "<<pointB<<std::endl;
                }
//                else if(nSiteA!=nSiteB)std::cout<<pointB<<" outside "<<d<<" "<<(ri+rj+probe_radius)<<std::endl;
                }
//                else std::cout<<"invisible "<<pointA<<std::endl;
            }
            return circles;
        };

        bool adjustAnyTangentNorthPoles(sylvanmats::constitution::Graph& graph, lemon::ListGraph::Node& nSiteA, double ri, std::vector<circle<double>>& circles){
            bool atLeastOne=false;
            sylvanmats::linear::Vector3d pointA(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z);
            for(sylvanmats::surface::circle<double>& neighborCircle : circles){
                double minimalNorthPoleSquared=std::abs(std::pow(pointA.x()-neighborCircle.center3d.x(), 2)+std::pow(pointA.y()-neighborCircle.center3d.y(), 2)+std::pow(pointA.z()-neighborCircle.center3d.z()+ri-neighborCircle.r0, 2)-neighborCircle.r0*neighborCircle.r0);
                
                if(minimalNorthPoleSquared<toleranceSquared)neighborCircle.r0*=0.9999;
            }
            return atLeastOne;
        };
        
        std::tuple<unsigned int, unsigned int, unsigned int, unsigned int> graphProjection(lemon::ListDigraph& projectedGraph, std::vector<circle<double>>& circles, lemon::ListDigraph::ArcMap<arc<double>>& arcMap){
            unsigned int clockwiseCount=0;
            unsigned int countOverlays=0;
            unsigned int countIntersections=0;
            unsigned int countFulls=0;
            std::map<unsigned int, std::vector<std::tuple<double, lemon::ListDigraph::Node>>> circleAngleMap;
            for(sylvanmats::surface::circle<double>& circleA : circles){
                if(circleA.direction==CLOCKWISE)clockwiseCount++;
                for(sylvanmats::surface::circle<double>& circleB : circles | std::views::filter([&circleA](sylvanmats::surface::circle<double>& c){return c.id>circleA.id;})){
                    sylvanmats::linear::Vector2d P0=circleA.center;
                    sylvanmats::linear::Vector2d P1=circleB.center;
                    double d=(P1-P0).norm();
                    if(d>circleA.r0+circleB.r0){
                    }
                    else if(d<std::abs(circleA.r0-circleB.r0)){
                    }
                    else if(d==0.0 && circleA.r0==circleB.r0){
                       countOverlays++;
                    }
                    else{
                        double a=(std::pow(circleA.r0, 2)-std::pow(circleB.r0, 2)+std::pow(d, 2))/(2.0*d);
                        double h=std::sqrt(std::pow(circleA.r0, 2)-std::pow(a, 2));
                        double b=d-a;
                        sylvanmats::linear::Vector2d P2=P0+a*(P1-P0)/d;
                        sylvanmats::linear::Vector2d P3a(P2.x()+h*(P1.y()-P0.y())/d, P2.y()-h*(P1.x()-P0.x())/d);
                        sylvanmats::linear::Vector2d P3b(P2.x()-h*(P1.y()-P0.y())/d, P2.y()+h*(P1.x()-P0.x())/d);
                        sylvanmats::linear::Vector2d unitX=sylvanmats::linear::Vector2d::UnitX();
                        double α0=findAngleBetween(unitX, (P3a-P0));
                        double β0=findAngleBetween(unitX, (P3b-P0));
                        //if(b>d)std::swap(α0, β0);
                        //if(β0<α0)β0+=2.0*std::numbers::pi;
                        lemon::ListDigraph::Node nA=projectedGraph.addNode();
                        lemon::ListDigraph::Node nB=projectedGraph.addNode();
                        circleAngleMap[circleA.id].push_back(std::make_tuple(α0, nA));
                        circleAngleMap[circleA.id].push_back(std::make_tuple(β0, nB));
                        //if(β>2.0*std::numbers::pi){α-=2.0*std::numbers::pi;β-=2.0*std::numbers::pi;};
//                        std::cout<<"t1 "<<circleA.center[0]<<" s1 "<<circleA.center[1]<<" r1 "<<circleA.r0<<" α "<<α0<<" β "<<β0<<" "<<(β0-α0)<<std::endl;
                        double α1=findAngleBetween(unitX, (P3b-P1));
                        double β1=findAngleBetween(unitX, (P3a-P1));
                        //if(a>d)std::swap(α1, β1);
                        //if(β1<α1)β1+=2.0*std::numbers::pi;
                        circleAngleMap[circleB.id].push_back(std::make_tuple(α1, nB));
                        circleAngleMap[circleB.id].push_back(std::make_tuple(β1, nA));
                        //if(β>2.0*std::numbers::pi){α-=2.0*std::numbers::pi;β-=2.0*std::numbers::pi;};
//                        std::cout<<"t1 "<<circleB.center[0]<<" s1 "<<circleB.center[1]<<" r1 "<<circleB.r0<<" α "<<α1<<" β "<<β1<<" "<<(β1-α1)<<std::endl;
                        countIntersections++;
                        circleA.connected=true;
                        circleB.connected=true;
                    }
                }
            }
            for(auto& circleAngle: circleAngleMap){
                std::sort(circleAngle.second.begin(), circleAngle.second.end());
                //auto last = std::unique(circleAngle.second.begin(), circleAngle.second.end(), [](std::tuple<double, lemon::ListDigraph::Node>& angleA, std::tuple<double, lemon::ListDigraph::Node>& angleB){ return std::get<0>(angleA)==std::get<0>(angleB);});
                //circleAngle.second.erase(last, circleAngle.second.end());
                unsigned int idA=circleAngle.first;
                for(unsigned angleIndex=0;angleIndex<circleAngle.second.size();angleIndex++){
                    double α=std::get<0>(circleAngle.second[angleIndex]);
                    double β=(angleIndex<circleAngle.second.size()-1) ? std::get<0>(circleAngle.second[angleIndex+1]) : std::get<0>(circleAngle.second[0])+2.0*std::numbers::pi;
//                    std::cout<<idA<<" α: "<<α<<" β: "<<β<<std::endl;
                    lemon::ListDigraph::Arc a=projectedGraph.addArc(std::get<1>(circleAngle.second[angleIndex]), (angleIndex<circleAngle.second.size()-1) ? std::get<1>(circleAngle.second[angleIndex+1]) : std::get<1>(circleAngle.second[0]));
                    for(sylvanmats::surface::circle<double>& circleA : circles | std::views::filter([&idA](sylvanmats::surface::circle<double>& c){return c.id==idA;})){
                        arcMap[a]=arc<double>(α, β, circleA.id, circleA.center[0], circleA.center[1], circleA.r0, circleA.direction, circleA.a, circleA.b, circleA.c, circleA.d);
                        arcMap[a].center3d=circleA.center3d;
                    }
                }
            }
            
            lemon::ListDigraph::ArcIt aSiteA(projectedGraph);
            double lowestLength=(aSiteA!=lemon::INVALID) ? arcMap[aSiteA].length : 0.0;
            for(; aSiteA!=lemon::INVALID; ++aSiteA){
                lowestLength = std::min(lowestLength, arcMap[aSiteA].length);
            }
            for(lemon::ListDigraph::ArcIt aSiteB(projectedGraph); aSiteB!=lemon::INVALID; ++aSiteB){
                bool clockwiseOutSide=true;
                bool counterClockwiseOutSide=true;
                sylvanmats::linear::Vector2d&& pointMidArc=polarToCartesian(arcMap[aSiteB].center, arcMap[aSiteB].r0, (arcMap[aSiteB].α+arcMap[aSiteB].β)/2.0);
                
                if(arcMap[aSiteB].direction==CLOCKWISE){
                    for(sylvanmats::surface::circle<double>& circleA : circles | std::views::filter([&aSiteB, &arcMap, &pointMidArc](sylvanmats::surface::circle<double>& c){return c.id!=arcMap[aSiteB].id && (c.center-pointMidArc).norm()<c.r0;})){
                        clockwiseOutSide=false;
                    }
                    /*for(sylvanmats::surface::circle<double>& circleA : circles | std::views::filter([&aSiteB, &arcMap, &pointMidArc](sylvanmats::surface::circle<double>& c){return c.id!=arcMap[aSiteB].id && c.direction==COUNTER_CLOCKWISE && (c.center-pointMidArc).norm()<c.r0;})){
                        clockwiseOutSide=false;
                    }*/
                }
                else{
                    counterClockwiseOutSide=false;
                    for(sylvanmats::surface::circle<double>& circleA : circles | std::views::filter([&aSiteB, &arcMap, &pointMidArc](sylvanmats::surface::circle<double>& c){return c.id!=arcMap[aSiteB].id && c.direction==CLOCKWISE && (c.center-pointMidArc).norm()<c.r0;})){
                        counterClockwiseOutSide=true;
                    }
                    /*for(sylvanmats::surface::circle<double>& circleA : circles | std::views::filter([&aSiteB, &arcMap, &pointMidArc](sylvanmats::surface::circle<double>& c){return c.id!=arcMap[aSiteB].id && c.direction==CLOCKWISE && (c.center-pointMidArc).norm()<c.r0;})){
                        counterClockwiseOutSide=false;
                    }*/
                }
//                if(clockwiseOutSide && counterClockwiseOutSide)arcMap[aSiteB].length=0.0;
//                else 
                    arcMap[aSiteB].length-=lowestLength;
            }
//            std::erase_if(circles, [](sylvanmats::surface::circle<double>& c){return !c.connected && c.direction==COUNTER_CLOCKWISE;});
            for(sylvanmats::surface::circle<double> circleA : circles){
                if(!circleA.connected /*&& circleA.direction==COUNTER_CLOCKWISE*/)countFulls++;
            }
            return std::make_tuple(clockwiseCount, countOverlays, countIntersections, countFulls);
        };

        std::tuple<double, double> integrateAlongDomainPath(unsigned int countIntersections, unsigned int countFulls, double zi, double ri, lemon::ListDigraph& projectedGraph, std::vector<circle<double>>& circles, lemon::ListDigraph::ArcMap<arc<double>>& arcMap, unsigned int& clockwiseCount){
                if(countIntersections==0){
                    lemon::ListDigraph::Node nA=projectedGraph.addNode();
                    lemon::ListDigraph::Arc e=projectedGraph.addArc(nA, nA);
                    arcMap[e]=arc<double>(0.0, 2.0*std::numbers::pi, circles[0].id, circles[0].center[0], circles[0].center[1], circles[0].r0, circles[0].direction, circles[0].a, circles[0].b, circles[0].c, circles[0].d);
                    arcMap[e].center3d=circles[0].center3d;
                    if(arcMap[e].direction==CLOCKWISE)clockwiseCount++;
                    arcMap[e].member_of_domain=true;
                    std::cout<<"no intersections "<<std::endl;
                    return integrate(zi, ri, arcMap[e].center[0], arcMap[e].center[1], arcMap[e].r0, arcMap[e].direction);
                }
                double dV=0.0;
                double dA=0.0;
                if(countFulls>0){
                    std::cout<<"countFulls "<<countFulls<<" "<<circles.size()<<std::endl;
                    for(sylvanmats::surface::circle<double>& circleA : circles | std::views::filter([](sylvanmats::surface::circle<double>& c){return !c.connected;})){
//                            lemon::ListDigraph::Node nA=projectedGraph.addNode();
//                            lemon::ListDigraph::Arc e=projectedGraph.addArc(nA, nA);
//                            arcMap[e]=arc<double>(0.0, 2.0*std::numbers::pi, circleA.id, circleA.center[0], circleA.center[1], circleA.r0, circleA.direction, circleA.a, circleA.b, circleA.c, circleA.d);
//                            arcMap[e].center3d=circleA.center3d;
//                            if(arcMap[e].direction==CLOCKWISE)clockwiseCount++;
//                            arcMap[e].member_of_domain=true;
//                            std::cout<<"\tarc "<<arcMap[e].r0<<" "<<arcMap[e].α<<" "<<arcMap[e].β<<std::endl;
                            auto [dVi, dAi] =  integrate(zi, ri, circleA.center[0], circleA.center[1],  circleA.r0,  circleA.direction);
                            std::cout<<"  F "<<dVi<<" "<<dAi<<std::endl;
                            dV+=dVi;
                            dA+=dAi;
                    }
                }
                {
//                    std::cout<<" nodes: "<<lemon::countNodes(projectedGraph)<<" edges: "<<lemon::countArcs(projectedGraph)<<" connected "<<lemon::countConnectedComponents(projectedGraph)<<" "<<lemon::countStronglyConnectedComponents(projectedGraph)<<" eulerian? "<<lemon::eulerian(projectedGraph)<<" loop? "<<lemon::loopFree(projectedGraph)<<" parallel? "<<lemon::parallelFree(projectedGraph)<<" "<<std::endl;
                    lemon::ListDigraph::NodeMap<int> componentMap(projectedGraph);
                    sylvanmats::linear::Array<double, -1l> componentCounts(lemon::connectedComponents(projectedGraph, componentMap));
                    for(lemon::ListDigraph::NodeIt nProjected(projectedGraph); nProjected!=lemon::INVALID; ++nProjected){
                        //std::cout<<"componentMap "<<componentMap[nProjected]<<std::endl;
                        componentCounts[componentMap[nProjected]]++;
                    }
//                    if(componentCounts.size()>1)std::cout<<"componentCounts "<<componentCounts.transpose()<<std::endl;
                    lemon::ListDigraph::ArcMap<double> lengthMap(projectedGraph);
                    for(lemon::ListDigraph::ArcIt eSiteA(projectedGraph); eSiteA!=lemon::INVALID; ++eSiteA){
                        lengthMap[eSiteA]=arcMap[eSiteA].length;
                        //std::cout<<" "<<projectedGraph.id(projectedGraph.source(eSiteA))<<" "<<projectedGraph.id(projectedGraph.target(eSiteA))<<" "<<arcMap[eSiteA].center.transpose()<<" r0: "<<arcMap[eSiteA].r0<<" α: "<<arcMap[eSiteA].α<<" β: "<<arcMap[eSiteA].β<<" l: "<<arcMap[eSiteA].length<<" dir: "<<arcMap[eSiteA].direction<<std::endl;
                    }
                    if(lemon::eulerian(projectedGraph)){
                    lemon::Path<lemon::ListDigraph> shortestPath;
                    double bestLength=0.0;

                    lemon::BellmanFord<lemon::ListDigraph, lemon::ListDigraph::ArcMap<double>> bellmanFord(projectedGraph, lengthMap);
                    for(lemon::ListDigraph::NodeIt nIntersectionA(projectedGraph);nIntersectionA!=lemon::INVALID; ++nIntersectionA){
                        bellmanFord.init();
                        lemon::Path<lemon::ListDigraph> testPath;
                        bellmanFord.addSource(nIntersectionA);
                        bool negativeCycles=bellmanFord.checkedStart();
                        if(!negativeCycles)std::cout<<negativeCycles<<" bellmanFord.ran "<<std::endl;
                        for(lemon::ListDigraph::InArcIt aSiteA(projectedGraph, nIntersectionA);aSiteA!=lemon::INVALID; ++aSiteA){
                            lemon::ListDigraph::Node inNode=projectedGraph.runningNode(aSiteA);
                            double length=lengthMap[aSiteA];
                            lemon::pathCopy(bellmanFord.path(inNode), testPath);
                            for(unsigned int j=0;j<testPath.length();j++){
                                length+=lengthMap[testPath.nth(j)];
                            }
                            if(bestLength==0.0){
                                bestLength=length;
                                lemon::pathCopy(testPath, shortestPath);
                                shortestPath.addFront(aSiteA);
                            }
                            else if(bestLength>length){
                                bestLength=length;
                                lemon::pathCopy(testPath, shortestPath);
                                shortestPath.addFront(aSiteA);
                            }
//                        std::cout<<length<<" bestLength: "<<bestLength<<" "<<testPath.length()<<" "<<shortestPath.length()<<std::endl;
                        }
                    }
                    auto [dVi, dAi] =  integrate(zi, ri, shortestPath, arcMap);
                    std::cout<<"  S "<<dVi<<" "<<dAi<<std::endl;
                    dV+=dVi;
                    dA+=dAi;
                    }
                }
            return std::make_tuple(dV, dA);
        };

        std::tuple<double, double> integrate(double zi, double ri, lemon::Path<lemon::ListDigraph>& shortestPath,lemon::ListDigraph::ArcMap<arc<double>>& arcMap){
            sylvanmats::linear::ArrayXd t0(shortestPath.length());
            sylvanmats::linear::ArrayXd s0(shortestPath.length());
            sylvanmats::linear::ArrayXd r0(shortestPath.length());
            sylvanmats::linear::ArrayXd αj(shortestPath.length());
            sylvanmats::linear::ArrayXd βj(shortestPath.length());
            sylvanmats::linear::ArrayXd sign(shortestPath.length());
            sylvanmats::linear::ArrayXd a(shortestPath.length());
            sylvanmats::linear::ArrayXd b(shortestPath.length());
            sylvanmats::linear::ArrayXd c(shortestPath.length());
            sylvanmats::linear::ArrayXd d(shortestPath.length());
            for(unsigned int j=0;j<shortestPath.length();j++){
                t0[j]=arcMap[shortestPath.nth(j)].center[0];
                s0[j]=arcMap[shortestPath.nth(j)].center[1];
                r0[j]=arcMap[shortestPath.nth(j)].r0;
                αj[j]=arcMap[shortestPath.nth(j)].α;//β-2.0*std::numbers::pi;
                βj[j]=arcMap[shortestPath.nth(j)].β;//α;
                sign[j]=(arcMap[shortestPath.nth(j)].direction==COUNTER_CLOCKWISE) ? 1.0 : -1.0;
                arcMap[shortestPath.nth(j)].member_of_domain=true;
                a[j]=arcMap[shortestPath.nth(j)].a;
                b[j]=arcMap[shortestPath.nth(j)].b;
                c[j]=arcMap[shortestPath.nth(j)].c;
                d[j]=arcMap[shortestPath.nth(j)].d;
            }
            sylvanmats::linear::ArrayXd A=(4.0*std::pow(ri,2)+t0.pow(2)+s0.pow(2)+r0.pow(2))/2.0;
            sylvanmats::linear::ArrayXd B=t0*r0;
            sylvanmats::linear::ArrayXd C=s0*r0;
            sylvanmats::linear::ArrayXd D=A.pow(2)-B.pow(2)-C.pow(2);
//            std::cout<<" A "<<A.transpose()<<std::endl;
//            std::cout<<" B "<<B.transpose()<<std::endl;
//            std::cout<<" C "<<C.transpose()<<std::endl;
//            std::cout<<" D "<<D.transpose()<<std::endl;
            //std::cout<<" αj "<<αj.transpose()<<std::endl;
            //std::cout<<" βj "<<βj.transpose()<<std::endl;
            sylvanmats::linear::ArrayXd I1=(2.0/D.sqrt())*(std::numbers::pi/2.0 - ((A*((βj-αj)/2.0).cos()+B*((αj+βj)/2.0).cos()+C*((αj+βj)/2.0).sin())/(D.sqrt()*((βj-αj)/2.0).sin())).atan());
            sylvanmats::linear::ArrayXd I2=(1.0/D)*(((-B*βj.sin()+C*βj.cos())/(A+B*βj.cos()+C*βj.sin()))-((-B*αj.sin()+C*αj.cos())/(A+B*αj.cos()+C*αj.sin()))+A*I1);
            sylvanmats::linear::ArrayXd I3=(1.0/(2.0*D))*(((-B*βj.sin()+C*βj.cos())/(A+B*βj.cos()+C*βj.sin()).pow(2))-((-B*αj.sin()+C*αj.cos())/(A+B*αj.cos()+C*αj.sin()).pow(2))+(((-B/A)*βj.sin()+(C/A)*βj.cos())/(A+B*βj.cos()+C*βj.sin()))-(((-B/A)*αj.sin()+(C/A)*αj.cos())/(A+B*αj.cos()+C*αj.sin())))+((2.0*A.pow(2)+B.pow(2)+C.pow(2))/(2.0*A*D))*I2;
//            std::cout<<" I1 "<<I1.transpose()<<std::endl;
//            std::cout<<" I2 "<<I2.transpose()<<std::endl;
//            std::cout<<" I3 "<<I3.transpose()<<std::endl;
            sylvanmats::linear::ArrayXd J1=(βj-αj+(r0.pow(2)-A)*I1)/2.0;
            sylvanmats::linear::ArrayXd J2=(I1+(r0.pow(2)-A)*I2)/4.0;
            sylvanmats::linear::ArrayXd J3=(I2+(r0.pow(2)-A)*I3)/8.0;
//            std::cout<<" J1 "<<J1.transpose()<<std::endl;
//            std::cout<<" J2 "<<J2.transpose()<<std::endl;
//            std::cout<<" J3 "<<J3.transpose()<<std::endl;
            //std::cout<<" ri "<<ri<<" zi "<<zi<<std::endl;
            double dV=(sign*((128.0*J3*std::pow(ri, 7)+8.0*J2*std::pow(ri,5)+ 2.0*J1*std::pow(ri,3))/3.0-8.0*std::pow(ri,4)*J2*(zi+ri))).sum();
            double dA=(sign*2*J1*ri*ri).sum();
//            std::cout<<" arc dV "<<(dV)<<" dA "<<dA<<std::endl;
            //gradients
            sylvanmats::linear::ArrayXd amplitude=8.0*std::pow(ri, 4)*std::numbers::pi/((4.0*std::pow(ri, 2)*a-d).pow(2)+4.0*std::pow(ri, 2)*(b.pow(2)+c.pow(2))).pow(1.5);
            sylvanmats::linear::ArrayXd pA=amplitude*(4.0*std::pow(ri, 2)*(b.pow(2)+c.pow(2)-2.0*a*d)+2.0*d.pow(2));
            sylvanmats::linear::ArrayXd pB=amplitude*(-b*(d+4.0*std::pow(ri, 2)*a));
            sylvanmats::linear::ArrayXd pC=amplitude*(-c*(d+4.0*std::pow(ri, 2)*a));
            sylvanmats::linear::ArrayXd pD=amplitude*(b.pow(2)+c.pow(2)-2.0*a*d+8.0*std::pow(ri, 2)*a.pow(2));
            
            return std::forward_as_tuple(dV, dA);
        }

        std::tuple<double, double> integrate(double zi, double ri, double t0, double s0, double r0, DIRECTION direction){
                    double A=(4.0*std::pow(ri, 2)+std::pow(t0, 2)+std::pow(s0, 2)+std::pow(r0, 2))/2.0;
                    double B=t0*r0;
                    double C=s0*r0;
                    double D=std::pow(A, 2)-std::pow(B, 2)-std::pow(C, 2);
//                    std::cout<<"A "<<(A)<<" B "<<(B)<<" C "<<(C)<<" D "<<std::sqrt(D)<<std::endl;
                    
                    double I1=2.0*std::numbers::pi/std::sqrt(D);
                    double I2=2.0*std::numbers::pi*A/std::pow(D, 3.0/2.0);
                    double I3=std::numbers::pi*(2.0*std::pow(A, 2)+std::pow(B, 2)+std::pow(C, 2))/std::pow(D, 5.0/2.0);
//                    std::cout<<"I1 "<<(I1)<<" I2 "<<(I2)<<" I3 "<<(I3)<<std::endl;

                    double J1=(2.0*std::numbers::pi+(std::pow(r0, 2)-A)*I1)/2.0;
                    double J2=(I1+(std::pow(r0, 2)-A)*I2)/4.0;
                    double J3=(I2+(std::pow(r0, 2)-A)*I3)/8.0;
//                    std::cout<<"J1 "<<(J1)<<" J2 "<<(J2)<<" J3 "<<(J3)<<" ri "<<ri<<" zi "<<zi<<std::endl;
                   //delta_vint=(128.0*vJthree*pow(r1,7.)+8.0*vJtwo*pow(r1,5.)+ 2.0*vJone*pow(r1,3.))/3.0-8.0*pow(r1,4.)*vJtwo*(z1+r1);
                    double dV=(128.0*J3*std::pow(ri, 7)+8.0*J2*std::pow(ri,5)+ 2.0*J1*std::pow(ri,3))/3.0-8.0*std::pow(ri,4)*J2*(zi+ri);
                    double dA=2*J1*ri*ri;
                    if(direction==CLOCKWISE){
                        dV=-dV;
                        dA=-dA;
                    }
//                    std::cout<<"full dV "<<(dV)<<" dA "<<dA<<std::endl;
                    return std::forward_as_tuple(dV, dA);
        }

        inline double findAngleBetween(sylvanmats::linear::Vector2d v1, sylvanmats::linear::Vector2d v2){
//            double dot=v1.normalized().dot(v2.normalized());
            //std::cout<<"dot "<<dot<<std::endl;
            //return (dot>=0) ? std::acos(dot) : 2.0*std::numbers::pi-std::acos(dot);
//            v2.normalize();
            return (v2.y()>=0.0) ? std::atan2(v2.y(), v2.x()) : 2.0*std::numbers::pi + std::atan2(v2.y(), v2.x()); 
        }
        inline sylvanmats::linear::Vector2d polarToCartesian(double centerX, double centerY, double radius, double θ) {
            return sylvanmats::linear::Vector2d(centerX + (radius * std::cos(θ)), centerY + (radius * std::sin(θ)));
        };
        inline bool checkInsideAtLeastOneCircle(std::vector<circle<double>>& circles, sylvanmats::linear::Vector2d& center, double r0){
            bool hit=false;
            for(sylvanmats::surface::circle<double>& circleA : circles| std::views::filter([&hit, &r0, &center](sylvanmats::surface::circle<double>& c){return !hit && c.direction==CLOCKWISE && (c.center-center).norm()+r0<c.r0;})){
                hit=true;
            }
            return hit;
        };
        inline void eraseAnyPreviousCirclesInside(std::vector<circle<double>>& circles, sylvanmats::linear::Vector2d& center, double r0){
            bool circleHit=false;
            bool hit=false;
            do{
                circleHit=false;
                for(std::vector<circle<double>>::iterator cit=circles.begin();!circleHit && cit!=circles.end();cit++){
                    if(((*cit).center-center).norm()+(*cit).r0<r0){
                        circles.erase(cit);
                        circleHit=true;
                        hit=true;
                    }
                }
            }while(circleHit);
            if(hit)for(unsigned int index=0;index<circles.size();index++){
                circles[index].id=index;
            }
        };
        inline bool checkOutsideAtLeastOneCircle(std::vector<circle<double>>& circles, sylvanmats::linear::Vector2d& center, double r0){
            bool hit=false;
            for(sylvanmats::surface::circle<double>& circleA : circles| std::views::filter([&hit, &r0, &center](sylvanmats::surface::circle<double>& c){return !hit && (c.center-center).norm()+c.r0<r0;})){
                hit=true;
            }
            return hit;
        };
        inline void eraseAnyPreviousCirclesOutside(std::vector<circle<double>>& circles, sylvanmats::linear::Vector2d& center, double r0){
            bool circleHit=false;
            bool hit=false;
            do{
                circleHit=false;
                for(std::vector<circle<double>>::iterator cit=circles.begin();!circleHit && cit!=circles.end();cit++){
                    if(((*cit).center-center).norm()+r0<(*cit).r0){
                        circles.erase(cit);
                        circleHit=true;
                        hit=true;
                    }
                }
            }while(circleHit);
            if(hit)for(unsigned int index=0;index<circles.size();index++){
                circles[index].id=index;
            }
        };

        inline sylvanmats::linear::Vector2d polarToCartesian(sylvanmats::linear::Vector2d& center, double radius, double angleInRadians) {
            return sylvanmats::linear::Vector2d(center[0] + (radius * std::cos(angleInRadians)), center[1] + (radius * std::sin(angleInRadians)));
        };
    };

}
