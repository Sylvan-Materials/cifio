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
        xmlReaper([&xmlReaper, &graph](std::u16string& utf16, std::vector<std::pair<sylvanmats::io::xml::tag_indexer, std::vector<sylvanmats::io::xml::tag_indexer>>>& dag){
            if(dag.size()>0){
                std::unordered_map<std::u16string, lemon::ListGraph::Node> nodeIDMap;
                std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
                for(std::vector<std::pair<sylvanmats::io::xml::tag_indexer, std::vector<sylvanmats::io::xml::tag_indexer>>>::iterator it=dag.begin();it!=dag.end();it++){
                    if(utf16.substr((*it).first.angle_start, (*it).first.angle_end-(*it).first.angle_start).starts_with(u"<graph ")){
//                std::cout<<(*it).first.index<<" "<<dag[(*it).first.index].second.size()<<" "<<cv.to_bytes(utf16.substr((*it).first.angle_start, (*it).first.angle_end-(*it).first.angle_start))<<" "<<(*it).second.size()<<" "<<cv.to_bytes(utf16.substr(std::get<1>((*it).second.back()), std::get<5>((*it).second.back())-std::get<1>((*it).second.back())))<<std::endl;
                    for(auto itS : dag[(*it).first.index].second | std::views::filter([&utf16](sylvanmats::io::xml::tag_indexer& di){ return utf16.substr(di.angle_start, di.angle_end-di.angle_start).starts_with(u"<node "); })){
                        lemon::ListGraph::Node n=graph.addNode();
                        std::u16string&& idValue=xmlReaper.findAttribute(u"id", itS.angle_start, itS.angle_end);
                        nodeIDMap[idValue]=n;
                        for(auto itD : dag[itS.index].second | std::views::filter([&utf16](sylvanmats::io::xml::tag_indexer& di){ return utf16.substr(di.angle_start, di.angle_end-di.angle_start).starts_with(u"<data "); })){
//                            std::cout<<"? "<<(n!=lemon::INVALID)<<" "<<(std::get<5>(itD)+1)<<" "<<(dag[std::get<0>(itD)].second.size())<<" "<<std::get<1>(dag[std::get<0>(itD)].second.back())<<std::endl;
                            std::u16string name=utf16.substr(itD.angle_end+1, dag[itD.index].second.back().angle_start-itD.angle_end-1);
                    graph.atomSites[n].type_symbol=std::string(cv.to_bytes(name));
//                std::cout<<"\t\t"<<std::get<5>(itD)<<" "<<std::get<1>(dag[std::get<0>(itD)].second.back())<<" "<<dag[std::get<0>(itD)].second.size()<<" "<<graph.atomSites[n].type_symbol<<std::endl;
//                std::cout<<" "<<cv.to_bytes(utf16.substr(std::get<1>(dag[std::get<0>(itD)].first), std::get<5>(dag[std::get<0>(itD)].first)-std::get<1>(dag[std::get<0>(itD)].first)))<<std::endl;
//                std::cout<<" "<<cv.to_bytes(utf16.substr(std::get<1>(dag[std::get<0>(itD)].second.back()), std::get<5>(dag[std::get<0>(itD)].second.back())-std::get<1>(dag[std::get<0>(itD)].second.back())))<<std::endl;
                        }
                    }
                    }
                }
                for(std::vector<std::pair<sylvanmats::io::xml::tag_indexer, std::vector<sylvanmats::io::xml::tag_indexer>>>::iterator it=dag.begin();it!=dag.end();it++){
                    if(utf16.substr((*it).first.angle_start, (*it).first.angle_end-(*it).first.angle_start).starts_with(u"<graph ")){
                    for(auto itS : dag[(*it).first.index].second | std::views::filter([&utf16](sylvanmats::io::xml::tag_indexer& di){ return utf16.substr(di.angle_start, di.angle_end-di.angle_start).starts_with(u"<edge "); })){
                        std::u16string&& sourceValue=xmlReaper.findAttribute(u"source", itS.angle_start, itS.angle_end);
                        std::u16string&& targetValue=xmlReaper.findAttribute(u"target", itS.angle_start, itS.angle_end);
                        lemon::ListGraph::Edge e=graph.addEdge(nodeIDMap[sourceValue], nodeIDMap[targetValue]);
                        for(auto itD : dag[itS.index].second | std::views::filter([&utf16](sylvanmats::io::xml::tag_indexer& di){ return utf16.substr(di.angle_start, di.angle_end-di.angle_start).starts_with(u"<data "); })){
                            std::u16string name=utf16.substr(itD.angle_end+1, dag[itD.index].second.back().angle_start-itD.angle_end-1);
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
