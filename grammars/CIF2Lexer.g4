lexer grammar CIF2Lexer;

Magic_Code = '#\#CIF_2.0' ;

Data_Token : [Dd] [Aa] [Tt] [Aa] '_';

Save_Token : [Ss] [Aa] [Vv] [Ee] '_';

Loop_Token : [Ll] [Oo] [Oo] [Pp] '_' ;

Global_Token : [Gg] [Ll] [Oo] [Bb] [Aa] [Ll] '_' ;

Stop_Token : [Ss] [Tt] [Oo] [Pp] '_' ;

WSpace : ( Inline_WSpace | Line_Term ) WSpace_Any;

WSpace_Lines : ( Inline_WSpace Inline_WSpace* ( Comment )? )? Line_Term WSpace_To_Eol* ;

WSpace_Any : ( WSpace_To_Eol | Inline_WSpace )+ ;

WSpace_To_Eol : Inline_WSpace* Comment? Line_Term ;

Comment : '#'  Char ;

fragment
Char : ~('\u000A')* ;

Inline_WSpace : '\u0020' | '\u0009' ;

Line_Term : ( '\u000D' '\u000A'? ) | '\u000A' ;

AllChars : '\u0009' | '\u000A' | '\u000D' | '\u0020'..'\u007E'
  | '\u00A0'..'\uD7FF' | '\uE000'..'\uFDCF' | '\uFDF0'..'\uFFFD'
  | '\u{10000}'..'\u{1FFFD}'  | '\u{20000}'..'\u{2FFFD}'  | '\u{30000}'..'\u{3FFFD}'
  | '\u{40000}'..'\u{4FFFD}'  | '\u{50000}'..'\u{5FFFD}'  | '\u{60000}'..'\u{6FFFD}'
  | '\u{70000}'..'\u{7FFFD}'  | '\u{80000}'..'\u{8FFFD}'  | '\u{90000}'..'\u{9FFFD}'
  | '\u{A0000}'..'\u{AFFFD}'  | '\u{B0000}'..'\u{BFFFD}'  | '\u{C0000}'..'\u{CFFFD}'
  | '\u{D0000}'..'\u{DFFFD}'  | '\u{E0000}'..'\u{EFFFD}'  | '\u{F0000}'..'\u{FFFFD}'
  | '\u{100000}'..'\u{10FFFD}' ;
