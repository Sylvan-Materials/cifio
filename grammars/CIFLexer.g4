lexer grammar CIFLexer;

DATA_ : ([Dd] [Aa] [Tt] [Aa] Underscore);
LOOP_ : [Ll] [Oo] [Oo] [Pp] Underscore;
SAVE_ : [Ss] [Aa] [Vv] [Ee] Underscore;

//Tag : Underscore NonBlankChars;
NonBlankChars : (PartialChar | Semi | LBracket | RBracket | Period)+;
//Value : ( Period | Questionmark | Numeric | NonBlankChars | SingleQuotedString | DoubleQuotedString | TextField )+;
Numeric : (Number | Number '(' UnsignedInteger ')' );
Number : (Float | Integer );
Float : ( ( (Plus|Dash) ? ( (Digit) * Period UnsignedInteger ) |
         (Digit) + Period ) | Integer Exponent ) (Exponent) ? ;
fragment
Exponent : (('e' | 'E' ) | ('e' | 'E' )( Plus | Dash )) UnsignedInteger;
Integer : ( Plus | Dash )? UnsignedInteger;
fragment
UnsignedInteger : ( Digit )+;

//CharString : NonBlankChars | SingleQuotedString | DoubleQuotedString;
TextField : (SemiColonTextField ) Eol SemiColonTextField Eol Semi ( (AnyPrintChars|Digit|WhiteSpace|Eol)* Eol ((TextLeadChar (AnyPrintChars|Digit|WhiteSpace)*)? Eol)*) Semi;

//UnquotedString : NonBlankChars;
SingleQuotedString : Single_Quote (AnyPrintChars|Digit|WhiteSpace|Eol)* Single_Quote;
DoubleQuotedString : Double_Quote (AnyPrintChars|Digit|WhiteSpace|Eol)* Double_Quote;
SemiColonTextField : ((TextLeadChar (AnyPrintChars|Digit|WhiteSpace|Eol)*)? Eol*) Semi;

AnyPrintChars : (PartialChar | Semi | LBracket | RBracket)+;
TokenizedComments : (WhiteSpace | Eol)+  Comments;
Comments : ( Pound (AnyPrintChars|WhiteSpace)* Eol)+;

Eol : [\n\r];

WhiteSpace : [ \t];
Semi : ';';
LBracket : '[';
RBracket : ']';
Pound : '#';
Underscore : '_';
Period : '.';
Questionmark : '?';
Plus : '+';
Dash : '-';

fragment
Double_Quote : '"';

fragment
Single_Quote : '\'';

fragment
TextLeadChar : PartialChar | LBracket | RBracket;
fragment
PartialChar : OrdinaryChar | Double_Quote | Pound | '$' | Single_Quote ;

fragment
OrdinaryChar : ( '!' | '%' | '&' | '(' | ')' | '*' | Plus | ',' | Dash | Period | '/' | ':' | '<' | '=' | '>' | Questionmark | '@' | [A-Z] | '\\' | '^' | '`' | [a-z] | '{' | '|' | '}' | '~' );

Digit : '0'..'9';
