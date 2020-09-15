#pragma once

#include <memory>
#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <ranges>
#include <vector>
#include <functional>

#include "zlib.h"

#include "reading/gz/GZReader.h"

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"

namespace sylvanmats::standards{

    template<typename T>
    struct chem_comp_atom{
        std::string comp_id;
        std::string atom_id;
        std::string alt_atom_id;
        std::string type_symbol;
        short charge;
        short pdbx_align;
        std::string pdbx_aromatic_flag;
        std::string pdbx_leaving_atom_flag;
        std::string pdbx_stereo_config;
        T model_Cartn_x;
        T model_Cartn_y;
        T model_Cartn_z;
        T pdbx_model_Cartn_x_ideal;
        T pdbx_model_Cartn_y_ideal;
        T pdbx_model_Cartn_z_ideal;
        unsigned long long pdbx_ordinal;
    };
    struct chem_comp_bond{
        std::string comp_id;
        std::string atom_id_1;
        std::string atom_id_2;
        short value_order=1;
        bool pdbx_aromatic_flag=false;
        bool pdbx_stereo_config=false;
        unsigned int pdbx_ordinal;
    }; 


  class AminoStandards{
    protected:
        std::shared_ptr<antlr4::ANTLRInputStream> input;
        std::shared_ptr<sylvanmats::CIFLexer> lexer;
        std::shared_ptr<antlr4::CommonTokenStream> tokens;
        std::shared_ptr<sylvanmats::CIFParser> parser;
        std::shared_ptr<antlr4::tree::xpath::XPath> xPath;
        antlr4::tree::ParseTree* tree;
        std::vector<antlr4::tree::ParseTree*> dataTag;
    public:
    AminoStandards(){
        std::string filePath="~/Downloads/aa-variants-v1.cif.gz";
    sylvanmats::reading::GZReader gzReader;
    gzReader(filePath, [&](std::istream& content){

        input=std::make_shared<antlr4::ANTLRInputStream>(content);
        lexer=std::make_shared<sylvanmats::CIFLexer>(input.get());
        tokens=std::make_shared<antlr4::CommonTokenStream>(lexer.get());

        parser=std::make_shared<sylvanmats::CIFParser>(tokens.get());
        parser->setBuildParseTree(true);
        tree = parser->cif();

        //std::cout << tree->toStringTree(&parser) << std::endl << std::endl;

        const std::string thePath="/cif/*/*/tag";
        xPath=std::make_shared<antlr4::tree::xpath::XPath>(parser.get(), thePath);
        dataTag=xPath->evaluate(tree);
        std::cout<<"dataTag.size() "<<dataTag.size()<<std::endl;
        const std::string thePath2="/cif/dataBlock/dataItems/tag";
        xPath=std::make_shared<antlr4::tree::xpath::XPath>(parser.get(), thePath2);
        std::cout<<"s? "<<xPath->evaluate(tree).size()<<std::endl;
    });
    };

    AminoStandards(const AminoStandards& orig) = delete;
    virtual ~AminoStandards() = default;

    public:
        bool operator()(std::string_view& comp_id, std::function<void(chem_comp_bond ccb)> apply){
            bool ret=false;
            for(std::vector<antlr4::tree::ParseTree*>::iterator it=dataTag.begin();!ret && it!=dataTag.end();it++){
                    //std::cout<<" "<<(*it)->toStringTree()<<std::endl;
                bool atomSites=false;
                if (sylvanmats::CIFParser::TagContext* r=dynamic_cast<sylvanmats::CIFParser::TagContext*>((*it))) {
                    if(r->getText().compare("_chem_comp.id")==0){
                        if(sylvanmats::CIFParser::DataItemsContext* dic=dynamic_cast<sylvanmats::CIFParser::DataItemsContext*>(r->parent))
                        if(dic->value()->getText().compare(comp_id)==0){
std::cout<<" "<<comp_id<<" "<<dataTag.size()<<std::endl;
                            sylvanmats::CIFParser::DataBlockContext* db=dynamic_cast<sylvanmats::CIFParser::DataBlockContext*>(dic->parent);
                            auto oi=db->dataItems();
                            for(sylvanmats::CIFParser::DataItemsContext* l: oi | std::views::filter([](sylvanmats::CIFParser::DataItemsContext* di){ return di->loop()!=nullptr && di->loop()->loopHeader()->tag().size()>0 && di->loop()->loopHeader()->tag(0)->getText().rfind("_chem_comp_bond.", 0) == 0; })){
                                unsigned int columnCount=0;
                                 chem_comp_bond ccb;
                                 for(unsigned int valueIndex=0;valueIndex<l->loop()->loopBody()->value().size();valueIndex++){
                                     std::cout<<l->loop()->loopBody()->value(valueIndex)->getText()<<" ";
                                     switch(columnCount){
                                          case 0:
                                              ccb.comp_id=l->loop()->loopBody()->value(valueIndex)->getText();
                                          break;
                                          case 1:
                                              ccb.atom_id_1=l->loop()->loopBody()->value(valueIndex)->getText();
                                          break;
                                          case 2:
                                              ccb.atom_id_2=l->loop()->loopBody()->value(valueIndex)->getText();
                                          break;
                                          case 3:
                                            if(l->loop()->loopBody()->value(valueIndex)->getText().compare("DOUB")==0){
                                              ccb.value_order=2;
                                            }
                                            else if(l->loop()->loopBody()->value(valueIndex)->getText().compare("TRIP")==0){
                                              ccb.value_order=3;
                                            }
                                            else ccb.value_order=1;
                                          break;
                                     }
                                     columnCount++;
                                     if(valueIndex % (l->loop()->loopHeader()->tag().size()) == l->loop()->loopHeader()->tag().size()-1){
                                         std::cout<<std::endl;
                                         apply(ccb);
                                         columnCount=0;
                                         ret=true;
                                     }
                                  }
                            }
                            //dic->loop()
                            break;
                        }
                    }
                }
            }
            return ret;
        };
  };
}
