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
            id=orig.id;
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
        std::string dbLocation = (getenv("CIFIO_DB_LOCATION")!=NULL) ? getenv("CIFIO_DB_LOCATION"): "../db";
        std::filesystem::path filePath=dbLocation+"/aa-variants-v1.cif.gz";
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
        bool operator()(const std::string& comp_id, std::function<void(chem_comp_atom<double>& cca1, chem_comp_bond& ccb, chem_comp_atom<double>& cca2)> apply);
        unsigned int getNumberOfEntries(){return dataBlock.size();};
  };
}
