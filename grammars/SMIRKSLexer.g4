lexer grammar SMIRKSLexer;

UnsignedInteger : ( Digit )+;
PlusPlus : Plus Plus;
MinusMinus : Minus Minus;
Double : '=';

Connectivity : 'X';
Ring_Connectivity : 'x';
Ring_size : 'r';
Proton : 'H';
Aromatic : 'a';
LSquare : '[';
RSquare : ']';
Dollar : '$';
LParenthese : '(';
RParenthese : ')';
Colon : ':';
Semicolon : ';';
Comma : ',';
Pound : '#';
Star : '*';
Plus : '+';
Minus : '-';
Any : '~';
Not : '!';
At : '@';

fragment
Digit : '0'..'9';
