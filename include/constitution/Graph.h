#pragma once

#include <stdlib.h>
#include <string>
#include <memory>

#include "lemon/list_graph.h"
#include "lemon/connectivity.h"

//#include "constitution/Selection.h"

namespace sylvanmats::constitution {
    
    struct symmetry_labels{
        std::string type_symbol="";

        friend bool operator==(symmetry_labels& a, symmetry_labels& b);
    };

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
        std::string pdbx_PDB_ins_code;
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
        unsigned short proton_count=0;

    };

    struct _comp_bond{
        std::string comp_id;
        std::string atom_id_1;
        std::string atom_id_2;
        short value_order=1;
        bool pdbx_aromatic_flag=false;
        bool pdbx_stereo_config=false;
        unsigned int pdbx_ordinal;
        bool flexible=false;
        int ring=0;
    };

    enum TERMINATION{
        NEUTRAL,
        N_TERMINAL,
        C_TERMINAL,
        MONOMER
    };
    struct _pdbx_poly_seq_scheme {
        std::string asym_id;
        long long entity_id;
        long long seq_id;
        std::string mon_id;
        long long ndb_seq_num;
        long long pdb_seq_num;
        long long auth_seq_num;
        std::string pdb_mon_id;
        std::string auth_mon_id;
        std::string pdb_strand_id;
        std::string pdb_ins_code;
        std::string hetero;
        TERMINATION termination=NEUTRAL;
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

    struct _pdbx_struct_oper_list{
        unsigned int id;
        std::string type;
        std::string name;
        std::string symmetry_operation;
        double matrix[3][3];
        double vector[3];
    };

    class Graph : public lemon::ListGraph {
        protected:
            unsigned int currRing=0;
        
        public:
            lemon::ListGraph::NodeMap<_atom_site<double>> atomSites;
            lemon::ListGraph::EdgeMap<_comp_bond> compBond;
            _cell<double> cell;
            _symmetry symmetry;
            std::vector<_pdbx_struct_oper_list> operationList;
            lemon::ListGraph componentGraph;
            lemon::ListGraph::NodeMap<_pdbx_poly_seq_scheme> componentProperties;

            Graph() : atomSites(*this), compBond(*this), componentProperties(componentGraph){
            };

            unsigned long getNumberOfAtomSites(){return lemon::countNodes(*this);};
            _cell<double>& getCell(){return cell;};
            _symmetry& getSymmetry(){return symmetry;};

            void identifyFusedSystems(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph, std::function<void(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subSelectionGraph)> apply);
            void identifyRings(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subGraph);
            unsigned int getNumberOfRings(){return currRing;};
            unsigned int countFlexibles(){
                unsigned int ret=0;
                for(lemon::ListGraph::EdgeIt eSiteA(*this); eSiteA!=lemon::INVALID; ++eSiteA){
                    if(compBond[eSiteA].flexible)ret++;
                }
                return ret;
            };
    };

}

