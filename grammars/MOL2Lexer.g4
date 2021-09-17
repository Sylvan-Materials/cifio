lexer grammar MOL2Lexer;


TRIPOS_ALT_TYPE              : '@<TRIPOS>ALT_TYPE';
TRIPOS_ANCHOR_ATOM           : '@<TRIPOS>ANCHOR_ATOM';
TRIPOS_ASSOCIATED_ANNOTATION : '@<TRIPOS>ASSOCIATED_ANNOTATION';
TRIPOS_ATOM                  : '@<TRIPOS>ATOM';
TRIPOS_BOND                  : '@<TRIPOS>BOND';
TRIPOS_CENTER_OF_MASS        : '@<TRIPOS>CENTER_OF_MASS';
TRIPOS_CENTROID              : '@<TRIPOS>CENTROID';
TRIPOS_COMMENT               : '@<TRIPOS>COMMENT';
TRIPOS_CRYSIN                : '@<TRIPOS>CRYSIN';
TRIPOS_DICT                  : '@<TRIPOS>DICT';
TRIPOS_DATA_FILE             : '@<TRIPOS>DATA_FILE';
TRIPOS_EXTENSION_POINT       : '@<TRIPOS>EXTENSION_POINT';
TRIPOS_FF_PBC                : '@<TRIPOS>FF_PBC';
TRIPOS_FFCON_ANGLE           : '@<TRIPOS>FFCON_ANGLE';
TRIPOS_FFCON_DIST            : '@<TRIPOS>FFCON_DIST';
TRIPOS_FFCON_MULTI           : '@<TRIPOS>FFCON_MULTI';
TRIPOS_FFCON_RANGE           : '@<TRIPOS>FFCON_RANGE';
TRIPOS_FFCON_TORSION         : '@<TRIPOS>FFCON_TORSION';
TRIPOS_LINE                  : '@<TRIPOS>LINE';
TRIPOS_LSPLANE               : '@<TRIPOS>LSPLANE';
TRIPOS_MOLECULE              : '@<TRIPOS>MOLECULE';
TRIPOS_NORMAL                : '@<TRIPOS>NORMAL';
TRIPOS_QSAR_ALIGN_RULE       : '@<TRIPOS>QSAR_ALIGN_RULE';
TRIPOS_RING_CLOSURE          : '@<TRIPOS>RING_CLOSURE';
TRIPOS_ROTATABLE_BOND        : '@<TRIPOS>ROTATABLE_BOND';
TRIPOS_SEARCH_DIST           : '@<TRIPOS>SEARCH_DIST';
TRIPOS_SEARCH_OPTIONS        : '@<TRIPOS>SEARCH_OPTIONS';
TRIPOS_SET                   : '@<TRIPOS>SET';
TRIPOS_SUBSTRUCTURE          : '@<TRIPOS>SUBSTRUCTURE';
TRIPOS_U_FEAT                : '@<TRIPOS>U_FEAT';
TRIPOS_UNITY_ATOM_ATTR       : '@<TRIPOS>UNITY_ATOM_ATTR';
TRIPOS_UNITY_BOND_ATTR       : '@<TRIPOS>UNITY_BOND_ATTR';

SMALL        :'SMALL';
BIOPOLYMER   :'BIOPOLYMER';
PROTEIN      :'PROTEIN';
NUCLEIC_ACID :'NUCLEIC_ACID';
SACCHARIDE   :'SACCHARIDE';

NO_CHARGES       : 'NO_CHARGES';
DEL_RE           : 'DEL_RE';
GASTEIGER        : 'GASTEIGER';
GAST_HUCK        : 'GAST_HUCK';
HUCKEL           : 'HUCKEL';
PULLMAN          : 'PULLMAN';
GAUSS80_CHARGES  : 'GAUSS80_CHARGES';
AMPAC_CHARGES    : 'AMPAC_CHARGES';
MULLIKEN_CHARGES : 'MULLIKEN_CHARGES';
DICT_CHARGES     : 'DICT_CHARGES';
MMFF94_CHARGES   : 'MMFF94_CHARGES';
USER_CHARGES     : 'USER_CHARGES';

System : 'system';
Invalid_charges : 'invalid_charges';
Analyzed : 'analyzed';
Substituted : 'substituted';
Altered : 'altered';
Ref_angle : 'ref_angle';


DSPMOD : 'DSPMOD';
TYPECOL : 'TYPECOL';
CAP : 'CAP';
BACKBONE : 'BACKBONE';
DICT : 'DICT';
ESSENTIAL : 'ESSENTIAL';
WATER : 'WATER';
DIRECT : 'DIRECT';

GROUP : 'GROUP';
INTERRES : 'INTERRES';
LEAF : 'LEAF';
ROOT : 'ROOT';
BACKWARD : 'BACKWARD';
BLOCK : 'BLOCK';

WhiteSpace : (Space | Tab)+ ->channel(HIDDEN);
Eol : '\n\r'|'\n'|'\r';

Number : (Float | Integer );
Float : ( ( (Plus|Dash) ? ( (Digit) * Period UnsignedInteger ) |
         (Digit) + Period ) | Integer Exponent ) (Exponent) ? ;
fragment
Exponent : (('e' | 'E' ) | ('e' | 'E' )( Plus | Dash )) UnsignedInteger;
Integer : ( Plus | Dash )? UnsignedInteger;
UnsignedInteger : ( Digit )+;

Literal : (Chars | Digit| Period)+;
fragment
Digit : '0'..'9';

Pound : '#';
Star : '*';
Period : '.';
Underscore : '_';
Colon : ':';
Plus : '+';
Dash : '-';
fragment
Space : ' ';
fragment
Tab : '\t';

Chars : '\'' | '\u0030'..'\u007E'
  | '\u00A0'..'\uD7FF' | '\uE000'..'\uFDCF' | '\uFDF0'..'\uFFFD'
  | '\u{10000}'..'\u{1FFFD}'  | '\u{20000}'..'\u{2FFFD}'  | '\u{30000}'..'\u{3FFFD}'
  | '\u{40000}'..'\u{4FFFD}'  | '\u{50000}'..'\u{5FFFD}'  | '\u{60000}'..'\u{6FFFD}'
  | '\u{70000}'..'\u{7FFFD}'  | '\u{80000}'..'\u{8FFFD}'  | '\u{90000}'..'\u{9FFFD}'
  | '\u{A0000}'..'\u{AFFFD}'  | '\u{B0000}'..'\u{BFFFD}'  | '\u{C0000}'..'\u{CFFFD}'
  | '\u{D0000}'..'\u{DFFFD}'  | '\u{E0000}'..'\u{EFFFD}'  | '\u{F0000}'..'\u{FFFFD}'
  | '\u{100000}'..'\u{10FFFD}' ;