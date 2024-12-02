#include <cstdio>
#include <ranges>

#include "constitution/MOL2Populator.h"
#include "standards/ComponentStandards.h"

#include "antlr4-runtime.h"
#include "parsing/MOL2Lexer.h"
#include "parsing/MOL2Parser.h"
#include "reading/gz/GZReader.h"

#include "standards/AminoStandards.h"

namespace sylvanmats::constitution {
    void MOL2Populator::operator()(std::filesystem::path& filePath, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply){
        sylvanmats::reading::GZReader gzReader;
        gzReader(filePath, [&](std::istream& content){
         this->operator()(content, graph, apply);
        });
     }

    void MOL2Populator::operator()(std::istream& content, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply){
        standardAACount=0;
        standardCompCount=0;

        antlr4::ANTLRInputStream input(content);
        sylvanmats::MOL2Lexer lexer(&input);
        antlr4::CommonTokenStream tokens(&lexer);

        sylvanmats::MOL2Parser parser(&tokens);
        parser.setBuildParseTree(true);
        antlr4::tree::ParseTree* tree = parser.mol2();

        sylvanmats::standards::AminoStandards aminoStandards;
        sylvanmats::standards::ComponentStandards componentStandards;
        lemon::ListGraph::Node nCompA=lemon::INVALID;
        const std::string substructureRecordPath="/mol2/tripos_substructure_record";
        antlr4::tree::xpath::XPath xsubstructureRecordPath(&parser,substructureRecordPath);
        std::vector<antlr4::tree::ParseTree*> dataRecord=xsubstructureRecordPath.evaluate(tree);
        for(std::vector<antlr4::tree::ParseTree*>::iterator it=dataRecord.begin();it!=dataRecord.end();it++){
            if (sylvanmats::MOL2Parser::Tripos_substructure_recordContext* r=dynamic_cast<sylvanmats::MOL2Parser::Tripos_substructure_recordContext*>((*it))) {
                for(unsigned int substIndex=0;substIndex<r->subst_name().size();substIndex++){
                   lemon::ListGraph::Node n=graph.componentGraph.addNode();
                   graph.componentProperties[n].auth_mon_id.assign(r->subst_name(substIndex)->getText());
                   graph.componentProperties[n].pdb_seq_num=1;
                   graph.componentProperties[n].auth_seq_num=1;
                   nCompA=n;
                }
            }
        }
        
        const std::string recordPath="/mol2/tripos_atom_record";
        antlr4::tree::xpath::XPath xrecordPath(&parser,recordPath);
        dataRecord=xrecordPath.evaluate(tree);
        std::unordered_map<unsigned long long, lemon::ListGraph::Node> atomMap;
        for(std::vector<antlr4::tree::ParseTree*>::iterator it=dataRecord.begin();it!=dataRecord.end();it++){
            if (sylvanmats::MOL2Parser::Tripos_atom_recordContext* r=dynamic_cast<sylvanmats::MOL2Parser::Tripos_atom_recordContext*>((*it))) {
                for(unsigned int atomIndex=0;atomIndex<r->atom_id().size();atomIndex++){
                    lemon::ListGraph::Node n=graph.addNode();
                    graph.atomSites[n].id=std::strtol(r->atom_id(atomIndex)->getText().c_str(), nullptr, 10);
                    graph.atomSites[n].label_atom_id.assign(r->atom_name(atomIndex)->getText());
                    graph.atomSites[n].Cartn_x=std::strtod(r->x(atomIndex)->getText().c_str(), nullptr);
                    graph.atomSites[n].Cartn_y=std::strtod(r->y(atomIndex)->getText().c_str(), nullptr);
                    graph.atomSites[n].Cartn_z=std::strtod(r->z(atomIndex)->getText().c_str(), nullptr);
                    std::string::size_type index=r->atom_type(atomIndex)->getText().find_first_of('.');
                    graph.atomSites[n].type_symbol = (index==std::string::npos) ? r->atom_type(atomIndex)->getText() : r->atom_type(atomIndex)->getText().substr(0, index);
                    graph.atomSites[n].auth_seq_id=1;
                    graph.atomSites[n].label_comp_id.assign(r->atom_subst_name(atomIndex)->getText());
                    atomMap[atomIndex+1]=n;
                    graph.atomSites[n].pdbx_formal_charge=0;
                    graph.atomSites[n].partial_charge=(!r->charge().empty())? std::strtod(r->charge(atomIndex)->getText().c_str(), nullptr): 0.0;
                }
            }
        }
        const std::string bondRecordPath="/mol2/tripos_bond_record";
        antlr4::tree::xpath::XPath xbondRecordPath(&parser,bondRecordPath);
        dataRecord=xbondRecordPath.evaluate(tree);
        for(std::vector<antlr4::tree::ParseTree*>::iterator it=dataRecord.begin();it!=dataRecord.end();it++){
            if (sylvanmats::MOL2Parser::Tripos_bond_recordContext* r=dynamic_cast<sylvanmats::MOL2Parser::Tripos_bond_recordContext*>((*it))) {
                for(unsigned int bondIndex=0;bondIndex<r->origin_atom_id().size();bondIndex++){
                    
                    lemon::ListGraph::Node nSiteA=atomMap[std::strtol(r->origin_atom_id(bondIndex)->getText().c_str(), nullptr, 10)];
                    lemon::ListGraph::Node nSiteB=atomMap[std::strtol(r->target_atom_id(bondIndex)->getText().c_str(), nullptr, 10)];
                    lemon::ListGraph::Edge e=graph.addEdge(nSiteA, nSiteB);
                    if(r->bond_type(bondIndex)->getText().compare("am")==0){
                        graph.compBond[e].type=forcefield::BOND_SINGLE;
                    }
                    else if(r->bond_type(bondIndex)->getText().compare("ar")==0){
//                        graph.compBond[e].value_order=2;
                        graph.compBond[e].type=forcefield::BOND_AROMATIC;
                    }
                    else{
                        graph.compBond[e].value_order=std::strtol(r->bond_type(bondIndex)->getText().c_str(), nullptr, 10);
                        if(graph.compBond[e].value_order==3)
                            graph.compBond[e].type=forcefield::BOND_TRIPLE;
                        else if(graph.compBond[e].value_order==2)
                            graph.compBond[e].type=forcefield::BOND_DOUBLE;
                        else if(graph.compBond[e].value_order==1)
                            graph.compBond[e].type=forcefield::BOND_SINGLE;
                    }
                    
                }
            }
        }
        for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
             bool hit=false;
             for(lemon::ListGraph::NodeIt nCompA(graph.componentGraph); !hit && nCompA!=lemon::INVALID; ++nCompA){
//                 if(graph.atomSites[nSiteA].auth_comp_id.compare("U3S")==0 && graph.componentProperties[nCompA].auth_mon_id.compare("U3S")==0)
//                     std::cout<<"componentNavigation "<<graph.atomSites[nSiteA].label_atom_id<<" " << graph.atomSites[nSiteA].label_asym_id<<" =? "<<graph.componentProperties[nCompA].asym_id<<" "<<graph.atomSites[nSiteA].auth_seq_id<<" =? "<<graph.componentProperties[nCompA].pdb_seq_num<<" "<<graph.atomSites[nSiteA].pdbx_PDB_ins_code<<" =? "<<graph.componentProperties[nCompA].pdb_ins_code<<std::endl;
//                 if(graph.atomSites[nSiteA].auth_comp_id.compare(graph.componentProperties[nCompA].auth_mon_id)==0 && graph.atomSites[nSiteA].auth_seq_id==graph.componentProperties[nCompA].pdb_seq_num && graph.atomSites[nSiteA].label_asym_id.compare(graph.componentProperties[nCompA].asym_id)==0 && compareInsertionCode(graph.atomSites[nSiteA].pdbx_PDB_ins_code, graph.componentProperties[nCompA].pdb_ins_code)){
                     graph.componentNavigation.set(nSiteA, nCompA);
                     hit=true;
//                     if(graph.atomSites[nSiteA].auth_comp_id.compare("U3S")==0)std::cout<<"componentNavigation "<<graph.atomSites[nSiteA].label_atom_id<<" "<<graph.atomSites[nSiteA].auth_seq_id<<" "<<graph.atomSites[nSiteA].pdbx_PDB_ins_code<<std::endl;
//                 }
             }
         }
    }    
}

