#include"global.h"

class token{
    public:
        static int count;
        static void inc_cnt();
};

class id: public token{
    public:
        static int id_cnt;
        void add_id(char* s);
        void inc_id_cnt();

    private:
        static map<int, string>wordlist;
};

class num: public token{
    public:
        void display(char* s);
};

class operators: public token{
    public:
        void display(char* s);
};

class delimiter: public token{
    public:
        void display(char* s);
};

class keyword: public token{
    public:
        void display(char* s);
};