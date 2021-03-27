#include <cstdio>

#include "constitution/Populator.h"

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"
#include "reading/gz/GZReader.h"

#include "standards/ComponentStandards.h"
#include "standards/AminoStandards.h"

namespace sylvanmats::constitution {


    void Populator::operator()(std::filesystem::path& filePath, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply){
        sylvanmats::reading::GZReader gzReader;
        gzReader(filePath, [&](std::istream& content){
         this->operator()(content, graph, apply);
        });
     };

    void Populator::operator()(std::istream& content, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply){
        unsigned int standardAACount=0;
        unsigned int standardCompCount=0;

        antlr4::ANTLRInputStream input(content);
        sylvanmats::CIFLexer lexer(&input);
        antlr4::CommonTokenStream tokens(&lexer);

        sylvanmats::CIFParser parser(&tokens);
        parser.setBuildParseTree(true);
        antlr4::tree::ParseTree* tree = parser.cif();

        sylvanmats::standards::AminoStandards aminoStandards;
        sylvanmats::standards::ComponentStandards componentStandards;

        const std::string blockPath="/cif/dataBlock";
        antlr4::tree::xpath::XPath xblockPath(&parser, blockPath);
        std::vector<antlr4::tree::ParseTree*> dataBlock=xblockPath.evaluate(tree);
        for(std::vector<antlr4::tree::ParseTree*>::iterator it=dataBlock.begin();it!=dataBlock.end();it++){
            if (sylvanmats::CIFParser::DataBlockContext* r=dynamic_cast<sylvanmats::CIFParser::DataBlockContext*>((*it))) {
                auto oi=r->dataItems();
                for(sylvanmats::CIFParser::DataItemsContext* cdi: oi | std::views::filter([](sylvanmats::CIFParser::DataItemsContext* di){ return di->tag()!=nullptr && di->tag()->getText().starts_with("\n_cell."); })){
                    if(cdi->tag()->getText().compare("\n_cell.length_a")==0)graph.cell.length_a=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().compare("\n_cell.length_b")==0)graph.cell.length_b=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().compare("\n_cell.length_c")==0)graph.cell.length_c=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().compare("\n_cell.angle_alpha")==0)graph.cell.angle_alpha=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().compare("\n_cell.angle_beta")==0)graph.cell.angle_beta=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().compare("\n_cell.angle_gamma")==0)graph.cell.angle_gamma=std::strtod(cdi->value()->getText().c_str(), nullptr);
                }
                for(sylvanmats::CIFParser::DataItemsContext* cdi: oi | std::views::filter([](sylvanmats::CIFParser::DataItemsContext* di){ return di->tag()!=nullptr && di->tag()->getText().starts_with("\n_symmetry."); })){
                    if(cdi->tag()->getText().compare("\n_symmetry.space_group_name_H-M")==0){
                        if(cdi->value()->singleQuotedString()!=nullptr) graph.symmetry.space_group_name_H_M=cdi->value()->singleQuotedString()->getText().substr(1, cdi->value()->singleQuotedString()->getText().size()-2);
                            else graph.symmetry.space_group_name_H_M=cdi->value()->getText();
                    }
                    else if(cdi->tag()->getText().compare("\n_symmetry.Int_Tables_number")==0)graph.symmetry.Int_Tables_number=std::strtol(cdi->value()->getText().c_str(), nullptr, 10);
                }
                
                for(sylvanmats::CIFParser::DataItemsContext* cdi: oi | std::views::filter([](sylvanmats::CIFParser::DataItemsContext* di){ return di->tag()!=nullptr && di->tag()->getText().starts_with("\n_pdbx_struct_oper_list."); })){
                    if(graph.operationList.empty())graph.operationList.push_back(_pdbx_struct_oper_list<double>());
                    if(cdi->tag()->getText().ends_with(".id"))graph.operationList.back().id=std::strtol(cdi->value()->getText().c_str(), nullptr, 10);
                    else if(cdi->tag()->getText().ends_with(".type"))graph.operationList.back().type=cdi->value()->singleQuotedString()->getText().substr(1, cdi->value()->singleQuotedString()->getText().size()-2);
                    else if(cdi->tag()->getText().ends_with(".name"))graph.operationList.back().name=cdi->value()->getText();
                    else if(cdi->tag()->getText().ends_with(".symmetry_operation"))graph.operationList.back().symmetry_operation=cdi->value()->getText();
                    else if(cdi->tag()->getText().ends_with(".matrix[1][1]"))graph.operationList.back().matrix[0][0]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".matrix[1][2]"))graph.operationList.back().matrix[0][1]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".matrix[1][3]"))graph.operationList.back().matrix[0][2]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".vector[1]"))graph.operationList.back().vector[0]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".matrix[2][1]"))graph.operationList.back().matrix[1][0]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".matrix[2][2]"))graph.operationList.back().matrix[1][1]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".matrix[2][3]"))graph.operationList.back().matrix[1][2]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".vector[2]"))graph.operationList.back().vector[1]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".matrix[3][1]"))graph.operationList.back().matrix[2][0]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".matrix[3][2]"))graph.operationList.back().matrix[2][1]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".matrix[3][3]"))graph.operationList.back().matrix[2][2]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".vector[3]"))graph.operationList.back().vector[2]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                }
                for(sylvanmats::CIFParser::DataItemsContext* cdi: oi | std::views::filter([](sylvanmats::CIFParser::DataItemsContext* di){ return di->tag()!=nullptr && di->tag()->getText().starts_with("\n_atom_sites."); })){
                    if(cdi->tag()->getText().ends_with(".id"))graph.fractionalAtomSites.entry_id=cdi->value()->getText();
                    else if(cdi->tag()->getText().ends_with(".fract_transf_matrix[1][1]"))graph.fractionalAtomSites.fract_transf_matrix[0][0]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".fract_transf_matrix[1][2]"))graph.fractionalAtomSites.fract_transf_matrix[0][1]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".fract_transf_matrix[1][3]"))graph.fractionalAtomSites.fract_transf_matrix[0][2]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".fract_transf_matrix[2][1]"))graph.fractionalAtomSites.fract_transf_matrix[1][0]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".fract_transf_matrix[2][2]"))graph.fractionalAtomSites.fract_transf_matrix[1][1]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".fract_transf_matrix[2][3]"))graph.fractionalAtomSites.fract_transf_matrix[1][2]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".fract_transf_matrix[3][1]"))graph.fractionalAtomSites.fract_transf_matrix[2][0]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".fract_transf_matrix[3][2]"))graph.fractionalAtomSites.fract_transf_matrix[2][1]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".fract_transf_matrix[3][3]"))graph.fractionalAtomSites.fract_transf_matrix[2][2]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".fract_transf_vector[1]"))graph.fractionalAtomSites.fract_transf_vector[0]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".fract_transf_vector[2]"))graph.fractionalAtomSites.fract_transf_vector[1]=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    else if(cdi->tag()->getText().ends_with(".fract_transf_vector[3]"))graph.fractionalAtomSites.fract_transf_vector[2]=std::strtod(cdi->value()->getText().c_str(), nullptr);
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
                         case 1:
                             graph.componentProperties[n].entity_id = std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                         break;
                         case 2:
                             graph.componentProperties[n].seq_id = std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                         break;
                         case 3:
                             graph.componentProperties[n].mon_id.assign(values[valueIndex]->getText());
                         break;
                         case 4:
                             graph.componentProperties[n].ndb_seq_num = std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                         break;
                         case 5:
                             graph.componentProperties[n].pdb_seq_num = std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                         break;
                         case 6:
                             graph.componentProperties[n].auth_seq_num = std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                         break;
                         case 7:
                             graph.componentProperties[n].pdb_mon_id.assign(values[valueIndex]->getText());
                         break;
                         case 8:
                             graph.componentProperties[n].auth_mon_id.assign(values[valueIndex]->getText());
                         break;
                         case 9:
                             graph.componentProperties[n].pdb_strand_id.assign(values[valueIndex]->getText());
                         break;
                         case 10:
                             graph.componentProperties[n].pdb_ins_code.assign(values[valueIndex]->getText());
                             if(graph.componentProperties[n].pdb_ins_code.compare(".")==0)graph.componentProperties[n].pdb_ins_code="?";
                         break;
                         case 11:
                             graph.componentProperties[n].hetero.assign(values[valueIndex]->getText());
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
                            graph.structConnType[e]=COVALE;
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
                    if((valueIndex==r->loopBody()->value().size()-1) && previousNode!=lemon::INVALID)graph.componentProperties[previousNode].termination=C_TERMINAL;
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
                         case 2:
                             graph.componentProperties[n].mon_id.assign(values[valueIndex]->getText());
                         break;
                         case 4:
                             graph.componentProperties[n].auth_seq_num = std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                             graph.componentProperties[n].seq_id = graph.componentProperties[n].auth_seq_num;
                         break;
                         case 7:
                             graph.componentProperties[n].auth_mon_id.assign(values[valueIndex]->getText());
                         break;
                         case 9:
                             graph.componentProperties[n].pdb_ins_code.assign(values[valueIndex]->getText());
                             if(graph.componentProperties[n].pdb_ins_code.compare(".")==0)graph.componentProperties[n].pdb_ins_code="?";
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
                             graph.atomSites[n].group_PDB.assign(values[valueIndex]->getText());
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
                         case 15:
                             graph.atomSites[n].pdbx_formal_charge =(short)std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                         break;
                         case 16:
                             graph.atomSites[n].auth_seq_id =std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                             seq_id=graph.atomSites[n].auth_seq_id;
                         break;
                         case 17:
                             graph.atomSites[n].auth_comp_id.assign(values[valueIndex]->getText());
                         break;
                         case 18:
                             graph.atomSites[n].auth_asym_id.assign(values[valueIndex]->getText());
                         break;
                         case 19:
                             graph.atomSites[n].auth_atom_id.assign(values[valueIndex]->getText());
                         break;
                         case 20:
                             graph.atomSites[n].pdbx_PDB_model_num =(int)std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
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
//                         std::cout<<entityCount<<" "<<previous_comp_id<<" comp_id |"<<comp_id<<"|"<<seq_id<<std::endl;
                            nNode=lemon::INVALID;
                            cNode=lemon::INVALID;
                            lemon::ListGraph::Node currentCompNode=lemon::INVALID;
                             for(lemon::ListGraph::NodeIt nComp(graph.componentGraph); currentCompNode==lemon::INVALID && nComp!=lemon::INVALID; ++nComp){
                                //std::cout<<"\t"<<graph.componentProperties[nComp].auth_seq_num<<" "<<previous_seq_id<<" previous_asym_id "<<previous_asym_id<<" "<<graph.componentProperties[nComp].asym_id<<" "<<graph.componentProperties[nComp].auth_mon_id<<" "<<previous_comp_id<<" "<<graph.componentProperties[nComp].pdb_ins_code<<" "<<previous_ins_code<<" "<<graph.componentProperties[nComp].termination<<std::endl;
                                if(graph.componentProperties[nComp].auth_seq_num==previous_seq_id && graph.componentProperties[nComp].asym_id.compare(previous_asym_id)==0 && graph.componentProperties[nComp].auth_mon_id.compare(previous_comp_id)==0 && graph.componentProperties[nComp].pdb_ins_code.compare(previous_ins_code)==0)currentCompNode=nComp;
                            }
                            std::string current_comp_id(previous_comp_id.begin(), previous_comp_id.end());
                            //if(currentCompNode!=lemon::INVALID)std::cout<<" "<<graph.componentProperties[currentCompNode].mon_id<<" "<<graph.componentProperties[currentCompNode].termination<<std::endl;
                            //else std::cout<<lemon::countNodes(graph.componentGraph)<<" no comp graph "<<previous_comp_id<<std::endl;
                            std::vector<std::string> comp_ids;
                            if(currentCompNode!=lemon::INVALID && graph.componentProperties[currentCompNode].termination==sylvanmats::constitution::N_TERMINAL){
                                comp_ids.push_back(current_comp_id+"_LSN3");
                            }
                            else if(currentCompNode!=lemon::INVALID && graph.componentProperties[currentCompNode].termination==sylvanmats::constitution::C_TERMINAL){
                                comp_ids.push_back(current_comp_id+"_LEO2H");
                            }
                            else if(currentCompNode!=lemon::INVALID && graph.componentProperties[currentCompNode].termination==sylvanmats::constitution::NEUTRAL){
                                comp_ids.push_back(current_comp_id+"_LL");
                                comp_ids.push_back(current_comp_id);
                            }
                            else comp_ids.push_back(current_comp_id);

                            bool ret=aminoStandards(comp_ids[0], [&](sylvanmats::standards::chem_comp_atom<double>& cca1, sylvanmats::standards::chem_comp_bond& ccb, sylvanmats::standards::chem_comp_atom<double>& cca2){
                             unsigned int countA=0;
                             bool matched=false;
                             for(lemon::ListGraph::NodeIt nSiteA(graph); countA<=entityCount+1 && nSiteA!=lemon::INVALID; ++nSiteA){
                             unsigned int countB=0;
                             for(lemon::ListGraph::NodeIt nSiteB(graph); countB<=entityCount+1 && nSiteB!=lemon::INVALID; ++nSiteB){
                                 if(countA<countB && nSiteA!=nSiteB && graph.atomSites[nSiteA].auth_seq_id == graph.atomSites[nSiteB].auth_seq_id && graph.atomSites[nSiteA].auth_comp_id.compare(graph.atomSites[nSiteB].auth_comp_id)==0){
                                     if((graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_1)==0 && graph.atomSites[nSiteB].label_atom_id.compare(ccb.atom_id_2)==0) || (graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_2)==0 && graph.atomSites[nSiteB].label_atom_id.compare(ccb.atom_id_1)==0)){
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
//                                     if(graph.atomSites[nSiteA].auth_atom_id.compare("N")==0)std::cout<<"??? "<<countA<<" "<<proton1<<" "<<proton2<<" "<<entityCount<<" "<<graph.atomSites[nSiteA].label_atom_id<<" |"<<graph.atomSites[nSiteA].auth_atom_id<<"|"<<graph.atomSites[nSiteA].auth_seq_id<<std::endl;
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
                         if(ret)standardAACount++;
                         if(!ret){
                            //if(current_comp_id.compare("SO4")==0)std::cout<<current_comp_id<<" comp_id "<<comp_id<<" "<<ins_code<<std::endl;
                            ret=componentStandards(comp_ids, [&](sylvanmats::standards::chem_comp_atom<double>& cca1, sylvanmats::standards::chem_comp_bond& ccb, sylvanmats::standards::chem_comp_atom<double>& cca2){
                             //std::cout<<"got std "<<previous_comp_id<<" "<<entityCount<<std::endl;
                             unsigned int countA=0;
                             bool matched=false;
                             for(lemon::ListGraph::NodeIt nSiteA(graph); countA<=entityCount+1 && nSiteA!=lemon::INVALID; ++nSiteA){
                             unsigned int countB=0;
                             for(lemon::ListGraph::NodeIt nSiteB(graph); countB<=entityCount+1 && nSiteB!=lemon::INVALID; ++nSiteB){
                                 //if(current_comp_id.compare("SO4")==0)std::cout<<(entityCount+1)<<" ??? "<<countA<<" "<<countB<<" "<<entityCount<<" "<<graph.atomSites[nSiteA].label_asym_id<<" "<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.atomSites[nSiteB].label_asym_id<<" "<<graph.atomSites[nSiteB].label_atom_id<<std::endl;
                                 if(countA<countB && nSiteA!=nSiteB && graph.atomSites[nSiteA].auth_seq_id == graph.atomSites[nSiteB].auth_seq_id && graph.atomSites[nSiteA].auth_comp_id.compare(graph.atomSites[nSiteB].auth_comp_id)==0 && graph.atomSites[nSiteA].label_asym_id.compare(graph.atomSites[nSiteB].label_asym_id)==0){
                                     if((graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_1)==0 && graph.atomSites[nSiteB].label_atom_id.compare(ccb.atom_id_2)==0) || (graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_2)==0 && graph.atomSites[nSiteB].label_atom_id.compare(ccb.atom_id_1)==0)){
                                        //if(current_comp_id.compare("SO4")==0)std::cout<<"\t"<<entityCount<<" "<<graph.atomSites[nSiteA].auth_seq_id<<" "<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<graph.atomSites[nSiteB].auth_seq_id<<std::endl;
                                        lemon::ListGraph::Edge e=graph.addEdge(nSiteA, nSiteB);
                                        graph.compBond[e].value_order=ccb.value_order;
                                        matched=true;
                                     }
                                    //if(graph.atomSites[nSiteA].label_atom_id.compare("N")==0)nNode=nSiteA;
                                    //else if(graph.atomSites[nSiteA].label_atom_id.compare("C")==0)cNode=nSiteA;
                                    //if(graph.atomSites[nSiteB].label_atom_id.compare("N")==0)nNode=nSiteB;
                                    //else if(graph.atomSites[nSiteB].label_atom_id.compare("C")==0)cNode=nSiteB;

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
                            //std::cout<<"ret "<<ret<<" "<<standardCompCount<<std::endl; //https://files.rcsb.org/ligands/view/CGX.cif
                            if(ret)standardCompCount++;
                         }
                         previousNNode=nNode;
                         previousCNode=cNode;
                         entityCount=0;
                         previous_seq_id=seq_id;
                         previous_comp_id=comp_id;
                         previous_asym_id=asym_id;
                         previous_alt_id=alt_id;
                         previous_ins_code=ins_code;
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
        for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
             bool hit=false;
             for(lemon::ListGraph::NodeIt nCompA(graph.componentGraph); !hit && nCompA!=lemon::INVALID; ++nCompA){
                 if(graph.atomSites[nSiteA].auth_seq_id==graph.componentProperties[nCompA].seq_id && graph.atomSites[nSiteA].auth_asym_id.compare(graph.componentProperties[nCompA].asym_id)==0 && graph.atomSites[nSiteA].pdbx_PDB_ins_code.compare(graph.componentProperties[nCompA].pdb_ins_code)==0){
                     graph.componentNavigation.set(nSiteA, nCompA);
                     hit=true;
                     //if(graph.atomSites[nSiteA].auth_seq_id<15)std::cout<<"componentNavigation "<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.atomSites[nSiteA].auth_seq_id<<" "<<graph.atomSites[nSiteA].pdbx_PDB_ins_code<<std::endl;
                 }
             }
         }
        for(std::vector<antlr4::tree::ParseTree*>::iterator it=dataBlockLoop.begin();it!=dataBlockLoop.end();it++){
            if (sylvanmats::CIFParser::LoopContext* r=dynamic_cast<sylvanmats::CIFParser::LoopContext*>((*it))) {
                bool once=true;
                auto tags=r->loopHeader()->tag();
                for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return once && tag->getText().rfind("\n_pdbx_struct_oper_list.", 0) == 0; })){
                    once=false;
                    unsigned int columnCount=0;
                    graph.operationList.push_back(_pdbx_struct_oper_list<double>());
                    std::vector<sylvanmats::CIFParser::ValueContext *> values=r->loopBody()->value();
                    for(unsigned int valueIndex=0;valueIndex<values.size();valueIndex++){
                         switch(columnCount){
                            case 0:
                                graph.operationList.back().id=std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                            break;
                            case 1:
                                graph.operationList.back().type.assign(values[valueIndex]->getText());
                            break;
                            case 2:
                                graph.operationList.back().name.assign(values[valueIndex]->getText());
                            break;
                            case 3:
                                graph.operationList.back().symmetry_operation.assign(values[valueIndex]->getText());
                            break;
                            case 4:
                                graph.operationList.back().matrix[0][0]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case 5:
                                graph.operationList.back().matrix[0][1]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case 6:
                                graph.operationList.back().matrix[0][2]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case 7:
                                graph.operationList.back().vector[0]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case 8:
                                graph.operationList.back().matrix[1][0]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case 9:
                                graph.operationList.back().matrix[1][1]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case 10:
                                graph.operationList.back().matrix[1][2]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case 11:
                                graph.operationList.back().vector[1]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case 12:
                                graph.operationList.back().matrix[2][0]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case 13:
                                graph.operationList.back().matrix[2][1]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case 14:
                                graph.operationList.back().matrix[2][2]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case 15:
                                graph.operationList.back().vector[2]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                        }
                        columnCount++;
                        if((valueIndex % r->loopHeader()->tag().size() == r->loopHeader()->tag().size()-1) || valueIndex==values.size()-1){
                            columnCount=0;
                            if(valueIndex<r->loopBody()->value().size()-1)graph.operationList.push_back(_pdbx_struct_oper_list<double>());
                        }
                    }
                }
                once=true;
                for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return once && tag->getText().rfind("\n_struct_conn.", 0) == 0; })){
                    once=false;
                    unsigned int columnCount=0;
                    graph.structureConnections.push_back(_struct_conn<double>());
                    std::vector<sylvanmats::CIFParser::ValueContext *> values=r->loopBody()->value();
                    for(unsigned int valueIndex=0;valueIndex<values.size();valueIndex++){
                         switch(columnCount){
                            case 0:
                                graph.structureConnections.back().id.assign(values[valueIndex]->getText());
                            break;
                            case 1:
                                graph.structureConnections.back().conn_type_id.assign(values[valueIndex]->getText());
                            break;
                            case 2:
                                graph.structureConnections.back().pdbx_leaving_atom_flag.assign(values[valueIndex]->getText());
                            break;
                            case 3:
                                graph.structureConnections.back().pdbx_PDB_id.assign(values[valueIndex]->getText());
                            break;
                            case 4:
                                graph.structureConnections.back().ptnr1_label_asym_id.assign(values[valueIndex]->getText());
                            break;
                            case 5:
                                graph.structureConnections.back().ptnr1_label_comp_id.assign(values[valueIndex]->getText());
                            break;
                            case 6:
                                graph.structureConnections.back().ptnr1_label_seq_id=std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                            break;
                            case 7:
                                graph.structureConnections.back().ptnr1_label_atom_id.assign(values[valueIndex]->getText());
                            break;
                            case 8:
                                graph.structureConnections.back().pdbx_ptnr1_label_alt_id.assign(values[valueIndex]->getText());
                            break;
                            case 9:
                                graph.structureConnections.back().pdbx_ptnr1_PDB_ins_code.assign(values[valueIndex]->getText());
                            break;
                            case 10:
                                graph.structureConnections.back().pdbx_ptnr1_standard_comp_id.assign(values[valueIndex]->getText());
                            break;
                            case 11:
                                graph.structureConnections.back().ptnr1_symmetry.assign(values[valueIndex]->getText());
                            break;
                            case 12:
                                graph.structureConnections.back().ptnr2_label_asym_id.assign(values[valueIndex]->getText());
                            break;
                            case 13:
                                graph.structureConnections.back().ptnr2_label_comp_id.assign(values[valueIndex]->getText());
                            break;
                            case 14:
                                graph.structureConnections.back().ptnr2_label_seq_id=std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                            break;
                            case 15:
                                graph.structureConnections.back().ptnr2_label_atom_id.assign(values[valueIndex]->getText());
                            break;
                            case 16:
                                graph.structureConnections.back().pdbx_ptnr2_label_alt_id.assign(values[valueIndex]->getText());
                            break;
                            case 17:
                                graph.structureConnections.back().pdbx_ptnr2_PDB_ins_code.assign(values[valueIndex]->getText());
                            break;
                            case 18:
                                graph.structureConnections.back().ptnr1_auth_asym_id.assign(values[valueIndex]->getText());
                            break;
                            case 19:
                                graph.structureConnections.back().ptnr1_auth_comp_id.assign(values[valueIndex]->getText());
                            break;
                            case 20:
                                graph.structureConnections.back().ptnr1_auth_seq_id=std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                            break;
                            case 21:
                                graph.structureConnections.back().ptnr2_auth_asym_id.assign(values[valueIndex]->getText());
                            break;
                            case 22:
                                graph.structureConnections.back().ptnr2_auth_comp_id.assign(values[valueIndex]->getText());
                            break;
                            case 23:
                                graph.structureConnections.back().ptnr2_auth_seq_id=std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                            break;
                            case 24:
                                graph.structureConnections.back().ptnr2_symmetry.assign(values[valueIndex]->getText());
                            break;
                            case 25:
                                graph.structureConnections.back().pdbx_ptnr3_label_atom_id.assign(values[valueIndex]->getText());
                            break;
                            case 26:
                                graph.structureConnections.back().pdbx_ptnr3_label_seq_id=std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                            break;
                            case 27:
                                graph.structureConnections.back().pdbx_ptnr3_label_comp_id.assign(values[valueIndex]->getText());
                            break;
                            case 28:
                                graph.structureConnections.back().pdbx_ptnr3_label_asym_id.assign(values[valueIndex]->getText());
                            break;
                            case 29:
                                graph.structureConnections.back().pdbx_ptnr3_label_alt_id.assign(values[valueIndex]->getText());
                            break;
                            case 30:
                                graph.structureConnections.back().pdbx_ptnr3_PDB_ins_code.assign(values[valueIndex]->getText());
                            break;
                            case 31:
                                graph.structureConnections.back().details.assign(values[valueIndex]->getText());
                            break;
                            case 32:
                                graph.structureConnections.back().pdbx_dist_value=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                           break;
                            case 33:
                                graph.structureConnections.back().pdbx_value_order.assign(values[valueIndex]->getText());
                            break;
                        }
                        columnCount++;
                        if((valueIndex % r->loopHeader()->tag().size() == r->loopHeader()->tag().size()-1) || valueIndex==values.size()-1){
                            columnCount=0;
                            if(valueIndex<r->loopBody()->value().size()-1)graph.structureConnections.push_back(_struct_conn<double>());
                        }
                    }
                    for(std::vector<_struct_conn<double>>::iterator it=graph.structureConnections.begin();it!=graph.structureConnections.end();it++){
                        if((*it).ptnr1_symmetry.compare((*it).ptnr2_symmetry)!=0)continue;
                        lemon::ListGraph::Node ptnr1Node=lemon::INVALID;
                        lemon::ListGraph::Node ptnr2Node=lemon::INVALID;
                        std::string conn_type_id="";
                        for(lemon::ListGraph::NodeIt nComp(graph.componentGraph); (ptnr1Node==lemon::INVALID || ptnr2Node==lemon::INVALID) && nComp!=lemon::INVALID; ++nComp){
                            if((*it).ptnr1_auth_seq_id==graph.componentProperties[nComp].seq_id && (*it).ptnr1_auth_comp_id.compare(graph.componentProperties[nComp].mon_id)==0 && (*it).ptnr1_label_asym_id.compare(graph.componentProperties[nComp].asym_id)==0 && (*it).pdbx_ptnr1_PDB_ins_code.compare(graph.componentProperties[nComp].pdb_ins_code)==0){
                                ptnr1Node=nComp;
                                conn_type_id=(*it).conn_type_id;
                            }
                            /*else if((*it).ptnr1_auth_comp_id.compare("CA")==0 && graph.componentProperties[nComp].mon_id.compare("CA")==0){
                            std::cout<<"seq? "<<(*it).ptnr1_auth_seq_id<<" "<<graph.componentProperties[nComp].seq_id<<std::endl;
                            std::cout<<"comp? "<<(*it).ptnr1_auth_comp_id<<" "<<graph.componentProperties[nComp].mon_id<<std::endl;
                            std::cout<<"asym? "<<(*it).ptnr1_label_asym_id<<" "<<graph.componentProperties[nComp].asym_id<<std::endl;
                            std::cout<<"ins? "<<(*it).pdbx_ptnr1_PDB_ins_code<<" "<<graph.componentProperties[nComp].pdb_ins_code<<std::endl;
                            }*/
                            if((*it).ptnr2_auth_seq_id==graph.componentProperties[nComp].seq_id && (*it).ptnr2_auth_comp_id.compare(graph.componentProperties[nComp].mon_id)==0 && (*it).ptnr2_label_asym_id.compare(graph.componentProperties[nComp].asym_id)==0 && (*it).pdbx_ptnr2_PDB_ins_code.compare(graph.componentProperties[nComp].pdb_ins_code)==0){
                                ptnr2Node=nComp;
                            }
                        }
                        //std::cout<<"adding structure connections "<<(ptnr1Node!=lemon::INVALID)<<" "<<(ptnr2Node!=lemon::INVALID)<<" "<<(conn_type_id.compare("metalc")!=0)<<std::endl;
                        if(ptnr1Node!=lemon::INVALID && ptnr2Node!=lemon::INVALID ){
                            lemon::ListGraph::Edge e=lemon::findEdge(graph.componentGraph, ptnr1Node, ptnr2Node);
                            if(e!=lemon::INVALID){
                                e=graph.componentGraph.addEdge(ptnr1Node, ptnr2Node);
                                if(conn_type_id.compare("disulf")!=0){
                                    graph.structConnType[e]=DISULF;
                                }
                                else if(conn_type_id.compare("hydrog")!=0){
                                    graph.structConnType[e]=HYDROG;
                                }
                                else if(conn_type_id.compare("metalc")!=0){
                                    graph.structConnType[e]=METALC;
                                }
                                else if(conn_type_id.compare("mismat")!=0){
                                    graph.structConnType[e]=MISMAT;
                                }
                                else if(conn_type_id.compare("saltbr")!=0){
                                    graph.structConnType[e]=SALTBR;
                                }
                                else if(conn_type_id.compare("modres")!=0){
                                    graph.structConnType[e]=MODRES;
                                }
                                else if(conn_type_id.compare("covale_base")!=0){
                                    graph.structConnType[e]=COVALE_BASE;
                                }
                                else if(conn_type_id.compare("covale_sugar")!=0){
                                    graph.structConnType[e]=COVALE_SUGAR;
                                }
                                else if(conn_type_id.compare("covale_phosphate")!=0){
                                    graph.structConnType[e]=COVALE_PHOSPHATE;
                                }
                                else graph.structConnType[e]=COVALE;
                                lemon::ListGraph::Node n1=lemon::INVALID;
                                for(lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt itemIt(graph.componentNavigation, ptnr1Node); n1==lemon::INVALID && itemIt!=lemon::INVALID; ++itemIt){
                                    if(graph.atomSites[itemIt].label_atom_id.compare((*it).ptnr1_label_atom_id)==0)n1=itemIt;
                                }
                                lemon::ListGraph::Node n2=lemon::INVALID;
                                for(lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node>::ItemIt itemIt(graph.componentNavigation, ptnr2Node); n2==lemon::INVALID && itemIt!=lemon::INVALID; ++itemIt){
                                    if(graph.atomSites[itemIt].label_atom_id.compare((*it).ptnr2_label_atom_id)==0)n2=itemIt;
                                }
                                if(n1!=lemon::INVALID && n2!=lemon::INVALID){
                                    lemon::ListGraph::Edge e=graph.addEdge(n1, n2);
                                    graph.compBond[e].value_order=1;//(*it)..value_order;
                                }
                            }
                        }
                    }
                }
            }
        }
        apply(graph);
       std::cout<<"standardCompCount "<<standardCompCount<<" "<<standardAACount<<std::endl;
    };
    
}
