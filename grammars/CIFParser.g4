parser grammar CIFParser;


options { tokenVocab = CIFLexer; }

cif : (comments)* (WhiteSpace)? (dataBlock ((WhiteSpace)? dataBlock)* ( WhiteSpace|Eol)* )?;
comments : Eol Comments | Comments;
dataBlock : dataBlockHeading ((WhiteSpace)?( dataItems | saveFrame) )*;

dataBlockHeading : Eol? DATA_ blockHeading;
blockHeading : BHText;

dataItems : (WhiteSpace? loop) | ((tag | Eol tag) (WhiteSpace|Eol)* value (WhiteSpace)*);
tag : Tag;
value : singleQuotedString | doubleQuotedString| semiColonTextField | (Value|Underscore )+;
singleQuotedString : SingleQuotedString ;
doubleQuotedString : DoubleQuotedString ;
semiColonTextField : SemiColonTextField;

loop : loopHeader loopBody;
loopHeader : LOOP_ ((WhiteSpace)? tag)+;
loopBody : (WhiteSpace|Eol)+ value ((WhiteSpace|Eol)+ value)*;

saveFrame : saveFrameHeading ((WhiteSpace) dataItems)+ (WhiteSpace) SAVE_;
saveFrameHeading : SAVE_ OrdinaryChar+;

