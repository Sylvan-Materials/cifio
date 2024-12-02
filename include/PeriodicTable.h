#pragma once

#include <cstdio>
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

#include "io/json/Binder.h"

namespace sylvanmats{

    struct element{
        int id;
        mutable unsigned int atomic_number;
        mutable std::string name;
        mutable std::string symbol;
        mutable double mass;
        element() = default;
        element(int id) :  id (id) {};
        element(int id, unsigned int atomic_number, std::string name, std::string symbol, double mass): id (id), atomic_number (atomic_number), name (name), symbol (symbol), mass (mass) {};
        element(const element& orig) = default;
        virtual ~element() = default;
        bool operator<(const element& e)const{return id<e.id;}
    };

    struct name{};
    struct symbol{};

    using element_set =  multi_index::multi_index_container<
      element,
      multi_index::indexed_by<
        // sort by element
        multi_index::ordered_unique<multi_index::identity<element> >,

        //multi_index::ordered_non_unique<multi_index::tag<name>,multi_index::member<element,std::string,&element::name> >,
        multi_index::ordered_non_unique<multi_index::tag<symbol>,multi_index::member<element,std::string,&element::symbol> >,

        // sort by less<int> on terminal
        multi_index::ordered_non_unique<multi_index::member<element,unsigned int,&element::atomic_number> >
      >
    >;
    
    using element_set_by_symbol = multi_index::index<element_set,symbol>::type;

    class PeriodicTable {
        static PeriodicTable* periodicTable;
        PeriodicTable();
        PeriodicTable(const PeriodicTable& orig) = delete;
        virtual ~PeriodicTable() = default;
        protected:
        sylvanmats::io::json::Binder jsonBinder;
        element_set elementSet;
        element_set_by_symbol& symbolIndex;
        public:
        static PeriodicTable* getInstance();

        element index(std::string& symbol);
    };
}
