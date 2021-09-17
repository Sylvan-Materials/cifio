#include <string>
#include <map>
#include <thread>
#include <cstddef>
#include <typeinfo>
#include "PeriodicTable.h"

#include "nlohmann/json.hpp"

namespace sylvanmats{

    PeriodicTable* PeriodicTable::periodicTable=nullptr;
    //PeriodicTable* PeriodicTable::jin=nullptr;
    PeriodicTable::PeriodicTable() : symbolIndex(elementSet.get<symbol>()) {
        std::filesystem::path path="../cpp_modules/Periodic-Table-JSON/PeriodicTableJSON.json";
        std::ifstream file(path);
        std::string jsonContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        jin=nlohmann::json::parse(jsonContent);
        file.close();
    }
    
    PeriodicTable::~PeriodicTable(){
    };

    PeriodicTable* PeriodicTable::PeriodicTable::getInstance(){
        if(periodicTable==nullptr)
            periodicTable=new PeriodicTable();
        return periodicTable;
    };

    element PeriodicTable::index(std::string& symbol) {
        element_set_by_symbol::iterator it=symbolIndex.find(symbol);
        if(it==symbolIndex.end()){
//std::cout << typeid(jin["elements"]).name() << '\n';
            for(auto it : jin["elements"]){ // | std::views::filter([&symbol](std::map& e){ std::this_thread::yield();return e["symbol"].compare(symbol)==0; })
                //std::cout << typeid((it)).name() << '\n';
                //std::cout<<" "<<(it)["number"].get<int>()<<" "<<(it)["name"].get<std::string>()<<" "<<(it)["symbol"].get<std::string>()<<" "<<(it)["atomic_mass"].get<double>()<<std::endl;
                if((it)["symbol"].get<std::string>().compare(symbol)==0){
                    elementSet.insert(element(elementSet.size(), (it)["number"].get<unsigned int>(), (it)["name"].get<std::string>(), (it)["symbol"].get<std::string>(), (it)["atomic_mass"].get<double>()));
                    break;
                }
            }
            it=symbolIndex.find(symbol);
        }
        return static_cast<element>(*it);
    }
    
}
