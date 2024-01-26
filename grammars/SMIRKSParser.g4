parser grammar SMIRKSParser;


options { tokenVocab = SMIRKSLexer; }

patterns : atoms (bonds_and_atoms)+;
atoms : LSquare (atomic_primitives Comma?)+ (Colon map_class)? RSquare (LParenthese branch RParenthese)*;
bonds_and_atoms : bond_primitives atoms;
branch : bond_primitives atoms;
    
atomic_primitives : (Not? (Pound atomic_number) | Star |(Dollar LParenthese branch RParenthese)) ((Connectivity connectivity)|(Ring_Connectivity ring_connectivity)|(Ring_size ring_size?)|(Proton proton_count) |Aromatic|Semicolon|Not|formal_charge)*;

atomic_number : UnsignedInteger;
connectivity : UnsignedInteger;
ring_connectivity : UnsignedInteger;
ring_size : UnsignedInteger;
proton_count : UnsignedInteger;
formal_charge : PlusPlus | MinusMinus| (Minus|Plus) UnsignedInteger;
map_class : UnsignedInteger;
    
bond_primitives : (Minus|Double|Pound|Colon|Semicolon|(Any UnsignedInteger?)|Not|Star|At|Comma)+;