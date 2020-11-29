lexer grammar CIFLexer;

DATA_ : [Dd] [Aa] [Tt] [Aa] Underscore ->pushMode(BlockHeadingMode);
LOOP_ : Eol [Ll] [Oo] [Oo] [Pp] Underscore ;
SAVE_ : Eol [Ss] [Aa] [Vv] [Ee] Underscore;

Comments : Pound (OrdinaryChar|WhiteSpace|Dash|Double_Quote | Pound | Single_Quote | Semi | LBracket | RBracket)*;
Tag :  Eol Underscore OrdinaryChar(OrdinaryChar|Underscore|Dash|Numeric|Period|Double_Quote | Pound | Single_Quote | Semi | LBracket | RBracket)+;
SingleQuotedString : Single_Quote ( '\\\'' | . )*? Single_Quote;//(OrdinaryChar|WhiteSpace|Space|Tab|Dash|Numeric|Period|Digit|'\\\'')+ Single_Quote;
DoubleQuotedString : Double_Quote ( '\\"' | . )*? Double_Quote;//(OrdinaryChar|Space|Tab|'\\"')* Double_Quote;
SemiColonTextField : Semi (.)+? Eol Semi;//Eol (WhiteSpace? (OrdinaryChar (OrdinaryChar|Space|Tab|Dash|Period|Underscore|Double_Quote|Single_Quote|Numeric)*)* Eol)* Semi ;
Value : (Period | Questionmark | (OrdinaryChar|LBracket)(OrdinaryChar | LBracket | RBracket |Numeric|Period|Dash)* | Numeric);

WhiteSpace : (TokenizedComments | Space | Tab)+ ;
TokenizedComments : (Space | Tab | Eol)+  Comments+;

Numeric : (Number | Number '(' UnsignedInteger ')' );
Number : (Float | Integer );
Float : ( ( (Plus|Dash) ? ( (Digit) * Period UnsignedInteger ) |
         (Digit) + Period ) | Integer Exponent ) (Exponent) ? ;
fragment
Exponent : (('e' | 'E' ) | ('e' | 'E' )( Plus | Dash )) UnsignedInteger;
Integer : ( Plus | Dash )? UnsignedInteger;
UnsignedInteger : ( Digit )+;

Eol : '\n\r'|'\n'|'\r';

Semi : ';';
Pound : '#';
//Dollar : '$';
Underscore : '_';
Period : '.';
Questionmark : '?';
fragment
LBracket : '[';
fragment
RBracket : ']';
Plus : '+';
Dash : '-';
Space : ' ';
Tab : '\t';

Double_Quote : '"';

Single_Quote : '\'';

OrdinaryChar : ( [a-z] | [A-Z] | '!' | '%' | '&' | '(' | ')' | '*' | Plus | ',' | Dash | Period | '/' | Digit | ':' | '<' | '=' | '>' | Questionmark | '@' | '\\' | '^' | '`' | '{' | '|' | '}' | '~' | '$' ) ;

fragment
Digit : '0'..'9';

//mode ANY;

mode BlockHeadingMode;
BHText : (OrdinaryChar|Dash|Underscore|Numeric|Double_Quote | Pound | Single_Quote | Semi | LBracket | RBracket)+ ->popMode;

