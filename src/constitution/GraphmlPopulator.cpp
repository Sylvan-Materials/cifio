#include <cstdio>
#include <ranges>
#include <typeinfo>

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
        std::string xmlPath=(getenv("CIFIO_DB_LOCATION")!=nullptr) ? std::string(getenv("CIFIO_DB_LOCATION"))+"/oxygen_fragments.graphml": "../oxygen_fragments.graphml";
        sylvanmats::io::xml::Binder xmlReaper(xmlPath, "");
        xmlReaper([&xmlReaper, &graph](std::u16string& utf16, sylvanmats::io::xml::G& dagGraph){
            if(graph::num_edges(dagGraph)>0){
                std::cout<<"dagGraph "<<graph::num_vertices(dagGraph)<<" "<<graph::num_edges(dagGraph)<<std::endl;
                std::unordered_map<std::u16string, lemon::ListGraph::Node> nodeIDMap;
                std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
                auto it = std::ranges::find_if(graph::vertices(dagGraph),
                                         [&](auto& u) { return graph::vertex_value(dagGraph, u).index == 0; });
                graph::vertex_id_t<sylvanmats::io::xml::G> vid=static_cast<graph::vertex_id_t<sylvanmats::io::xml::G>>(it - begin(graph::vertices(dagGraph)));
                auto& v=dagGraph[vid];
                for (auto&& oe : graph::edges(dagGraph, v) | std::views::filter([&utf16, &dagGraph, &cv](auto& ei){auto& nv=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, ei)]); return utf16.substr(nv.angle_start, nv.angle_end-nv.angle_start).starts_with(u"<graph "); })) {
                    auto oid=graph::target_id(dagGraph, oe);
                    auto& o=dagGraph[oid];
                    for(auto&& itS : graph::edges(dagGraph, o) | std::views::filter([&utf16, &dagGraph, &cv](auto& ei){auto& nv=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, ei)]); return utf16.substr(nv.angle_start, nv.angle_end-nv.angle_start).starts_with(u"<node "); })){
                        lemon::ListGraph::Node n=graph.addNode();
                        auto& nv=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, itS)]);
                        std::u16string&& idValue=xmlReaper.findAttribute(u"id", nv.angle_start, nv.angle_end);
                        nodeIDMap[idValue]=n;
                        auto& d=dagGraph[graph::target_id(dagGraph, itS)];
                        size_t incEdgeCount=0;
                        for(auto&& itS : graph::edges(dagGraph, d))incEdgeCount++;
                        //std::cout<<"id? "<<cv.to_bytes(idValue)<<" "<<size(graph::edges(dagGraph, d))<<std::endl;
                        for(auto&& itD : graph::edges(dagGraph, d) | std::views::filter([&utf16, &dagGraph, &cv](auto& ei){auto& nv=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, ei)]);std::cout<<"data? "<<cv.to_bytes(utf16.substr(nv.angle_start, nv.angle_end-nv.angle_start))<<std::endl;return utf16.substr(nv.angle_start, nv.angle_end-nv.angle_start).starts_with(u"<data "); })){
                            auto& dv=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, itD)]);
                            auto& edv=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, itD)+1]);
                            std::string name=cv.to_bytes(utf16.substr(dv.angle_end+1, edv.angle_start-dv.angle_end-1));
                            graph.atomSites[n].type_symbol=std::string(name.begin(), name.end());
                            std::cout<<"\t\t"<<dv.index<<" "<<edv.index<<" |"<<graph.atomSites[n].type_symbol<<"|"<<std::endl;
                        }
                    }
                }
                for (auto&& oe : graph::edges(dagGraph, v)) {
                    auto oid=graph::target_id(dagGraph, oe);
                    auto& o=dagGraph[oid];
                    for(auto&& itS : graph::edges(dagGraph, o) | std::views::filter([&utf16, &dagGraph](auto& ei){auto& nv=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, ei)]);return utf16.substr(nv.angle_start, nv.angle_end-nv.angle_start).starts_with(u"<edge "); })){
                        auto& nv=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, itS)]);
                        std::u16string&& sourceValue=xmlReaper.findAttribute(u"source", nv.angle_start, nv.angle_end);
                        std::u16string&& targetValue=xmlReaper.findAttribute(u"target", nv.angle_start, nv.angle_end);
                        lemon::ListGraph::Edge e=graph.addEdge(nodeIDMap[sourceValue], nodeIDMap[targetValue]);
                        auto& d=dagGraph[graph::target_id(dagGraph, itS)];
                        for(auto&& itD : graph::edges(dagGraph, d) | std::views::filter([&utf16, &dagGraph](auto& ei){auto& nv=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, ei)]);return utf16.substr(nv.angle_start, nv.angle_end-nv.angle_start).starts_with(u"<data "); })){
                            auto& dv=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, itD)]);
                            auto& edv=graph::vertex_value(dagGraph, dagGraph[graph::target_id(dagGraph, itD)+1]);
                             std::string name=cv.to_bytes(utf16.substr(dv.angle_end+1, edv.angle_start-dv.angle_end-1));
                            graph.compBond[e].atom_id_1=cv.to_bytes(sourceValue);
                            graph.compBond[e].atom_id_2=cv.to_bytes(targetValue);
                            graph.compBond[e].value_order=std::strtol(name.c_str(), nullptr, 10);
                        }
                        
                    }
                }
            }
        });
        apply(graph);
    }    
}
