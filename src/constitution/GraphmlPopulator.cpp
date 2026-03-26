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
                auto it = std::ranges::find_if(dagGraph.vertex_ids(),
                                         [&](auto u) { return dagGraph.vertex_value(u).index == 0; });
                auto vid=static_cast<graph::vertex_id_t<sylvanmats::io::xml::G>>(it - std::begin(dagGraph.vertex_ids()));
                auto v=*graph::find_vertex(dagGraph, vid);
                for (auto&& oe : graph::views::incidence(dagGraph, v) | std::views::filter([&utf16, &dagGraph, &cv](auto ei){auto& nv=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, ei.target_id)); return utf16.substr(nv.angle_start, nv.angle_end-nv.angle_start).starts_with(u"<graph "); })) {
                    auto oid=oe.target_id;
                    auto o=*graph::find_vertex(dagGraph, oid);
                    for(auto&& itS : graph::views::incidence(dagGraph, o) | std::views::filter([&utf16, &dagGraph, &cv](auto ei){auto& nv=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, ei.target_id)); return utf16.substr(nv.angle_start, nv.angle_end-nv.angle_start).starts_with(u"<node "); })){
                        lemon::ListGraph::Node n=graph.addNode();
                        auto nv=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, itS.target_id));
                        std::u16string&& idValue=xmlReaper.findAttribute(u"id", nv.angle_start, nv.angle_end);
                        nodeIDMap[idValue]=n;
                        auto d=*graph::find_vertex(dagGraph, itS.target_id);
                        size_t incEdgeCount=0;
                        for(auto&& itS : graph::views::incidence(dagGraph, d))incEdgeCount++;
                        //std::cout<<"id? "<<cv.to_bytes(idValue)<<" "<<size(graph::views::incidence(dagGraph, d))<<std::endl;
                        for(auto&& itD : graph::views::incidence(dagGraph, d) | std::views::filter([&utf16, &dagGraph, &cv](auto ei){auto& nv=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, ei.target_id));std::cout<<"data? "<<cv.to_bytes(utf16.substr(nv.angle_start, nv.angle_end-nv.angle_start))<<std::endl;return utf16.substr(nv.angle_start, nv.angle_end-nv.angle_start).starts_with(u"<data "); })){
                            auto dv=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, itD.target_id));
                            auto edv=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, itD.target_id+1));
                            std::string name=cv.to_bytes(utf16.substr(dv.angle_end+1, edv.angle_start-dv.angle_end-1));
                            graph.atomSites[n].type_symbol=std::string(name.begin(), name.end());
                            std::cout<<"\t\t"<<dv.index<<" "<<edv.index<<" |"<<graph.atomSites[n].type_symbol<<"|"<<std::endl;
                        }
                    }
                }
                for (auto&& oe : graph::views::incidence(dagGraph, v)) {
                    auto oid=oe.target_id;
                    auto o=*graph::find_vertex(dagGraph, oid);
                    for(auto&& itS : graph::views::incidence(dagGraph, o) | std::views::filter([&utf16, &dagGraph](auto ei){auto& nv=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, ei.target_id));return utf16.substr(nv.angle_start, nv.angle_end-nv.angle_start).starts_with(u"<edge "); })){
                        auto& nv=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, itS.target_id));
                        std::u16string&& sourceValue=xmlReaper.findAttribute(u"source", nv.angle_start, nv.angle_end);
                        std::u16string&& targetValue=xmlReaper.findAttribute(u"target", nv.angle_start, nv.angle_end);
                        lemon::ListGraph::Edge e=graph.addEdge(nodeIDMap[sourceValue], nodeIDMap[targetValue]);
                        auto d=*graph::find_vertex(dagGraph, itS.target_id);
                        for(auto&& itD : graph::views::incidence(dagGraph, d) | std::views::filter([&utf16, &dagGraph](auto ei){auto& nv=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, ei.target_id));return utf16.substr(nv.angle_start, nv.angle_end-nv.angle_start).starts_with(u"<data "); })){
                            auto dv=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, itD.target_id));
                            auto edv=graph::vertex_value(dagGraph, *graph::find_vertex(dagGraph, itD.target_id+1));
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
