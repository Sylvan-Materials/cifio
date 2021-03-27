mol2(entry_id, num_atoms, num_bonds, num_subst, atom_sites, atom_bonds, substructures) ::= <<
@\<TRIPOS>MOLECULE
<entry_id>
<num_atoms> <num_bonds> <num_subst> 0 0
SMALL
NO_CHARGES


@\<TRIPOS>ATOM
<atom_sites:{ atom_site |<atom_site.value0; format="%-4d"> <atom_site.value1; format="%-3s"> <atom_site.value2; format="%,.4f"> <atom_site.value3; format="%,.4f"> <atom_site.value4; format="%,.4f"> <atom_site.value5; format="%-4s"> <atom_site.value6; format="%-3d"> <atom_site.value7> <atom_site.value8> <\n>}>
 @\<TRIPOS>BOND
<atom_bonds:{ atom_bond |<atom_bond.value0; format="%-5d"> <atom_bond.value1; format="%-5d"> <atom_bond.value2; format="%-5d"> <atom_bond.value3; format="%-5s"> <\n>}>
@\<TRIPOS>SUBSTRUCTURE
<substructures:{ substructure |<substructure.value0; format="%-5d"> <substructure.value1> <substructure.value2> <substructure.value3> <substructure.value4; format="%-3s"> <substructure.value5; format="%-3s"> <substructure.value6> <substructure.value7> <substructure.value8> <\n>}>

>>