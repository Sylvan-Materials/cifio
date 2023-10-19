#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <functional>

#include "constitution/Graph.h"

namespace sylvanmats::constitution {

    class GraphmlPopulator{
    protected:
        unsigned int standardAACount=0;
        unsigned int standardCompCount=0;

    public:
        GraphmlPopulator() = default;
        GraphmlPopulator(const GraphmlPopulator& orig) = delete;
        virtual ~GraphmlPopulator() = default;

        void operator()(std::filesystem::path& filePath, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply);
        void operator()(std::istream& content, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply);
           
    };
}

