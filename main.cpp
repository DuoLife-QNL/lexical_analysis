#include"global.h"
#include"token.h"
#include"lex.h"

int main(int argc, char** argv){
    if (argc > 1){
        if(!(yyin = fopen(argv[1], "r"))){
            perror(argv[1]);
            return 1;
        }
    }else{
        cout << "Please input the C code" << endl;        
    }

    int d = 1;
    while(d){
        d = yylex();
        if (d >= AUTO && d <= WHILE){
            keyword k;
            k.detected(yytext);
        }else if(d == RELOP){
            operators o;
            o.detected(yytext);
        }else if(d == DELIMITER){
            delimiter de;
            de.detected(yytext);
        }else if (d == IDENTIFIER){
            ;
        }else if(d == NUM){
            ;
        }else{
            ;
        }
    }
    return 0;
}