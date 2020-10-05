#pragma once

#include <stdlib.h>
#include <string>
#include <memory>

#include "lemon/list_graph.h"

namespace sylvanmats::lattice {

    template<typename T>
    struct _atom_site {
        std::string type_symbol;
        std::string label;
        T fract_x;
        T fract_y;
        T fract_z;
        T U_iso_or_equiv;
        std::string adp_type;
        T occupancy;
    };


    struct _comp_bond{
        std::string comp_id;
        std::string atom_id_1;
        std::string atom_id_2;
        short value_order=1;
        bool pdbx_aromatic_flag=false;
        bool pdbx_stereo_config=false;
        unsigned int pdbx_ordinal;
    };

    template<typename T>
    struct _cell {
        T length_a;
        T length_b;
        T length_c;
        T angle_alpha;
        T angle_beta;
        T angle_gamma;
    };

    struct _symmetry {
        std::string space_group_name_H_M;
        std::string entry_id;
        unsigned int Int_Tables_number;
        std::string pdbx_full_space_group_name_H_M;
        std::string cell_setting;
        std::string space_group_name_Hall;
    };

    class Graph : public lemon::ListGraph {

        
        public:
            lemon::ListGraph::NodeMap<_atom_site<double>> atomSites;
            lemon::ListGraph::EdgeMap<_comp_bond> compBond;
            _cell<double> cell;
            _symmetry symmetry;

            Graph() : atomSites(*this), compBond(*this){
            };

            unsigned long getNumberOfAtomSites(){return lemon::countNodes(*this);};
            _cell<double>& getCell(){return cell;};
            _symmetry& getSymmetry(){return symmetry;};
    };

}


