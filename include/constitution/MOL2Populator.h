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

    class MOL2Populator{
    protected:
        unsigned int standardAACount=0;
        unsigned int standardCompCount=0;

    public:
        MOL2Populator() = default;
        MOL2Populator(const MOL2Populator& orig) = delete;
        virtual ~MOL2Populator() = default;

        void operator()(std::filesystem::path& filePath, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply);
        void operator()(std::istream& content, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply);
           
    };
}
