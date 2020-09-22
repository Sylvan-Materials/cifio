
#include "constitution/Populator.h"

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"
#include "reading/gz/GZReader.h"

#include "standards/AminoStandards.h"

namespace sylvanmats::constitution {


    void Populator::operator()(std::string& filePath, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply){
        sylvanmats::reading::GZReader gzReader;
        gzReader(filePath, [&](std::istream& content){

            antlr4::ANTLRInputStream input(content);
            sylvanmats::CIFLexer lexer(&input);
            antlr4::CommonTokenStream tokens(&lexer);

            sylvanmats::CIFParser parser(&tokens);
            parser.setBuildParseTree(true);
            antlr4::tree::ParseTree* tree = parser.cif();

            std::cout<<"AminoStandards "<<std::endl;
            sylvanmats::standards::AminoStandards aminoStandards;

            const std::string thePath="/cif/dataBlock/dataItems/loop";
            antlr4::tree::xpath::XPath xPath(&parser, thePath);
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
                        std::string alt_id="";
                        std::string comp_id="";
                        std::string asym_id="";
                        int seq_id=0;
                        std::string ins_code="";
                        std::string previous_alt_id="";
                        std::string previous_comp_id="";
                        std::string previous_asym_id="";
                        int previous_seq_id=0;
                        std::string previous_ins_code="";
                        bool first=true;
                    for(unsigned int valueIndex=0;valueIndex<r->loopBody()->value().size();valueIndex++){
                        switch(columnCount){
                             case 0:

                             break;
                             case 1:
                                 graph.atomSites[n].id =std::strtol(r->loopBody()->value(valueIndex)->getText().c_str(), nullptr, 10);
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
                        if((valueIndex % r->loopHeader()->tag().size() == r->loopHeader()->tag().size()-1) || valueIndex==r->loopBody()->value().size()-1){
                            columnCount=0;
         //                   std::cout<<valueIndex<<" "<<(r->loopHeader()->tag().size() % valueIndex)<<" "<<(r->loopHeader()->tag().size()-1)<<" entityCount "<<entityCount<<" "<<previous_comp_id<<" "<<comp_id<<" "<<previous_seq_id<<" "<<seq_id<<" "<<previous_asym_id<<" "<<asym_id<<" "<<previous_alt_id<<" "<<alt_id<<" "<<previous_ins_code<<" "<<ins_code<<std::endl;
                            if(first){
                             first=false;
                             previous_seq_id=seq_id;
                             previous_comp_id=comp_id;
                             previous_asym_id=asym_id;
                             previous_alt_id=alt_id;
                             previous_ins_code=ins_code;
                            }
                            else if((previous_seq_id!=seq_id || previous_comp_id.compare(comp_id)!=0 || previous_asym_id.compare(asym_id)!=0 || previous_alt_id.compare(alt_id)!=0 || previous_ins_code.compare(ins_code)!=0) || valueIndex==r->loopBody()->value().size()-1){
                             std::cout<<entityCount<<" "<<previous_comp_id<<" comp_id |"<<comp_id<<"|"<<std::endl;
                             bool ret=aminoStandards(previous_comp_id, [&](sylvanmats::standards::chem_comp_bond ccb){
         //                        std::cout<<"got std "<<previous_comp_id<<" "<<entityCount<<std::endl;
                                 unsigned int countA=0;
                                 for(lemon::ListGraph::NodeIt nSiteA(graph); countA<=entityCount+1 && nSiteA!=lemon::INVALID; ++nSiteA){
                                 unsigned int countB=0;
                                 for(lemon::ListGraph::NodeIt nSiteB(graph); countB<=entityCount+1 && nSiteB!=lemon::INVALID; ++nSiteB){
                                     //std::cout<<"??? "<<countA<<" "<<countB<<" "<<entityCount<<" "<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.atomSites[nSiteB].label_atom_id<<std::endl;
                                     if(countA<countB && nSiteA!=nSiteB && graph.atomSites[nSiteA].auth_seq_id == graph.atomSites[nSiteB].auth_seq_id && graph.atomSites[nSiteA].auth_comp_id.compare(graph.atomSites[nSiteB].auth_comp_id)==0 && ((graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_1)==0 && graph.atomSites[nSiteB].label_atom_id.compare(ccb.atom_id_2)==0) || (graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_2)==0 && graph.atomSites[nSiteB].label_atom_id.compare(ccb.atom_id_1)==0))){
                                         std::cout<<"\t"<<entityCount<<" "<<graph.atomSites[nSiteA].auth_seq_id<<" "<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<graph.atomSites[nSiteB].auth_seq_id<<std::endl;
                                         lemon::ListGraph::Edge e=graph.addEdge(nSiteA, nSiteB);
                                         graph.compBond[e].value_order=ccb.value_order;
                                     }                       
                                     countB++;
                                 }
                                 countA++;
                                 }
                             });
                             entityCount=0;
                             previous_seq_id=seq_id;
                             std::cout<<previous_comp_id.size()<<" "<<previous_comp_id<<" comp_id "<<comp_id<<std::endl;
                             previous_comp_id=comp_id;
                             previous_asym_id=asym_id;
                             previous_alt_id=alt_id;
                             previous_ins_code=ins_code;
                            }
                            else entityCount++;
                            if(valueIndex<r->loopBody()->value().size()-1)n=graph.addNode();
                        }
                    }
                    }

                }
            }
            apply(graph);
        });
    };
    
}
