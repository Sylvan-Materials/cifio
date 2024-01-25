parser grammar SDFParser;


options { tokenVocab = SDFLexer; }


compound : (header_block Delimiter)+;

header_block : title;

title : 