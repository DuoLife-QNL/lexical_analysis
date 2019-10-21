%{
    #include"token.h"
    num g_num;
    id g_id;
    int yylval;
%}

%option yylineno

line    \n
blank   [ \t]
blanks  {blank}+
letter  [A-Za-z_]
digit   [0-9]
dlmt    [#$@]
id      {letter}({letter}|{digit})*
num     [+\-]?{digit}+(\.{digit}+)?(e[+\-]?{digit}+)?
num_err [+\-]?{digit}+(\.[^0-9])[^\t\n #$@\[\]\{\}\(\)\?:;\.,\+-/\%=&\|\^!<>*]*
id_err1 {digit}({letter}|{digit})*{letter}({letter}|{digit})*
id_err2 ({letter})({letter}|{digit})*{dlmt}+({letter}|{digit})*

%%


{blanks}    ;
{line}      line::inc_cnt();
"/*".*"*/"  ;
"//".*      ;
("\"")(.)*("\"")  return STRINGS;


"auto" 		{yylval = AUTO ; return KEYWORD;}
"break" 	{yylval = BREAK ; return KEYWORD;}
"case" 		{yylval = CASE ; return KEYWORD;}
"char" 		{yylval = CHAR ; return KEYWORD;}
"const" 	{yylval = CONST ; return KEYWORD;}
"continue" 	{yylval = CONTINUE ; return KEYWORD;}
"default" 	{yylval = DEFAULT ; return KEYWORD;}
"do" 		{yylval = DO ; return KEYWORD;}
"double" 	{yylval = DOUBLE ; return KEYWORD;}
"else" 		{yylval = ELSE ; return KEYWORD;}
"enum" 		{yylval = ENUM ; return KEYWORD;}
"extern" 	{yylval = EXTERN ; return KEYWORD;}
"float" 	{yylval = FLOAT ; return KEYWORD;}
"for" 		{yylval = FOR ; return KEYWORD;}
"goto" 		{yylval = GOTO ; return KEYWORD;}
"if" 		{yylval = IF ; return KEYWORD;}
"int" 		{yylval = INT ; return KEYWORD;}
"long" 		{yylval = LONG ; return KEYWORD;}
"register" 	{yylval = REGISTER ; return KEYWORD;}
"return" 	{yylval = RETURN ; return KEYWORD;}
"short" 	{yylval = SHORT ; return KEYWORD;}
"signed" 	{yylval = SIGNED ; return KEYWORD;}
"sizeof" 	{yylval = SIZEOF ; return KEYWORD;}
"static" 	{yylval = STATIC ; return KEYWORD;}
"struct" 	{yylval = STRUCT ; return KEYWORD;}
"switch" 	{yylval = SWITCH ; return KEYWORD;}
"typedef" 	{yylval = TYPEDEF ; return KEYWORD;}
"union" 	{yylval = UNION ; return KEYWORD;}
"unsigned" 	{yylval = UNSIGNED ; return KEYWORD;}
"void" 		{yylval = VOID ; return KEYWORD;}
"volatile" 	{yylval = VOLATILE ; return KEYWORD;}
"while" 	{yylval = WHILE ; return KEYWORD;}
"\\t"       {yylval = TAB; return KEYWORD;}
"\\n"       {yylval = NEWLINE; return KEYWORD;}

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

"#" 		{yylval = HASH ; return DELIMITER;} 
"$" 		{yylval = DOLLAR ; return DELIMITER;} 
"@" 		{yylval = ATRATE ; return DELIMITER;} 
"[" 		{yylval = LS ; return DELIMITER;} 
"]" 		{yylval = RS ; return DELIMITER;} 
"{" 		{yylval = LB ; return DELIMITER;} 
"}" 		{yylval = RB ; return DELIMITER;} 
"(" 		{yylval = LP; return DELIMITER;} 
")" 		{yylval = RP ; return DELIMITER;} 
"?" 		{yylval = QMARK ; return DELIMITER;} 
":" 		{yylval = COLON ; return DELIMITER;} 
"." 		{yylval = DOT ; return DELIMITER;} 
";"         {yylval = SEMI ; return DELIMITER;} 
"," 		{yylval = COMMA ; return DELIMITER;} 

{id_err1}   {return ERROR;}
{id_err2}   {return ERROR;}
{num_err}   {return ERROR;}

{id}        {yylval = g_id.detected(); return IDENTIFIER;}
{num}       {yylval = g_num.detected(); return NUM;}


%%

int yywrap(void)
{
	return 1 ;
}