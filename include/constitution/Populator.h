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
    std::unordered_map<std::string, size_t> tagPolySeqMap = { {"asym_id", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, asym_id)},
                                                       {"entity_id", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, entity_id)},
                                                       {"seq_id", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, seq_id)},
                                                       {"mon_id", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, mon_id)},
                                                       {"ndb_seq_num", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, ndb_seq_num)},
                                                       {"pdb_seq_num", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, pdb_seq_num)},
                                                       {"auth_seq_num", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, auth_seq_num)},
                                                       {"pdb_mon_id", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, pdb_mon_id)},
                                                       {"auth_mon_id", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, auth_mon_id)},
                                                       {"pdb_strand_id", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, pdb_strand_id)},
                                                       {"pdb_ins_code", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, pdb_ins_code)},
                                                       {"hetero", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, hetero)}
                                                    };

    std::unordered_map<std::string, size_t> tagAtomSiteMap = {  {"group_PDB", offsetof(sylvanmats::constitution::_atom_site<double>, group_PDB)},
                                                                {"id", offsetof(sylvanmats::constitution::_atom_site<double>, id)},
                                                                {"type_symbol", offsetof(sylvanmats::constitution::_atom_site<double>, type_symbol)},
                                                                {"label_atom_id", offsetof(sylvanmats::constitution::_atom_site<double>, label_atom_id)},
                                                                {"label_alt_id", offsetof(sylvanmats::constitution::_atom_site<double>, label_alt_id)},
                                                                {"label_comp_id", offsetof(sylvanmats::constitution::_atom_site<double>, label_comp_id)},
                                                                {"label_asym_id", offsetof(sylvanmats::constitution::_atom_site<double>, label_asym_id)},
                                                                {"label_entity_id", offsetof(sylvanmats::constitution::_atom_site<double>, label_entity_id)},
                                                                {"label_seq_id", offsetof(sylvanmats::constitution::_atom_site<double>, label_seq_id)},
                                                                {"pdbx_PDB_ins_code", offsetof(sylvanmats::constitution::_atom_site<double>, pdbx_PDB_ins_code)},
                                                                {"Cartn_x", offsetof(sylvanmats::constitution::_atom_site<double>, Cartn_x)},
                                                                {"Cartn_y", offsetof(sylvanmats::constitution::_atom_site<double>, Cartn_y)},
                                                                {"Cartn_z", offsetof(sylvanmats::constitution::_atom_site<double>, Cartn_z)},
                                                                {"occupancy", offsetof(sylvanmats::constitution::_atom_site<double>, occupancy)},
                                                                {"B_iso_or_equiv", offsetof(sylvanmats::constitution::_atom_site<double>, B_iso_or_equiv)},
                                                                {"pdbx_formal_charge", offsetof(sylvanmats::constitution::_atom_site<double>, pdbx_formal_charge)},
                                                                {"auth_seq_id", offsetof(sylvanmats::constitution::_atom_site<double>, auth_seq_id)},
                                                                {"auth_comp_id", offsetof(sylvanmats::constitution::_atom_site<double>, auth_comp_id)}, 
                                                                {"auth_asym_id", offsetof(sylvanmats::constitution::_atom_site<double>, auth_asym_id)}, 
                                                                {"auth_atom_id", offsetof(sylvanmats::constitution::_atom_site<double>, auth_atom_id)}, 
                                                                {"pdbx_PDB_model_num", offsetof(sylvanmats::constitution::_atom_site<double>, pdbx_PDB_model_num)}
                                                            };

    std::unordered_map<std::string, size_t> tagOperationsMap = {  {"id", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, id)},
                                                                {"type", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, type)},
                                                                {"name", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, name)},
                                                                {"symmetry_operation", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, symmetry_operation)},
                                                                {"matrix[1][1]", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix)},
                                                                {"matrix[1][2]", (sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix))},
                                                                {"matrix[1][3]", (2*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix))},
                                                                {"matrix[2][1]", (3*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix))},
                                                                {"matrix[2][2]", (4*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix))},
                                                                {"matrix[2][3]", (5*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix))},
                                                                {"matrix[3][1]", (6*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix))},
                                                                {"matrix[3][2]", (7*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix))},
                                                                {"matrix[3][3]", (8*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix))},
                                                                {"vector[1]", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, vector)},
                                                                {"vector[2]", (sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, vector))},
                                                                {"vector[3]", (2*sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, vector))}
                                                             };

    std::unordered_map<std::string, size_t> tagConnectionsMap = {  {"id ", offsetof(sylvanmats::constitution::_struct_conn<double>, id)},
                                                                    {"conn_type_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, conn_type_id)},
                                                                    {"pdbx_leaving_atom_flag ", offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_leaving_atom_flag)},
                                                                    {"pdbx_PDB_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_PDB_id)},
                                                                    {"ptnr1_label_asym_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_label_asym_id)},
                                                                    {"ptnr1_label_comp_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_label_comp_id)},
                                                                    {"ptnr1_label_seq_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_label_seq_id)},
                                                                    {"ptnr1_label_atom_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_label_atom_id)},
                                                                    {"pdbx_ptnr1_label_alt_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr1_label_alt_id)},
                                                                    {"pdbx_ptnr1_PDB_ins_code ", offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr1_PDB_ins_code)},
                                                                    {"pdbx_ptnr1_standard_comp_id", offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr1_standard_comp_id)},
                                                                    {"ptnr1_symmetry ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_symmetry)},
                                                                    {"ptnr2_label_asym_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_label_asym_id)},
                                                                    {"ptnr2_label_comp_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_label_comp_id)},
                                                                    {"ptnr2_label_seq_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_label_seq_id)},
                                                                    {"ptnr2_label_atom_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_label_atom_id)},
                                                                    {"pdbx_ptnr2_label_alt_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr2_label_alt_id)},
                                                                    {"pdbx_ptnr2_PDB_ins_code ", offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr2_PDB_ins_code)},
                                                                    {"ptnr1_auth_asym_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_auth_asym_id)},
                                                                    {"ptnr1_auth_comp_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_auth_comp_id)},
                                                                    {"ptnr1_auth_seq_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr1_auth_seq_id)},
                                                                    {"ptnr2_auth_asym_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_auth_asym_id)},
                                                                    {"ptnr2_auth_comp_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_auth_comp_id)},
                                                                    {"ptnr2_auth_seq_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_auth_seq_id)},
                                                                    {"ptnr2_symmetry ", offsetof(sylvanmats::constitution::_struct_conn<double>, ptnr2_symmetry)},
                                                                    {"pdbx_ptnr3_label_atom_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr3_label_atom_id)},
                                                                    {"pdbx_ptnr3_label_seq_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr3_label_seq_id)},
                                                                    {"pdbx_ptnr3_label_comp_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr3_label_comp_id)},
                                                                    {"pdbx_ptnr3_label_asym_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr3_label_asym_id)},
                                                                    {"pdbx_ptnr3_label_alt_id ", offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr3_label_alt_id)},
                                                                    {"pdbx_ptnr3_PDB_ins_code ", offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_ptnr3_PDB_ins_code)},
                                                                    {"details ", offsetof(sylvanmats::constitution::_struct_conn<double>, details)},
                                                                    {"pdbx_dist_value ", offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_dist_value)},
                                                                    {"pdbx_value_order ", offsetof(sylvanmats::constitution::_struct_conn<double>, pdbx_value_order)}
                                                                };

    std::unordered_map<std::string, size_t> tagConnectionAnglesMap = {  {"id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, id)},
                                                                        {"ptnr1_label_atom_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr1_label_atom_id)},
                                                                        {"ptnr1_label_alt_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr1_label_alt_id)},
                                                                        {"ptnr1_label_asym_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr1_label_asym_id)},
                                                                        {"ptnr1_label_comp_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr1_label_comp_id)},
                                                                        {"ptnr1_label_seq_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr1_label_seq_id)},
                                                                        {"ptnr1_auth_atom_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr1_auth_atom_id)},
                                                                        {"ptnr1_auth_asym_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr1_auth_asym_id)},
                                                                        {"ptnr1_auth_comp_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr1_auth_comp_id)},
                                                                        {"ptnr1_auth_seq_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr1_auth_seq_id)},
                                                                        {"ptnr1_PDB_ins_code", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr1_PDB_ins_code)},
                                                                        {"ptnr1_symmetry", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr1_symmetry)},
                                                                        {"ptnr2_label_atom_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr2_label_atom_id)},
                                                                        {"ptnr2_label_alt_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr2_label_alt_id)},
                                                                        {"ptnr2_label_asym_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr2_label_asym_id)},
                                                                        {"ptnr2_label_comp_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr2_label_comp_id)},
                                                                        {"ptnr2_label_seq_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr2_label_seq_id)},
                                                                        {"ptnr2_auth_atom_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr2_auth_atom_id)},
                                                                        {"ptnr2_auth_asym_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr2_auth_asym_id)},
                                                                        {"ptnr2_auth_comp_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr2_auth_comp_id)},
                                                                        {"ptnr2_auth_seq_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr2_auth_seq_id)},
                                                                        {"ptnr2_PDB_ins_code", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr2_PDB_ins_code)},
                                                                        {"ptnr2_symmetry", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr2_symmetry)},
                                                                        {"ptnr3_label_atom_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr3_label_atom_id)},
                                                                        {"ptnr3_label_alt_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr3_label_alt_id)},
                                                                        {"ptnr3_label_asym_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr3_label_asym_id)},
                                                                        {"ptnr3_label_comp_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr3_label_comp_id)},
                                                                        {"ptnr3_label_seq_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr3_label_seq_id)},
                                                                        {"ptnr3_auth_atom_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr3_auth_atom_id)},
                                                                        {"ptnr3_auth_asym_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr3_auth_asym_id)},
                                                                        {"ptnr3_auth_comp_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr3_auth_comp_id)},
                                                                        {"ptnr3_auth_seq_id", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr3_auth_seq_id)},
                                                                        {"ptnr3_PDB_ins_code", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr3_PDB_ins_code)},
                                                                        {"ptnr3_symmetry", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, ptnr3_symmetry)},
                                                                        {"value", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, value)},
                                                                        {"value_esd", offsetof(sylvanmats::constitution::_pdbx_struct_conn_angle<double>, value_esd)}
                                                                    };

   public:
        Populator() = default;
        Populator(const Populator& orig) = delete;
        virtual ~Populator() = default;

        void operator()(std::filesystem::path& filePath, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply);
        void operator()(std::istream& content, sylvanmats::constitution::Graph& graph, std::function<void(sylvanmats::constitution::Graph& graph)> apply);
    };
}
