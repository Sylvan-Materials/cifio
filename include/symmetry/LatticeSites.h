#pragma once

#include <stdlib.h>
#include <string>
#include <vector>
#include <tuple>
#include <functional>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "linear/Vector.h"

namespace sylvanmats::symmetry {

    class LatticeSites{
        protected:
            std::vector<std::string>& functionNames;
            std::string& symmetryOperations;
            sylvanmats::linear::Matrix3d opMatrix;
            sylvanmats::linear::Vector3d opVector;

        lua_State *L = luaL_newstate();

        public:
        LatticeSites(std::vector<std::string>& functionNames, std::string& symmetryOperations, sylvanmats::linear::Matrix3d& opMatrix, sylvanmats::linear::Vector<double, 3>& opVector);
        LatticeSites(const LatticeSites& orig) = delete;
        virtual ~LatticeSites() = default;

        void operator()(sylvanmats::linear::Vector3d& pA, std::function<void(sylvanmats::linear::Vector3d& pAPrime)> apply);
        
    };
}
    
