#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <memory>
#include <cstddef>
#include <ranges>

#include <multi_index_container.hpp>
#include <multi_index/sequenced_index.hpp>
#include <multi_index/ordered_index.hpp>
#include <multi_index/identity.hpp>
#include <multi_index/member.hpp>

#include "nlohmann/json.hpp"

namespace sylvanmats{

    struct element{
        int id;
        unsigned int atomic_number;
        std::string name;
        std::string symbol;
        double mass;
        element(int id, unsigned int atomic_number, std::string name, std::string symbol, double mass): id (id), atomic_number (atomic_number), name (name), symbol (symbol), mass (mass) {};
        element(const element& orig){
            id=orig.id;
            atomic_number=orig.atomic_number;
            name=orig.name;
            symbol=orig.symbol;
            mass=orig.mass;
        };
        virtual ~element() = default;
        bool operator<(const element& e)const{return id<e.id;}
    };

    struct name{};
    struct symbol{};

    typedef multi_index::multi_index_container<
      element,
      multi_index::indexed_by<
        // sort by element
        multi_index::ordered_unique<multi_index::identity<element> >,

        //multi_index::ordered_non_unique<multi_index::tag<name>,multi_index::member<element,std::string,&element::name> >,
        multi_index::ordered_non_unique<multi_index::tag<symbol>,multi_index::member<element,std::string,&element::symbol> >,

        // sort by less<int> on terminal
        multi_index::ordered_non_unique<multi_index::member<element,unsigned int,&element::atomic_number> >
      >
    > element_set;
    
    //typedef multi_index::index<element_set,name>::type element_set_by_name;
    typedef multi_index::index<element_set,symbol>::type element_set_by_symbol;

    class PeriodicTable {
        static PeriodicTable* periodicTable;
        PeriodicTable();
        PeriodicTable(const PeriodicTable& orig) = delete;
        virtual ~PeriodicTable();
        protected:
        nlohmann::json jin;
        element_set elementSet;
        element_set_by_symbol& symbolIndex;
        public:
        static PeriodicTable* getInstance();

        element index(std::string& symbol);
    };
}
