#include"token.h"

int token::count = 0;

void token::inc_cnt(){
    count++;
}

int id::id_cnt = 0;

void id::add_id(string s){   //should be added first and then increase id_cnt
    wordlist[id_cnt] = s;
}
void id::inc_id_cnt(){
    id_cnt++;
}

void num::display(string s){
    cout << "Number" << setw(20) << s << endl;
}

void operators::display(string s){
    cout << "Operator" << setw(20) << s << endl;    
}

void delimiter::display(string s){
    cout << "Delimiter" << setw(20) << s << endl;
}

void keyword::display(string s){
    cout << "Keyword" << setw(20) << s << endl;
}