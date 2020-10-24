#include <ranges>

#include "density/Populator.h"

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"
#include "reading/gz/GZReader.h"

namespace sylvanmats::density {


    void Populator::operator()(std::filesystem::path& filePath){
        sylvanmats::reading::GZReader gzReader;
        gzReader(filePath, [&](std::istream& content){

            antlr4::ANTLRInputStream input(content);
            sylvanmats::CIFLexer lexer(&input);
            antlr4::CommonTokenStream tokens(&lexer);

            sylvanmats::CIFParser parser(&tokens);
            parser.setBuildParseTree(true);
            antlr4::tree::ParseTree* tree = parser.cif();

            const std::string thePath="/cif/dataBlock/dataItems/loop";
            antlr4::tree::xpath::XPath xPath(&parser, thePath);
            std::vector<antlr4::tree::ParseTree*> dataBlockLoop=xPath.evaluate(tree);
            for(std::vector<antlr4::tree::ParseTree*>::iterator it=dataBlockLoop.begin();it!=dataBlockLoop.end();it++){
                if (sylvanmats::CIFParser::LoopContext* r=dynamic_cast<sylvanmats::CIFParser::LoopContext*>((*it))) {
                    bool once=true;
                    auto tags=r->loopHeader()->tag();
                    for(sylvanmats::CIFParser::TagContext* t: tags | std::views::filter([&once](sylvanmats::CIFParser::TagContext* tag){ return once && tag->getText().rfind("\n_refln.", 0) == 0; })){
                        once=false;
                        unsigned int columnCount=0;
                        structureFactors.push_back(_refln<double>());
                    std::vector<sylvanmats::CIFParser::ValueContext *> values=r->loopBody()->value();
                    for(unsigned int valueIndex=0;valueIndex<values.size();valueIndex++){
                        switch(columnCount){
                             case 0:
                                 structureFactors.back().wavelength_id =std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                             break;
                             case 1:
                                 structureFactors.back().crystal_id =std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                             break;
                             case 2:
                                 structureFactors.back().scale_group_code =std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                             break;
                             case 3:
                                 structureFactors.back().index_h =std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                             break;
                             case 4:
                                 structureFactors.back().index_k =std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                             break;
                             case 5:
                                 structureFactors.back().index_l =std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                             break;
                             case 6:
                                 structureFactors.back().status =values[valueIndex]->getText();
                             break;
                             case 7:
                                 structureFactors.back().F_meas_au =std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                             break;
                             case 8:
                                 structureFactors.back().F_meas_sigma_au =std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                             break;
                             case 9:
                                 structureFactors.back().F_calc =std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                             break;
                             case 10:
                                 structureFactors.back().phase_calc =std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                             break;
                             case 11:
                                 structureFactors.back().fom =std::strtod(values[valueIndex]->getText().c_str(), nullptr);
                             break;
                        }
                        columnCount++;
                        if((valueIndex % r->loopHeader()->tag().size() == r->loopHeader()->tag().size()-1) || valueIndex==r->loopBody()->value().size()-1){
                            columnCount=0;
                            if(valueIndex<r->loopBody()->value().size()-1)structureFactors.push_back(_refln<double>());
                        }
                    }
                    }

                }
            }
            
        });
    }
}