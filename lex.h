#pragma once
#include"global.h"

extern int yyleng;
extern char* yytext;
extern FILE* yyin;
extern int yylineno;
int yylex();