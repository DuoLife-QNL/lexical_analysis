#include"global.h"
#include"token.h"
#include"lex.h"

int argc;
char** argv;
ofstream outfile;
int ch_cnt = 0;

int main(int a, char** b){
    argc = a;
    argv = b;
    keyword k;
    operators o;
    delimiter de;
    strings s;
    error e;

    if (argc > 1){
        if(!(yyin = fopen(argv[1], "r"))){
            perror(argv[1]);
            return 1;
        }
        if (argc == 2){
            cout << left << setw(30) << "TOKEN_TYPE" << left << setw(30) << "CONTENT" << endl << endl;
        }else if (argc ==3){
            outfile.open(argv[2]);
            outfile << left << setw(30) << "TOKEN_TYPE" << left << setw(30) << "CONTENT" << endl << endl;
        }
    }else{
        cout << "Please input the token" << endl;        
    }

    int d = yylex();
    while(d){
        if (d >= AUTO && d <= NEWLINE){
            k.detected(yytext);
            ch_cnt += yyleng;
        }else if(d == RELOP){
            o.detected(yytext);
            ch_cnt += yyleng;
        }else if(d == DELIMITER){
            de.detected(yytext);
            ch_cnt += yyleng;
        }else if (d == IDENTIFIER){
            ch_cnt += yyleng;
        }else if(d == STRINGS){
            s.display(yytext);
            ch_cnt += yyleng;
        }else if(d == NUM){
            ch_cnt += yyleng;
        }else if(d == ERROR){
            e.display(yytext, yylineno);
        }
        else{
            ;
        }
        d = yylex();
    }

    if(argc == 2){
        cout << endl << endl << "STATISTICS" << endl << endl;

        cout << left << setw(30) << "ITEM" << left << setw(30) << "COUNT" << endl;
        cout << left << setw(30) << "Identifier" << left << setw(30) << id::id_cnt << endl;
        cout << left << setw(30) << "Number" << left << setw(30) << num::num_cnt << endl;
        cout << left << setw(30) << "Operator" << left << setw(30) << operators::opr_cnt << endl;
        cout << left << setw(30) << "Delimiter" << left << setw(30) << delimiter::dlmt_cnt << endl;
        cout << left << setw(30) << "Keyword" << left << setw(30) << keyword::kw_cnt << endl;
        cout << left << setw(30) << "Line" << left << setw(30) << line::count << endl;
        cout << left << setw(30) << "Total character number" << left << setw(30) << ch_cnt << endl;  
    }else if(argc == 3){
        outfile << endl << endl << "STATISTICS" << endl << endl;

        outfile << left << setw(30) << "ITEM" << left << setw(30) << "COUNT" << endl;
        outfile << left << setw(30) << "Identifier" << left << setw(30) << id::id_cnt << endl;
        outfile << left << setw(30) << "Number" << left << setw(30) << num::num_cnt << endl;
        outfile << left << setw(30) << "Operator" << left << setw(30) << operators::opr_cnt << endl;
        outfile << left << setw(30) << "Delimiter" << left << setw(30) << delimiter::dlmt_cnt << endl;
        outfile << left << setw(30) << "Keyword" << left << setw(30) << keyword::kw_cnt << endl;
        outfile << left << setw(30) << "Line" << left << setw(30) << line::count << endl;
        outfile << left << setw(30) << "Total character number" << left << setw(30) << ch_cnt << endl;  

    }

    outfile.close();
    return 0;
}