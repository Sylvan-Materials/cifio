parser grammar MOL2Parser;


options { tokenVocab = MOL2Lexer; }

mol2 : comment* tripos_molecule_record tripos_atom_record tripos_bond_record tripos_substructure_record;


tripos_alt_type_record              : TRIPOS_ALT_TYPE Eol ~(Eol)+ Eol;
tripos_anchor_atom_record           : TRIPOS_ANCHOR_ATOM ~(Eol)+ Eol;
tripos_associated_annotation_record : TRIPOS_ASSOCIATED_ANNOTATION ~(Eol)+ Eol;
tripos_atom_record                  : TRIPOS_ATOM Eol (atom_id atom_name x y z atom_type (atom_subst_id(atom_subst_name (charge (atom_status_bit)?)?)?)? Eol)+;
atom_id : Number;
atom_name : Literal|CAP;
x : Number;
y : Number;
z : Number;
atom_type : Literal(Period Literal|Number)?;
atom_subst_id  : Number;
atom_subst_name : Literal;
charge : Number;
atom_status_bit : DSPMOD | TYPECOL | CAP | BACKBONE | DICT | ESSENTIAL | WATER |DIRECT;

tripos_bond_record                  : TRIPOS_BOND Eol (bond_id origin_atom_id target_atom_id bond_type (bond_status_bits)? Eol)+;
bond_id : Number;
origin_atom_id : Number;
target_atom_id : Number;
bond_type : Literal|Number; 
bond_status_bits : TYPECOL | GROUP | CAP | BACKBONE | DICT | INTERRES;

tripos_center_of_mass_record        : TRIPOS_CENTER_OF_MASS ~(Eol)+ Eol;
tripos_centroid_record              : TRIPOS_CENTROID ~(Eol)+ Eol;
tripos_comment_record               : TRIPOS_COMMENT ~(Eol)+ Eol;
tripos_crysin_record                : TRIPOS_CRYSIN ~(Eol)+ Eol;
tripos_dict_record                  : TRIPOS_DICT ~(Eol)+ Eol;
tripos_data_file_record             : TRIPOS_DATA_FILE ~(Eol)+ Eol;
tripos_extension_point_record       : TRIPOS_EXTENSION_POINT ~(Eol)+ Eol;
tripos_ff_pbc_record                : TRIPOS_FF_PBC ~(Eol)+ Eol;
tripos_ffcon_angle_record           : TRIPOS_FFCON_ANGLE ~(Eol)+ Eol;
tripos_ffcon_dist_record            : TRIPOS_FFCON_DIST ~(Eol)+ Eol;
tripos_ffcon_multi_record           : TRIPOS_FFCON_MULTI ~(Eol)+ Eol;
tripos_ffcon_range_record           : TRIPOS_FFCON_RANGE ~(Eol)+ Eol;
tripos_ffcon_torsion_record         : TRIPOS_FFCON_TORSION ~(Eol)+ Eol;
tripos_line_record                  : TRIPOS_LINE ~(Eol)+ Eol;
tripos_lsplane_record               : TRIPOS_LSPLANE ~(Eol)+ Eol;
tripos_molecule_record              : TRIPOS_MOLECULE Eol mol_name Eol num_atoms (num_bonds (num_subst (num_feat(num_sets)?)?)?)? Eol mol_type Eol charge_type Eol status_bits Eol mol_comment Eol;
mol_name : Literal;
num_atoms : Number;
num_bonds : Number;
num_subst : Number;
num_feat : Number;
num_sets : Number;
mol_type : SMALL | BIOPOLYMER | PROTEIN | NUCLEIC_ACID | SACCHARIDE;
charge_type : NO_CHARGES | DEL_RE | GASTEIGER | GAST_HUCK | HUCKEL | PULLMAN | GAUSS80_CHARGES | AMPAC_CHARGES | MULLIKEN_CHARGES | DICT_CHARGES | MMFF94_CHARGES | USER_CHARGES;
status_bits : (System | Invalid_charges | Analyzed | Substituted | Altered | Ref_angle)* ;
mol_comment : ~(Eol)*;

tripos_normal_record                : TRIPOS_NORMAL ~(Eol)+ Eol;
tripos_qsar_align_rule_record       : TRIPOS_QSAR_ALIGN_RULE;
tripos_ring_closure_record          : TRIPOS_RING_CLOSURE ~(Eol)+ Eol;
tripos_rotatable_bond_record        : TRIPOS_ROTATABLE_BOND ~(Eol)+ Eol;
tripos_search_dist_recor            : TRIPOS_SEARCH_DIST ~(Eol)+ Eol;
tripos_search_options_record        : TRIPOS_SEARCH_OPTIONS ~(Eol)+ Eol;
tripos_set_record                   : TRIPOS_SET ~(Eol)+ Eol;
tripos_substructure_record          : TRIPOS_SUBSTRUCTURE Eol (subst_id subst_name root_atom (subst_type (dict_type(chain (sub_type (inter_bonds (status (sub_comment)?)?)?)?)?)?)? Eol)+;
subst_id : Number;
subst_name : Literal;
root_atom : Number;
subst_type : Literal;
dict_type : Number;
    chain : Literal|Star+;
sub_type : Literal|Star+;
inter_bonds : Number;
status : LEAF | ROOT | TYPECOL | DICT |BACKWARD | BLOCK;
sub_comment : Literal+;

tripos_u_feat_record                : TRIPOS_U_FEAT ~(Eol)+ Eol;
tripos_unity_atom_attr_record       : TRIPOS_UNITY_ATOM_ATTR ~(Eol)+ Eol;
tripos_unity_bond_attr_record       : TRIPOS_UNITY_BOND_ATTR ~(Eol)+ Eol;

comment : Pound ~(Eol)+ Eol+;