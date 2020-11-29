parser grammar DICParser;

options { tokenVocab = DICLexer; }

dic : (comments | semiColonTextField)* Eol* (data | WhiteSpace | Eol)+ ;

comments : Eol Comments | Comments;

data : DATA_ blockHeading (datablock | dictionary | loop | save | comments | WhiteSpace | Eol)+;
blockHeading : BHText;

datablock : (us_datablock_p_id | us_datablock_p_description)+;
us_datablock_p_id : USdatablock_p_id  WhiteSpace* (Value | Underscore)+;
us_datablock_p_description : USdatablock_p_description WhiteSpace* (singleQuotedString | semiColonTextField);

dictionary : (us_dictionary_p_title | us_dictionary_p_version | us_dictionary_p_datablock_id)+;
us_dictionary_p_title : USdictionary_p_title WhiteSpace* (Value | Underscore)+;
us_dictionary_p_version : USdictionary_p_version WhiteSpace* (Value | Underscore)+;
us_dictionary_p_datablock_id : USdictionary_p_datablock_id WhiteSpace* (Value | Underscore)+;

loop : LOOP_ (dictionary_history | sub_category | category_group_list | item_type_list | item_units_list | item_units_conversion)+ ((WhiteSpace|Eol)* value)+;
dictionary_history : (WhiteSpace|Eol)+ (us_dictionary_history_p_version | us_dictionary_history_p_update | us_dictionary_history_p_revision);
us_dictionary_history_p_version : USdictionary_history_p_version;
us_dictionary_history_p_update : USdictionary_history_p_update;
us_dictionary_history_p_revision : USdictionary_history_p_revision;
sub_category : (WhiteSpace|Eol)+ (us_sub_category_p_id | us_sub_category_p_description)+;
us_sub_category_p_id : USsub_category_p_id;
us_sub_category_p_description : USsub_category_p_description;
category_group_list : (WhiteSpace|Eol)+ (us_category_group_list_p_id| us_category_group_list_p_parent_id|us_category_group_list_p_description);
us_category_group_list_p_id : UScategory_group_list_p_id;
us_category_group_list_p_parent_id : UScategory_group_list_p_parent_id;
us_category_group_list_p_description : UScategory_group_list_p_description;
item_type_list : (WhiteSpace|Eol)+ (us_item_type_list_p_code | us_item_type_list_p_primitive_code | us_item_type_list_p_construct | us_item_type_list_p_detail);
us_item_type_list_p_code : USitem_type_list_p_code;
us_item_type_list_p_primitive_code : USitem_type_list_p_primitive_code;
us_item_type_list_p_construct : USitem_type_list_p_construct;
us_item_type_list_p_detail : USitem_type_list_p_detail;
item_units_list : (WhiteSpace|Eol)+ (us_item_units_list_p_code | us_item_units_list_p_detail);
us_item_units_list_p_code : USitem_units_list_p_code;
us_item_units_list_p_detail : USitem_units_list_p_detail;
item_units_conversion : (WhiteSpace|Eol)+ (us_item_units_conversion_p_from_code | us_item_units_conversion_p_to_code | us_item_units_conversion_p_operator | us_item_units_conversion_p_factor);
us_item_units_conversion_p_from_code : USitem_units_conversion_p_from_code;
us_item_units_conversion_p_to_code : USitem_units_conversion_p_to_code;
us_item_units_conversion_p_operator : USitem_units_conversion_p_operator;
us_item_units_conversion_p_factor : USitem_units_conversion_p_factor;

value : singleQuotedString | doubleQuotedString| semiColonTextField | (Value|Underscore )+;
singleQuotedString : SingleQuotedString ;
doubleQuotedString : DoubleQuotedString ;
semiColonTextField : SemiColonTextField;

save : SAVE_ saveBlockHeading (category | category_examples | loop | item | comments | WhiteSpace | Eol)+;
saveBlockHeading : SHText;

category : (us_category_p_description | us_category_p_id | us_category_p_mandatory_code)+;
us_category_p_description : UScategory_p_description WhiteSpace* (singleQuotedString | semiColonTextField);
us_category_p_id : UScategory_p_id  WhiteSpace* (singleQuotedString);
us_category_p_mandatory_code : UScategory_p_mandatory_code  WhiteSpace* (singleQuotedString);

category_examples : (us_category_examples_p_detail)+;
us_category_examples_p_detail : UScategory_examples_p_detail;

item : (us_item_p_name | us_item_p_category_id | us_item_p_mandatory_code);
us_item_p_name : USitem_p_name WhiteSpace* (singleQuotedString);
us_item_p_category_id : USitem_p_category_id WhiteSpace* (Value| Underscore)+;
us_item_p_mandatory_code :USitem_p_mandatory_code;