#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <iterator>
#include <utility>
#include <charconv>

#include "zlib.h"
#include "mio/mmap.hpp"

#include "antlr4-runtime.h"
#include "reading/gz/GZReader.h"
#include "reading/tcp/TCPReader.h"

#include "PeriodicTable.h"
#include "standards/ComponentStandards.h"
#include "standards/AminoStandards.h"
#include "constitution/Graph.h"
#include "constitution/Populator.h"
#include "constitution/MOL2Populator.h"
#include "constitution/Selection.h"
#include "dynamics/Particles.h"
#include "density/Populator.h"
#include "lattice/Populator.h"
#include "density/ccp4/MapInput.h"
#include "publishing/jgf/JGFPublisher.h"
#include "publishing/smiles/SMILESPublisher.h"
#include "publishing/st/MOL2Publisher.h"

TEST_SUITE("publishing"){

TEST_CASE("test writing to ligand to mol2"){


}

TEST_CASE("test writing mol2 from 1ebcs hem") {
    std::filesystem::path filePath="./examples/1ebc.cif.gz";
   
    sylvanmats::constitution::Graph graph;
    
    sylvanmats::constitution::Populator populator;
    populator(filePath, graph, [&filePath](sylvanmats::constitution::Graph& graph){
   CHECK_EQ(graph.getNumberOfAtomSites(), 1332);
   CHECK_EQ(graph.getCell().length_a, 64.87);
   CHECK_EQ(graph.getCell().length_b, 31.0);
   CHECK_EQ(graph.getCell().length_c, 35.29);
   CHECK_EQ(graph.getCell().angle_alpha, 90.000);
   CHECK_EQ(graph.getCell().angle_beta, 105.63);
   CHECK_EQ(graph.getCell().angle_gamma, 90.000);
   CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 1 21 1");
   CHECK_EQ(graph.getSymmetry().Int_Tables_number, 4);
   
   });
   CHECK_EQ(graph.getNumberOfAtomSites(), 1332);
   CHECK_EQ(lemon::countEdges(graph), 1298);
   CHECK_EQ(lemon::countNodes(graph.componentGraph), 222);  
   CHECK_EQ(lemon::countEdges(graph.componentGraph), 151);
    std::vector<sylvanmats::constitution::unique_component> uniqueComponents = {{.label_comp_id="HEM", .label_asym_id="D", .auth_seq_id=154}};
    sylvanmats::constitution::Selection selection(graph);
    selection(uniqueComponents, [&](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph){
        graph.identifyFusedSystems(selectionGraph, [&graph, &filePath](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>> subSelectionGraph){
            CHECK_EQ(lemon::countNodes(subSelectionGraph), 25);
            CHECK_EQ(lemon::countEdges(subSelectionGraph), 32);
            graph.identifyRings(subSelectionGraph);
            filePath.replace_extension(".mol2");
            std::filesystem::path path="../../cifio/templates/mol2";
            sylvanmats::publishing::st::MOL2Publisher mol2Publisher(path);   
            mol2Publisher.setEntryID("1ebc");
            mol2Publisher.setNumberOfAtoms(static_cast<size_t>(lemon::countNodes(subSelectionGraph)));
            mol2Publisher.setNumberOfBonds(static_cast<size_t>(lemon::countEdges(subSelectionGraph)));
//            mol2Publisher.add("num_subst", static_cast<unsigned long long>(1));
            unsigned long long ordinal=lemon::countNodes(subSelectionGraph);
            std::string subst_name="";
            std::string chain="";
            lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeMap<unsigned long long> ordinalMap(subSelectionGraph);
            std::vector<std::tuple<unsigned long long, std::string, double, double, double, std::string, long long, std::string, double>> atomSitesLoop;
            for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::NodeIt nSiteA(subSelectionGraph); nSiteA!=lemon::INVALID; ++nSiteA){
                ordinalMap[nSiteA]=ordinal;
                subst_name=graph.atomSites[nSiteA].auth_comp_id;
                chain=graph.atomSites[nSiteA].auth_asym_id;
                mol2Publisher.insertAtomSites(std::make_tuple(ordinal--, graph.atomSites[nSiteA].label_atom_id, graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z, graph.atomSites[nSiteA].type_symbol, 1, graph.atomSites[nSiteA].auth_comp_id, 0.0));
//                atomSitesLoop.insert(atomSitesLoop.begin(), std::make_tuple(ordinal--, graph.atomSites[nSiteA].label_atom_id, graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z, graph.atomSites[nSiteA].type_symbol, 1, graph.atomSites[nSiteA].auth_comp_id, 0.0));
            }
//            mol2Publisher.add("atom_sites", atomSitesLoop);
            ordinal=lemon::countEdges(subSelectionGraph);
//            std::vector<std::tuple<unsigned long long, unsigned long long, unsigned long long, std::string>> atomBondsLoop;
            for(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>::EdgeIt e(subSelectionGraph); e!=lemon::INVALID; ++e){
                mol2Publisher.insertAtomBonds(std::make_tuple(ordinal--,  ordinalMap[subSelectionGraph.u(e)], ordinalMap[subSelectionGraph.v(e)], std::to_string(graph.compBond[e].value_order)));
//                atomBondsLoop.insert(atomBondsLoop.begin(), std::make_tuple(ordinal--,  ordinalMap[subSelectionGraph.u(e)], ordinalMap[subSelectionGraph.v(e)], std::to_string(graph.compBond[e].value_order)));
            }
//            mol2Publisher.add("atom_bonds", atomBondsLoop);
            ordinal=1;
//            std::vector<std::tuple<unsigned long long, std::string, unsigned long long, std::string, unsigned long long, std::string, std::string, unsigned long long, std::string>> substructureLoop;
//                substructureLoop.insert(substructureLoop.begin(), std::make_tuple(ordinal--, subst_name, 1l, "RESIDUE", 1l, chain, subst_name, 0l, "ROOT"));
            mol2Publisher.insertSubstructures(std::make_tuple(ordinal--, subst_name, 1l, "RESIDUE", 1l, chain, subst_name, 0l, "ROOT"));
            std::string&& content = mol2Publisher.render();
            std::ofstream ofs(filePath);
            ofs<<content<<std::endl;
            ofs.close();
        });
        selection.compliment(selectionGraph, [](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& complimentGraph){
            CHECK_EQ(lemon::countNodes(complimentGraph), 1289);
            CHECK_EQ(lemon::countEdges(complimentGraph), 1248);
        });
    });
    CHECK_EQ(graph.getNumberOfRings(), 4);
    CHECK_EQ(graph.countFlexibles(), 35);

}

TEST_CASE("test writing to 4bkt ligand to smiles"){
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
    sylvanmats::constitution::Selection selection(graph, true);
    selection(uniqueComponents, [&](lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph){
   sylvanmats::dynamics::Particles particles(graph, selectionGraph);
   sylvanmats::publishing::SMILESPublisher smilesPublisher(graph, particles);
   std::cout<<" "<<smilesPublisher<<std::endl;
    });
    CHECK_EQ(graph.getNumberOfRings(), 2);
    CHECK_EQ(graph.countFlexibles(), 15);
}

}
