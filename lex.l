%{
    #include"token.h"
%}
line    \n
blank   [ \t]
blanks  {blank}+
letter  [A-Za-z]
digit   [0-9]
id      {letter|_}({letter}|{digit}|_)*
num     [+\-]?{digit}+(\.{digit}+)?(e[+\-]?{digit}+)?

%%

{blanks}    ;
{line}      line::inc_cnt();
"/*".*"*/"  ;
"auto" 		return AUTO ;
"break" 	return BREAK ;
"case" 		return CASE ;
"char" 		return CHAR ;
"const" 	return CONST ;
"continue" 	return CONTINUE ;
"default" 	return DEFAULT ;
"do" 		return DO ;
"double" 	return DOUBLE ;
"else" 		return ELSE ;
"enum" 		return ENUM ;
"extern" 	return EXTERN ;
"float" 	return FLOAT ;
"for" 		return FOR ;
"goto" 		return GOTO ;
"if" 		return IF ;
"int" 		return INT ;
"long" 		return LONG ;
"register" 	return REGISTER ;
"return" 	return RETURN ;
"short" 	return SHORT ;
"signed" 	return SIGNED ;
"sizeof" 	return SIZEOF ;
"static" 	return STATIC ;
"struct" 	return STRUCT ;
"switch" 	return SWITCH ;
"typedef" 	return TYPEDEF ;
"union" 	return UNION ;
"unsigned" 	return UNSIGNED ;
"void" 		return VOID ;
"volatile" 	return VOLATILE ;
"while" 	return WHILE ;

"+" 		{yylval = PLUS ;return RELOP;}
"-" 		{yylval = MINUS ;return RELOP;}
"*" 		{yylval = TIMES ;return RELOP;}
"/" 		{yylval = DIV ;return RELOP;}
"%" 		{yylval = MODULO ;return RELOP;}
"=" 		{yylval = EQUALS ;return RELOP;}
"&" 		{yylval = BITAND ;return RELOP;}
"|" 		{yylval = BITOR ;return RELOP;}
"^" 		{yylval = BITXOR ;return RELOP;}
"!" 		{yylval = NOT ;return RELOP;}
"<" 		{yylval = LESS ;return RELOP;}
">" 		{yylval = GREATER ;return RELOP;}
"+=" 		{yylval = PLUSEQ ;return RELOP;}
"-=" 		{yylval = MINUSEQ ;return RELOP;}
"*=" 		{yylval = TIMESEQ ;return RELOP;}
"/=" 		{yylval = DIVEQ ;return RELOP;}
"%=" 		{yylval = MODULOEQ ;return RELOP;}
"&&" 		{yylval = AND ;return RELOP;}
"||" 		{yylval = OR ;return RELOP;}
"==" 		{yylval = REQUALS ;return RELOP;}
"!=" 		{yylval = NOTEQUAL ;return RELOP;}
"<=" 		{yylval = LESSEQ ;return RELOP;}
">=" 		{yylval = GREATEREQ ;return RELOP;}
"++" 		{yylval = INCR ;return RELOP;}
"--" 		{yylval = DECR ;return RELOP;}
"<<" 		{yylval = LSHIFT ;return RELOP;}
">>" 		{yylval = RSHIFT ;return RELOP;}
"&=" 		{yylval = EMPEQ ;return RELOP;}
"|=" 		{yylval = OREQ ;return RELOP;}
"^=" 		{yylval = XOREQ ;return RELOP;}
"<<=" 		{yylval = LSHIFTEQ ;return RELOP;}
">>=" 		{yylval = RSHIFTEQ ;return RELOP;}
"->" 		{yylval = PTRREF ;return RELOP;}

"#" 		{yylval = HASH ; return DELIMETER;} 
"$" 		{yylval = DOLLAR ; return DELIMETER;} 
"@" 		{yylval = ATRATE ; return DELIMETER;} 
"[" 		{yylval = LS ; return DELIMETER;} 
"]" 		{yylval = RS ; return DELIMETER;} 
"{" 		{yylval = LB ; return DELIMETER;} 
"}" 		{yylval = RB ; return DELIMETER;} 
"(" 		{yylval = LP; return DELIMETER;} 
")" 		{yylval = RP ; return DELIMETER;} 
"?" 		{yylval = QMARK ; return DELIMETER;} 
":" 		{yylval = COLON ; return DELIMETER;} 
"." 		{yylval = DOT ; return DELIMETER;} 
";"         {yylval = SEMI ; return DELIMETER;} 
"," 		{yylval = COMMA ; return DELIMETER;} 

{id}        {yylval = id::detected(); return IDENTIFIER;}
{num}       {yylval = num::detected();return NUM;}

%%

int yywrap(void)
{
	return 1 ;
}