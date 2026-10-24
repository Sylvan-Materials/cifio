#include <string>
#include <map>
#include <thread>
#include <cstddef>
#include <typeinfo>
#include "PeriodicTable.h"

namespace sylvanmats{

    PeriodicTable* PeriodicTable::periodicTable=nullptr;
    //PeriodicTable* PeriodicTable::jin=nullptr;
    PeriodicTable::PeriodicTable() : symbolIndex(elementSet.get<symbol>()) {
        std::filesystem::path path="../cpp_modules/Periodic-Table-JSON/PeriodicTableJSON.json";
        std::ifstream is(path);
        std::string jsonContent((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        jsonBinder(jsonContent);
//        jsonBinder.display();
        is.close();
    }
    
    PeriodicTable* PeriodicTable::PeriodicTable::getInstance(){
        if(periodicTable==nullptr)
            periodicTable=new PeriodicTable();
        return periodicTable;
    };

    element PeriodicTable::index(std::string& symbol) {
        element_set_by_symbol::iterator it=symbolIndex.find(symbol);
        if(it==symbolIndex.end()){
            sylvanmats::io::json::Path jpName;
            jpName["elements"]["*"]["*"]["symbol"]==symbol;
            size_t currentSetSize=elementSet.size();
            jsonBinder(jpName, [&](std::string_view key, const sylvanmats::io::json::JsonValue& v){
//                std::cout<<"key "<<key<<std::endl;
                if(key.compare("name")==0){
                    if(currentSetSize==elementSet.size())elementSet.insert(element(elementSet.size()));
                    if(auto pVal = std::get_if<std::string_view>(&v)) {
                        elementSet.begin()->name.assign(*pVal);
                    }
                }
                else if(key.compare("number")==0){
                    if(currentSetSize==elementSet.size())elementSet.insert(element(elementSet.size()));
                    if(auto pVal = std::get_if<long>(&v)) {
                        elementSet.begin()->atomic_number=*pVal;
                    }
                }
                else if(key.compare("symbol")==0){
                    if(currentSetSize==elementSet.size())elementSet.insert(element(elementSet.size()));
                    if(auto pVal = std::get_if<std::string_view>(&v)) {
                        elementSet.begin()->symbol.assign(*pVal);
                    }
                }
                else if(key.compare("atomic_mass")==0){
                    if(currentSetSize==elementSet.size())elementSet.insert(element(elementSet.size()));
                    if(auto pVal = std::get_if<double>(&v)) {
                        elementSet.begin()->mass=*pVal;
                    }
                }
            });

                it=symbolIndex.find(symbol);
        }
        return static_cast<element>(*it);
    }
    
}
