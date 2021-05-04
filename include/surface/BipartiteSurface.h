#pragma once

#include <stdlib.h>
#include <string>
#include <memory>

#include "lemon/list_graph.h"
#include "lemon/connectivity.h"

//#include "constitution/Selection.h"

namespace sylvanmats::surface {

    class BipartiteSurface : public lemon::ListBpGraph{
        public:
        BipartiteSurface(){};
    };

}
