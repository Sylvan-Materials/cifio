#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <iterator>
#include <numbers>
#include <ranges>

#define protected public

#include "algebra/geometric/Bivector.h"
#include "algebra/geometric/Rotor.h"

#include "reading/gz/GZReader.h"
#include "reading/tcp/TCPReader.h"
#include "constitution/Graph.h"
#include "constitution/Populator.h"
#include "constitution/Selection.h"
#include "publishing/st/SVGPublisher.h"

#include "surface/Accessible.h"

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"

TEST_SUITE("accessible"){

TEST_CASE("test accessible two spheres"){
    SUBCASE("in the y-z plane"){
        sylvanmats::constitution::Graph graph;
        lemon::ListGraph::Node atomSiteA=graph.addNode();
        graph.atomSites[atomSiteA].Cartn_x=0.0;
        graph.atomSites[atomSiteA].Cartn_y=0.0;
        graph.atomSites[atomSiteA].Cartn_z=0.0;
        graph.atomSites[atomSiteA].type_symbol="N";
        atomSiteA=graph.addNode();
        graph.atomSites[atomSiteA].Cartn_x=2.0;
        graph.atomSites[atomSiteA].Cartn_y=0.0;
        graph.atomSites[atomSiteA].Cartn_z=0.0;
        graph.atomSites[atomSiteA].type_symbol="N";

        std::cout<<"accessible "<<std::endl;
        sylvanmats::surface::Accessible accessible(graph);
        accessible.probe_radius=0.0;
        accessible.radii["C"]=3.0;
        accessible.radii["N"]=2.0;
        accessible.radii["O"]=1.0;
        accessible.radii["P"]=4.0;
        accessible.radii["S"]=6.0;
        accessible([](std::string name, std::vector<sylvanmats::surface::circle<double>>& circles){
            std::filesystem::path path="../../cifio/templates/svg";
        std::cout<<"to svg "<<std::endl;
            sylvanmats::publishing::st::SVGPublisher svgPublisher(path);
            std::vector<std::tuple<double, double, double, std::string>> circleLoop;
            double maxRadius=0.0;
            for(sylvanmats::surface::circle<double> c : circles){
                circleLoop.insert(circleLoop.begin(), std::make_tuple(c.center[0], c.center[1], c.r0, (c.direction==sylvanmats::surface::COUNTER_CLOCKWISE) ? "green" : "blue"));
                maxRadius=std::max(maxRadius, c.r0);
            }
            svgPublisher.setHeight(4*maxRadius);
            svgPublisher.setWidth(4*maxRadius);
//            svgPublisher.add("offset_x", 2*maxRadius);
//            svgPublisher.add("offset_y", 2*maxRadius);
//            svgPublisher.add("circles", circleLoop);
            std::string&& content = svgPublisher.render();
            std::ofstream ofs2("y_z.svg");
            ofs2<<content<<std::endl;
        });
    }
    SUBCASE("in the x-z plane"){
        sylvanmats::constitution::Graph graph;
        lemon::ListGraph::Node atomSiteA=graph.addNode();
        graph.atomSites[atomSiteA].Cartn_x=0.0;
        graph.atomSites[atomSiteA].Cartn_y=0.0;
        graph.atomSites[atomSiteA].Cartn_z=0.0;
        graph.atomSites[atomSiteA].type_symbol="N";
        atomSiteA=graph.addNode();
        graph.atomSites[atomSiteA].Cartn_x=0.0;
        graph.atomSites[atomSiteA].Cartn_y=2.0;
        graph.atomSites[atomSiteA].Cartn_z=0.0;
        graph.atomSites[atomSiteA].type_symbol="N";

        sylvanmats::surface::Accessible accessible(graph);
        accessible.probe_radius=0.0;
        accessible.radii["C"]=3.0;
        accessible.radii["N"]=2.0;
        accessible.radii["O"]=1.0;
        accessible.radii["P"]=4.0;
        accessible.radii["S"]=6.0;
        accessible([](std::string name, std::vector<sylvanmats::surface::circle<double>>& circles){
            std::filesystem::path path="../../cifio/templates/svg";
            sylvanmats::publishing::st::SVGPublisher svgPublisher(path);
            std::vector<std::tuple<double, double, double, std::string>> circleLoop;
            double maxRadius=0.0;
            for(sylvanmats::surface::circle<double> c : circles){
                circleLoop.insert(circleLoop.begin(), std::make_tuple(c.center[0], c.center[1], c.r0, (c.direction==sylvanmats::surface::COUNTER_CLOCKWISE) ? "green" : "blue"));
                maxRadius=std::max(maxRadius, c.r0);
            }
            svgPublisher.setHeight(4*maxRadius);
            svgPublisher.setWidth(4*maxRadius);
//            svgPublisher.add("offset_x", 2*maxRadius);
//            svgPublisher.add("offset_y", 2*maxRadius);
//            svgPublisher.add("circles", circleLoop);
            std::string&& content = svgPublisher.render();
            std::ofstream ofs2("x_z.svg");
            ofs2<<content<<std::endl;
        });
    }
    SUBCASE("in the x-y plane"){
        sylvanmats::constitution::Graph graph;
        lemon::ListGraph::Node atomSiteA=graph.addNode();
        graph.atomSites[atomSiteA].Cartn_x=0.0;
        graph.atomSites[atomSiteA].Cartn_y=0.0;
        graph.atomSites[atomSiteA].Cartn_z=0.0;
        graph.atomSites[atomSiteA].type_symbol="N";
        atomSiteA=graph.addNode();
        graph.atomSites[atomSiteA].Cartn_x=0.0;
        graph.atomSites[atomSiteA].Cartn_y=0.0;
        graph.atomSites[atomSiteA].Cartn_z=2.0;
        graph.atomSites[atomSiteA].type_symbol="N";

        sylvanmats::surface::Accessible accessible(graph);
        accessible.probe_radius=0.0;
        accessible.radii["C"]=3.0;
        accessible.radii["N"]=2.0;
        accessible.radii["O"]=1.0;
        accessible.radii["P"]=4.0;
        accessible.radii["S"]=6.0;
        accessible([](std::string name, std::vector<sylvanmats::surface::circle<double>>& circles){
            std::filesystem::path path="../../cifio/templates/svg";
            sylvanmats::publishing::st::SVGPublisher svgPublisher(path);
            std::vector<std::tuple<double, double, double, std::string>> circleLoop;
            double maxRadius=0.0;
            for(sylvanmats::surface::circle<double> c : circles){
                circleLoop.insert(circleLoop.begin(), std::make_tuple(c.center[0], c.center[1], c.r0, (c.direction==sylvanmats::surface::COUNTER_CLOCKWISE) ? "green" : "blue"));
                maxRadius=std::max(maxRadius, c.r0);
            }
            svgPublisher.setHeight(4*maxRadius);
            svgPublisher.setWidth(4*maxRadius);
//            svgPublisher.add("offset_x", 2*maxRadius);
//            svgPublisher.add("offset_y", 2*maxRadius);
//            svgPublisher.add("circles", circleLoop);
            std::string&& content = svgPublisher.render();
            std::ofstream ofs2("x_y.svg");
            ofs2<<content<<std::endl;
        });
    }
}

TEST_CASE("test accessible projected arc integrals"){
    sylvanmats::constitution::Graph graph;
    lemon::ListGraph::Node atomSiteA=graph.addNode();
    graph.atomSites[atomSiteA].Cartn_x=0.0;
    graph.atomSites[atomSiteA].Cartn_y=0.0;
    graph.atomSites[atomSiteA].Cartn_z=0.0;
    graph.atomSites[atomSiteA].type_symbol="C";

//    std::cout<<"acos(1.0) "<<std::acos(1.0)<<std::endl;
//    std::cout<<"acos(0.5) "<<std::acos(0.5)<<std::endl;
//    std::cout<<"acos(0.0) "<<std::acos(0.0)<<std::endl;
//    std::cout<<"acos(-0.5) "<<std::acos(-0.5)<<std::endl;
//    std::cout<<"acos(-1.0) "<<std::acos(-1.0)<<std::endl;

    lemon::ListGraph::NodeIt nSiteA(graph);
    std::unordered_map<std::string, double> radii={{"C", 2.0}, {"N", 2.0}, {"O", 1.0}, {"P", 4.0}, {"S", 6.0}};
    double ri=(radii.contains(graph.atomSites[nSiteA].type_symbol)) ? radii[graph.atomSites[nSiteA].type_symbol]: 2.0;
    sylvanmats::surface::AtomAreaExposure atomAreaExposure(graph, nSiteA, ri, radii, 0.0);
    sylvanmats::linear::Vector2d unitX=sylvanmats::linear::Vector2d::UnitX();
    sylvanmats::linear::Vector2d v1(1.0, 0.0);
//    std::cout<<"(1.0, 0.0) "<<atomAreaExposure.findAngleBetween(unitX, v1)<<std::endl;
    sylvanmats::linear::Vector2d v2(0.0, 1.0);
//    std::cout<<"(0.0, 1.0) "<<atomAreaExposure.findAngleBetween(unitX, v2)<<std::endl;
    sylvanmats::linear::Vector2d v3(-1.0, 0.0);
//    std::cout<<"-1.0, 0.0) "<<atomAreaExposure.findAngleBetween(unitX, v3)<<std::endl;
    sylvanmats::linear::Vector2d v4(0.0, -1.0);
//    std::cout<<"(0.0, -1.0) "<<atomAreaExposure.findAngleBetween(unitX, v4)<<std::endl;
    sylvanmats::linear::Vector2d v5(0.5, -0.5);
//    std::cout<<"(0.5, -0.5) "<<atomAreaExposure.findAngleBetween(unitX, v5)<<std::endl;
    lemon::ListDigraph projectedGraph;
    lemon::ListDigraph::ArcMap<sylvanmats::surface::arc<double>> arcMap(projectedGraph);
    lemon::ListDigraph::Node nA=projectedGraph.addNode();
    lemon::ListDigraph::Node nB=projectedGraph.addNode();
    lemon::ListDigraph::Arc e=projectedGraph.addArc(nA, nB);
    arcMap[e].α=1.60183;
    arcMap[e].β=5.39855;
    arcMap[e].center=sylvanmats::linear::Vector2d(1.58809, -6.15121);
    arcMap[e].r0=2.08793;
    arcMap[e].direction=sylvanmats::surface::COUNTER_CLOCKWISE;
    arcMap[e].length=std::abs(arcMap[e].β-arcMap[e].α)*arcMap[e].r0;
    lemon::Path<lemon::ListDigraph> shortestPath;
    shortestPath.addFront(e);
    auto [dV, dA] = atomAreaExposure.integrate(0.0, 3.0, shortestPath, arcMap);
    CHECK(dV == doctest::Approx(0.193042));
    CHECK(dA == doctest::Approx(2.18752));
}

TEST_CASE("test accessible three spheres"){
    sylvanmats::constitution::Graph graph;
    lemon::ListGraph::Node atomSiteA=graph.addNode();
    graph.atomSites[atomSiteA].Cartn_x=-0.75;
    graph.atomSites[atomSiteA].Cartn_y=2.905;//2.90473750965556;
    graph.atomSites[atomSiteA].Cartn_z=0.0;
    graph.atomSites[atomSiteA].type_symbol="O";
    graph.atomSites[atomSiteA].label_atom_id="O";
    atomSiteA=graph.addNode();
    graph.atomSites[atomSiteA].Cartn_x=2.0;
    graph.atomSites[atomSiteA].Cartn_y=0.0;
    graph.atomSites[atomSiteA].Cartn_z=0.0;
    graph.atomSites[atomSiteA].type_symbol="N";
    graph.atomSites[atomSiteA].label_atom_id="N";
    atomSiteA=graph.addNode();
    graph.atomSites[atomSiteA].Cartn_x=0.0;
    graph.atomSites[atomSiteA].Cartn_y=0.0;
    graph.atomSites[atomSiteA].Cartn_z=0.0;
    graph.atomSites[atomSiteA].type_symbol="C";
    graph.atomSites[atomSiteA].label_atom_id="C";

    sylvanmats::surface::Accessible accessible(graph);
    accessible.probe_radius=0.0;
    accessible.radii["C"]=3.0;
    accessible.radii["N"]=2.0;
    accessible.radii["O"]=1.0;
    accessible.radii["P"]=4.0;
    accessible.radii["S"]=6.0;
    accessible([](std::string name, std::vector<sylvanmats::surface::circle<double>>& circles){
        std::filesystem::path path="../../cifio/templates/svg";
        sylvanmats::publishing::st::SVGPublisher svgPublisher(path);
        std::vector<std::tuple<double, double, double, std::string>> circleLoop;
        double maxRadius=0.0;
        for(sylvanmats::surface::circle<double> c : circles){
            circleLoop.insert(circleLoop.begin(), std::make_tuple(c.center[0], c.center[1], c.r0, (c.direction==sylvanmats::surface::COUNTER_CLOCKWISE) ? "green" : "blue"));
            maxRadius=std::max(maxRadius, c.r0);
                maxRadius=std::max(maxRadius, c.r0);
        }
            svgPublisher.setHeight(4*maxRadius);
            svgPublisher.setWidth(4*maxRadius);
//            svgPublisher.add("offset_x", 2*maxRadius);
//            svgPublisher.add("offset_y", 2*maxRadius);
//            svgPublisher.add("circles", circleLoop);
        std::string&& content = svgPublisher.render();
        std::ofstream ofs2("three"+name);
        ofs2<<content<<std::endl;
    });
    for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
        if(graph.atomSites[nSiteA].type_symbol.compare("H")!=0)std::cout<<"three /"<<graph.atomSites[nSiteA].label_comp_id<<"/"<<graph.atomSites[nSiteA].label_seq_id<<"/"<<graph.atomSites[nSiteA].label_atom_id<<" "<<accessible.getAccessibleSurface()[nSiteA].atom_site_volume<<" "<<accessible.getAccessibleSurface()[nSiteA].atom_site_area<<std::endl;
    }
    std::cout<<"V "<<accessible.getVolume()<<" A "<<accessible.getArea()<<std::endl;
}

TEST_CASE("test accessible eight spheres"){
    sylvanmats::constitution::Graph graph;
    lemon::ListGraph::Node atomSiteA=graph.addNode();
    graph.atomSites[atomSiteA].id=1;
    graph.atomSites[atomSiteA].Cartn_x=0.0;
    graph.atomSites[atomSiteA].Cartn_y=0.0;
    graph.atomSites[atomSiteA].Cartn_z=0.0;
    graph.atomSites[atomSiteA].type_symbol="N";
    graph.atomSites[atomSiteA].label_atom_id="N";
    graph.atomSites[atomSiteA].label_comp_id="EIG";
    atomSiteA=graph.addNode();
    graph.atomSites[atomSiteA].id=2;
    graph.atomSites[atomSiteA].Cartn_x=-5.0;
    graph.atomSites[atomSiteA].Cartn_y=0.0;
    graph.atomSites[atomSiteA].Cartn_z=0.0;
    graph.atomSites[atomSiteA].type_symbol="S";
    graph.atomSites[atomSiteA].label_atom_id="S";
    graph.atomSites[atomSiteA].label_comp_id="EIG";
    atomSiteA=graph.addNode();
    graph.atomSites[atomSiteA].id=3;
    graph.atomSites[atomSiteA].Cartn_x=5.0;
    graph.atomSites[atomSiteA].Cartn_y=0.0;
    graph.atomSites[atomSiteA].Cartn_z=0.0;
    graph.atomSites[atomSiteA].type_symbol="S";
    graph.atomSites[atomSiteA].label_atom_id="S";
    graph.atomSites[atomSiteA].label_comp_id="EIG";
    atomSiteA=graph.addNode();
    graph.atomSites[atomSiteA].id=4;
    graph.atomSites[atomSiteA].Cartn_x=0.0;
    graph.atomSites[atomSiteA].Cartn_y=1.0;
    graph.atomSites[atomSiteA].Cartn_z=0.0;
    graph.atomSites[atomSiteA].type_symbol="P";
    graph.atomSites[atomSiteA].label_atom_id="P";
    graph.atomSites[atomSiteA].label_comp_id="EIG";
    atomSiteA=graph.addNode();
    graph.atomSites[atomSiteA].id=5;
    graph.atomSites[atomSiteA].Cartn_x=-1.0;
    graph.atomSites[atomSiteA].Cartn_y=2.0;
    graph.atomSites[atomSiteA].Cartn_z=3.0;
    graph.atomSites[atomSiteA].type_symbol="N";
    graph.atomSites[atomSiteA].label_atom_id="N";
    graph.atomSites[atomSiteA].label_comp_id="EIG";
    atomSiteA=graph.addNode();
    graph.atomSites[atomSiteA].id=6;
    graph.atomSites[atomSiteA].Cartn_x=1.0;
    graph.atomSites[atomSiteA].Cartn_y=1.0;
    graph.atomSites[atomSiteA].Cartn_z=1.0;
    graph.atomSites[atomSiteA].type_symbol="O";
    graph.atomSites[atomSiteA].label_atom_id="O";
    graph.atomSites[atomSiteA].label_comp_id="EIG";
    atomSiteA=graph.addNode();
    graph.atomSites[atomSiteA].id=7;
    graph.atomSites[atomSiteA].Cartn_x=0.0;
    graph.atomSites[atomSiteA].Cartn_y=0.0;
    graph.atomSites[atomSiteA].Cartn_z=1.0;
    graph.atomSites[atomSiteA].type_symbol="N";
    graph.atomSites[atomSiteA].label_atom_id="N";
    graph.atomSites[atomSiteA].label_comp_id="EIG";
    atomSiteA=graph.addNode();
    graph.atomSites[atomSiteA].id=8;
    graph.atomSites[atomSiteA].Cartn_x=10.0;
    graph.atomSites[atomSiteA].Cartn_y=0.0;
    graph.atomSites[atomSiteA].Cartn_z=0.0;
    graph.atomSites[atomSiteA].type_symbol="S";
    graph.atomSites[atomSiteA].label_atom_id="S";
    graph.atomSites[atomSiteA].label_comp_id="EIG";
    CHECK_EQ(lemon::countNodes(graph), 8);

    SUBCASE("test first of eight spheres"){
        lemon::ListGraph::NodeIt nSiteA(graph);
        ++nSiteA;
        ++nSiteA;
        ++nSiteA;
        ++nSiteA;
//        ++nSiteA;
        //++nSiteA;
        //++nSiteA;
        std::unordered_map<std::string, double> radii={{"C", 3.0}, {"N", 2.0}, {"O", 1.0}, {"P", 4.0}, {"S", 6.0}};
        double ri=(radii.contains(graph.atomSites[nSiteA].type_symbol)) ? radii[graph.atomSites[nSiteA].type_symbol]: 2.0;
        sylvanmats::surface::AtomAreaExposure atomAreaExposure(graph, nSiteA, ri, radii, 0.0);
        atomAreaExposure.probe_radius=0.0;
        atomAreaExposure.radii["C"]=3.0;
        atomAreaExposure.radii["N"]=2.0;
        atomAreaExposure.radii["O"]=1.0;
        atomAreaExposure.radii["P"]=4.0;
        atomAreaExposure.radii["S"]=6.0;
        bool engulfed=false;
        std::vector<sylvanmats::surface::circle<double>>&& circles=atomAreaExposure.project(graph, nSiteA, ri, engulfed);
        if(atomAreaExposure.adjustAnyTangentNorthPoles(graph, nSiteA, ri, circles))std::cout<<"hit at least one north pole "<<std::endl;
        if(!engulfed){
            lemon::ListDigraph projectedGraph;
            lemon::ListDigraph::ArcMap<sylvanmats::surface::arc<double>> arcMap(projectedGraph);
            auto [clockwiseCount, countOverlays, countIntersections, countFulls] = atomAreaExposure.graphProjection(projectedGraph, circles, arcMap);
            std::cout<<" circles.size() "<<circles.size()<<" "<<graph.atomSites[nSiteA].id<<" "<<graph.atomSites[nSiteA].label_atom_id<<" clockwiseCount "<<clockwiseCount<<" circles: "<<circles.size()<<" nodes: "<<lemon::countNodes(projectedGraph)<<" arcs: "<<lemon::countArcs(projectedGraph)<<" countOverlays: "<<countOverlays<<" countIntersections "<<countIntersections<<" countFulls: "<<countFulls<<std::endl;
            clockwiseCount=0;
            auto [dV, dA] = atomAreaExposure.integrateAlongDomainPath(countIntersections, countFulls, graph.atomSites[nSiteA].Cartn_z, ri, projectedGraph, circles, arcMap, clockwiseCount);
            std::cout<<"domain connectivity nodes: "<<lemon::countNodes(projectedGraph)<<" edges: "<<lemon::countArcs(projectedGraph)<<" connected "<<lemon::countConnectedComponents(projectedGraph)<<" "<<lemon::countStronglyConnectedComponents(projectedGraph)<<" eulerian? "<<lemon::eulerian(projectedGraph)<<" loop? "<<lemon::loopFree(projectedGraph)<<" parallel? "<<lemon::parallelFree(projectedGraph)<<" "<<std::endl;
            if(clockwiseCount==0 || circles.size()<=1){
                dV+=(4.0*std::numbers::pi*std::pow(ri, 3)/3.0);
                dA+=(4.0*std::numbers::pi*std::pow(ri, 2));
            }
            std::cout<<"atom site V /"<<graph.atomSites[nSiteA].label_comp_id<<"/"<<graph.atomSites[nSiteA].label_seq_id<<"/"<<graph.atomSites[nSiteA].label_atom_id<<" "<<dV<<" "<<dA<<std::endl;
            std::filesystem::path path="../../cifio/templates/svg";
            sylvanmats::publishing::st::SVGPublisher svgPublisher(path);
            std::vector<std::tuple<double, double, double, std::string>> circleLoop;
            double maxRadius=20.0;
            double scale=1.0;
            for(sylvanmats::surface::circle<double> c : circles){
                circleLoop.insert(circleLoop.begin(), std::make_tuple(scale*c.center[0], scale*c.center[1], scale*c.r0, (c.direction==sylvanmats::surface::COUNTER_CLOCKWISE) ? "green" : "blue"));
                maxRadius=std::max(maxRadius, c.r0+c.center.norm());
            }
            svgPublisher.setHeight(8*maxRadius);
            svgPublisher.setWidth(8*maxRadius);
            svgPublisher.setXOffset(4*maxRadius);
            svgPublisher.setYOffset(4*maxRadius);
            svgPublisher.addCircles("circles", circleLoop);
            std::vector<std::tuple<double, double, double, double, double, int, std::string>> arcLoop;
            std::vector<std::tuple<double, double, double, double, double, int, std::string, double>> textLoop;
            for(lemon::ListDigraph::ArcIt aSiteA(projectedGraph); aSiteA!=lemon::INVALID; ++aSiteA){
                double α=arcMap[aSiteA].α;
                double β=arcMap[aSiteA].β;
                std::cout<<"arcs "<<arcMap[aSiteA].member_of_domain<<" "<<scale*arcMap[aSiteA].r0<<" "<<(180.0*α/std::numbers::pi)<<" "<<(180.0*β/std::numbers::pi)<<" "<<(180.0*(β-α)/std::numbers::pi)<<" "<<(arcMap[aSiteA].r0*(β-α))<<" l="<<arcMap[aSiteA].length<<std::endl;
                if(arcMap[aSiteA].member_of_domain)std::swap(α, β);
                arcLoop.insert(arcLoop.begin(), std::make_tuple(scale*arcMap[aSiteA].center[0], scale*arcMap[aSiteA].center[1], scale*arcMap[aSiteA].r0, α, β, (arcMap[aSiteA].direction==sylvanmats::surface::COUNTER_CLOCKWISE) ? 1 : 1, (arcMap[aSiteA].member_of_domain) ? "black" : "red"));
                textLoop.insert(textLoop.begin(), std::make_tuple(scale*arcMap[aSiteA].center[0], scale*arcMap[aSiteA].center[1], scale*arcMap[aSiteA].r0, arcMap[aSiteA].α, arcMap[aSiteA].β, (arcMap[aSiteA].direction==sylvanmats::surface::COUNTER_CLOCKWISE) ? 1 : 0, (arcMap[aSiteA].member_of_domain) ? "black" : "red", arcMap[aSiteA].length));
            }
            std::cout<<"# of arcs "<<arcLoop.size()<<std::endl;
            svgPublisher.addWedges("arcs", arcLoop);
//            std::cout<<"set has_text "<<std::endl;
////            svgPublisher.rawSetAttribute("has_text", true); 
//            std::cout<<"set has_text loop"<<std::endl;
//            for(lemon::ListDigraph::ArcIt aSiteA(projectedGraph); aSiteA!=lemon::INVALID; ++aSiteA){
//                textLoop.insert(textLoop.begin(), std::make_tuple(scale*arcMap[aSiteA].center[0], scale*arcMap[aSiteA].center[1], scale*arcMap[aSiteA].r0, arcMap[aSiteA].α, arcMap[aSiteA].β, (arcMap[aSiteA].direction==sylvanmats::surface::COUNTER_CLOCKWISE) ? 1 : 0, (arcMap[aSiteA].member_of_domain) ? "black" : "red", arcMap[aSiteA].length));
//            }
            std::cout<<"# of text listed "<<textLoop.size()<<std::endl;
            svgPublisher.addLabelToWedges("text_list", textLoop);
            std::string&& content = svgPublisher.render();
            std::ofstream ofs2("eight_one.svg");
            ofs2<<content<<std::endl;
        }
        else std::cout<<"engulfed /"<<graph.atomSites[nSiteA].label_comp_id<<"/"<<graph.atomSites[nSiteA].label_seq_id<<"/"<<graph.atomSites[nSiteA].label_atom_id<<""<<std::endl;
    }

    SUBCASE("test all eight spheres"){
        sylvanmats::surface::Accessible accessible(graph);
        accessible.probe_radius=0.0;
        accessible.radii["C"]=3.0;
        accessible.radii["N"]=2.0;
        accessible.radii["O"]=1.0;
        accessible.radii["P"]=4.0;
        accessible.radii["S"]=6.0;
        accessible([](std::string name, std::vector<sylvanmats::surface::circle<double>>& circles){
            std::filesystem::path path="../templates/svg";
            sylvanmats::publishing::st::SVGPublisher svgPublisher(path);
            std::vector<std::tuple<double, double, double, std::string>> circleLoop;
            double maxRadius=0.0;
            for(sylvanmats::surface::circle<double> c : circles){
                circleLoop.insert(circleLoop.begin(), std::make_tuple(c.center[0], c.center[1], c.r0, (c.direction==sylvanmats::surface::COUNTER_CLOCKWISE) ? "green" : "blue"));
                maxRadius=std::max(maxRadius, c.r0+c.center.norm());
            }
            svgPublisher.setHeight(4*maxRadius);
            svgPublisher.setWidth(4*maxRadius);
//            svgPublisher.add("offset_x", 2*maxRadius);
//            svgPublisher.add("offset_y", 2*maxRadius);
//            svgPublisher.add("circles", circleLoop);
            std::string&& content = svgPublisher.render();
            std::ofstream ofs2("eight"+name);
            ofs2<<content<<std::endl;
        });
    for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
        if(graph.atomSites[nSiteA].type_symbol.compare("H")!=0)std::cout<<"eight /"<<graph.atomSites[nSiteA].label_comp_id<<"/"<<graph.atomSites[nSiteA].label_seq_id<<"/"<<graph.atomSites[nSiteA].label_atom_id<<" "<<accessible.getAccessibleSurface()[nSiteA].atom_site_volume<<" "<<accessible.getAccessibleSurface()[nSiteA].atom_site_area<<" "<<graph.atomSites[nSiteA].Cartn_x<<","<<graph.atomSites[nSiteA].Cartn_y<<","<<graph.atomSites[nSiteA].Cartn_z<<std::endl;
    }
    std::cout<<"V "<<accessible.getVolume()<<" A "<<accessible.getArea()<<std::endl;
    }
}

TEST_CASE("test accessible 3sgs.cif.gz"){
    std::string comp_id="3sgs";
    std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
    sylvanmats::constitution::Graph graph;
    std::filesystem::path  filePath="./"+comp_id+".cif.gz";
    sylvanmats::reading::TCPReader tcpReader;
    tcpReader(url, [&graph, &filePath, &comp_id](std::istream& is){
        sylvanmats::constitution::Populator populator;
        populator(is, graph, [&filePath](sylvanmats::constitution::Graph& graph){
       CHECK_EQ(graph.getNumberOfAtomSites(), 46);
       CHECK_EQ(graph.getCell().length_a, 4.821);
       CHECK_EQ(graph.getCell().length_b, 19.5);
       CHECK_EQ(graph.getCell().length_c, 21.004);
       CHECK_EQ(graph.getCell().angle_alpha, 90.000);
       CHECK_EQ(graph.getCell().angle_beta, 94.23);
       CHECK_EQ(graph.getCell().angle_gamma, 90.000);
       CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 1 21 1");
       CHECK_EQ(graph.getSymmetry().Int_Tables_number, 4);
       });
       CHECK_EQ(graph.getNumberOfAtomSites(), 46);
       CHECK_EQ(lemon::countEdges(graph), 43);
       CHECK_EQ(lemon::countNodes(graph.componentGraph), 8);
       CHECK_EQ(lemon::countEdges(graph.componentGraph), 4);
//       std::cout<<"Accessible "<<std::endl;
        sylvanmats::surface::Accessible accessible(graph);
        
        accessible([](std::string name, std::vector<sylvanmats::surface::circle<double>>& circles){
            /*std::filesystem::path path="../../cifio/templates/svg";
            sylvanmats::publishing::st::SVGPublisher svgPublisher(path);
            std::vector<std::tuple<double, double, double, std::string>> circleLoop;
            double maxRadius=0.0;
            for(sylvanmats::surface::circle<double> c : circles){
                circleLoop.insert(circleLoop.begin(), std::make_tuple(c.t0, c.s0, c.r0, (c.direction==sylvanmats::surface::COUNTER_CLOCKWISE) ? "green" : "blue"));
                maxRadius=std::max(maxRadius, c.r0);
            }
            svgPublisher.setHeight(4*maxRadius);
            svgPublisher.setWidth(4*maxRadius);
//            svgPublisher.add("offset_x", 2*maxRadius);
//            svgPublisher.add("offset_y", 2*maxRadius);
//            svgPublisher.add("circles", circleLoop);
            std::string&& content = svgPublisher.render();
            std::ofstream ofs2(name);
            ofs2<<content<<std::endl;*/
        });
        for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
            std::cout<<graph.atomSites[nSiteA].label_atom_id<<" "<<accessible.getAccessibleSurface()[nSiteA].atom_site_volume<<" "<<accessible.getAccessibleSurface()[nSiteA].atom_site_area<<std::endl;
        }
        std::cout<<"V "<<accessible.getVolume()<<" A "<<accessible.getArea()<<std::endl;
    });

}

TEST_CASE("test accessible 1j4m.cif.gz"){
    std::string comp_id="1j4m";
    std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
    sylvanmats::constitution::Graph graph;
    std::filesystem::path  filePath="./"+comp_id+".cif.gz";
    sylvanmats::reading::TCPReader tcpReader;
    tcpReader(url, [&graph, &filePath, &comp_id](std::istream& is){
        sylvanmats::constitution::Populator populator;
        populator(is, graph, [&filePath](sylvanmats::constitution::Graph& graph){
       CHECK_EQ(graph.getNumberOfAtomSites(), 236);
       CHECK_EQ(graph.getCell().length_a, 4.821);
       CHECK_EQ(graph.getCell().length_b, 19.5);
       CHECK_EQ(graph.getCell().length_c, 21.004);
       CHECK_EQ(graph.getCell().angle_alpha, 90.000);
       CHECK_EQ(graph.getCell().angle_beta, 94.23);
       CHECK_EQ(graph.getCell().angle_gamma, 90.000);
       CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 1 21 1");
       CHECK_EQ(graph.getSymmetry().Int_Tables_number, 4);
       });
       CHECK_EQ(graph.getNumberOfAtomSites(), 236);
       CHECK_EQ(lemon::countEdges(graph), 239);
       CHECK_EQ(lemon::countNodes(graph.componentGraph), 14);
       CHECK_EQ(lemon::countEdges(graph.componentGraph), 12);
//       std::cout<<"Accessible "<<std::endl;
//       graph.hydrogenVisibilityOff();
        sylvanmats::surface::Accessible accessible(graph);
        
        accessible([](std::string name, std::vector<sylvanmats::surface::circle<double>>& circles){
            /*std::filesystem::path path="../../cifio/templates/svg";
            sylvanmats::publishing::st::SVGPublisher svgPublisher(path);
            std::vector<std::tuple<double, double, double, std::string>> circleLoop;
            double maxRadius=0.0;
            for(sylvanmats::surface::circle<double> c : circles){
                circleLoop.insert(circleLoop.begin(), std::make_tuple(c.t0, c.s0, c.r0, (c.direction==sylvanmats::surface::COUNTER_CLOCKWISE) ? "green" : "blue"));
                maxRadius=std::max(maxRadius, c.r0);
            }
            svgPublisher.setHeight(4*maxRadius);
            svgPublisher.setWidth(4*maxRadius);
//            svgPublisher.add("offset_x", 2*maxRadius);
//            svgPublisher.add("offset_y", 2*maxRadius);
//            svgPublisher.add("circles", circleLoop);
            std::string&& content = svgPublisher.render();
            std::ofstream ofs2(name);
            ofs2<<content<<std::endl;*/
        });
        for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
            if(graph.atomSites[nSiteA].type_symbol.compare("H")!=0)std::cout<<"/"<<graph.atomSites[nSiteA].label_comp_id<<"/"<<graph.atomSites[nSiteA].label_seq_id<<"/"<<graph.atomSites[nSiteA].label_atom_id<<" "<<accessible.getAccessibleSurface()[nSiteA].atom_site_volume<<" "<<accessible.getAccessibleSurface()[nSiteA].atom_site_area<<std::endl;
        }
        std::cout<<"V "<<accessible.getVolume()<<" A "<<accessible.getArea()<<std::endl;
    });

}

}
