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

TEST_SUITE("protons"){

TEST_CASE("test putting standard protons on 3SGS"){

    SUBCASE("test tcp for 3sgs.cif.gz"){
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
           CHECK_EQ(graph.getCell().angle_alpha, 90.0);
           CHECK_EQ(graph.getCell().angle_beta, 94.23);
           CHECK_EQ(graph.getCell().angle_gamma, 90.000);
           CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 1 21 1");
           CHECK_EQ(graph.getSymmetry().Int_Tables_number, 4);
                sylvanmats::standards::Protons protons;
                protons(graph);
           });
           CHECK_EQ(graph.getNumberOfAtomSites(), 81);
           CHECK_EQ(lemon::countEdges(graph), 78);
           CHECK_EQ(lemon::countNodes(graph.componentGraph), 8);
           CHECK_EQ(lemon::countEdges(graph.componentGraph), 4);

        });
        std::filesystem::path path="../../cifio/templates/cif";
        sylvanmats::publishing::st::CIFPublisher cifPublisher(path);
        cifPublisher.setEntryID("3SGS");
        std::vector<std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>> atomSitesLoop;
        for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
            cifPublisher.insertAtomSites(std::make_tuple(graph.atomSites[nSiteA].group_PDB, graph.atomSites[nSiteA].id, graph.atomSites[nSiteA].type_symbol, graph.atomSites[nSiteA].label_atom_id, graph.atomSites[nSiteA].label_alt_id, graph.atomSites[nSiteA].label_comp_id, graph.atomSites[nSiteA].label_asym_id, graph.atomSites[nSiteA].label_entity_id, graph.atomSites[nSiteA].label_seq_id, graph.atomSites[nSiteA].pdbx_PDB_ins_code, graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z, graph.atomSites[nSiteA].occupancy, graph.atomSites[nSiteA].B_iso_or_equiv, graph.atomSites[nSiteA].pdbx_formal_charge, graph.atomSites[nSiteA].auth_seq_id, graph.atomSites[nSiteA].auth_comp_id, graph.atomSites[nSiteA].auth_asym_id, graph.atomSites[nSiteA].auth_atom_id, graph.atomSites[nSiteA].pdbx_PDB_model_num));
        }
        std::string&& content = cifPublisher.render();
        std::ofstream ofs("test_3sgs.cif");
        ofs<<content<<std::endl;
        path="../../cifio/templates/obj";
        sylvanmats::publishing::st::OBJPublisher objPublisher(path);
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
        ofs2<<content2<<std::endl;
    }
}

TEST_CASE("test putting standard protons on"){

    SUBCASE("test tcp for 4k7t.cif.gz"){
        std::string comp_id="4k7t";
        std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
        sylvanmats::constitution::Graph graph;
        std::filesystem::path  filePath="./"+comp_id+".cif.gz";
        sylvanmats::reading::TCPReader tcpReader;
        tcpReader(url, [&graph, &filePath, &comp_id](std::istream& is){

            sylvanmats::constitution::Populator populator;
            populator(is, graph, [&filePath](sylvanmats::constitution::Graph& graph){
           CHECK_EQ(graph.getNumberOfAtomSites(), 260);
           CHECK_EQ(graph.getCell().length_a, 35.98);
           CHECK_EQ(graph.getCell().length_b, 35.98);
           CHECK_EQ(graph.getCell().length_c, 16.12);
           CHECK_EQ(graph.getCell().angle_alpha, 90.0);
           CHECK_EQ(graph.getCell().angle_beta, 90.000);
           CHECK_EQ(graph.getCell().angle_gamma, 120.000);
           CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 62");
           CHECK_EQ(graph.getSymmetry().Int_Tables_number, 171);
                sylvanmats::standards::Protons protons;
                protons(graph);
           });
           CHECK_EQ(graph.getNumberOfAtomSites(), 273);
           CHECK_EQ(lemon::countEdges(graph), 242);
           CHECK_EQ(lemon::countNodes(graph.componentGraph), 35);
           CHECK_EQ(lemon::countEdges(graph.componentGraph), 10);

        });
        std::filesystem::path path="../../cifio/templates/cif";
        sylvanmats::publishing::st::CIFPublisher cifPublisher(path);
        cifPublisher.setEntryID("4K7T");
        std::vector<std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>> atomSitesLoop;
        for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
            cifPublisher.insertAtomSites(std::make_tuple(graph.atomSites[nSiteA].group_PDB, graph.atomSites[nSiteA].id, graph.atomSites[nSiteA].type_symbol, graph.atomSites[nSiteA].label_atom_id, graph.atomSites[nSiteA].label_alt_id, graph.atomSites[nSiteA].label_comp_id, graph.atomSites[nSiteA].label_asym_id, graph.atomSites[nSiteA].label_entity_id, graph.atomSites[nSiteA].label_seq_id, graph.atomSites[nSiteA].pdbx_PDB_ins_code, graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z, graph.atomSites[nSiteA].occupancy, graph.atomSites[nSiteA].B_iso_or_equiv, graph.atomSites[nSiteA].pdbx_formal_charge, graph.atomSites[nSiteA].auth_seq_id, graph.atomSites[nSiteA].auth_comp_id, graph.atomSites[nSiteA].auth_asym_id, graph.atomSites[nSiteA].auth_atom_id, graph.atomSites[nSiteA].pdbx_PDB_model_num));
        }
        std::string&& content = cifPublisher.render();
        std::ofstream ofs("test.cif");
        ofs<<content<<std::endl;
        path="../templates/obj";
        sylvanmats::publishing::st::OBJPublisher objPublisher(path);
        objPublisher.setVertexCount(2*12*lemon::countEdges(graph));
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
        ofs2<<content2<<std::endl;
    }

    SUBCASE("test standard protons for 6fgr"){
        std::string comp_id="6fgr";
        std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
        sylvanmats::constitution::Graph graph;
        std::filesystem::path  filePath="./"+comp_id+".cif.gz";
        sylvanmats::reading::TCPReader tcpReader;
        tcpReader(url, [&graph, &filePath, &comp_id](std::istream& is){

            sylvanmats::constitution::Populator populator;
            populator(is, graph, [&filePath](sylvanmats::constitution::Graph& graph){
           CHECK_EQ(graph.getNumberOfAtomSites(), 127);
           CHECK_EQ(graph.getCell().length_a, 4.83);
           CHECK_EQ(graph.getCell().length_b, 22.38);
           CHECK_EQ(graph.getCell().length_c, 23.06);
           CHECK_EQ(graph.getCell().angle_alpha, 107.0);
           CHECK_EQ(graph.getCell().angle_beta, 90.01);
           CHECK_EQ(graph.getCell().angle_gamma, 96.2);
           CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 1");
           CHECK_EQ(graph.getSymmetry().Int_Tables_number, 1);
                sylvanmats::standards::Protons protons;
                protons(graph);
           });
           CHECK_EQ(graph.getNumberOfAtomSites(), 246);
           CHECK_EQ(lemon::countEdges(graph), 230);
           CHECK_EQ(lemon::countNodes(graph.componentGraph), 18);
           CHECK_EQ(lemon::countEdges(graph.componentGraph), 9);

        });
        std::cout<<" cif obj out"<<std::endl;
        std::filesystem::path path="../templates/cif";
        /*sylvanmats::publishing::st::CIFPublisher cifPublisher(path);
        cifPublisher.add("entry_id", "3SGS");
        std::vector<std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>> atomSitesLoop;
        for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
            atomSitesLoop.insert(atomSitesLoop.begin(), std::make_tuple(graph.atomSites[nSiteA].group_PDB, graph.atomSites[nSiteA].id, graph.atomSites[nSiteA].type_symbol, graph.atomSites[nSiteA].label_atom_id, graph.atomSites[nSiteA].label_alt_id, graph.atomSites[nSiteA].label_comp_id, graph.atomSites[nSiteA].label_asym_id, graph.atomSites[nSiteA].label_entity_id, graph.atomSites[nSiteA].label_seq_id, graph.atomSites[nSiteA].pdbx_PDB_ins_code, graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z, graph.atomSites[nSiteA].occupancy, graph.atomSites[nSiteA].B_iso_or_equiv, graph.atomSites[nSiteA].pdbx_formal_charge, graph.atomSites[nSiteA].auth_seq_id, graph.atomSites[nSiteA].auth_comp_id, graph.atomSites[nSiteA].auth_asym_id, graph.atomSites[nSiteA].auth_atom_id, graph.atomSites[nSiteA].pdbx_PDB_model_num));
        }
        cifPublisher.add("atom_sites", atomSitesLoop);
        std::string&& content = cifPublisher.render();
        //std::cout<<"\n"<<content<<std::endl;
        std::ofstream ofs("test.cif");
        ofs<<content<<std::endl;*/
        path="../templates/obj";
        sylvanmats::publishing::st::OBJPublisher objPublisher(path);
        objPublisher.setVertexCount(2*12*lemon::countEdges(graph));
        objPublisher.setFaceCount(12*lemon::countEdges(graph));
        objPublisher.setMaterialCount(1);
        std::vector<std::tuple<double, double, double, double, double, double>> vertexLoop;
        std::vector<std::tuple<double, double, double>> normalLoop;
        std::vector<std::tuple<unsigned long long, unsigned long long, unsigned long long, unsigned long long, unsigned long long, unsigned long long, unsigned long long, unsigned long long>> indexLoop;
        unsigned int faceIndex=1;
        unsigned int connectionIndex=0;
        sylvanmats::linear::Vector3d t = {0.0, 0.0, 0.0};
        for(lemon::ListGraph::EdgeIt eSiteA(graph); eSiteA!=lemon::INVALID; ++eSiteA){
            lemon::ListGraph::Node nSiteA=graph.u(eSiteA);
            lemon::ListGraph::Node nSiteB=graph.v(eSiteA);
            sylvanmats::linear::Vector3d pA(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z);
            sylvanmats::linear::Vector3d pB(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z);
            sylvanmats::linear::Vector3d rotationVector=pB-pA;
            sylvanmats::linear::Vector3d averageVector=(pB+pA)/2.0;
            double d=rotationVector.norm();
            sylvanmats::linear::Vector3d ek=sylvanmats::linear::Vector3d::UnitX();
            double dot=std::abs(rotationVector.dot(sylvanmats::linear::Vector3d::UnitX()));
            if(std::abs(rotationVector.dot(sylvanmats::linear::Vector3d::UnitY()))<dot){ek=sylvanmats::linear::Vector3d::UnitY();dot=std::abs(rotationVector.dot(sylvanmats::linear::Vector3d::UnitY()));}
            if(std::abs(rotationVector.dot(sylvanmats::linear::Vector3d::UnitZ()))<dot)ek=sylvanmats::linear::Vector3d::UnitZ();
            ek.normalize();
            //ek=sylvanmats::linear::Vector3d(0.0, 1.0, 0.0);
            sylvanmats::linear::Vector3d vk=rotationVector.normalized().cross(ek);
            sylvanmats::linear::Vector3d vn=rotationVector.normalized().cross(vk);
            vn.normalize();
            double cylinderRadius=0.075;
            //std::cout<<rotationVector.transpose()<<" "<<averageVector.transpose()<<" vk "<<vk<<" vn "<<vn.transpose()<<" "<<d<<" angle "<<(180.0*std::atan(cylinderRadius/(d/2.0))/std::numbers::pi)<<std::endl;
            sylvanmats::algebra::geometric::Motord quatA(t, (vk).normalized(), std::atan(cylinderRadius/(d/2.0)));
            sylvanmats::algebra::geometric::Motord quatB(t, (vk).normalized(), -std::atan(cylinderRadius/(d/2.0)));
            sylvanmats::linear::Vector3d vA=quatA*(pA-averageVector);
            sylvanmats::linear::Vector3d vB=quatB*(pB-averageVector);
            //std::cout<<"vA "<<vA.transpose()<<"vB "<<vB.transpose()<<std::endl;
            sylvanmats::algebra::geometric::Motord quat(t, rotationVector.normalized(), std::numbers::pi/6.0);
            sylvanmats::algebra::geometric::Motord quatHalf(t, rotationVector.normalized(), std::numbers::pi/6.0/2.0);
            vn=quatHalf*vn;
            sylvanmats::linear::Vector3d v=vA+averageVector;
            sylvanmats::linear::Vector3d c(0.8, 0.8, 0.8);
            if(graph.atomSites[nSiteA].type_symbol.compare("C")==0)c=sylvanmats::linear::Vector3d(1.0, 0.0, 0.0);
            else if(graph.atomSites[nSiteA].type_symbol.compare("O")==0)c=sylvanmats::linear::Vector3d(0.0, 0.0, 1.0);
            else if(graph.atomSites[nSiteA].type_symbol.compare("N")==0)c=sylvanmats::linear::Vector3d(0.0, 1.0, 0.0);
            vertexLoop.insert(vertexLoop.begin(), std::make_tuple(v[0], v[1], v[2], c[0], c[1], c[2]));
            for(unsigned int index=0;index<11;index++){
                vA=quat*(vA);
                v=vA+averageVector;
                vertexLoop.insert(vertexLoop.begin(), std::make_tuple(v[0], v[1], v[2], c[0], c[1], c[2]));
            }
            v=vB+averageVector;
            if(graph.atomSites[nSiteB].type_symbol.compare("C")==0)c=sylvanmats::linear::Vector3d(1.0, 0.0, 0.0);
            else if(graph.atomSites[nSiteB].type_symbol.compare("O")==0)c=sylvanmats::linear::Vector3d(0.0, 0.0, 1.0);
            else if(graph.atomSites[nSiteB].type_symbol.compare("N")==0)c=sylvanmats::linear::Vector3d(0.0, 1.0, 0.0);
            else c=sylvanmats::linear::Vector3d(0.7, 0.7, 0.8);
            vertexLoop.insert(vertexLoop.begin(), std::make_tuple(v[0], v[1], v[2], c[0], c[1], c[2]));
            for(unsigned int index=0;index<11;index++){
                vB=quat*(vB);
                v=vB+averageVector;
                vertexLoop.insert(vertexLoop.begin(), std::make_tuple(v[0], v[1], v[2], c[0], c[1], c[2]));
            }
            normalLoop.insert(normalLoop.begin(), std::make_tuple(vn[0], vn[1], vn[2]));
            for(unsigned int index=0;index<11;index++){
                vn=quat*vn;
                normalLoop.insert(normalLoop.begin(), std::make_tuple(vn[0], vn[1], vn[2]));
            }
            for(unsigned long long index=1+connectionIndex;index<=12-1+connectionIndex;index++){//[ a, c, b, c, b, d ]
                indexLoop.insert(indexLoop.begin(), std::make_tuple(index, faceIndex, index+1, faceIndex, index+12, faceIndex, index+12+1, faceIndex));
                //indexLoop.insert(indexLoop.begin(), std::make_tuple(index+12, 2*index, index, 2*index, index+12+1, 2*index));
                faceIndex++;
            }
            unsigned long long index=1+connectionIndex;
            indexLoop.insert(indexLoop.begin(), std::make_tuple(index-1+12, faceIndex, index, faceIndex, index-1+2*12, faceIndex, index+12, faceIndex));
            //indexLoop.insert(indexLoop.begin(), std::make_tuple(2*12, 2*index, 12, 2*index, 12+1, 2*index));
            connectionIndex+=24;
            faceIndex++;
        }
//        objPublisher.add("vertices", vertexLoop);
//        objPublisher.add("texture_vertices", vertexLoop);
//        objPublisher.add("normals", normalLoop);
//        objPublisher.add("indices", indexLoop);
        std::string&& content2 = objPublisher.render();
        std::ofstream ofs2("../../server/public/molecule.obj");
        ofs2<<content2<<std::endl;
    }
}

TEST_CASE("test putting standard protons on 1Q8H"){
    SUBCASE("test tcp for 1q8h.cif.gz"){
        auto& facet = std::use_facet<std::ctype<char>>(std::locale());
        std::string comp_id="1q8h";
        std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
        sylvanmats::constitution::Graph graph;
        std::filesystem::path  filePath="./"+comp_id+".cif.gz";
        sylvanmats::reading::TCPReader tcpReader;
        tcpReader(url, [&graph, &filePath, &comp_id](std::istream& is){

            sylvanmats::constitution::Populator populator;
            populator(is, graph, [&filePath](sylvanmats::constitution::Graph& graph){
            CHECK_EQ(graph.getNumberOfAtomSites(), 378);
            CHECK_EQ(graph.getCell().length_a, 51.491);
            CHECK_EQ(graph.getCell().length_b, 51.491);
            CHECK_EQ(graph.getCell().length_c, 35.389);
            CHECK_EQ(graph.getCell().angle_alpha, 90.000);
            CHECK_EQ(graph.getCell().angle_beta, 90.0);
            CHECK_EQ(graph.getCell().angle_gamma, 120.0);
            CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 31 2 1");
            CHECK_EQ(graph.getSymmetry().Int_Tables_number, 152);
                sylvanmats::standards::Protons protons;
                protons(graph);
           });
            CHECK_EQ(graph.getNumberOfAtomSites(), 633);
            CHECK_EQ(lemon::countEdges(graph), 571);
            CHECK_EQ(lemon::countNodes(graph.componentGraph), 113);
            CHECK_EQ(lemon::countEdges(graph.componentGraph), 47);

        });
        std::cout<<"CIFPublisher "<<std::endl;
        std::filesystem::path path="../templates/cif";
        sylvanmats::publishing::st::CIFPublisher cifPublisher(path);
        facet.toupper(&comp_id[0], &comp_id[0] + comp_id.size());
        cifPublisher.setEntryID(comp_id);
        std::vector<std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>> atomSitesLoop;
        for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
            cifPublisher.insertAtomSites(std::make_tuple(graph.atomSites[nSiteA].group_PDB, graph.atomSites[nSiteA].id, graph.atomSites[nSiteA].type_symbol, graph.atomSites[nSiteA].label_atom_id, graph.atomSites[nSiteA].label_alt_id, graph.atomSites[nSiteA].label_comp_id, graph.atomSites[nSiteA].label_asym_id, graph.atomSites[nSiteA].label_entity_id, graph.atomSites[nSiteA].label_seq_id, graph.atomSites[nSiteA].pdbx_PDB_ins_code, graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z, graph.atomSites[nSiteA].occupancy, graph.atomSites[nSiteA].B_iso_or_equiv, graph.atomSites[nSiteA].pdbx_formal_charge, graph.atomSites[nSiteA].auth_seq_id, graph.atomSites[nSiteA].auth_comp_id, graph.atomSites[nSiteA].auth_asym_id, graph.atomSites[nSiteA].auth_atom_id, graph.atomSites[nSiteA].pdbx_PDB_model_num));
        }
        for(lemon::ListGraph::NodeIt nR(graph.componentGraph); nR!=lemon::INVALID; ++nR){
            if(graph.componentProperties[nR].termination!=sylvanmats::constitution::MONOMER)
                cifPublisher.insertPolymers(std::make_tuple(graph.componentProperties[nR].asym_id, graph.componentProperties[nR].entity_id, graph.componentProperties[nR].seq_id, graph.componentProperties[nR].mon_id, graph.componentProperties[nR].ndb_seq_num, graph.componentProperties[nR].pdb_seq_num, graph.componentProperties[nR].auth_seq_num, graph.componentProperties[nR].pdb_mon_id, graph.componentProperties[nR].auth_mon_id, graph.componentProperties[nR].pdb_strand_id, graph.componentProperties[nR].pdb_ins_code, graph.componentProperties[nR].hetero));
            else
                cifPublisher.insertNonpolymers(std::make_tuple(graph.componentProperties[nR].asym_id, graph.componentProperties[nR].entity_id, graph.componentProperties[nR].mon_id, graph.componentProperties[nR].ndb_seq_num, graph.componentProperties[nR].pdb_seq_num, graph.componentProperties[nR].auth_seq_num, graph.componentProperties[nR].pdb_mon_id, graph.componentProperties[nR].auth_mon_id, graph.componentProperties[nR].pdb_strand_id, graph.componentProperties[nR].pdb_ins_code));
        }
        std::string&& content = cifPublisher.render();
        std::ofstream ofs("test_"+comp_id+".cif");
        ofs<<content<<std::endl;
        path="../templates/obj";
       std::cout<<"OBJPublisher "<<std::endl;
        sylvanmats::publishing::st::OBJPublisher objPublisher(path);
        objPublisher.setVertexCount(2*12*lemon::countEdges(graph));
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
        std::string&& content2 = objPublisher.render();
        std::ofstream ofs2("test.obj");
        ofs2<<content2<<std::endl;
    }
}

}