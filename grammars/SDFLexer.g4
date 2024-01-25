lexer grammar SDFLexer;


Delimiter : '$$$$';

Number : (Float | Integer );
Float : ( ( (Plus|Dash) ? ( (Digit) * Period UnsignedInteger ) |
         (Digit) + Period ) | Integer Exponent ) (Exponent) ? ;
fragment
Exponent : (('e' | 'E' ) | ('e' | 'E' )( Plus | Dash )) UnsignedInteger;
Integer : ( Plus | Dash )? UnsignedInteger;
UnsignedInteger : ( Digit )+;

fragment
Digit : '0'..'9';
Plus : '+';
Dash : '-';
Period : '.';
