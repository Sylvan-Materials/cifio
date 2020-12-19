parser grammar CIF2Parser;


options { tokenVocab = CIF2Lexer; }

cif : file_heading AllChars;

file_heading : Magic_Code;