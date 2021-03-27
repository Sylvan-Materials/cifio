#pragma once

#include <stdio.h>
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

#include "mio/mmap.hpp"
#include "nlohmann/json.hpp"

#include "standards/AminoStandards.h"

namespace sylvanmats::standards{

    /**
    * ftp://ftp.wwpdb.org/pub/pdb/data/monomers/components.cif.gz
    **/
    class ComponentStandards{
    protected:
        std::filesystem::path path="../db/components.cif";
        nlohmann::json jin;
        standard_set componentStandardSet;
        standard_set_by_name& nameIndex;
    public:
        ComponentStandards() : nameIndex (componentStandardSet.get<name>()) {
            std::string dbLocation = (getenv("CIFIO_DB_LOCATION")!=NULL) ? getenv("CIFIO_DB_LOCATION"): "../db";
            path=dbLocation+"/components.cif";
            path.replace_extension(".json");
            std::ifstream file(path);
            std::string jsonContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//std::cout<<"jsonContent "<<jsonContent.substr(0, 50)<<std::endl;
            jin=nlohmann::json::parse(jsonContent);
            file.close();
            path.replace_extension(".cif");
        };
    ComponentStandards(const ComponentStandards& orig) = delete;
    virtual ~ComponentStandards() = default;

    public:
        bool operator()(const std::vector<std::string>& comp_id, std::function<void(chem_comp_atom<double>& cca1, chem_comp_bond& ccb, chem_comp_atom<double>& cca2)> apply);            
    };
}

