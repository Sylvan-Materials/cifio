module;

#include <stdlib.h>
#include <string>
#include <memory>

#include "lemon/list_graph.h"

export module graph;

namespace sylvanmats::constitution {

    template<typename T>
    struct _atom_site{
        std::string group_PDB;
        unsigned long long id;
        std::string type_symbol;
        std::string label_atom_id; 
        std::string label_alt_id; 
        std::string label_comp_id; 
        std::string label_asym_id; 
        long long label_entity_id;
        long long label_seq_id;
        char pdbx_PDB_ins_code;
        T Cartn_x;
        T Cartn_y;
        T Cartn_z;
        T occupancy;
        T B_iso_or_equiv;
        short pdbx_formal_charge;
        int auth_seq_id;
        std::string auth_comp_id;
        std::string auth_asym_id;
        std::string auth_atom_id;
        int pdbx_PDB_model_num;

    };


    class Graph : public lemon::ListGraph {

        
        public:
            lemon::ListGraph::NodeMap<_atom_site<double>> atomSites;
            Graph() : atomSites(*this){
            };

            unsigned long getNumberOfAtomSites(){return lemon::countNodes(*this);};
    };

}
