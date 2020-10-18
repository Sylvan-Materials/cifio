#pragma once

#include <memory>
#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <filesystem>
#include <ranges>
#include <vector>
#include <functional>
#include <chrono>

#include "zlib.h"

#include "reading/gz/GZReader.h"

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"

#include <multi_index_container.hpp>
#include <multi_index/sequenced_index.hpp>
#include <multi_index/ordered_index.hpp>
#include <multi_index/identity.hpp>
#include <multi_index/member.hpp>

namespace sylvanmats::standards{

    template<typename T>
    struct chem_comp_atom{
        std::string comp_id;
        std::string atom_id;
        std::string alt_atom_id;
        std::string type_symbol;
        char8_t charge;
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

    struct standard{
        int id;
        std::string name;
        mutable std::vector<chem_comp_atom<double>> chemCompAtoms;
        mutable std::vector<chem_comp_bond> chemCombonds;
        long terminal;
        standard(int id, std::string name, std::vector<chem_comp_atom<double>>& chemCompAtoms, std::vector<chem_comp_bond>& chemCombonds, long terminal=0): id (id), name (name), chemCompAtoms(chemCompAtoms), chemCombonds (chemCombonds), terminal (terminal) {};
        standard(const standard& orig){
            name=orig.name;
            chemCompAtoms=orig.chemCompAtoms;
            chemCombonds=orig.chemCombonds;
            terminal=orig.terminal;
        };
        virtual ~standard() = default;
        bool operator<(const standard& e)const{return id<e.id;}
    };

    struct name{};

    typedef multi_index::multi_index_container<
      standard,
      multi_index::indexed_by<
        // sort by standard
        multi_index::ordered_unique<multi_index::identity<standard> >,

        multi_index::ordered_non_unique<multi_index::tag<name>,multi_index::member<standard,std::string,&standard::name> >,

        // sort by less<int> on terminal
        multi_index::ordered_non_unique<multi_index::member<standard,long,&standard::terminal> >
      >
    > standard_set;
    
    typedef multi_index::index<standard_set,name>::type standard_set_by_name;

  class AminoStandards{
    protected:
        std::shared_ptr<antlr4::ANTLRInputStream> input;
        std::shared_ptr<sylvanmats::CIFLexer> lexer;
        std::shared_ptr<antlr4::CommonTokenStream> tokens;
        std::shared_ptr<sylvanmats::CIFParser> parser;
        std::shared_ptr<antlr4::tree::xpath::XPath> xPath;
        antlr4::tree::ParseTree* tree;
        std::vector<antlr4::tree::ParseTree*> dataBlock;
        standard_set aminoStandardSet;
        standard_set_by_name& nameIndex;
    public:
    AminoStandards() : nameIndex (aminoStandardSet.get<name>()) {
        std::filesystem::path filePath="../db/aa-variants-v1.cif.gz";
    sylvanmats::reading::GZReader gzReader;
    gzReader(filePath, [&](std::istream& content){

        input=std::make_shared<antlr4::ANTLRInputStream>(content);
        lexer=std::make_shared<sylvanmats::CIFLexer>(input.get());
        tokens=std::make_shared<antlr4::CommonTokenStream>(lexer.get());

        parser=std::make_shared<sylvanmats::CIFParser>(tokens.get());
        //parser->setBuildParseTree(true);
        tree = parser->cif();

        //std::cout << tree->toStringTree(&parser) << std::endl << std::endl;

        const std::string thePath="/cif/dataBlock";
        xPath=std::make_shared<antlr4::tree::xpath::XPath>(parser.get(), thePath);
        dataBlock=xPath->evaluate(tree);
    });
    };

    AminoStandards(const AminoStandards& orig) = delete;
    virtual ~AminoStandards() = default;

    public:
        bool operator()(const std::string& comp_id, std::function<void(chem_comp_atom<double>& cca1, chem_comp_bond& ccb, chem_comp_atom<double>& cca2)> apply){
            bool ret=false;
                     standard_set_by_name::iterator it=nameIndex.find(comp_id);
                   //std::cout<<comp_id<<" (it!=nameIndex.end())."<<(it!=nameIndex.end())<<std::endl;
            /*if(it!=nameIndex.end()){
                for(std::vector<chem_comp_bond>::iterator itStd=(*it).chemCombonds.begin();itStd!=(*it).chemCombonds.end();itStd++){
                    std::vector<chem_comp_atom<double>>::iterator cca1=std::find_if((*it).chemCompAtoms.begin(), (*it).chemCompAtoms.end(), [&](chem_comp_atom<double>& cca){return (*itStd).atom_id_1.compare(cca.atom_id)==0;});
                    std::vector<chem_comp_atom<double>>::iterator cca2=std::find_if((*it).chemCompAtoms.begin(), (*it).chemCompAtoms.end(), [&](chem_comp_atom<double>& cca){return (*itStd).atom_id_2.compare(cca.atom_id)==0;});
                    apply((*cca1), (*itStd), (*cca2));
                }
                return true;
            }*/
            for(std::vector<antlr4::tree::ParseTree*>::iterator itDB=dataBlock.begin();!ret && itDB!=dataBlock.end();itDB++){
                    //std::cout<<" "<<(*it)->toStringTree()<<std::endl;
                bool atomSites=false;
                if (sylvanmats::CIFParser::DataBlockContext* r=dynamic_cast<sylvanmats::CIFParser::DataBlockContext*>((*itDB))) {
                    if(r->dataItems().size()>0 && r->dataItems(0)->tag()!=nullptr &&  r->dataItems(0)->tag()->getText().compare("\n_chem_comp.id")==0){
                        if(r->dataItems(0)->value()->getText().compare(comp_id)==0){
                        //std::cout<<""<<comp_id<<" ? "<<r->dataItems(0)->value()->getText()<<std::endl;
                            auto oi=r->dataItems();
                            std::vector<chem_comp_atom<double>> chemCompAtoms;
                            for(sylvanmats::CIFParser::DataItemsContext* l: oi | std::views::filter([](sylvanmats::CIFParser::DataItemsContext* di){ return di->loop()!=nullptr && di->loop()->loopHeader()->tag().size()>0 && di->loop()->loopHeader()->tag(0)->getText().rfind("\n_chem_comp_atom.", 0) == 0; })){
                                unsigned int columnCount=0;
                                 chemCompAtoms.push_back(chem_comp_atom<double>());
                                std::vector<sylvanmats::CIFParser::ValueContext *> values=l->loop()->loopBody()->value();
                                 for(unsigned int valueIndex=0;valueIndex<values.size();valueIndex++){
                                     //std::cout<<values[valueIndex]->getText()<<" ";
                                     switch(columnCount){
                                          case 0:
                                              chemCompAtoms.back().comp_id.assign(values[valueIndex]->getText());
                                          break;
                                          case 1:
                                              chemCompAtoms.back().atom_id.assign(values[valueIndex]->getText());
                                          break;
                                          case 2:
                                              chemCompAtoms.back().alt_atom_id.assign(values[valueIndex]->getText());
                                          break;
                                          case 3:
                                              chemCompAtoms.back().type_symbol.assign(values[valueIndex]->getText());
                                          break;
                                          case 4:
                                              chemCompAtoms.back().charge=std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                                          break;
                                          case 6:
                                              chemCompAtoms.back().pdbx_aromatic_flag.assign(values[valueIndex]->getText());
                                          break;
                                          case 7:
                                              chemCompAtoms.back().pdbx_leaving_atom_flag.assign(values[valueIndex]->getText());
                                          break;
                                          case 8:
                                              chemCompAtoms.back().pdbx_stereo_config.assign(values[valueIndex]->getText());
                                          break;
                                     }
                                     columnCount++;
                                     if(valueIndex % (l->loop()->loopHeader()->tag().size()) == l->loop()->loopHeader()->tag().size()-1){
                                         //std::cout<<std::endl;
                                         columnCount=0;
                                        chemCompAtoms.push_back(chem_comp_atom<double>());
                                     }
                                  }
                            }
                            std::vector<chem_comp_bond> chemCombonds;
                            for(sylvanmats::CIFParser::DataItemsContext* l: oi | std::views::filter([](sylvanmats::CIFParser::DataItemsContext* di){ return di->loop()!=nullptr && di->loop()->loopHeader()->tag().size()>0 && di->loop()->loopHeader()->tag(0)->getText().rfind("\n_chem_comp_bond.", 0) == 0; })){
                                unsigned int columnCount=0;
                                 chemCombonds.push_back(chem_comp_bond());
                                 std::vector<sylvanmats::CIFParser::ValueContext *> values=l->loop()->loopBody()->value();
                                 for(unsigned int valueIndex=0;valueIndex<l->loop()->loopBody()->value().size();valueIndex++){
                                     //std::cout<<values[valueIndex]->getText()<<" ";
                                     switch(columnCount){
                                          case 0:
                                              chemCombonds.back().comp_id.assign(values[valueIndex]->getText());
                                          break;
                                          case 1:
                                              chemCombonds.back().atom_id_1.assign(values[valueIndex]->getText());
                                          break;
                                          case 2:
                                              chemCombonds.back().atom_id_2.assign(values[valueIndex]->getText());
                                          break;
                                          case 3:
                                            if(values[valueIndex]->getText().compare("DOUB")==0){
                                              chemCombonds.back().value_order=2;
                                            }
                                            else if(values[valueIndex]->getText().compare("TRIP")==0){
                                              chemCombonds.back().value_order=3;
                                            }
                                            else chemCombonds.back().value_order=1;
                                          break;
                                     }
                                     columnCount++;
                                     if(valueIndex % (l->loop()->loopHeader()->tag().size()) == l->loop()->loopHeader()->tag().size()-1){
                                         //std::cout<<std::endl;
                                         std::vector<chem_comp_atom<double>>::iterator cca1=std::find_if(std::begin(chemCompAtoms), std::end(chemCompAtoms), [&](chem_comp_atom<double>& cca){return chemCombonds.back().atom_id_1.compare(cca.atom_id)==0;});
                                         std::vector<chem_comp_atom<double>>::iterator cca2=std::find_if(std::begin(chemCompAtoms), std::end(chemCompAtoms), [&](chem_comp_atom<double>& cca){return chemCombonds.back().atom_id_2.compare(cca.atom_id)==0;});
                                         apply((*cca1), chemCombonds.back(), (*cca2));
                                         columnCount=0;
                                         ret=true;
                                     }
                                  }
                            }
                            aminoStandardSet.insert(standard(aminoStandardSet.size(), static_cast<std::string>(comp_id), chemCompAtoms, chemCombonds));
                            //dic->loop()
                            break;
                        }
                    }
                }
            }
            return ret;
        };

        unsigned int getNumberOfEntries(){return dataBlock.size();};
  };
}
