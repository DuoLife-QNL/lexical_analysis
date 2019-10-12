#include"global.h"
#include"lex.h"

class token{
    public:
        static int count;
        void inc_cnt();
};

class id: public token{
    public:
        static int id_cnt;
        static int id_id;
        void add_id(string s);
        void inc_id_cnt();
        void display(string s);
        bool exit(string s);
        int detected();
        static map<string, int> wordlist;
};

class num: public token{
    public:
        void display(string s);
        static int num_cnt;
        void inc_num_cnt(); 
        double detected();
};

class operators: public token{
    public:
        void display(string s);
        static int opr_cnt;
        void inc_opr_cnt();
        void detected(char* yytext);
};

class delimiter: public token{
    public:
        void display(string s);
        static int dlmt_cnt;
        void inc_dlmt_cnt();
        void detected(char* yytext);
};

class keyword: public token{
    public:
        void display(string s);
        static int kw_cnt;
        void inc_kw_cnt();
        void detected(char* yytext);
};

class line{
    public:
        static int count;
        static void inc_cnt();
};

#define EOI 0
#define AUTO 1
#define BREAK 2
#define CASE 3
#define CHAR 4
#define CONST 5
#define CONTINUE 6
#define DEFAULT 7
#define DO  8
#define DOUBLE 9
#define ELSE 10
#define ENUM 11
#define EXTERN 12
#define FLOAT 13
#define FOR 14
#define GOTO 15
#define IF 16
#define INT 17
#define LONG 18
#define REGISTER 19
#define RETURN 20
#define SHORT 21
#define SIGNED 22
#define SIZEOF 23
#define STATIC 24
#define STRUCT 25
#define SWITCH 26
#define TYPEDEF 27
#define UNION 28
#define UNSIGNED 29
#define VOID 30
#define VOLATILE 31
#define WHILE 32
#define TAB 33
#define NEWLINE 34

#define RELOP 39
#define PLUS 40
#define MINUS 41
#define TIMES 42
#define DIV 43
#define MODULO 44
#define EQUALS 45
#define PLUSEQ 46
#define MINUSEQ 47
#define TIMESEQ 48
#define DIVEQ 49
#define MODULOEQ 50
#define EMPEQ 51
#define OREQ 52
#define XOREQ 53
#define LSHIFTEQ 54
#define RSHIFTEQ 55
#define BITAND 56
#define BITOR 57
#define BITXOR 58
#define LSHIFT 59
#define RSHIFT 60
#define NOT 61
#define AND 62
#define OR 63
#define REQUALS 64
#define NOTEQUAL 65
#define INCR  66
#define DECR 67
#define LESS 68
#define LESSEQ 69
#define GREATER 70
#define GREATEREQ 71
#define PTRREF 72

#define DELIMITER 79
#define HASH 80
#define DOLLAR 81
#define ATRATE 82
#define LS 83
#define RS 84
#define LB 85
#define RB 86
#define LP 87
#define RP 88
#define QMARK 89
#define COLON 90
#define DOT 91
#define QUOTE  92
#define SEMI   93
#define COMMA  94

#define IDENTIFIER  95
#define NUM 96