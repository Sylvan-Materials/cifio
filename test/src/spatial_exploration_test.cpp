#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <iterator>
#include <numbers>
#include <ranges>
#include <locale>

#include "algebra/geometric/Bivector.h"
#include "algebra/geometric/Rotor.h"

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"
#include "reading/gz/GZReader.h"
#include "reading/tcp/TCPReader.h"
#include "publishing/st/CIFPublisher.h"
#include "publishing/st/OBJPublisher.h"
#include "constitution/Graph.h"
#include "constitution/Populator.h"
#include "constitution/Selection.h"

#include "standards/Protons.h"
#include "space/Octree.h"

TEST_SUITE("space"){

TEST_CASE("test octree"){
    sylvanmats::space::Octree tree(1, 1, 1, 4, 4, 4);
    std::cout << "Insert (3, 3, 3)\n";
    tree.insert(3, 3, 3);
    std::cout << "Insert (3, 3, 4)\n";
    tree.insert(3, 3, 4);
    tree.insert(6, 5, 5);

    std::cout << "Find (3, 3, 3):\n";
    std::cout << (tree.find(3, 3, 3) ? "True\n" : "False\n");
    std::cout << "Find (3, 4, 4):\n";
    std::cout << (tree.find(3, 4, 4) ? "True\n" : "False\n");
    std::cout << "Insert (3, 4, 4)\n";
    tree.insert(3, 4, 4);
    std::cout << "Find (3, 4, 4):\n";
    std::cout << (tree.find(3, 4, 4) ? "True\n" : "False\n");
}

TEST_CASE("test nearest neighbors on 3SGS"){

    SUBCASE("test octree for 3sgs.cif.gz"){
        std::string comp_id="3sgs";
        std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
        sylvanmats::constitution::Graph graph;
        std::filesystem::path  filePath="./"+comp_id+".cif.gz";
        sylvanmats::reading::TCPReader tcpReader;
        tcpReader(url, [&graph, &filePath, &comp_id](std::istream& is){

            sylvanmats::constitution::Populator populator;
            populator(is, graph, [&filePath](sylvanmats::constitution::Graph& graph){
//std::cout<<std::setprecision(16)<<std::scientific;
                auto&& [tlf, brb] = graph.getLimitingCorners();
                CHECK(tlf[0] == doctest::Approx(-2.085));
                CHECK(tlf[1] == doctest::Approx(-3.852));
                CHECK(tlf[2] == doctest::Approx(-7.2255));
                CHECK(brb[0] == doctest::Approx(1.60825));
                CHECK(brb[1] == doctest::Approx(3.217));
                CHECK(brb[2] == doctest::Approx(8.865));
                sylvanmats::linear::Vector3i tlfi(tlf.x(), tlf.y(), tlf.z());
                sylvanmats::linear::Vector3i brbi(brb.x(), brb.y(), brb.z());
                sylvanmats::space::Octree octree(tlfi, brbi);
                for(lemon::ListGraph::NodeIt nR(graph.componentGraph); nR!=lemon::INVALID; ++nR){
                    sylvanmats::linear::Vector3d&& centroid=graph.getComponentCentroid(nR);
                    octree.insert(centroid.x(), centroid.y(), centroid.z());
                    //std::cout<<"centroid "<<centroid.transpose()<<std::endl;
                }
                CHECK(octree.find(1, 0, 0));
            });

        });
        std::filesystem::path path="../../cifio/templates/obj";
        /*sylvanmats::publishing::st::OBJPublisher objPublisher(path);
        objPublisher.setVertexCount(2*12*lemon::countEdges(graph));
        std::vector<std::tuple<double, double, double>> vertexLoop;
        for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
            sylvanmats::linear::Vector3d vA(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z);
            objPublisher.append(vA);            
        }
        for(lemon::ListGraph::EdgeIt eSiteA(graph); eSiteA!=lemon::INVALID; ++eSiteA){
            lemon::ListGraph::Node nSiteA=graph.u(eSiteA);
            lemon::ListGraph::Node nSiteB=graph.v(eSiteA);
            sylvanmats::linear::Vector3d vA(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z);
            sylvanmats::linear::Vector3d vB(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z);
            
            //vertexLoop.insert(vertexLoop.begin(), std::make_tuple(v(0), v(1), v(2)));
        }
        std::string&& content2 = cifPublisher.render();
        std::ofstream ofs2("test.obj");
        ofs2<<content2<<std::endl;*/
    }

}

}