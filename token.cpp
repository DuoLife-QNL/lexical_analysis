#include"token.h"

int token::count = 0;

void token::inc_cnt(){
    count++;
}

int token::get_cnt(){
    return count;
}

void id::add_id(char* s){
    list.push_back(string(s));
}

void num::display(char* s){
    cout << "Number" << setw(20) << s << endl;
}

void operators::display(char* s){
    cout << "Operator" << setw(20) << s << endl;    
}

void delimiter::display(char* s){
    cout << "Delimiter" << setw(20) << s << endl;
}

void keyword::display(char* s){
    cout << "Keyword" << setw(20) << s << endl;
}