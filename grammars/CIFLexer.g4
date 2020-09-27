lexer grammar CIFLexer;

DATA_ : ([Dd] [Aa] [Tt] [Aa] Underscore);
LOOP_ : [Ll] [Oo] [Oo] [Pp] Underscore;
SAVE_ : [Ss] [Aa] [Vv] [Ee] Underscore;

Numeric : (Number | Number '(' UnsignedInteger ')' );
Number : (Float | Integer );
Float : ( ( (Plus|Dash) ? ( (Digit) * Period UnsignedInteger ) |
         (Digit) + Period ) | Integer Exponent ) (Exponent) ? ;
fragment
Exponent : (('e' | 'E' ) | ('e' | 'E' )( Plus | Dash )) UnsignedInteger;
Integer : ( Plus | Dash )? UnsignedInteger;
UnsignedInteger : ( Digit )+;


//UnquotedString : NonBlankChars;
SingleQuotedString : Single_Quote (AnyPrintChars|Digit|WhiteSpace|Double_Quote|Eol)* Single_Quote;
DoubleQuotedString : Double_Quote (AnyPrintChars|Digit|WhiteSpace|Single_Quote|Eol)* Double_Quote;
//CharString : NonBlankChars | SingleQuotedString | DoubleQuotedString;
TextField : (SemiColonTextField ) ;
SemiColonTextField : Eol Semi ( (AnyPrintChars)* Eol ((TextLeadChar (AnyPrintChars|Digit|WhiteSpace|Eol)*)? Eol)* ) Semi;

NonBlankChars : (AnyPrintChars | Period | Double_Quote | Single_Quote)+;

AnyPrintChars : (PartialChar | Semi | LBracket | RBracket)+;
TokenizedComments : (WhiteSpace | Eol)+  Comments;
Comments : ( Pound (AnyPrintChars|WhiteSpace | Double_Quote | Single_Quote)* Eol)+;

Eol : [\n\r];

WhiteSpace : [ \t];
Semi : ';';
Pound : '#';
Underscore : '_';
Period : '.';
Questionmark : '?';
fragment
LBracket : '[';
fragment
RBracket : ']';
fragment
Plus : '+';
fragment
Dash : '-';

Double_Quote : '"';

Single_Quote : '\'';

TextLeadChar : PartialChar | LBracket | RBracket;
PartialChar : OrdinaryChar | Pound | '$' ;

OrdinaryChar : ( [a-z] | [A-Z] | '!' | '%' | '&' | '(' | ')' | '*' | Plus | ',' | Dash | Period | '/' | ':' | '<' | '=' | '>' | Questionmark | '@' | '\\' | '^' | '`' | '{' | '|' | '}' | '~' );

Digit : '0'..'9';

//mode ANY;

mode SEMI;
EndSemi : Semi ->popMode;

mode SINGLE;
SingleQuote : Single_Quote ->popMode;
STEXT : . -> more;

mode DOUBLE;
DoubleQuote : Double_Quote ->popMode;
DTEXT : . -> more;
