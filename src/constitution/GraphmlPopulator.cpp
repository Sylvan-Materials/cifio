#include <cstdio>
#include <ranges>

#include "constitution/GraphmlPopulator.h"

#include "reading/gz/GZReader.h"

#include "io/xml/Binder.h"

namespace sylvanmats::constitution {
    void GraphmlPopulator::operator()(std::filesystem::path& filePath, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply){
        sylvanmats::reading::GZReader gzReader;
        gzReader(filePath, [&](std::istream& content){
         this->operator()(content, graph, apply);
        });
     }

    void GraphmlPopulator::operator()(std::istream& content, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply){
        sylvanmats::io::xml::Binder xmlReaper("../../cifio/db/oxygen_fragments.graphml", "");
        xmlReaper([&xmlReaper, &graph](std::u16string& utf16, std::vector<std::pair<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>, std::vector<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>>>>& dag){
            if(dag.size()>0){
                std::unordered_map<std::u16string, lemon::ListGraph::Node> nodeIDMap;
                std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
                for(std::vector<std::pair<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>, std::vector<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>>>>::iterator it=dag.begin();it!=dag.end();it++){
                    if(utf16.substr(std::get<1>((*it).first), std::get<5>((*it).first)-std::get<1>((*it).first)).starts_with(u"<graph ")){
//                std::cout<<std::get<0>((*it).first)<<" "<<dag[std::get<0>((*it).first)].second.size()<<" "<<cv.to_bytes(utf16.substr(std::get<1>((*it).first), std::get<5>((*it).first)-std::get<1>((*it).first)))<<" "<<(*it).second.size()<<" "<<cv.to_bytes(utf16.substr(std::get<1>((*it).second.back()), std::get<5>((*it).second.back())-std::get<1>((*it).second.back())))<<std::endl;
                    for(auto itS : dag[std::get<0>((*it).first)].second | std::views::filter([&utf16](std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>& di){ return utf16.substr(std::get<1>(di), std::get<5>(di)-std::get<1>(di)).starts_with(u"<node "); })){
                        lemon::ListGraph::Node n=graph.addNode();
                        std::u16string&& idValue=xmlReaper.findAttribute(u"id", std::get<1>(itS), std::get<5>(itS));
                        nodeIDMap[idValue]=n;
                        for(auto itD : dag[std::get<0>(itS)].second | std::views::filter([&utf16](std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>& di){ return utf16.substr(std::get<1>(di), std::get<5>(di)-std::get<1>(di)).starts_with(u"<data "); })){
//                            std::cout<<"? "<<(n!=lemon::INVALID)<<" "<<(std::get<5>(itD)+1)<<" "<<(dag[std::get<0>(itD)].second.size())<<" "<<std::get<1>(dag[std::get<0>(itD)].second.back())<<std::endl;
                            std::u16string name=utf16.substr(std::get<5>(itD)+1, std::get<1>(dag[std::get<0>(itD)].second.back())-std::get<5>(itD)-1);
                    graph.atomSites[n].type_symbol=std::string(cv.to_bytes(name));
//                std::cout<<"\t\t"<<std::get<5>(itD)<<" "<<std::get<1>(dag[std::get<0>(itD)].second.back())<<" "<<dag[std::get<0>(itD)].second.size()<<" "<<graph.atomSites[n].type_symbol<<std::endl;
//                std::cout<<" "<<cv.to_bytes(utf16.substr(std::get<1>(dag[std::get<0>(itD)].first), std::get<5>(dag[std::get<0>(itD)].first)-std::get<1>(dag[std::get<0>(itD)].first)))<<std::endl;
//                std::cout<<" "<<cv.to_bytes(utf16.substr(std::get<1>(dag[std::get<0>(itD)].second.back()), std::get<5>(dag[std::get<0>(itD)].second.back())-std::get<1>(dag[std::get<0>(itD)].second.back())))<<std::endl;
                        }
                    }
                    }
                }
                for(std::vector<std::pair<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>, std::vector<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>>>>::iterator it=dag.begin();it!=dag.end();it++){
                    if(utf16.substr(std::get<1>((*it).first), std::get<5>((*it).first)-std::get<1>((*it).first)).starts_with(u"<graph ")){
                    for(auto itS : dag[std::get<0>((*it).first)].second | std::views::filter([&utf16](std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>& di){ return utf16.substr(std::get<1>(di), std::get<5>(di)-std::get<1>(di)).starts_with(u"<edge "); })){
                        std::u16string&& sourceValue=xmlReaper.findAttribute(u"source", std::get<1>(itS), std::get<5>(itS));
                        std::u16string&& targetValue=xmlReaper.findAttribute(u"target", std::get<1>(itS), std::get<5>(itS));
                        lemon::ListGraph::Edge e=graph.addEdge(nodeIDMap[sourceValue], nodeIDMap[targetValue]);
                        for(auto itD : dag[std::get<0>(itS)].second | std::views::filter([&utf16](std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>& di){ return utf16.substr(std::get<1>(di), std::get<5>(di)-std::get<1>(di)).starts_with(u"<data "); })){
                            std::u16string name=utf16.substr(std::get<5>(itD)+1, std::get<1>(dag[std::get<0>(itD)].second.back())-std::get<5>(itD)-1);
                            graph.compBond[e].atom_id_1=cv.to_bytes(sourceValue);
                            graph.compBond[e].atom_id_2=cv.to_bytes(targetValue);
                            graph.compBond[e].value_order=std::strtol(cv.to_bytes(name).c_str(), nullptr, 10);
                        }
                        
                    }
                    }
                }
            }
        });
        apply(graph);
    }    
}
