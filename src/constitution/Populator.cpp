#include <cstdio>
#include <ranges>

#include "constitution/Populator.h"
#include "standards/ComponentStandards.h"

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"
#include "reading/gz/GZReader.h"

#include "standards/AminoStandards.h"

namespace sylvanmats::constitution {


    void Populator::operator()(std::filesystem::path& filePath, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply){
        sylvanmats::reading::GZReader gzReader;
        gzReader(filePath, [&](std::istream& content){
         this->operator()(content, graph, apply);
        });
     };

    void Populator::operator()(std::istream& content, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply){
        standardAACount=0;
        standardCompCount=0;

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
                        if(cdi->value()->singleQuotedString()!=nullptr) {std::string spaceGroup=cdi->value()->singleQuotedString()->getText().substr(1, cdi->value()->singleQuotedString()->getText().size()-2);graph.symmetry.space_group_name_H_M.assign(spaceGroup);}
                        else graph.symmetry.space_group_name_H_M=cdi->value()->getText();
                    }
                    else if(cdi->tag()->getText().compare("\n_symmetry.Int_Tables_number")==0)graph.symmetry.Int_Tables_number=std::strtol(cdi->value()->getText().c_str(), nullptr, 10);
                }
                lemon::ListGraph::Node n=lemon::INVALID;
                for(sylvanmats::CIFParser::DataItemsContext* cdi: oi | std::views::filter([](sylvanmats::CIFParser::DataItemsContext* di){return di->tag()!=nullptr && di->tag()->getText().starts_with("\n_pdbx_nonpoly_scheme."); })){
                    if(n==lemon::INVALID)n=graph.componentGraph.addNode();
                    if(cdi->tag()->getText().compare("\n_pdbx_nonpoly_scheme.asym_id")==0)graph.componentProperties[n].asym_id.assign(cdi->value()->getText());
                    else if(cdi->tag()->getText().compare("\n_pdbx_nonpoly_scheme.mon_id")==0)graph.componentProperties[n].mon_id.assign(cdi->value()->getText());
                    else if(cdi->tag()->getText().compare("\n_pdbx_nonpoly_scheme.pdb_seq_num")==0)graph.componentProperties[n].pdb_seq_num=std::strtol(cdi->value()->getText().c_str(), nullptr, 10);
                    else if(cdi->tag()->getText().compare("\n_pdbx_nonpoly_scheme.auth_seq_num")==0)graph.componentProperties[n].auth_seq_num=std::strtol(cdi->value()->getText().c_str(), nullptr, 10);
                    else if(cdi->tag()->getText().compare("\n_pdbx_nonpoly_scheme.pdb_mon_id")==0)graph.componentProperties[n].pdb_mon_id.assign(cdi->value()->getText());
                    else if(cdi->tag()->getText().compare("\n_pdbx_nonpoly_scheme.auth_mon_id")==0)graph.componentProperties[n].auth_mon_id.assign(cdi->value()->getText());
                    else if(cdi->tag()->getText().compare("\n_pdbx_nonpoly_scheme.pdb_strand_id")==0){
                        graph.componentProperties[n].pdb_strand_id.assign(cdi->value()->getText());
                        if(graph.componentProperties[n].pdb_strand_id.compare(".")==0)graph.componentProperties[n].pdb_strand_id="?";
                    }
                    else if(cdi->tag()->getText().compare("\n_pdbx_nonpoly_scheme.pdb_ins_code")==0){
                        graph.componentProperties[n].pdb_ins_code.assign(cdi->value()->getText());
                        if(graph.componentProperties[n].pdb_ins_code.compare(".")==0)graph.componentProperties[n].pdb_ins_code="?";
                    }
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
                std::unordered_map<size_t, std::string> tagLabels;
                unsigned int columnCount=0;
                for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return tag->getText().rfind("\n_pdbx_poly_seq_scheme.", 0) == 0; })){
                    tagLabels[columnCount++]=t->getText().substr(23);
                }
                for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return once && tag->getText().rfind("\n_pdbx_poly_seq_scheme.", 0) == 0; })){
                    once=false;
                    columnCount=0;
                    unsigned int compCount=0;
                    lemon::ListGraph::Node previousNode=lemon::INVALID;
//        std::cout<<"pop componentGraph.addComponentNode "<<std::endl;
                    lemon::ListGraph::Node&& n=graph.addComponentNode();
//        std::cout<<"pop componentGraph.added Node "<<std::endl;
                    std::string_view asym_id="";
                    std::string_view previous_asym_id="";
                    bool first=true;
                std::vector<sylvanmats::CIFParser::ValueContext *> values=r->loopBody()->value();
                 for(unsigned int valueIndex=0;valueIndex<values.size();valueIndex++){
                    switch(tagPolySeqMap[tagLabels[columnCount]]){
                        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, asym_id):
                             graph.componentProperties[n].asym_id.assign(values[valueIndex]->getText());
                             asym_id=graph.componentProperties[n].asym_id;
                        break;
                        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, entity_id):
                            graph.componentProperties[n].entity_id = std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                        break;
                        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, seq_id):
                           graph.componentProperties[n].seq_id = std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                        break;
                        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, mon_id):
                            graph.componentProperties[n].mon_id.assign(values[valueIndex]->getText());
                        break;
                        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, ndb_seq_num):
                            graph.componentProperties[n].ndb_seq_num = std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                        break;
                        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, pdb_seq_num):
                            graph.componentProperties[n].pdb_seq_num = std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                        break;
                        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, auth_seq_num):
                            graph.componentProperties[n].auth_seq_num = std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                        break;
                        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, pdb_mon_id):
                            graph.componentProperties[n].pdb_mon_id.assign(values[valueIndex]->getText());
                        break;
                        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, auth_mon_id):
                            graph.componentProperties[n].auth_mon_id.assign(values[valueIndex]->getText());
                        break;
                        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, pdb_strand_id):
                            graph.componentProperties[n].pdb_strand_id.assign(values[valueIndex]->getText());
                        break;
                        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, pdb_ins_code):
                            graph.componentProperties[n].pdb_ins_code.assign(values[valueIndex]->getText());
                            if(graph.componentProperties[n].pdb_ins_code.compare(".")==0)graph.componentProperties[n].pdb_ins_code="?";
                        break;
                        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, hetero):
                            graph.componentProperties[n].hetero.assign(values[valueIndex]->getText());
                        break;
                    }
                   columnCount++;
                    if((valueIndex % tags.size() == tags.size()-1) || valueIndex==values.size()-1){
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
//                             std::cout<<"edge "<<" "<<std::endl;
                         }
                        }
                        else if(previousNode!=lemon::INVALID){
                            graph.componentProperties[previousNode].termination=C_TERMINAL;
                            graph.componentProperties[n].termination=N_TERMINAL;
                        }
                        previous_asym_id=asym_id;
                        previousNode=n;
                        if(valueIndex<values.size()-1){
                            n=graph.componentGraph.addNode();
                        }
                    }
                    if((valueIndex==values.size()-1) && previousNode!=lemon::INVALID)graph.componentProperties[previousNode].termination=C_TERMINAL;
                }
                }
                std::unordered_map<size_t, std::string> tagNonpolyLabels;
                columnCount=0;
                for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return tag->getText().rfind("\n_pdbx_nonpoly_scheme.", 0) == 0; })){
                   tagNonpolyLabels[columnCount++]=t->getText().substr(22);
                }
                for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return once && tag->getText().rfind("\n_pdbx_nonpoly_scheme.", 0) == 0; })){
                    once=false;
                    columnCount=0;
                    lemon::ListGraph::Node n=graph.componentGraph.addNode();
                std::vector<sylvanmats::CIFParser::ValueContext *> values=r->loopBody()->value();
                for(unsigned int valueIndex=0;valueIndex<values.size();valueIndex++){
                    switch(tagPolySeqMap[tagNonpolyLabels[columnCount]]){
                         case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, asym_id):
                             graph.componentProperties[n].asym_id.assign(values[valueIndex]->getText());
                         break;
                         case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, mon_id):
                             graph.componentProperties[n].mon_id.assign(values[valueIndex]->getText());
                         break;
                         case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, pdb_seq_num):
                             graph.componentProperties[n].pdb_seq_num = std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                         break;
                         case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, auth_seq_num):
                             graph.componentProperties[n].auth_seq_num = std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                             graph.componentProperties[n].seq_id = graph.componentProperties[n].auth_seq_num;
                         break;
                         case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, pdb_mon_id):
                             graph.componentProperties[n].pdb_mon_id.assign(values[valueIndex]->getText());
                         break;
                         case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, auth_mon_id):
                             graph.componentProperties[n].auth_mon_id.assign(values[valueIndex]->getText());
                         break;
                         case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, pdb_strand_id):
                             graph.componentProperties[n].pdb_strand_id.assign(values[valueIndex]->getText());
                             if(graph.componentProperties[n].pdb_strand_id.compare(".")==0)graph.componentProperties[n].pdb_strand_id="?";
                         break;
                         case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme<double>, pdb_ins_code):
                             graph.componentProperties[n].pdb_ins_code.assign(values[valueIndex]->getText());
                             if(graph.componentProperties[n].pdb_ins_code.compare(".")==0)graph.componentProperties[n].pdb_ins_code="?";
                         break;
                    }
                   columnCount++;
                    if((valueIndex % tags.size() == tags.size()-1) || valueIndex==values.size()-1){
                        columnCount=0;
                         graph.componentProperties[n].termination=MONOMER;
                         //previous_asym_id=asym_id;
                        if(valueIndex<values.size()-1){
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
                size_t tagsSize=tags.size();
                std::unordered_map<size_t, std::string> tagLabels;
                unsigned int columnCount=0;
                for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return tag->getText().rfind("\n_atom_site.", 0) == 0; })){
                   tagLabels[columnCount++]=t->getText().substr(12);
                }
                   for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return once && tag->getText().rfind("\n_atom_site.", 0) == 0; })){
                    once=false;
                    columnCount=0;
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
        size_t innerTime = 0;
                std::vector<sylvanmats::CIFParser::ValueContext *> values=r->loopBody()->value();
                for(unsigned int valueIndex=0;valueIndex<values.size();valueIndex++){
                    switch(tagAtomSiteMap[tagLabels[columnCount]]){
                         case offsetof(sylvanmats::constitution::_atom_site<double>, group_PDB):
                             graph.atomSites[n].group_PDB.assign(values[valueIndex]->getText());
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, id):
                             graph.atomSites[n].id =std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, type_symbol):
                             graph.atomSites[n].type_symbol.assign(values[valueIndex]->getText());
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, label_atom_id):
                             graph.atomSites[n].label_atom_id.assign(values[valueIndex]->getText());
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, label_alt_id):
                             graph.atomSites[n].label_alt_id.assign(values[valueIndex]->getText());
                             alt_id=graph.atomSites[n].label_alt_id;
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, label_comp_id):
                             graph.atomSites[n].label_comp_id.assign(values[valueIndex]->getText());
                             comp_id=graph.atomSites[n].label_comp_id;
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, label_asym_id):
                             graph.atomSites[n].label_asym_id.assign(values[valueIndex]->getText());
                             asym_id=graph.atomSites[n].label_asym_id;
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, label_seq_id):
                             if(std::count_if(values[valueIndex]->getText().begin(), values[valueIndex]->getText().end(), [](int i){return std::isdigit(i);})){
                                graph.atomSites[n].label_seq_id =std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                                seq_id=graph.atomSites[n].label_seq_id;
                             }
                         case offsetof(sylvanmats::constitution::_atom_site<double>, pdbx_PDB_ins_code):
                             graph.atomSites[n].pdbx_PDB_ins_code.assign(values[valueIndex]->getText());
                             ins_code=graph.atomSites[n].pdbx_PDB_ins_code;
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, Cartn_x):
                             graph.atomSites[n].Cartn_x =std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, Cartn_y):
                             graph.atomSites[n].Cartn_y =std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, Cartn_z):
                             graph.atomSites[n].Cartn_z =std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, occupancy):
                             graph.atomSites[n].occupancy =std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, B_iso_or_equiv):
                             graph.atomSites[n].B_iso_or_equiv =std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, pdbx_formal_charge):
                             graph.atomSites[n].pdbx_formal_charge =(short)std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, auth_seq_id):
                             if(std::count_if(values[valueIndex]->getText().begin(), values[valueIndex]->getText().end(), [](int i){return std::isdigit(i);})){
                                graph.atomSites[n].auth_seq_id =std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                                seq_id=graph.atomSites[n].auth_seq_id;
                             }
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, auth_comp_id):
                             graph.atomSites[n].auth_comp_id.assign(values[valueIndex]->getText());
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, auth_asym_id):
                             graph.atomSites[n].auth_asym_id.assign(values[valueIndex]->getText());
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, auth_atom_id):
                             graph.atomSites[n].auth_atom_id.assign(values[valueIndex]->getText());
                         break;
                         case offsetof(sylvanmats::constitution::_atom_site<double>, pdbx_PDB_model_num):
                             graph.atomSites[n].pdbx_PDB_model_num =(int)std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                         break;
                    }
                    columnCount++;
                    if((valueIndex % tagsSize == tagsSize-1) || valueIndex==values.size()-1){
                        columnCount=0;
//                        if(comp_id.compare("1OB")==0)std::cout<<valueIndex<<" "<<(r->loopHeader()->tag().size() % valueIndex)<<" "<<(r->loopHeader()->tag().size()-1)<<" entityCount "<<entityCount<<" "<<previous_comp_id<<" "<<comp_id<<" "<<previous_seq_id<<" "<<seq_id<<" "<<previous_asym_id<<" "<<asym_id<<" "<<previous_alt_id<<" "<<alt_id<<" "<<previous_ins_code<<" "<<ins_code<<std::endl;
                        if(first){
                         first=false;
                         previous_seq_id=seq_id;
                         previous_comp_id=comp_id;
                         previous_asym_id=asym_id;
                         previous_alt_id=alt_id;
                         previous_ins_code=ins_code;
                        }
                        else if((previous_seq_id!=seq_id || previous_comp_id.compare(comp_id)!=0 || previous_asym_id.compare(asym_id)!=0 || previous_alt_id.compare(alt_id)!=0 || previous_ins_code.compare(ins_code)!=0) || valueIndex==values.size()-1){
//auto startInner = std::chrono::high_resolution_clock::now();
//                         std::cout<<entityCount<<" "<<previous_comp_id<<" comp_id |"<<comp_id<<"|"<<seq_id<<"|"<<asym_id<<"|"<<ins_code<<"|"<<alt_id<<std::endl;
                            nNode=lemon::INVALID;
                            cNode=lemon::INVALID;
                            lemon::ListGraph::Node currentCompNode=lemon::INVALID;
                             for(lemon::ListGraph::NodeIt nComp(graph.componentGraph); currentCompNode==lemon::INVALID && nComp!=lemon::INVALID; ++nComp){
                                //std::cout<<"\t"<<graph.componentProperties[nComp].auth_seq_num<<" "<<previous_seq_id<<" previous_asym_id "<<previous_asym_id<<" "<<graph.componentProperties[nComp].asym_id<<" "<<graph.componentProperties[nComp].pdb_mon_id<<" "<<previous_comp_id<<" "<<graph.componentProperties[nComp].pdb_ins_code<<" "<<previous_ins_code<<" "<<graph.componentProperties[nComp].termination<<std::endl;
                                if(graph.componentProperties[nComp].pdb_seq_num==previous_seq_id && graph.componentProperties[nComp].asym_id.compare(previous_asym_id)==0 && graph.componentProperties[nComp].pdb_mon_id.compare(previous_comp_id)==0 && graph.componentProperties[nComp].pdb_ins_code.compare(previous_ins_code)==0)currentCompNode=nComp;
                            }
                            std::string current_comp_id(previous_comp_id.begin(), previous_comp_id.end());
//                            if(currentCompNode!=lemon::INVALID)std::cout<<" "<<graph.componentProperties[currentCompNode].mon_id<<" "<<graph.componentProperties[currentCompNode].termination<<std::endl;
//                            else std::cout<<lemon::countNodes(graph.componentGraph)<<" no comp graph "<<previous_comp_id<<std::endl;
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
                             if(graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_1)==0 && cca1.charge!=0)graph.atomSites[nSiteA].pdbx_formal_charge=cca1.charge;
                             else if(graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_2)==0 && cca2.charge!=0)graph.atomSites[nSiteA].pdbx_formal_charge=cca2.charge;
                             unsigned int countB=0;
                             for(lemon::ListGraph::NodeIt nSiteB(graph); countB<=entityCount+1 && nSiteB!=lemon::INVALID; ++nSiteB){
                                 if(countA<countB && nSiteA!=nSiteB && graph.atomSites[nSiteA].auth_seq_id == graph.atomSites[nSiteB].auth_seq_id && graph.atomSites[nSiteA].auth_comp_id.compare(graph.atomSites[nSiteB].auth_comp_id)==0){
                                     if((graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_1)==0 && graph.atomSites[nSiteB].label_atom_id.compare(ccb.atom_id_2)==0) || (graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_2)==0 && graph.atomSites[nSiteB].label_atom_id.compare(ccb.atom_id_1)==0)){
                                        lemon::ListGraph::Edge e=graph.addEdge(nSiteA, nSiteB);
                                        graph.compBond[e].value_order=ccb.value_order;
                                        if(graph.compBond[e].value_order==3)
                                            graph.compBond[e].type=forcefield::BOND_TRIPLE;
                                        else if(graph.compBond[e].value_order==2)
                                            graph.compBond[e].type=forcefield::BOND_DOUBLE;
                                        else if(graph.compBond[e].value_order==1)
                                            graph.compBond[e].type=forcefield::BOND_SINGLE;
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
//                          std::cout<<"compCount "<<compCount<<std::endl;
                         if(compCount>1 && cNode!=lemon::INVALID  && nNode!=lemon::INVALID && previousNNode!=lemon::INVALID  && previousCNode!=lemon::INVALID){
                            lemon::ListGraph::Edge e=graph.addEdge(previousCNode, nNode);
                            graph.compBond[e].value_order=1;

                         }
                         if(ret)standardAACount++;
                         if(!ret){
                            comp_ids.insert(comp_ids.begin(), current_comp_id);
                            //if(current_comp_id.compare("SO4")==0)std::cout<<current_comp_id<<" comp_id "<<comp_id<<" "<<ins_code<<" "<<comp_ids.size()<<" "<<comp_ids[0]<<" "<<comp_ids.back()<<std::endl;
                            ret=componentStandards(comp_ids, [&](sylvanmats::standards::chem_comp_atom<double>& cca1, sylvanmats::standards::chem_comp_bond& ccb, sylvanmats::standards::chem_comp_atom<double>& cca2){
//                             std::cout<<"got std "<<previous_comp_id<<" "<<entityCount<<std::endl;
                             unsigned int countA=0;
                             bool matched=false;
                             for(lemon::ListGraph::NodeIt nSiteA(graph); countA<=entityCount+1 && nSiteA!=lemon::INVALID; ++nSiteA){
                             if(graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_1)==0 && cca1.charge!=0)graph.atomSites[nSiteA].pdbx_formal_charge=cca1.charge;
                             else if(graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_2)==0 && cca2.charge!=0)graph.atomSites[nSiteA].pdbx_formal_charge=cca2.charge;
                             unsigned int countB=0;
                             for(lemon::ListGraph::NodeIt nSiteB(graph); countB<=entityCount+1 && nSiteB!=lemon::INVALID; ++nSiteB){
                                 //if(current_comp_id.compare("SO4")==0)std::cout<<(entityCount+1)<<" ??? "<<countA<<" "<<countB<<" "<<entityCount<<" "<<graph.atomSites[nSiteA].label_asym_id<<" "<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.atomSites[nSiteB].label_asym_id<<" "<<graph.atomSites[nSiteB].label_atom_id<<std::endl;
                                 if(countA<countB && nSiteA!=nSiteB && graph.atomSites[nSiteA].auth_seq_id == graph.atomSites[nSiteB].auth_seq_id && graph.atomSites[nSiteA].auth_comp_id.compare(graph.atomSites[nSiteB].auth_comp_id)==0 && graph.atomSites[nSiteA].label_asym_id.compare(graph.atomSites[nSiteB].label_asym_id)==0){
                                     if((graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_1)==0 && graph.atomSites[nSiteB].label_atom_id.compare(ccb.atom_id_2)==0) || (graph.atomSites[nSiteA].label_atom_id.compare(ccb.atom_id_2)==0 && graph.atomSites[nSiteB].label_atom_id.compare(ccb.atom_id_1)==0)){
                                        //if(current_comp_id.compare("SO4")==0)std::cout<<"\t"<<entityCount<<" "<<graph.atomSites[nSiteA].auth_seq_id<<" "<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.atomSites[nSiteB].label_atom_id<<" "<<graph.atomSites[nSiteB].auth_seq_id<<std::endl;
                                        lemon::ListGraph::Edge e=graph.addEdge(nSiteA, nSiteB);
                                        graph.compBond[e].value_order=ccb.value_order;
                                        if(graph.compBond[e].value_order==3)
                                            graph.compBond[e].type=forcefield::BOND_TRIPLE;
                                        else if(graph.compBond[e].value_order==2)
                                            graph.compBond[e].type=forcefield::BOND_DOUBLE;
                                        else if(graph.compBond[e].value_order==1)
                                            graph.compBond[e].type=forcefield::BOND_SINGLE;
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
//                            std::cout<<"ret "<<ret<<" "<<standardCompCount<<std::endl; //https://files.rcsb.org/ligands/view/CGX.cif
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
//                    auto endInner = std::chrono::high_resolution_clock::now();
//                    innerTime+=std::chrono::duration_cast<std::chrono::nanoseconds>(endInner-startInner).count();
                         }
                        else entityCount++;
                        if(valueIndex<values.size()-1)n=graph.addNode();
                   }
                }
        auto end = std::chrono::high_resolution_clock::now();
//        std::cout << "inner time: " << innerTime*1.0e-9 << "s\n";
//        std::cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count()*1.0e-9 << "s\n";

                }
                std::unordered_map<size_t, std::string> tagAnisotropicLabels;
                for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return tag->getText().rfind("\n_atom_site_anisotrop.", 0) == 0; })){
                   tagAnisotropicLabels[columnCount++]=t->getText().substr(20);
                }
                for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return once && tag->getText().rfind("\n_atom_site_anisotrop.", 0) == 0; })){
                    once=false;
                    columnCount=0;
                    unsigned int entityCount=0;
                    std::vector<sylvanmats::CIFParser::ValueContext *> values=r->loopBody()->value();
                    for(unsigned int valueIndex=0;valueIndex<values.size();valueIndex++){
                        switch(tagAtomSiteAnisotropMap[tagAnisotropicLabels[columnCount]]){
                             case offsetof(sylvanmats::constitution::_atom_site_anisotrop<double>, id):
                                 //graph.atomSites[n].group_PDB.assign(values[valueIndex]->getText());
                             break;
                        }
                    }
                }

         }
         }
        for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
             bool hit=false;
             for(lemon::ListGraph::NodeIt nCompA(graph.componentGraph); !hit && nCompA!=lemon::INVALID; ++nCompA){
//                 if(graph.atomSites[nSiteA].auth_comp_id.compare("U3S")==0 && graph.componentProperties[nCompA].pdb_mon_id.compare("U3S")==0)
//                     std::cout<<"componentNavigation "<<graph.atomSites[nSiteA].label_atom_id<<" " << graph.atomSites[nSiteA].label_asym_id<<" =? "<<graph.componentProperties[nCompA].asym_id<<" "<<graph.atomSites[nSiteA].auth_seq_id<<" =? "<<graph.componentProperties[nCompA].pdb_seq_num<<" "<<graph.atomSites[nSiteA].pdbx_PDB_ins_code<<" =? "<<graph.componentProperties[nCompA].pdb_ins_code<<std::endl;
                 if(graph.atomSites[nSiteA].auth_comp_id.compare(graph.componentProperties[nCompA].pdb_mon_id)==0 && graph.atomSites[nSiteA].auth_seq_id==graph.componentProperties[nCompA].pdb_seq_num && graph.atomSites[nSiteA].label_asym_id.compare(graph.componentProperties[nCompA].asym_id)==0 && compareInsertionCode(graph.atomSites[nSiteA].pdbx_PDB_ins_code, graph.componentProperties[nCompA].pdb_ins_code)){
                     graph.componentNavigation.set(nSiteA, nCompA);
                     hit=true;
//                     if(graph.atomSites[nSiteA].auth_comp_id.compare("U3S")==0)std::cout<<"componentNavigation "<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.atomSites[nSiteA].auth_seq_id<<" "<<graph.atomSites[nSiteA].pdbx_PDB_ins_code<<std::endl;
                 }
             }
         }
        for(std::vector<antlr4::tree::ParseTree*>::iterator it=dataBlockLoop.begin();it!=dataBlockLoop.end();it++){
            if (sylvanmats::CIFParser::LoopContext* r=dynamic_cast<sylvanmats::CIFParser::LoopContext*>((*it))) {
                bool once=true;
                auto tags=r->loopHeader()->tag();
                std::unordered_map<size_t, std::string> tagLabels;
                unsigned int columnCount=0;
                for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return tag->getText().rfind("\n_pdbx_struct_oper_list.", 0) == 0; })){
                   tagLabels[columnCount++]=t->getText().substr(24);
                }
                for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return once && tag->getText().rfind("\n_pdbx_struct_oper_list.", 0) == 0; })){
                    once=false;
                    columnCount=0;
                    graph.operationList.push_back(_pdbx_struct_oper_list<double>());
                    std::vector<sylvanmats::CIFParser::ValueContext *> values=r->loopBody()->value();
                    for(unsigned int valueIndex=0;valueIndex<values.size();valueIndex++){
                         switch(tagOperationsMap[tagLabels[columnCount]]){
                            case offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, id):
                                graph.operationList.back().id=std::strtoul(values[valueIndex]->getText().c_str(), nullptr, 10);
                            break;
                            case offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, type):
                                graph.operationList.back().type.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, name):
                                graph.operationList.back().name.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, symmetry_operation):
                                graph.operationList.back().symmetry_operation.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix):
                                graph.operationList.back().matrix[0][0]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case (sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix)):
                                graph.operationList.back().matrix[0][1]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case (2*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix)):
                                graph.operationList.back().matrix[0][2]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, vector):
                                graph.operationList.back().vector[0]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case (3*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix)):
                                graph.operationList.back().matrix[1][0]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case (4*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix)):
                                graph.operationList.back().matrix[1][1]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case (5*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix)):
                                graph.operationList.back().matrix[1][2]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case (sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, vector)):
                                graph.operationList.back().vector[1]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case (6*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix)):
                                graph.operationList.back().matrix[2][0]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case (7*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix)):
                                graph.operationList.back().matrix[2][1]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case (8*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix)):
                                graph.operationList.back().matrix[2][2]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                            case (2*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, vector)):
                                graph.operationList.back().vector[2]=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                            break;
                        }
                        columnCount++;
                        if((valueIndex % r->loopHeader()->tag().size() == r->loopHeader()->tag().size()-1) || valueIndex==values.size()-1){
                            columnCount=0;
                            if(valueIndex<values.size()-1)graph.operationList.push_back(_pdbx_struct_oper_list<double>());
                        }
                    }
                }
                once=true;
                std::unordered_map<size_t, std::string> tagConnectionsLabels;
                columnCount=0;
                for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return tag->getText().rfind("\n_struct_conn.", 0) == 0; })){
                   tagConnectionsLabels[columnCount++]=t->getText().substr(14);
                }
                for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return once && tag->getText().rfind("\n_struct_conn.", 0) == 0; })){
                    once=false;
                    columnCount=0;
                    graph.structureConnections.push_back(_struct_conn<double>());
                    std::vector<sylvanmats::CIFParser::ValueContext *> values=r->loopBody()->value();
                    for(unsigned int valueIndex=0;valueIndex<values.size();valueIndex++){
                         switch(tagConnectionsMap[tagConnectionsLabels[columnCount]]){
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, id):
                                graph.structureConnections.back().id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, conn_type_id):
                                graph.structureConnections.back().conn_type_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_leaving_atom_flag):
                                graph.structureConnections.back().pdbx_leaving_atom_flag.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_PDB_id):
                                graph.structureConnections.back().pdbx_PDB_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_label_asym_id):
                                graph.structureConnections.back().ptnr1_label_asym_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_label_comp_id):
                                graph.structureConnections.back().ptnr1_label_comp_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_label_seq_id):
                                graph.structureConnections.back().ptnr1_label_seq_id=std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_label_atom_id):
                                graph.structureConnections.back().ptnr1_label_atom_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr1_label_alt_id):
                                graph.structureConnections.back().pdbx_ptnr1_label_alt_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr1_PDB_ins_code):
                                graph.structureConnections.back().pdbx_ptnr1_PDB_ins_code.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr1_standard_comp_id):
                                graph.structureConnections.back().pdbx_ptnr1_standard_comp_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_symmetry):
                                graph.structureConnections.back().ptnr1_symmetry.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_label_asym_id):
                                graph.structureConnections.back().ptnr2_label_asym_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_label_comp_id):
                                graph.structureConnections.back().ptnr2_label_comp_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_label_seq_id):
                                graph.structureConnections.back().ptnr2_label_seq_id=std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_label_atom_id):
                                graph.structureConnections.back().ptnr2_label_atom_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr2_label_alt_id):
                                graph.structureConnections.back().pdbx_ptnr2_label_alt_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr2_PDB_ins_code):
                                graph.structureConnections.back().pdbx_ptnr2_PDB_ins_code.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_auth_asym_id):
                                graph.structureConnections.back().ptnr1_auth_asym_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_auth_comp_id):
                                graph.structureConnections.back().ptnr1_auth_comp_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_auth_seq_id):
                                graph.structureConnections.back().ptnr1_auth_seq_id=std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_auth_asym_id):
                                graph.structureConnections.back().ptnr2_auth_asym_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_auth_comp_id):
                                graph.structureConnections.back().ptnr2_auth_comp_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_auth_seq_id):
                                graph.structureConnections.back().ptnr2_auth_seq_id=std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_symmetry):
                                graph.structureConnections.back().ptnr2_symmetry.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr3_label_atom_id):
                                graph.structureConnections.back().pdbx_ptnr3_label_atom_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr3_label_seq_id):
                                graph.structureConnections.back().pdbx_ptnr3_label_seq_id=std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr3_label_comp_id):
                                graph.structureConnections.back().pdbx_ptnr3_label_comp_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr3_label_asym_id):
                                graph.structureConnections.back().pdbx_ptnr3_label_asym_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr3_label_alt_id):
                                graph.structureConnections.back().pdbx_ptnr3_label_alt_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr3_PDB_ins_code):
                                graph.structureConnections.back().pdbx_ptnr3_PDB_ins_code.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, details):
                                graph.structureConnections.back().details.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_dist_value):
                                graph.structureConnections.back().pdbx_dist_value=std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                           break;
                            case offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_value_order):
                                graph.structureConnections.back().pdbx_value_order.assign(values[valueIndex]->getText());
                                
                            break;
                        }
                        columnCount++;
                        if((valueIndex % r->loopHeader()->tag().size() == r->loopHeader()->tag().size()-1) || valueIndex==values.size()-1){
                            columnCount=0;
                            if(valueIndex<values.size()-1)graph.structureConnections.push_back(_struct_conn<double>());
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
//                        std::cout<<"adding structure connections "<<(ptnr1Node!=lemon::INVALID)<<" "<<(ptnr2Node!=lemon::INVALID)<<" "<<(conn_type_id.compare("metalc")!=0)<<std::endl;
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
                once=true;
                std::unordered_map<size_t, std::string> tagSequenceLabels;
                columnCount=0;
                for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return tag->getText().rfind("\n_entity_poly_seq.", 0) == 0; })){
                   tagSequenceLabels[columnCount]=t->getText().substr(18);
                   columnCount++;
                }
                for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return once && tag->getText().rfind("\n_entity_poly_seq.", 0) == 0; })){
                    once=false;
                    columnCount=0;
                    graph.residueSequence.push_back(_entity_poly_seq<double>());
                    std::vector<sylvanmats::CIFParser::ValueContext *> values=r->loopBody()->value();
                    for(unsigned int valueIndex=0;valueIndex<values.size();valueIndex++){
                         switch(tagSequenceMap[tagSequenceLabels[columnCount]]){
                            case offsetof(sylvanmats::constitution::_entity_poly_seq<double>, entity_id):
                                graph.residueSequence.back().entity_id=std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                            break;
                            case offsetof(sylvanmats::constitution::_entity_poly_seq<double>, num):
                                graph.residueSequence.back().num=std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                            break;
                            case offsetof(sylvanmats::constitution::_entity_poly_seq<double>, mon_id):
                                graph.residueSequence.back().mon_id.assign(values[valueIndex]->getText());
                            break;
                            case offsetof(sylvanmats::constitution::_entity_poly_seq<double>, hetero):
                                graph.residueSequence.back().hetero=(short)std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                            break;
                        }
                        columnCount++;
                        if((valueIndex % r->loopHeader()->tag().size() == r->loopHeader()->tag().size()-1) || valueIndex==values.size()-1){
                            columnCount=0;
                            if(valueIndex<values.size()-1)graph.residueSequence.push_back(_entity_poly_seq<double>());
                        }
                    }
                }
            }
        }
        apply(graph);
//       std::cout<<"standardCompCount "<<standardCompCount<<" "<<standardAACount<<std::endl;
    };
    
}
