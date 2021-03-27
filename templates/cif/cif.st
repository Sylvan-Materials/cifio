cif(entry_id, atom_sites, pdbx_poly_seq_schemes, pdbx_nonpoly_schemes) ::= <<
data_<entry_id>
# 
_entry.id   <entry_id> 
# 
_audit_conform.dict_name       mmcif_pdbx.dic 
_audit_conform.dict_version    5.281 
_audit_conform.dict_location   http://mmcif.pdb.org/dictionaries/ascii/mmcif_pdbx.dic 
#
loop_
_atom_site.group_PDB 
_atom_site.id 
_atom_site.type_symbol 
_atom_site.label_atom_id 
_atom_site.label_alt_id 
_atom_site.label_comp_id 
_atom_site.label_asym_id 
_atom_site.label_entity_id 
_atom_site.label_seq_id 
_atom_site.pdbx_PDB_ins_code 
_atom_site.Cartn_x 
_atom_site.Cartn_y 
_atom_site.Cartn_z 
_atom_site.occupancy 
_atom_site.B_iso_or_equiv 
_atom_site.pdbx_formal_charge 
_atom_site.auth_seq_id 
_atom_site.auth_comp_id 
_atom_site.auth_asym_id 
_atom_site.auth_atom_id 
_atom_site.pdbx_PDB_model_num 
<atom_sites:{ atom_site |<atom_site.value0; format="%-6s"> <atom_site.value1; format="%-4d"> <atom_site.value2; format="%-2s"> <atom_site.value3; format="%-4s"> <atom_site.value4> <atom_site.value5> <atom_site.value6> <atom_site.value7> <atom_site.value8> <atom_site.value9> <atom_site.value10; format="%,.4f"> <atom_site.value11; format="%,2.4f"> <atom_site.value12; format="%,.4f"> <atom_site.value13> <atom_site.value14> <atom_site.value15> <atom_site.value16> <atom_site.value17> <atom_site.value18> <atom_site.value19; format="%-4s"> <atom_site.value20> <\n>}>
# 
loop_
_pdbx_poly_seq_scheme.asym_id 
_pdbx_poly_seq_scheme.entity_id 
_pdbx_poly_seq_scheme.seq_id 
_pdbx_poly_seq_scheme.mon_id 
_pdbx_poly_seq_scheme.ndb_seq_num 
_pdbx_poly_seq_scheme.pdb_seq_num 
_pdbx_poly_seq_scheme.auth_seq_num 
_pdbx_poly_seq_scheme.pdb_mon_id 
_pdbx_poly_seq_scheme.auth_mon_id 
_pdbx_poly_seq_scheme.pdb_strand_id 
_pdbx_poly_seq_scheme.pdb_ins_code 
_pdbx_poly_seq_scheme.hetero 
<pdbx_poly_seq_schemes:{ pdbx_poly_seq_scheme |<pdbx_poly_seq_scheme.value0> <pdbx_poly_seq_scheme.value1> <pdbx_poly_seq_scheme.value2> <pdbx_poly_seq_scheme.value3> <pdbx_poly_seq_scheme.value4> <pdbx_poly_seq_scheme.value5> <pdbx_poly_seq_scheme.value6> <pdbx_poly_seq_scheme.value7> <pdbx_poly_seq_scheme.value8> <pdbx_poly_seq_scheme.value9> <pdbx_poly_seq_scheme.value10> <pdbx_poly_seq_scheme.value11> <\n>}>
# 
loop_
_pdbx_nonpoly_scheme.asym_id 
_pdbx_nonpoly_scheme.entity_id 
_pdbx_nonpoly_scheme.mon_id 
_pdbx_nonpoly_scheme.ndb_seq_num 
_pdbx_nonpoly_scheme.pdb_seq_num 
_pdbx_nonpoly_scheme.auth_seq_num 
_pdbx_nonpoly_scheme.pdb_mon_id 
_pdbx_nonpoly_scheme.auth_mon_id 
_pdbx_nonpoly_scheme.pdb_strand_id 
_pdbx_nonpoly_scheme.pdb_ins_code 
<pdbx_nonpoly_schemes:{ pdbx_nonpoly_scheme |<pdbx_nonpoly_scheme.value0> <pdbx_nonpoly_scheme.value1> <pdbx_nonpoly_scheme.value2> <pdbx_nonpoly_scheme.value3> <pdbx_nonpoly_scheme.value4> <pdbx_nonpoly_scheme.value5> <pdbx_nonpoly_scheme.value6> <pdbx_nonpoly_scheme.value7> <pdbx_nonpoly_scheme.value8> <pdbx_nonpoly_scheme.value9> <\n>}>
>>