#include"global.h"

class token{
    public:
        static int count;
        static void inc_cnt();
        int get_cnt();
};

class id: public token{
    public:
        void add_id(char* s);

    private:
        static vector<string>list;
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