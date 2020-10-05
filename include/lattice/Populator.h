#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <functional>

#include "lattice/Graph.h"

namespace sylvanmats::lattice {

    class Populator{
    protected:
    public:
        Populator() = default;
        Populator(const Populator& orig) = delete;
        virtual ~Populator() = default;

        void operator()(std::string& filePath, sylvanmats::lattice::Graph& graph, std::function<void(sylvanmats::lattice::Graph& graph)> apply);
    };
}


