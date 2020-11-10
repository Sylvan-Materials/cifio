#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <functional>

#include "constitution/Graph.h"

namespace sylvanmats::constitution {

    class Populator{
    protected:
    public:
        Populator() = default;
        Populator(const Populator& orig) = delete;
        virtual ~Populator() = default;

        void operator()(std::filesystem::path& filePath, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply);
        void operator()(std::istream& content, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply);
    };
}
