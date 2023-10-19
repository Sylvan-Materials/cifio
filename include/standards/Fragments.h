#pragma once

#include <stdlib.h>
#include <string>
#include <memory>

#include "constitution/Graph.h"

#include "linear/Vector.h"
#include "algebra/geometric/Motor.h"

namespace sylvanmats::standards {

        struct peptide_properties{
            char atomic_number;
            int sticky=-1;
        };

        bool operator==(const peptide_properties& lpp, const peptide_properties& rpp){
            return lpp.atomic_number==rpp.atomic_number;
        };
        
        struct type_properties : public peptide_properties{
            std::string type;
        };

        bool operator==(const type_properties& ltp, const type_properties& rtp){
            return ltp.atomic_number==rtp.atomic_number && ltp.type.compare(rtp.type)==0;
        };
        
    class Fragments{
        public:
        Fragments() = default;
        Fragments(const Fragments& orig) = delete;
        virtual ~Fragments() = default;
        void operator()(std::string name, sylvanmats::constitution::Graph& graph){
            
        };

    };

}


