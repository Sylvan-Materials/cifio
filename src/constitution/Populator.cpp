
#include "constitution/Populator.h"

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"
#include "reading/gz/GZReader.h"

#include "standards/AminoStandards.h"

namespace sylvanmats::constitution {


    void Populator::operator()(std::filesystem::path& filePath, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply){
            unsigned int standardCompCount=0;
        sylvanmats::reading::GZReader gzReader;
        gzReader(filePath, [&](std::istream& content){

            antlr4::ANTLRInputStream input(content);
            sylvanmats::CIFLexer lexer(&input);
            antlr4::CommonTokenStream tokens(&lexer);

            sylvanmats::CIFParser parser(&tokens);
            parser.setBuildParseTree(true);
            antlr4::tree::ParseTree* tree = parser.cif();

            sylvanmats::standards::AminoStandards aminoStandards;

            const std::string blockPath="/cif/dataBlock";
            antlr4::tree::xpath::XPath xblockPath(&parser, blockPath);
            std::vector<antlr4::tree::ParseTree*> dataBlock=xblockPath.evaluate(tree);
            for(std::vector<antlr4::tree::ParseTree*>::iterator it=dataBlock.begin();it!=dataBlock.end();it++){
                if (sylvanmats::CIFParser::DataBlockContext* r=dynamic_cast<sylvanmats::CIFParser::DataBlockContext*>((*it))) {
                    auto oi=r->dataItems();
                    for(sylvanmats::CIFParser::DataItemsContext* cdi: oi | std::views::filter([](sylvanmats::CIFParser::DataItemsContext* di){ return di->tag()!=nullptr && di->tag()->getText().rfind("\n_cell.", 0) == 0; })){
                        if(cdi->tag()->getText().compare("\n_cell.length_a")==0)graph.cell.length_a=std::strtod(cdi->value()->getText().c_str(), nullptr);
                        else if(cdi->tag()->getText().compare("\n_cell.length_b")==0)graph.cell.length_b=std::strtod(cdi->value()->getText().c_str(), nullptr);
                        else if(cdi->tag()->getText().compare("\n_cell.length_c")==0)graph.cell.length_c=std::strtod(cdi->value()->getText().c_str(), nullptr);
                        else if(cdi->tag()->getText().compare("\n_cell.angle_alpha")==0)graph.cell.angle_alpha=std::strtod(cdi->value()->getText().c_str(), nullptr);
                        else if(cdi->tag()->getText().compare("\n_cell.angle_beta")==0)graph.cell.angle_beta=std::strtod(cdi->value()->getText().c_str(), nullptr);
                        else if(cdi->tag()->getText().compare("\n_cell.angle_gamma")==0)graph.cell.angle_gamma=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    }
                    for(sylvanmats::CIFParser::DataItemsContext* cdi: oi | std::views::filter([](sylvanmats::CIFParser::DataItemsContext* di){ return di->tag()!=nullptr && di->tag()->getText().rfind("\n_symmetry.", 0) == 0; })){
                        if(cdi->tag()->getText().compare("\n_symmetry.space_group_name_H-M")==0){
                            if(cdi->value()->singleQuotedString()!=nullptr) graph.symmetry.space_group_name_H_M=cdi->value()->singleQuotedString()->getText().substr(1, cdi->value()->singleQuotedString()->getText().size()-2);
                                else graph.symmetry.space_group_name_H_M=cdi->value()->getText();
                        }
                        else if(cdi->tag()->getText().compare("\n_symmetry.Int_Tables_number")==0)graph.symmetry.Int_Tables_number=std::strtol(cdi->value()->getText().c_str(), nullptr, 10);
                    }
                }
            }
            const std::string thePath="/cif/dataBlock/dataItems/loop";
            antlr4::tree::xpath::XPath xPath(&parser, thePath);
            std::vector<antlr4::tree::ParseTree*> dataBlockLoop=xPath.evaluate(tree);
            for(std::vector<antlr4::tree::ParseTree*>::iterator it=dataBlockLoop.begin();it!=dataBlockLoop.end();it++){
                if (sylvanmats::CIFParser::LoopContext* r=dynamic_cast<sylvanmats::CIFParser::LoopContext*>((*it))) {
                    bool once=true;
                    auto tags=r->loopHeader()->tag();
                    for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return once && tag->getText().rfind("\n_pdbx_poly_seq_scheme.", 0) == 0; })){
                        once=false;
                        unsigned int columnCount=0;
                        unsigned int compCount=0;
                        lemon::ListGraph::Node previousNode=lemon::INVALID;
                        lemon::ListGraph::Node n=graph.componentGraph.addNode();
                        std::string_view asym_id="";
                        std::string_view previous_asym_id="";
                        bool first=true;
                    std::vector<sylvanmats::CIFParser::ValueContext *> values=r->loopBody()->value();
                    for(unsigned int valueIndex=0;valueIndex<values.size();valueIndex++){
                        switch(columnCount){
                             case 0:
                                 graph.componentProperties[n].asym_id.assign(values[valueIndex]->getText());
                                 asym_id=graph.componentProperties[n].asym_id;
                             break;
                             case 6:
                                 graph.componentProperties[n].auth_seq_num = std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                             break;
                             case 8:
                                 graph.componentProperties[n].auth_mon_id.assign(values[valueIndex]->getText());
                             break;
                             case 10:
                                 graph.componentProperties[n].pdb_ins_code.assign(values[valueIndex]->getText());
                             break;
                        }
                       columnCount++;
                        if((valueIndex % r->loopHeader()->tag().size() == r->loopHeader()->tag().size()-1) || valueIndex==r->loopBody()->value().size()-1){
                            columnCount=0;
                            if(first){
                             first=false;
                             graph.componentProperties[n].termination=N_TERMINAL;
                            }
                            else if((previous_asym_id.compare(asym_id)==0)){
                             compCount++;
                             if(compCount>1 && previousNode!=lemon::INVALID){
                                graph.componentProperties[n].termination=NEUTRAL;
                                lemon::ListGraph::Edge e=graph.componentGraph.addEdge(previousNode, n);
                                 //std::cout<<"edge "<<" "<<std::endl;
                             }
                            }
                            else if(previousNode!=lemon::INVALID){
                                graph.componentProperties[previousNode].termination=C_TERMINAL;
                                graph.componentProperties[n].termination=N_TERMINAL;
                            }
                            previous_asym_id=asym_id;
                            previousNode=n;
                            if(valueIndex<r->loopBody()->value().size()-1){
                                n=graph.componentGraph.addNode();
                            }
                        }
                    }
                    }
                    for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return once && tag->getText().rfind("\n_pdbx_nonpoly_scheme.", 0) == 0; })){
                        once=false;
                        unsigned int columnCount=0;
                        unsigned int compCount=0;
                        lemon::ListGraph::Node n=graph.componentGraph.addNode();
                        std::string asym_id="";
                        std::string previous_asym_id="";
                        bool first=true;
                    std::vector<sylvanmats::CIFParser::ValueContext *> values=r->loopBody()->value();
                    for(unsigned int valueIndex=0;valueIndex<values.size();valueIndex++){
                        switch(columnCount){
                             case 0:
                                 graph.componentProperties[n].asym_id.assign(values[valueIndex]->getText());
                             break;
                             case 6:
                                 graph.componentProperties[n].auth_seq_num = std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                             break;
                             case 8:
                                 graph.componentProperties[n].auth_mon_id.assign(values[valueIndex]->getText());
                             break;
                             case 9:
                                 graph.componentProperties[n].pdb_ins_code.assign(values[valueIndex]->getText());
                             break;
                        }
                       columnCount++;
                        if((valueIndex % r->loopHeader()->tag().size() == r->loopHeader()->tag().size()-1) || valueIndex==r->loopBody()->value().size()-1){
                            columnCount=0;
                             graph.componentProperties[n].termination=MONOMER;
                             //previous_asym_id=asym_id;
                            if(valueIndex<r->loopBody()->value().size()-1){
                                n=graph.componentGraph.addNode();
                            }
                        }
                    }
                    }
                }
            }
            for(std::vector<antlr4::tree::ParseTree*>::iterator it=dataBlockLoop.begin();it!=dataBlockLoop.end();it++){
                if (sylvanmats::CIFParser::LoopContext* r=dynamic_cast<sylvanmats::CIFParser::LoopContext*>((*it))) {
                    bool once=true;
                    auto tags=r->loopHeader()->tag();
                    for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return once && tag->getText().rfind("\n_atom_site.", 0) == 0; })){
                        once=false;
                        unsigned int columnCount=0;
                        unsigned int entityCount=0;
                        unsigned int compCount=0;
                        lemon::ListGraph::Node n=graph.addNode();
                        std::string alt_id="";
                        std::string comp_id="";
                        std::string asym_id="";
                        int seq_id=0;
                        std::string ins_code="";
                        lemon::ListGraph::Node nNode=lemon::INVALID;
                        lemon::ListGraph::Node cNode=lemon::INVALID;
                        
                        std::string previous_alt_id="";
                        std::string previous_comp_id="";
                        std::string previous_asym_id="";
                        int previous_seq_id=0;
                        std::string previous_ins_code="";
                        lemon::ListGraph::Node previousNNode=lemon::INVALID;
                        lemon::ListGraph::Node previousCNode=lemon::INVALID;
                        
                        bool first=true;
            auto start = std::chrono::high_resolution_clock::now();
                    std::vector<sylvanmats::CIFParser::ValueContext *> values=r->loopBody()->value();
                    for(unsigned int valueIndex=0;valueIndex<values.size();valueIndex++){
                        switch(columnCount){
                             case 0:

                             break;
                             case 1:
                                 graph.atomSites[n].id =std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                             break;
                             case 2:
                                 graph.atomSites[n].type_symbol.assign(values[valueIndex]->getText());
                             break;
                             case 3:
                                 graph.atomSites[n].label_atom_id.assign(values[valueIndex]->getText());
                             break;
                             case 4:
                                 graph.atomSites[n].label_alt_id.assign(values[valueIndex]->getText());
                                 alt_id=graph.atomSites[n].label_alt_id;
                             break;
                             case 5:
                                 graph.atomSites[n].label_comp_id.assign(values[valueIndex]->getText());
                                 comp_id=graph.atomSites[n].label_comp_id;
                             break;
                             case 6:
                                 graph.atomSites[n].label_asym_id.assign(values[valueIndex]->getText());
                                 asym_id=graph.atomSites[n].label_asym_id;
                             break;
                             case 9:
                                 graph.atomSites[n].pdbx_PDB_ins_code.assign(values[valueIndex]->getText());
                                 ins_code=graph.atomSites[n].pdbx_PDB_ins_code;
                             break;
                             case 10:
                                 graph.atomSites[n].Cartn_x =std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                             break;
                             case 11:
                                 graph.atomSites[n].Cartn_y =std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                             break;
                             case 12:
                                 graph.atomSites[n].Cartn_z =std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                             break;
                             case 13:
                                 graph.atomSites[n].occupancy =std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                             break;
                             case 14:
                                 graph.atomSites[n].B_iso_or_equiv =std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                             break;
                             case 16:
                                 graph.atomSites[n].auth_seq_id =std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                                 seq_id=graph.atomSites[n].auth_seq_id;
                             break;
                             case 19:
                                 graph.atomSites[n].auth_atom_id.assign(values[valueIndex]->getText());
                             break;
                        }
                        columnCount++;
                        if((valueIndex % r->loopHeader()->tag().size() == r->loopHeader()->tag().size()-1) || valueIndex==values.size()-1){
                            columnCount=0;
//                            std::cout<<valueIndex<<" "<<(r->loopHeader()->tag().size() % valueIndex)<<" "<<(r->loopHeader()->tag().size()-1)<<" entityCount "<<entityCount<<" "<<previous_comp_id<<" "<<comp_id<<" "<<previous_seq_id<<" "<<seq_id<<" "<<previous_asym_id<<" "<<asym_id<<" "<<previous_alt_id<<" "<<alt_id<<" "<<previous_ins_code<<" "<<ins_code<<std::endl;
                            if(first){
                             first=false;
                             previous_seq_id=seq_id;
                             previous_comp_id=comp_id;
                             previous_asym_id=asym_id;
                             previous_alt_id=alt_id;
                             previous_ins_code=ins_code;
                            }
                            else if((previous_seq_id!=seq_id || previous_comp_id.compare(comp_id)!=0 || previous_asym_id.compare(asym_id)!=0 || previous_alt_id.compare(alt_id)!=0 || previous_ins_code.compare(ins_code)!=0) || valueIndex==r->loopBody()->value().size()-1){
                             //std::cout<<entityCount<<" "<<previous_comp_id<<" comp_id |"<<comp_id<<"|"<<std::endl;
                                nNode=lemon::INVALID;
                                cNode=lemon::INVALID;
                                lemon::ListGraph::Node currentCompNode=lemon::INVALID;
                                 for(lemon::ListGraph::NodeIt nComp(graph.componentGraph); currentCompNode==lemon::INVALID && nComp!=lemon::INVALID; ++nComp){
                                    if(graph.componentProperties[nComp].auth_seq_num==previous_seq_id && graph.componentProperties[nComp].asym_id.compare(previous_asym_id)==0 && graph.componentProperties[nComp].auth_mon_id.compare(previous_comp_id)==0 && graph.componentProperties[nComp].pdb_ins_code.compare(previous_ins_code)==0)currentCompNode=nComp;
                                }
                                std::string current_comp_id(previous_comp_id.begin(), previous_comp_id.end());
                                if(currentCompNode!=lemon::INVALID && graph.componentProperties[currentCompNode].termination==sylvanmats::constitution::N_TERMINAL)current_comp_id+="_LSN3";
                                if(currentCompNode!=lemon::INVALID && graph.componentProperties[currentCompNode].termination==sylvanmats::constitution::C_TERMINAL)current_comp_id+="_LEO2H";
            //std::cout<<"term ? "<<current_comp_id<<std::endl;
                                bool ret=aminoStandards(current_comp_id, [&](sylvanmats::standards::chem_comp_atom<double>& cca1, sylvanmats::standards::chem_comp_bond& ccb, sylvanmats::standards::chem_comp_atom<double>& cca2){
//                                 std::cout<<"got std "<<previous_comp_id<<" "<<entityCount<<std::endl;
                                 unsigned int countA=0;
                                 bool matched=false;
                                 for(lemon::ListGraph::NodeIt nSiteA(graph); countA<=entityCount+1 && nSiteA!=lemon::INVALID; ++nSiteA){
                                 unsigned int countB=0;
                                 for(lemon::ListGraph::NodeIt nSiteB(graph); countB<=entityCount+1 && nSiteB!=lemon::INVALID; ++nSiteB){
                                     //std::cout<<"??? "<<countA<<" "<<countB<<" "<<entityCount<<" "<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.atomSites[nSiteB].label_atom_id<<std::endl;
                                     if(countA<countB && nSiteA!=nSiteB && graph.atomSites[nSiteA].auth_seq_id == graph.atomSites[nSiteB].auth_seq_id && graph.atomSites[nSiteA].auth_comp_id.compare(graph.atomSites[nSiteB].auth_comp_id)==0){
                                         if((graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_1)==0 && graph.atomSites[nSiteB].label_atom_id.compare(ccb.atom_id_2)==0) || (graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_2)==0 && graph.atomSites[nSiteB].label_atom_id.compare(ccb.atom_id_1)==0)){
//                                            std::cout<<"\t"<<entityCount<<" "<<graph.atomSites[nSiteA].auth_seq_id<<" "<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<graph.atomSites[nSiteB].auth_seq_id<<std::endl;
                                            lemon::ListGraph::Edge e=graph.addEdge(nSiteA, nSiteB);
                                            graph.compBond[e].value_order=ccb.value_order;
                                            matched=true;
                                         }
                                        if(graph.atomSites[nSiteA].label_atom_id.compare("N")==0)nNode=nSiteA;
                                        else if(graph.atomSites[nSiteA].label_atom_id.compare("C")==0)cNode=nSiteA;
                                        if(graph.atomSites[nSiteB].label_atom_id.compare("N")==0)nNode=nSiteB;
                                        else if(graph.atomSites[nSiteB].label_atom_id.compare("C")==0)cNode=nSiteB;
                                        
                                     }                       
                                     countB++;
                                 }
                                 countA++;
                                 }
                                 if(!matched){
                                 countA=0;
                                 bool proton1=cca1.type_symbol.compare("H")==0;
                                 bool proton2=cca2.type_symbol.compare("H")==0;
                                 for(lemon::ListGraph::NodeIt nSiteA(graph); countA<=entityCount && nSiteA!=lemon::INVALID; ++nSiteA){
//                                     std::cout<<"??? "<<countA<<" "<<proton1<<" "<<proton2<<" "<<entityCount<<" "<<graph.atomSites[nSiteA].label_atom_id<<" |"<<graph.atomSites[nSiteA].auth_atom_id<<"|"<<std::endl;
                                     if(proton1 && graph.atomSites[nSiteA].auth_atom_id.compare(ccb.atom_id_2)==0){
                                        graph.atomSites[nSiteA].proton_count++;
                                     }
                                     else if(proton2 && graph.atomSites[nSiteA].auth_atom_id.compare(ccb.atom_id_1)==0){
                                        graph.atomSites[nSiteA].proton_count++;
                                     } 
                                    countA++;
                                 }
                                 }
                             });
                              compCount++;
                             if(compCount>1 && cNode!=lemon::INVALID  && nNode!=lemon::INVALID && previousNNode!=lemon::INVALID  && previousCNode!=lemon::INVALID){
                                lemon::ListGraph::Edge e=graph.addEdge(previousCNode, nNode);
                                graph.compBond[e].value_order=1;
                                 
                             }
                             previousNNode=nNode;
                             previousCNode=cNode;
                             entityCount=0;
                             previous_seq_id=seq_id;
                             previous_comp_id=comp_id;
                             previous_asym_id=asym_id;
                             previous_alt_id=alt_id;
                             previous_ins_code=ins_code;
                             if(ret)standardCompCount++;
                             if(!ret)std::cout<<"comp_id "<<comp_id<<" "<<ins_code<<std::endl;
                             }
                            else entityCount++;
                            if(valueIndex<r->loopBody()->value().size()-1)n=graph.addNode();
                       }
                    }
            auto end = std::chrono::high_resolution_clock::now();
            std::cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count()*1.0e-9 << "s\n";

                    }

                }
            }
            apply(graph);
        });
        std::cout<<"standardCompCount "<<standardCompCount<<std::endl;
    };
    
}
