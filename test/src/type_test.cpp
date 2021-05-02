#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <iterator>
#include <utility>
#include <charconv>
#include <cstddef>
#include <typeinfo>
#include <cxxabi.h>

#include "antlr4-runtime.h"

#include "standards/ComponentStandards.h"
#include "standards/AminoStandards.h"
#include "constitution/Graph.h"

TEST_SUITE("type"){

TEST_CASE("test atom site struct"){

    sylvanmats::constitution::_atom_site<double> atomSite;
    std::cout << std::boolalpha;
    const std::type_info  &ti = typeid(atomSite);
    int     status;
    char   *realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
    std::cout <<"name: "<< ti.name()<<" "<< realname<<" "<<status<< '\n';
    CHECK(std::is_class<sylvanmats::constitution::_atom_site<double>>::value);
    std::cout<<"label_atom_id: " << offsetof(sylvanmats::constitution::_atom_site<double>, label_atom_id)<<std::endl;
    std::cout << ti.name()<<" " << '\n';

    std::unordered_map<std::string, size_t> tagMap = { {"asym_id", offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, asym_id)},
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
    switch(tagMap["auth_seq_num"]){
        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, asym_id):
            std::cout<<"in case "<<offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, asym_id)<<std::endl;
        break;
        case offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, auth_seq_num):
            std::cout<<"in case "<<offsetof(sylvanmats::constitution::_pdbx_poly_seq_scheme, auth_seq_num)<<std::endl;
        break;
    }

    std::unordered_map<std::string, size_t> tagOperationsMap = {  {"id", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, id)},
                                                                {"type", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, type)},
                                                                {"name", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, name)},
                                                                {"symmetry_operation", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, symmetry_operation)},
                                                                {"matrix[1][1]", offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix)},
                                                                {"matrix[1][2]", sizeof(double)+offsetof(sylvanmats::constitution::_pdbx_struct_oper_list<double>, matrix)},
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
    std::cout<<"\"id\" "<<tagOperationsMap["id"]<<std::endl;
    std::cout<<"\"vector[1]\" "<<tagOperationsMap["vector[1]"]<<std::endl;
    std::cout<<"\"vector[2]\" "<<tagOperationsMap["vector[2]"]<<std::endl;
    std::cout<<"\"vector[3]\" "<<tagOperationsMap["vector[3]"]<<std::endl;
    std::cout<<"\"matrix[1][1]\" "<<tagOperationsMap["matrix[1][1]"]<<std::endl;
    std::cout<<"\"matrix[3][3]\" "<<tagOperationsMap["matrix[3][3]"]<<std::endl;
}

}

