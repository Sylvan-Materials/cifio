
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>

#include "zlib.h"

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"
#include "reading/gz/GZReader.h"

#include "standards/AminoStandards.h"
#include "constitution/Graph.h"
#include "publishing/jgf/JGFPublisher.h"

TEST_CASE("test decompression of aa-variants-v1.cif.gz") {
 std::vector<sylvanmats::standards::chem_comp_bond> standardChemCompBond;
 standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="N", .atom_id_2="CA", .value_order=1});
standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="N", .atom_id_2="H", .value_order=1});
standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="N", .atom_id_2="HN2", .value_order=1});
standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="CA", .atom_id_2="C", .value_order=1});
standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="CA", .atom_id_2="CB", .value_order=1});
standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="CA", .atom_id_2="HA", .value_order=1});
standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="C", .atom_id_2="O", .value_order=2});
standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="C", .atom_id_2="OXT", .value_order=1});
standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="CB", .atom_id_2="HB1", .value_order=1});
standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="CB", .atom_id_2="HB2", .value_order=1});
standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="CB", .atom_id_2="HB3", .value_order=1});
standardChemCompBond.push_back({.comp_id="ALA", .atom_id_1="OXT", .atom_id_2="HXT", .value_order=1});
 
    sylvanmats::standards::AminoStandards aminoStandards;
   std::string_view comp_id="ALA";
   unsigned int counter=0;
   CHECK(aminoStandards(comp_id, [&](sylvanmats::standards::chem_comp_bond ccb){
       CHECK_EQ(standardChemCompBond[counter].comp_id, ccb.comp_id);
       CHECK_EQ(standardChemCompBond[counter].atom_id_1, ccb.atom_id_1);
       CHECK_EQ(standardChemCompBond[counter].atom_id_2, ccb.atom_id_2);
       CHECK_EQ(standardChemCompBond[counter++].value_order, ccb.value_order);
   }));

   comp_id="UNK";
   counter=0;
   REQUIRE_FALSE(aminoStandards(comp_id, [&](sylvanmats::standards::chem_comp_bond ccb){
       std::cout<<"unk? "<<ccb.comp_id<<" "<<ccb.atom_id_1<<" "<<ccb.atom_id_2<<" "<<ccb.value_order<<" "<<std::endl;
   }));
   
}

TEST_CASE("test 3sgs") {
    printf("hello from <test.cpp>\n");
    std::string filePath = "~/Downloads/3sgs.cif.gz";
    sylvanmats::reading::GZReader gzReader;
    gzReader(filePath, [](std::istream& content){
//    std::string name = "/home/roger/Downloads/3sgs.cif";
//    std::ifstream file(name);
//    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//    file.close();
    //std::cout<<content<<std::endl;
    antlr4::ANTLRInputStream input(content);
    sylvanmats::CIFLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
//tokens.fill();
//  for (auto token : tokens.getTokens()) {
//    std::cout << token->toString() << std::endl;
//  }    
    sylvanmats::CIFParser parser(&tokens);
    parser.setBuildParseTree(true);
    antlr4::tree::ParseTree* tree = parser.cif();

    sylvanmats::standards::AminoStandards aminoStandards;
   
    sylvanmats::constitution::Graph graph;
    
   const std::string thePath="/cif/dataBlock/dataItems/loop";
   antlr4::tree::xpath::XPath xPath(&parser, thePath);///dataBlock/dataBlockHeading/*");
   std::vector<antlr4::tree::ParseTree*> dataBlockHeading=xPath.evaluate(tree);
   for(std::vector<antlr4::tree::ParseTree*>::iterator it=dataBlockHeading.begin();it!=dataBlockHeading.end();it++){
       if (sylvanmats::CIFParser::LoopContext* r=dynamic_cast<sylvanmats::CIFParser::LoopContext*>((*it))) {
           bool once=true;
           auto tags=r->loopHeader()->tag();
           for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return once && tag->getText().rfind("_atom_site.", 0) == 0; })){
               once=false;
               unsigned int columnCount=0;
               unsigned int entityCount=0;
               lemon::ListGraph::Node n=graph.addNode();
               std::string_view alt_id="";
               std::string_view comp_id="";
               std::string_view asym_id="";
               int seq_id=0;
               std::string_view ins_code="";
               std::string_view previous_alt_id="";
               std::string_view previous_comp_id="";
               std::string_view previous_asym_id="";
               int previous_seq_id=0;
               std::string_view previous_ins_code="";
               bool first=true;
           for(unsigned int valueIndex=0;valueIndex<r->loopBody()->value().size();valueIndex++){
               switch(columnCount){
                    case 0:

                    break;
                    case 2:
                        graph.atomSites[n].type_symbol=r->loopBody()->value(valueIndex)->getText();
                    break;
                    case 3:
                        graph.atomSites[n].label_atom_id=r->loopBody()->value(valueIndex)->getText();
                    break;
                    case 4:
                        graph.atomSites[n].label_alt_id=r->loopBody()->value(valueIndex)->getText();
                        alt_id=graph.atomSites[n].label_alt_id;
                    break;
                    case 5:
                        graph.atomSites[n].label_comp_id=r->loopBody()->value(valueIndex)->getText();
                        comp_id=graph.atomSites[n].label_comp_id;
                    break;
                    case 6:
                        graph.atomSites[n].label_asym_id=r->loopBody()->value(valueIndex)->getText();
                        asym_id=graph.atomSites[n].label_asym_id;
                    break;
                    case 9:
                        graph.atomSites[n].pdbx_PDB_ins_code=r->loopBody()->value(valueIndex)->getText();
                        ins_code=graph.atomSites[n].pdbx_PDB_ins_code;
                    break;
                    case 10:
                        graph.atomSites[n].Cartn_x =std::strtod(r->loopBody()->value(valueIndex)->getText().c_str(), nullptr);
                    break;
                    case 11:
                        graph.atomSites[n].Cartn_y =std::strtod(r->loopBody()->value(valueIndex)->getText().c_str(), nullptr);
                    break;
                    case 12:
                        graph.atomSites[n].Cartn_z =std::strtod(r->loopBody()->value(valueIndex)->getText().c_str(), nullptr);
                    break;
                    case 13:
                        graph.atomSites[n].occupancy =std::strtod(r->loopBody()->value(valueIndex)->getText().c_str(), nullptr);
                    break;
                    case 14:
                        graph.atomSites[n].B_iso_or_equiv =std::strtod(r->loopBody()->value(valueIndex)->getText().c_str(), nullptr);
                    break;
                    case 16:
                        graph.atomSites[n].auth_seq_id =std::strtol(r->loopBody()->value(valueIndex)->getText().c_str(), nullptr, 10);
                        seq_id=graph.atomSites[n].auth_seq_id;
                    break;
               }
               columnCount++;
               if(valueIndex % r->loopHeader()->tag().size() == r->loopHeader()->tag().size()-1){
                   columnCount=0;
                   entityCount++;
                   std::cout<<" "<<entityCount<<std::endl;
                   if(first){
                    first=false;
                    entityCount=0;
                    previous_seq_id=seq_id;
                    previous_comp_id=comp_id;
                    previous_asym_id=asym_id;
                    previous_alt_id=alt_id;
                    previous_ins_code=ins_code;
                   }
                   else if(previous_seq_id!=seq_id || previous_comp_id.compare(comp_id)!=0 || previous_asym_id.compare(asym_id)!=0 || previous_alt_id.compare(alt_id)!=0 || previous_ins_code.compare(ins_code)!=0){
                    std::cout<<" "<<previous_comp_id<<" comp_id "<<comp_id<<std::endl;
                    bool ret=aminoStandards(comp_id, [&](sylvanmats::standards::chem_comp_bond ccb){
                        unsigned int countA=0;
                        for(lemon::ListGraph::NodeIt nSiteA(graph); countA<entityCount && nSiteA!=lemon::INVALID; ++nSiteA){
                        unsigned int countB=0;
                        for(lemon::ListGraph::NodeIt nSiteB(graph); countB<entityCount && nSiteB!=lemon::INVALID; ++nSiteB){
                            if(countA<countB && nSiteA!=nSiteB && ((graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_1)==0 && graph.atomSites[nSiteB].label_atom_id.compare(ccb.atom_id_2)==0) || (graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_2)==0 && graph.atomSites[nSiteB].label_atom_id.compare(ccb.atom_id_1)==0))){
                                lemon::ListGraph::Edge e=graph.addEdge(nSiteA, nSiteB);
                            }                       
                            countB++;
                        }
                        countA++;
                        }
                    });
                    entityCount=0;
                    previous_seq_id=seq_id;
                    previous_comp_id=comp_id;
                    previous_asym_id=asym_id;
                    previous_alt_id=alt_id;
                    previous_ins_code=ins_code;
                   }
                   if(valueIndex<r->loopBody()->value().size()-1)n=graph.addNode();
               }
           }
           }
           
       }
   }
    sylvanmats::publishing::JGFPublisher jgfPublisher(graph);   
   CHECK_EQ(graph.getNumberOfAtomSites(), 46);
   std::cout<<" "<<jgfPublisher<<std::endl;
   });
}
