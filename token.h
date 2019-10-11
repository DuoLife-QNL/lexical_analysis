#include"global.h"

class token{
    public:
        static int count;
        static void inc_cnt();
        int get_cnt();
};

class id: public token{
    public:
        void add_id(string s);

    private:
        static vector<string>list;
};

class num: public token{

};

class operators: public token{

};

class delimiter: public token{

};

class keyword: public token{

};