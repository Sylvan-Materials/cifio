#include "lattice/Populator.h"

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"
#include "reading/gz/GZReader.h"

namespace sylvanmats::lattice {


    void Populator::operator()(std::string& filePath, sylvanmats::lattice::Graph& graph, std::function<void(sylvanmats::lattice::Graph& graph)> apply){
        sylvanmats::reading::GZReader gzReader;
        gzReader(filePath, [&](std::istream& content){

            antlr4::ANTLRInputStream input(content);
            sylvanmats::CIFLexer lexer(&input);
            antlr4::CommonTokenStream tokens(&lexer);

            sylvanmats::CIFParser parser(&tokens);
            parser.setBuildParseTree(true);
            antlr4::tree::ParseTree* tree = parser.cif();

            const std::string blockPath="/cif/dataBlock";
            antlr4::tree::xpath::XPath xblockPath(&parser, blockPath);
            std::vector<antlr4::tree::ParseTree*> dataBlock=xblockPath.evaluate(tree);
            for(std::vector<antlr4::tree::ParseTree*>::iterator it=dataBlock.begin();it!=dataBlock.end();it++){
                if (sylvanmats::CIFParser::DataBlockContext* r=dynamic_cast<sylvanmats::CIFParser::DataBlockContext*>((*it))) {
                    auto oi=r->dataItems();
                    for(sylvanmats::CIFParser::DataItemsContext* cdi: oi | std::views::filter([](sylvanmats::CIFParser::DataItemsContext* di){ return di->tag()!=nullptr && di->tag()->getText().rfind("\n_cell_", 0) == 0; })){
                        if(cdi->tag()->getText().compare("\n_cell_length_a")==0)graph.cell.length_a=std::strtod(cdi->value()->getText().c_str(), nullptr);
                        else if(cdi->tag()->getText().compare("\n_cell_length_b")==0)graph.cell.length_b=std::strtod(cdi->value()->getText().c_str(), nullptr);
                        else if(cdi->tag()->getText().compare("\n_cell_length_c")==0)graph.cell.length_c=std::strtod(cdi->value()->getText().c_str(), nullptr);
                        else if(cdi->tag()->getText().compare("\n_cell_angle_alpha")==0)graph.cell.angle_alpha=std::strtod(cdi->value()->getText().c_str(), nullptr);
                        else if(cdi->tag()->getText().compare("\n_cell_angle_beta")==0)graph.cell.angle_beta=std::strtod(cdi->value()->getText().c_str(), nullptr);
                        else if(cdi->tag()->getText().compare("\n_cell_angle_gamma")==0)graph.cell.angle_gamma=std::strtod(cdi->value()->getText().c_str(), nullptr);
                    }
                    for(sylvanmats::CIFParser::DataItemsContext* cdi: oi | std::views::filter([](sylvanmats::CIFParser::DataItemsContext* di){ return di->tag()!=nullptr && di->tag()->getText().rfind("\n_symmetry_", 0) == 0; })){
                        if(cdi->tag()->getText().compare("\n_symmetry_space_group_name_H-M")==0){
                            if(cdi->value()->singleQuotedString()!=nullptr) graph.symmetry.space_group_name_H_M=cdi->value()->singleQuotedString()->getText().substr(1, cdi->value()->singleQuotedString()->getText().size()-2);
                                else graph.symmetry.space_group_name_H_M=cdi->value()->getText();
                        }
                        //else if(cdi->tag()->getText().compare("_symmetry_Int_Tables_number")==0)graph.symmetry.Int_Tables_number=std::strtol(cdi->value()->getText().c_str(), nullptr, 10);
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
                    for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return once && tag->getText().rfind("_atom_site_", 0) == 0; })){
                        once=false;
                        unsigned int columnCount=0;
                        lemon::ListGraph::Node n=graph.addNode();
                    for(unsigned int valueIndex=0;valueIndex<r->loopBody()->value().size();valueIndex++){
                        switch(columnCount){
                             case 0:
                                 graph.atomSites[n].type_symbol =r->loopBody()->value(valueIndex)->getText();
                             break;
                             case 1:
                                 graph.atomSites[n].label =r->loopBody()->value(valueIndex)->getText();
                             break;
                             case 2:
                                 graph.atomSites[n].fract_x =std::strtod(r->loopBody()->value(valueIndex)->getText().c_str(), nullptr);
                             break;
                             case 3:
                                 graph.atomSites[n].fract_y =std::strtod(r->loopBody()->value(valueIndex)->getText().c_str(), nullptr);
                             break;
                             case 4:
                                 graph.atomSites[n].fract_z =std::strtod(r->loopBody()->value(valueIndex)->getText().c_str(), nullptr);
                             break;
                             case 5:
                                 graph.atomSites[n].U_iso_or_equiv =std::strtod(r->loopBody()->value(valueIndex)->getText().c_str(), nullptr);
                             break;
                             case 6:
                                 graph.atomSites[n].adp_type =r->loopBody()->value(valueIndex)->getText();
                             break;
                             case 7:
                                 graph.atomSites[n].occupancy =std::strtod(r->loopBody()->value(valueIndex)->getText().c_str(), nullptr);
                             break;
                        }
                        columnCount++;
                        if((valueIndex % r->loopHeader()->tag().size() == r->loopHeader()->tag().size()-1) || valueIndex==r->loopBody()->value().size()-1){
                            columnCount=0;
                            if(valueIndex<r->loopBody()->value().size()-1)graph.addNode();
                        }
                    }
                    }

                }
            }
            
        });
    }
}
