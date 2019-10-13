#include"token.h"

int token::count = 0;

void token::inc_cnt(){
    count++;
}


int id::id_cnt = 0;
int id::id_id = 200;
void id::inc_id_cnt(){
    id_cnt++;
    token::count++;
}
map<string, int> id::wordlist;

void id::add_id(string s){   //should be added first and then increase id_cnt
    wordlist[s] = id_id;
    id_id++;
}
void id::display(string s){
    if(argc == 3){
        outfile << left << setw(30) << "Identifier" << left << setw(30) << s << endl;
    }else{
        cout << left << setw(30) << "Identifier" << left << setw(30) << s << endl;
    }
}
bool id::exit(string s){
    return wordlist.count(s);
};
int id::detected(){
    string s(yytext);
    if(exit(s)){
        inc_id_cnt();
        display(s);
        return wordlist[s];
    }else{
        add_id(s);
        display(s);
        return id_id - 1;
    }
}

int num::num_cnt = 0;
void num::inc_num_cnt(){
    num_cnt++;
    token::count++;
}
void num::display(string s){
    if (argc == 3){
    outfile << left << setw(30) << "Number" << left << setw(30) << s << endl;
    }else{
        cout << left << setw(30) << "Number" << left << setw(30) << s << endl;
    }
}
double num::detected(){
    inc_num_cnt();
    string s(yytext);
    display(s);
    double value = stod(s);
    return value;
}

int operators::opr_cnt = 0;
void operators::inc_opr_cnt(){
    opr_cnt++;
    token::count++;
}
void operators::display(string s){
    if (argc == 3){
        outfile << left << setw(30) << "Operator" << left << setw(30) << s << endl;            
    }else{
        cout << left << setw(30) << "Operator" << left << setw(30) << s << endl;    
    }
}
void operators::detected(char* yytext){
    string s(yytext);
    inc_opr_cnt();
    display(s);
}

int delimiter::dlmt_cnt = 0;
void delimiter::inc_dlmt_cnt(){
    dlmt_cnt++;
    token::count++;
}
void delimiter::display(string s){
    if (argc == 3){
        outfile << left << setw(30) << "Delimiter" << left << setw(30) << s << endl;        
    }else{
        cout << left << setw(30) << "Delimiter" << left << setw(30) << s << endl;
    }
}
void delimiter::detected(char* yytext){
    string s(yytext);
    inc_dlmt_cnt();
    display(s);
}

int keyword::kw_cnt = 0;
void keyword::inc_kw_cnt(){
    kw_cnt++;
    token::count++;
}
void keyword::display(string s){
    if (argc == 3){
        outfile << left << setw(30) << "Keyword" << left << setw(30) << s << endl;
    }else {
        cout << left << setw(30) << "Keyword" << left << setw(30) << s << endl;
    }
}
void keyword::detected(char* yytext){
    string s(yytext);
    inc_kw_cnt();
    display(s);
}

int line::count = 0;

void line::inc_cnt(){
    count++;
}

void strings::display(char* yytext){
    string s_temp(yytext);
    string s = s_temp.substr(1, s_temp.length() - 2);
    if (argc == 3){
        outfile << left << setw(30) << "String" << left << setw(30) << s << endl;
    }else{
        cout << left << setw(30) << "String" << left << setw(30) << s << endl;
    }
}

void error::display(char* yytext, int yylineno){
    string s(yytext);
    if (argc == 3){
        outfile << left << setw(30) << "Error" << left << setw(30) << s << left <<"Line: " << yylineno << endl;
    }else{
        cout << left << setw(30) << "Error" << left << setw(30) << s << left <<"Line: " << yylineno << endl;
    }
}