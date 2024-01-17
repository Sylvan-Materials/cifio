    #pragma once

#include <stdlib.h>
#include <string>
#include <memory>
#include <sstream>
#include <optional>

#include "lemon/list_graph.h"
#include "lemon/connectivity.h"

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
        bool visibility=true;
        //std::optiona<> accessibleArea;

    };

    template<typename T>
    struct _atom_site_anisotrop{
        unsigned long long id;
        std::string type_symbol;
        std::string pdbx_label_atom_id;
        std::string pdbx_label_alt_id;
        std::string pdbx_label_comp_id;
        std::string pdbx_label_asym_id;
        long long pdbx_label_seq_id;
        std::string pdbx_PDB_ins_code;
        T U[3][3];
        long long pdbx_auth_seq_id;
        std::string pdbx_auth_comp_id;
        std::string pdbx_auth_asym_id;
        std::string pdbx_auth_atom_id;
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
    template<typename T>
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
        bool disulfide=false;
        T maximum_diameter=0.0; 
    };

    enum STRUCT_CONN_TYPE{
        COVALE,          //'covalent bond'
        DISULF,           //'disulfide bridge'
        HYDROG,           //'hydrogen bond'
        METALC,           //'metal coordination'
        MISMAT,           //'mismatched base pairs'
        SALTBR,           //'ionic interaction'
        MODRES,           //'covalent residue modification'
        COVALE_BASE,      //'covalent modification of a nucleotide base'
        COVALE_SUGAR,     //'covalent modification of a nucleotide sugar'
        COVALE_PHOSPHATE  //'covalent modification of a nucleotide phosphate'
    };

    template<typename T>
    struct _struct_conn {
        std::string id;
        std::string conn_type_id;
        std::string pdbx_leaving_atom_flag;
        std::string pdbx_PDB_id;
        std::string ptnr1_label_asym_id;
        std::string ptnr1_label_comp_id;
        long long ptnr1_label_seq_id;
        std::string ptnr1_label_atom_id;
        std::string pdbx_ptnr1_label_alt_id;
        std::string pdbx_ptnr1_PDB_ins_code;
        std::string pdbx_ptnr1_standard_comp_id;
        std::string ptnr1_symmetry;
        std::string ptnr2_label_asym_id;
        std::string ptnr2_label_comp_id;
        long long ptnr2_label_seq_id;
        std::string ptnr2_label_atom_id;
        std::string pdbx_ptnr2_label_alt_id;
        std::string pdbx_ptnr2_PDB_ins_code;
        std::string ptnr1_auth_asym_id;
        std::string ptnr1_auth_comp_id;
        long long ptnr1_auth_seq_id;
        std::string ptnr2_auth_asym_id;
        std::string ptnr2_auth_comp_id;
        long long ptnr2_auth_seq_id;
        std::string ptnr2_symmetry;
        std::string pdbx_ptnr3_label_atom_id;
        long long pdbx_ptnr3_label_seq_id;
        std::string pdbx_ptnr3_label_comp_id;
        std::string pdbx_ptnr3_label_asym_id;
        std::string pdbx_ptnr3_label_alt_id;
        std::string pdbx_ptnr3_PDB_ins_code;
        std::string details;
        T pdbx_dist_value;
        std::string pdbx_value_order;
    };

    template<typename T>
    struct _pdbx_struct_conn_angle {
        std::string id;
        std::string ptnr1_label_atom_id;
        std::string ptnr1_label_alt_id;
        std::string ptnr1_label_asym_id;
        std::string ptnr1_label_comp_id;
        std::string ptnr1_label_seq_id;
        std::string ptnr1_auth_atom_id;
        std::string ptnr1_auth_asym_id;
        std::string ptnr1_auth_comp_id;
        std::string ptnr1_auth_seq_id;
        std::string ptnr1_PDB_ins_code;
        std::string ptnr1_symmetry;
        std::string ptnr2_label_atom_id;
        std::string ptnr2_label_alt_id;
        std::string ptnr2_label_asym_id;
        std::string ptnr2_label_comp_id;
        std::string ptnr2_label_seq_id;
        std::string ptnr2_auth_atom_id;
        std::string ptnr2_auth_asym_id;
        std::string ptnr2_auth_comp_id;
        std::string ptnr2_auth_seq_id;
        std::string ptnr2_PDB_ins_code;
        std::string ptnr2_symmetry;
        std::string ptnr3_label_atom_id;
        std::string ptnr3_label_alt_id;
        std::string ptnr3_label_asym_id;
        std::string ptnr3_label_comp_id;
        std::string ptnr3_label_seq_id;
        std::string ptnr3_auth_atom_id;
        std::string ptnr3_auth_asym_id;
        std::string ptnr3_auth_comp_id;
        std::string ptnr3_auth_seq_id;
        std::string ptnr3_PDB_ins_code;
        std::string ptnr3_symmetry;
        T value;
        T value_esd;
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
        mutable std::string space_group_name_H_M;
        std::string entry_id;
        unsigned int Int_Tables_number;
        std::string pdbx_full_space_group_name_H_M;
        std::string cell_setting;
        std::string space_group_name_Hall;
    };

    template<typename T>
    struct _pdbx_struct_oper_list{
        unsigned int id;
        std::string type;
        std::string name;
        std::string symmetry_operation;
        T vector[3];
        T matrix[3][3];
    };

    template<typename T>
    struct _atom_sites{
        std::string entry_id="";
        T fract_transf_matrix[3][3];
        T fract_transf_vector[3];
    };

    struct _pdbx_unobs_or_zero_occ_residues{
        std::string id;
        std::string PDB_model_num;
        std::string polymer_flag;
        std::string occupancy_flag;
        std::string auth_asym_id;
        std::string auth_comp_id;
        std::string auth_seq_id;
        std::string PDB_ins_code;
        std::string label_asym_id;
        std::string label_comp_id;
        std::string label_seq_id;
    };

    class Graph : public lemon::ListGraph {
        protected:
            unsigned int currRing=0;
        
        public:
            lemon::ListGraph::NodeMap<_atom_site<double>> atomSites;
            lemon::ListGraph::EdgeMap<_comp_bond> compBond;
            _cell<double> cell;
            _symmetry symmetry;
            std::vector<_pdbx_struct_oper_list<double>> operationList;
            lemon::ListGraph componentGraph;
            lemon::ListGraph::NodeMap<_pdbx_poly_seq_scheme<double>> componentProperties;
            lemon::ListGraph::EdgeMap<STRUCT_CONN_TYPE> structConnType;
            lemon::IterableValueMap<lemon::ListGraph, lemon::ListGraph::Node, lemon::ListGraph::Node> componentNavigation;
            std::vector<_struct_conn<double>> structureConnections;
            _atom_sites<double> fractionalAtomSites;

            Graph() : atomSites(*this), compBond(*this), componentProperties(componentGraph), structConnType(componentGraph), componentNavigation(*this) {
            };
            Graph(const Graph& orig) = delete;
            virtual ~Graph()=default;

            lemon::ListGraph::Node addComponentNode(){lemon::ListGraph::Node n=componentGraph.addNode();return std::move(n);};
            unsigned long getNumberOfAtomSites(){return lemon::countNodes(*this);};
            unsigned long getNumberOfNeutronicAtomSites(){
                unsigned long count=0;
                for(lemon::ListGraph::NodeIt nSiteA(*this); nSiteA!=lemon::INVALID; ++nSiteA){
                    if(atomSites[nSiteA].type_symbol.compare("H")!=0)count++;
                }
                return count;
            };
            unsigned long getNumberOfConnections(){return lemon::countEdges(*this);};
            void visibilityOn(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subGraph);
            void visibilityOff(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subGraph);
            void flipVisibility();
            _cell<double>& getCell(){return cell;};
            _symmetry& getSymmetry(){return symmetry;};
            std::vector<_pdbx_struct_oper_list<double>>& getOperationList(){return operationList;};
            std::stringstream symmetryAsLua(){
                std::stringstream ssLua;
//                ssLua<<R"(local sym =  {}
//
//)";
                for(unsigned int index=0;index<operationList.size();index++){
                    ssLua<<"function f"<<operationList[index].id<<"(x,y,z) return "<<operationList[index].symmetry_operation<<" end"<<std::endl<<std::endl;
                }
//                ssLua<<R"(return sym
//)";
                return ssLua;
            };
            _atom_sites<double>& getFractionalAtomSites(){ return fractionalAtomSites;};
            std::vector<_struct_conn<double>>& getStructureConnections(){return structureConnections;};

            void identifyFusedSystems(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& selectionGraph, std::function<void(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subSelectionGraph)> apply);
            void identifyRings(lemon::SubGraph<lemon::ListGraph, lemon::ListGraph::NodeMap<bool>, lemon::ListGraph::EdgeMap<bool>>& subGraph);
            unsigned int getNumberOfComponentAtoms(lemon::ListGraph::Node& n);
            unsigned int getNumberOfComponentNeutronicAtoms(lemon::ListGraph::Node& n, bool alts=false);
            bool componentHasAlternateIds(lemon::ListGraph::Node& n);
            std::vector<std::string> getComponentAlternateIds(lemon::ListGraph::Node& n);
            double getComponentMaximumDiameter(lemon::ListGraph::Node& n);
            unsigned int getNumberOfRings(){return currRing;};
            unsigned int countFlexibles(){
                unsigned int ret=0;
                for(lemon::ListGraph::EdgeIt eSiteA(*this); eSiteA!=lemon::INVALID; ++eSiteA){
                    if(compBond[eSiteA].flexible)ret++;
                }
                return ret;
            };
            

            std::string getXPath(lemon::ListGraph::Node& nSite){
                return "/"+atomSites[nSite].label_asym_id+"/"+atomSites[nSite].label_comp_id+"/"+std::to_string(atomSites[nSite].auth_seq_id)+"/"+atomSites[nSite].pdbx_PDB_ins_code+"/";
            };
            std::string getComponentXPath(lemon::ListGraph::Node& n);

            std::string getUniqueName(lemon::ListGraph::Node& nSite){
                return atomSites[nSite].label_asym_id+"_"+atomSites[nSite].label_comp_id+"_"+std::to_string(atomSites[nSite].auth_seq_id)+"_"+atomSites[nSite].pdbx_PDB_ins_code+"_"+atomSites[nSite].label_atom_id;
            };
    };

}

