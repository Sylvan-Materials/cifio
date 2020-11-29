lexer grammar DICLexer;

USdatablock_p_id : '_datablock.id';
USdatablock_p_description : '_datablock.description';
USdictionary_p_title : '_dictionary.title';
USdictionary_p_version : '_dictionary.version';
USdictionary_p_datablock_id : '_dictionary.datablock_id';

USdictionary_history_p_version : '_dictionary_history.version';
USdictionary_history_p_update : '_dictionary_history.update';
USdictionary_history_p_revision : '_dictionary_history.revision';

UScategory_p_description : '_category.description';
UScategory_p_id : '_category.id';
UScategory_p_mandatory_code : '_category.mandatory_code';
UScategory_key_p_name : '_category_key.name';

USsub_category_p_id : '_sub_category.id';
USsub_category_p_description : '_sub_category.description';

UScategory_group_list_p_id : '_category_group_list.id';
UScategory_group_list_p_parent_id : '_category_group_list.parent_id';
UScategory_group_list_p_description : '_category_group_list.description';
UScategory_examples_p_detail : '_category_examples.detail';

USitem_p_name : '_item.name';
USitem_p_category_id : '_item.category_id';
USitem_p_mandatory_code : '_item.mandatory_code';

USitem_type_list_p_code : '_item_type_list.code';
USitem_type_list_p_primitive_code : '_item_type_list.primitive_code';
USitem_type_list_p_construct : '_item_type_list.construct';
USitem_type_list_p_detail : '_item_type_list.detail';

USitem_units_list_p_code : '_item_units_list.code';
USitem_units_list_p_detail : '_item_units_list.detail';

USitem_units_conversion_p_from_code : '_item_units_conversion.from_code';
USitem_units_conversion_p_to_code : '_item_units_conversion.to_code';
USitem_units_conversion_p_operator : '_item_units_conversion.operator';
USitem_units_conversion_p_factor : '_item_units_conversion.factor';

DATA_ : [Dd] [Aa] [Tt] [Aa] Underscore ->pushMode(BlockHeadingMode);
LOOP_ : Eol Space* [Ll] [Oo] [Oo] [Pp] Underscore ;
SAVE_ : Eol Space* [Ss] [Aa] [Vv] [Ee] Underscore ->pushMode(SaveHeadingMode);

Comments : Pound (OrdinaryChar|WhiteSpace|Dash|Double_Quote | Pound | Underscore | Single_Quote | Semi | LBracket | RBracket)*;
SingleQuotedString : Single_Quote ( '\\\'' | . )*? Single_Quote;//(OrdinaryChar|WhiteSpace|Space|Tab|Dash|Numeric|Period|Digit|'\\\'')+ Single_Quote;
DoubleQuotedString : Double_Quote ( '\\"' | . )*? Double_Quote;//(OrdinaryChar|Space|Tab|'\\"')* Double_Quote;
SemiColonTextField : Eol Semi ( Semi | . )*? Eol Semi;//Eol (WhiteSpace? (OrdinaryChar (OrdinaryChar|Space|Tab|Dash|Period|Underscore|Double_Quote|Single_Quote|Numeric)*)* Eol)* Semi ;
Value : (Period | Questionmark | (OrdinaryChar|LBracket)(OrdinaryChar | LBracket | RBracket |Numeric|Period|Dash)* | Numeric);

//WhiteSpace : (TokenizedComments | Space+ | Tab)+ ;
WhiteSpace : ((Space+ | Tab+ | Eol)+  Comments?)+;

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

mode SaveHeadingMode;
SHText : (OrdinaryChar|Dash|Underscore|Numeric|Double_Quote | Pound | Single_Quote | Semi | LBracket | RBracket)+ ->popMode;
