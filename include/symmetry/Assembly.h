#pragma once

#include <stdlib.h>
#include <string>
#include <memory>
#include <ranges>
#include <numbers>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <tuple>

#include "constitution/Graph.h"

namespace sylvanmats::symmetry{

    class Assembly{
    protected:
        sylvanmats::constitution::Graph& graph;
    public:
        Assembly(sylvanmats::constitution::Graph& graph) : graph (graph){};
        Assembly(const Assembly& orig) = delete;
        virtual ~Assembly() = default;

        void operator()(){
        }

    };

}
