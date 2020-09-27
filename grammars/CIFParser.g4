parser grammar CIFParser;


options { tokenVocab = CIFLexer; }

cif : Comments? (WhiteSpace | Eol | TokenizedComments)? (dataBlock ((WhiteSpace | Eol | TokenizedComments) dataBlock)* ( WhiteSpace | Eol | TokenizedComments)? )?;

dataBlock : dataBlockHeading ((WhiteSpace | Eol | TokenizedComments)+( dataItems | saveFrame) )*;

dataBlockHeading : DATA_ blockHeading;
blockHeading : (NonBlankChars|Numeric|Period|Questionmark|Digit|Underscore)+;

dataItems : (tag (WhiteSpace | Eol )+ value) | loop;
tag : Underscore (NonBlankChars|Underscore|Numeric|Period | Digit|DATA_|LOOP_|SAVE_)+;
value : singleQuotedString | doubleQuotedString | ( NonBlankChars | Questionmark | Numeric | Underscore | TextField )+;
singleQuotedString : SingleQuotedString;
doubleQuotedString : DoubleQuotedString;

loop : loopHeader loopBody;
loopHeader : LOOP_ ((WhiteSpace | Eol | TokenizedComments)+ tag?)+;
loopBody : value ((WhiteSpace | Eol)+ value)*;

saveFrame : saveFrameHeading ((WhiteSpace | Eol | TokenizedComments) dataItems)+ (WhiteSpace | Eol | TokenizedComments) SAVE_;
saveFrameHeading : SAVE_ NonBlankChars;

