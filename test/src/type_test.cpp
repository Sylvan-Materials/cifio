#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <iterator>
#include <utility>
#include <charconv>
#include <cstddef>
#include <typeinfo>
#include <cxxabi.h>

#include "antlr4-runtime.h"

#include "standards/ComponentStandards.h"
#include "standards/AminoStandards.h"
#include "constitution/Graph.h"
#include "constitution/Selection.h"
#include "constitution/MOL2Populator.h"
#include "PeriodicTable.h"
#include "forcefield/OpenFF.h"

#include "io/xml/Path.h"
#include "io/xml/Binder.h"

TEST_SUITE("type"){

TEST_CASE("test atom site struct"){

    sylvanmats::constitution::_atom_site<double> atomSite;
//    std::cout << std::boolalpha;
    const std::type_info  &ti = typeid(atomSite);
    int     status;
    char   *realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
//    std::cout <<"name: "<< ti.name()<<" "<< realname<<" "<<status<< '\n';
    CHECK(std::is_class<sylvanmats::constitution::_atom_site<double>>::value);
//    std::cout<<"label_atom_id: " << offsetof(sylvanmats::constitution::_atom_site<double>, label_atom_id)<<std::endl;
//    std::cout << ti.name()<<" " << '\n';

    std::unordered_map<std::string, size_t> tagMap = { {"asym_id", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, asym_id)},
                                                       {"entity_id", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, entity_id)},
                                                       {"seq_id", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, seq_id)},
                                                       {"mon_id", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, mon_id)},
                                                       {"ndb_seq_num", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, ndb_seq_num)},
                                                       {"pdb_seq_num", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, pdb_seq_num)},
                                                       {"auth_seq_num", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, auth_seq_num)},
                                                       {"pdb_mon_id", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, pdb_mon_id)},
                                                       {"auth_mon_id", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, auth_mon_id)},
                                                       {"pdb_strand_id", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, pdb_strand_id)},
                                                       {"pdb_ins_code", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, pdb_ins_code)},
                                                       {"hetero", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, hetero)}
                                                    };
    switch(tagMap["auth_seq_num"]){
        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, asym_id):
//            std::cout<<"in case "<<offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, asym_id)<<std::endl;
        break;
        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, auth_seq_num):
//            std::cout<<"in case "<<offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, auth_seq_num)<<std::endl;
        break;
    }

    std::unordered_map<std::string, size_t> tagOperationsMap = {  {"id", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, id)},
                                                                {"type", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, type)},
                                                                {"name", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, name)},
                                                                {"symmetry_operation", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, symmetry_operation)},
                                                                {"matrix[1][1]", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix)},
                                                                {"matrix[1][2]", sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix)},
                                                                {"matrix[1][3]", (2*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix))},
                                                                {"matrix[2][1]", (3*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix))},
                                                                {"matrix[2][2]", (4*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix))},
                                                                {"matrix[2][3]", (5*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix))},
                                                                {"matrix[3][1]", (6*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix))},
                                                                {"matrix[3][2]", (7*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix))},
                                                                {"matrix[3][3]", (8*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix))},
                                                                {"vector[1]", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, vector)},
                                                                {"vector[2]", (sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, vector))},
                                                                {"vector[3]", (2*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, vector))}
                                                             };
//    std::cout<<"\"id\" "<<tagOperationsMap["id"]<<std::endl;
//    std::cout<<"\"vector[1]\" "<<tagOperationsMap["vector[1]"]<<std::endl;
//    std::cout<<"\"vector[2]\" "<<tagOperationsMap["vector[2]"]<<std::endl;
//    std::cout<<"\"vector[3]\" "<<tagOperationsMap["vector[3]"]<<std::endl;
//    std::cout<<"\"matrix[1][1]\" "<<tagOperationsMap["matrix[1][1]"]<<std::endl;
//    std::cout<<"\"matrix[3][3]\" "<<tagOperationsMap["matrix[3][3]"]<<std::endl;
}

TEST_CASE("test reading SMIRNOFF offxml"){
//    sylvanmats::forcefield::OpenFF openFF;
}

TEST_CASE("test QD0 direct typing from mol2") {
    sylvanmats::PeriodicTable* periodicTable=sylvanmats::PeriodicTable::getInstance();
    std::filesystem::path filePath="../../CASF-2016/coreset/4bkt/4bkt_ligand.mol2";
std::cout<<"filePath "<<filePath<<std::endl;   
    sylvanmats::constitution::Graph graph;
    
    sylvanmats::constitution::MOL2Populator populator;
    populator(filePath, graph, [&filePath](sylvanmats::constitution::Graph& graph){
std::cout<<"graph.getNumberOfAtomSites() "<<graph.getNumberOfAtomSites()<<std::endl;   
   CHECK_EQ(graph.getNumberOfAtomSites(), 36);
   
   });
   CHECK_EQ(graph.getNumberOfAtomSites(), 36);
   CHECK_EQ(lemon::countEdges(graph), 37);
   CHECK_EQ(lemon::countNodes(graph.componentGraph), 1);  
   CHECK_EQ(lemon::countEdges(graph.componentGraph), 0);
    std::vector<sylvanmats::constitution::unique_component> uniqueComponents = {{.label_comp_id="QD0", .label_asym_id="*", .pdbx_PDB_ins_code="*", .auth_seq_id=1}};
    sylvanmats::constitution::Selection selection(graph);
    selection(uniqueComponents, [&](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph){
        graph.identifyFusedSystems(selectionGraph, [&periodicTable, &graph, &filePath](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subSelectionGraph){
            CHECK_EQ(lemon::countNodes(subSelectionGraph), 5);
            CHECK_EQ(lemon::countEdges(subSelectionGraph), 5);
            graph.identifyRings(subSelectionGraph);
            for (lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(subSelectionGraph); nSiteA!=lemon::INVALID; ++nSiteA){
//                if(graph.atomSites[nSiteA].label_atom_id.compare("N03")==0)graph.atomSites[nSiteA].pdbx_formal_charge=-1;
                if(graph.atomSites[nSiteA].label_atom_id.compare("O13")==0)graph.atomSites[nSiteA].pdbx_formal_charge=-1;
                if(graph.atomSites[nSiteA].label_atom_id.compare("O18")==0)graph.atomSites[nSiteA].pdbx_formal_charge=-1;
                
            }
            for (lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::EdgeIt eSiteA(subSelectionGraph); eSiteA!=lemon::INVALID; ++eSiteA){
                lemon::ListGraph::Node nSiteA=graph.u(eSiteA);
                lemon::ListGraph::Node nSiteB=graph.v(eSiteA);
                sylvanmats::element eleA=periodicTable->index(graph.atomSites[nSiteA].type_symbol);
                std::cout<<"[#"<<eleA.atomic_number<<"X"<<lemon::countIncEdges(graph, nSiteA)<<":1]";
                if(graph.compBond[eSiteA].value_order==3)std::cout<<"#";
                else if(graph.compBond[eSiteA].value_order==2)std::cout<<"=";
                else std::cout<<"-";
                sylvanmats::element eleB=periodicTable->index(graph.atomSites[nSiteB].type_symbol);
                std::cout<<"[#"<<eleB.atomic_number<<"X"<<lemon::countIncEdges(graph, nSiteB)<<":2]";
                std::cout<<std::endl;
            }       
        });
        selection.compliment(selectionGraph, [](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& complimentGraph){
            CHECK_EQ(lemon::countNodes(complimentGraph), 0);
            CHECK_EQ(lemon::countEdges(complimentGraph), 0);
        });
    });
    CHECK_EQ(graph.getNumberOfRings(), 2);
    CHECK_EQ(graph.countFlexibles(), 31);
    sylvanmats::forcefield::OpenFF openFF(graph);
    openFF();
    std::cout<<"MM energy: "<<openFF.getEnergy()<<std::endl<<"grads: "<<openFF.getGradients()<<std::endl;
}

}

