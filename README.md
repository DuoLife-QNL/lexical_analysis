## 目标
设计实现C语言词法分析程序，需实现下述功能：
1. 可以识别出用C语言编写的源程序中每个单词符号，以记号的形式输出
2. 可以识别并跳过源程序中的注释
3. 可以统计出程序的：
   * 行数
   * 各类单词的个数
   * 字符总数
4. 可以检查出源程序中存在的语法错误，并报告位置
5. 对源程序中出现的错误进行适当的恢复，使得词法分析可以继续进行，实现对源程序进行一次扫描即可检查并报告出源程序中出现的所有错误

## 概述
### 实现方法
* 采用flex工具生成词法分析核心程序```lex.cpp```
* 采用面向对象程序设计思想
* 词法规则参照ANSI C
* 完整代码仓库参见[https://github.com/DuoLife-QNL/lexical_analysis](https://github.com/DuoLife-QNL/lexical_analysis)
 
### 程序运行说明
* 运行环境：linux/unix + flex词法分析工具
* 在linux环境中使用```make```指令生成词法分析程序```lexical_analysis```
* ```lexical_analysis```接受指令如下：
  * ```lexical_analysis```：直接在标准输入流中输入待识别的记号，并在标准输出流中输出结果
  * ```lexical_analysis [inputfile_path]```：从路径为```[inputfile_path]```的文件中读取输入，并在标准输出流中输出结果
  * ```lexical_analysis [inputfile_path] [outputfile_path]```：从路径为```inputfile_path```的文件中读取输入，并将词法分析器的输出结果保存在路径为```outputfile_path]``的文件中
* 各文件依赖关系及生成如下：
  ```Makefile
  HEAD = global.h lex.h token.h
  OBJ = main.o lex.o token.o
  BIN = lexical_analysis

  $(BIN): $(OBJ)
  	g++ $(OBJ) -o $(BIN)

  main.o: $(HEAD) main.cpp lex.cpp
  	g++ -c main.cpp -o main.o
  token.o: $(HEAD) token.cpp lex.cpp
  	g++ -c token.cpp -o token.o
  lex.o: $(HEAD) lex.cpp
  	g++ -c lex.cpp -o lex.o

  lex.cpp:lex.c
  	mv lex.c lex.cpp

  lex.c: lex.l
  	flex -o lex.c lex.l
  ```


## 程序设计说明
### 使用的lex内置变量
* ```yylineno```：记录当前程序读取到哪一行，以便报错时输出错误的位置
* ```yytext```：字符数组指针，指向当前接收的记号
* ```yyleng```：当前读入记号的长度
* ```yyin```：```FILE*```类型变量，是词法分析程序的文件读入指针，程序开始时指向用户输入的待识别文件：
* ```yylval```：当用yacc作为语法分析器时，可以将此值传递给yacc，作为记号的编号。在本程序中实际上未使用到此变量
    ```C++
    if(!(yyin = fopen(argv[1], "r"))){
            perror(argv[1]);
            return 1;
        }
    ```
    若没有如上设置，则```yyin```默认指向标准输入
### 用lex和C++配合编写词法分析器的实现思路
1. ```main.cpp```为入口函数，包含程序的主函数```int main(int argc, char** argv)```
2. 在主函数中循环调用函数```int yylex()```，```yylex()```函数每识别一个记号，返回一个值，根据此返回值判断```yylex()```函数识别出的记号类型，并进行进一步的处理。同时，通过```yytext```和```yyleng```获取读入记号的内容和长度。
3. ```token.h```和```token.cpp```中使用C++面向对象编程的方法，创建```token```类，由```token```类派生出五种标识符的类，分别是```id```、```num```、```operators```、```delimeter```、```keyword```，每个类中包含各自的方法，如计数、显示、存取标识符表等；此外，还创建了```line```、```strings```、```error```类，分别进行对换行、字符串、错误的处理
4. 对于输入记号的识别方法在```lex.l```文件中，由flex编译程序对```lex.l```编译生成```lex.c```文件，其中包含识别记号的函数```int yylex()```
5. 将以上的```.c```和```.cpp```文件，以及各自的头文件编译链接之后，形成最终的词法分析程序
### 对于各种记号的定义及操作
1. 对标识符的定义及操作：
   ```
   letter  [A-Za-z_]
   digit   [0-9]
   id      {letter}({letter}|{digit})*
   ```
   lex识别后操作如下：
   ```C++
   {id}        {yylval = g_id.detected(); return IDENTIFIER;}
   ```
   其中，```g_id```为定义的全局变量，为```id```对象，识别出标识符后，调用其类的方法，包括插入标识符表，返回其在标识符表中的入口编号，统计出现的标识符个数等

2. 对常数的定义及操作（参照PASCAL的规则扩充了指数）：
   ```C++
   letter  [A-Za-z_]
   digit   [0-9]
   num     [+\-]?{digit}+(\.{digit}+)?(e[+\-]?{digit}+)?
   ```
   其中允许数字前有正负号，并允许带有指数的数字
   ```C++
   {num}       {yylval = g_num.detected(); return NUM;}
   ```
   其中，```g_num```为定义的全局变量，是```num```对象，识别出常数后调用其类的方法，包括将识别的文本转成数字，统计常数记号的出现次数等

3. 对于关键字的定义及操作：
   ```C++
   "auto" 		{yylval = AUTO ; return KEYWORD;}
   "break" 	{yylval = BREAK ; return KEYWORD;}
   "case" 		{yylval = CASE ; return KEYWORD;}
   "char" 		{yylval = CHAR ; return KEYWORD;}
   "const" 	{yylval = CONST ; return KEYWORD;}
   "continue" 	{yylval = CONTINUE ; return KEYWORD;}
   "default" 	{yylval = DEFAULT ; return KEYWORD;}
   "do" 		{yylval = DO ; return KEYWORD;}
   "double" 	{yylval = DOUBLE ; return KEYWORD;}
   "else" 		{yylval = ELSE ; return KEYWORD;}
   "enum" 		{yylval = ENUM ; return KEYWORD;}
   "extern" 	{yylval = EXTERN ; return KEYWORD;}
   "float" 	{yylval = FLOAT ; return KEYWORD;}
   "for" 		{yylval = FOR ; return KEYWORD;}
   "goto" 		{yylval = GOTO ; return KEYWORD;}
   "if" 		{yylval = IF ; return KEYWORD;}
   "int" 		{yylval = INT ; return KEYWORD;}
   "long" 		{yylval = LONG ; return KEYWORD;}
   "register" 	{yylval = REGISTER ; return KEYWORD;}
   "return" 	{yylval = RETURN ; return KEYWORD;}
   "short" 	{yylval = SHORT ; return KEYWORD;}
   "signed" 	{yylval = SIGNED ; return KEYWORD;}
   "sizeof" 	{yylval = SIZEOF ; return KEYWORD;}
   "static" 	{yylval = STATIC ; return KEYWORD;}
   "struct" 	{yylval = STRUCT ; return KEYWORD;}
   "switch" 	{yylval = SWITCH ; return KEYWORD;}
   "typedef" 	{yylval = TYPEDEF ; return KEYWORD;}
   "union" 	{yylval = UNION ; return KEYWORD;}
   "unsigned" 	{yylval = UNSIGNED ; return KEYWORD;}
   "void" 		{yylval = VOID ; return KEYWORD;}
   "volatile" 	{yylval = VOLATILE ; return KEYWORD;}
   "while" 	{yylval = WHILE ; return KEYWORD;}
   ```
   若遇到关键字，则将其宏定义传递给```yylval```，并使```yylex()```函数返回```KEYWORD```

4. 对于运算符的定义和操作如下：
   ```C++
   "+" 		{yylval = PLUS ;return RELOP;}
   "-" 		{yylval = MINUS ;return RELOP;}
   "*" 		{yylval = TIMES ;return RELOP;}
   "/" 		{yylval = DIV ;return RELOP;}
   "%" 		{yylval = MODULO ;return RELOP;}
   "=" 		{yylval = EQUALS ;return RELOP;}
   "&" 		{yylval = BITAND ;return RELOP;}
   "|" 		{yylval = BITOR ;return RELOP;}
   "^" 		{yylval = BITXOR ;return RELOP;}
   "!" 		{yylval = NOT ;return RELOP;}
   "<" 		{yylval = LESS ;return RELOP;}
   ">" 		{yylval = GREATER ;return RELOP;}
   "+=" 		{yylval = PLUSEQ ;return RELOP;}
   "-=" 		{yylval = MINUSEQ ;return RELOP;}
   "*=" 		{yylval = TIMESEQ ;return RELOP;}
   "/=" 		{yylval = DIVEQ ;return RELOP;}
   "%=" 		{yylval = MODULOEQ ;return RELOP;}
   "&&" 		{yylval = AND ;return RELOP;}
   "||" 		{yylval = OR ;return RELOP;}
   "==" 		{yylval = REQUALS ;return RELOP;}
   "!=" 		{yylval = NOTEQUAL ;return RELOP;}
   "<=" 		{yylval = LESSEQ ;return RELOP;}
   ">=" 		{yylval = GREATEREQ ;return RELOP;}
   "++" 		{yylval = INCR ;return RELOP;}
   "--" 		{yylval = DECR ;return RELOP;}
   "<<" 		{yylval = LSHIFT ;return RELOP;}
   ">>" 		{yylval = RSHIFT ;return RELOP;}
   "&=" 		{yylval = EMPEQ ;return RELOP;}
   "|=" 		{yylval = OREQ ;return RELOP;}
   "^=" 		{yylval = XOREQ ;return RELOP;}
   "<<=" 		{yylval = LSHIFTEQ ;return RELOP;}
   ">>=" 		{yylval = RSHIFTEQ ;return RELOP;}
   "->" 		{yylval = PTRREF ;return RELOP;}
   ```
   若遇到关系运算符，则将其宏定义传递给```yylval```，并使```yylex()```函数返回```RELOP```

5. 对于分界符的定义和操作如下：
   ```C++
   "#" 		{yylval = HASH ; return DELIMITER;} 
   "$" 		{yylval = DOLLAR ; return DELIMITER;} 
   "@" 		{yylval = ATRATE ; return DELIMITER;} 
   "[" 		{yylval = LS ; return DELIMITER;} 
   "]" 		{yylval = RS ; return DELIMITER;} 
   "{" 		{yylval = LB ; return DELIMITER;} 
   "}" 		{yylval = RB ; return DELIMITER;} 
   "(" 		{yylval = LP; return DELIMITER;} 
   ")" 		{yylval = RP ; return DELIMITER;} 
   "?" 		{yylval = QMARK ; return DELIMITER;} 
   ":" 		{yylval = COLON ; return DELIMITER;} 
   "." 		{yylval = DOT ; return DELIMITER;} 
   ";"         {yylval = SEMI ; return DELIMITER;} 
   "," 		{yylval = COMMA ; return DELIMITER;} 
   ```
   若遇到分界符，则将其宏定义传递给```yylval```，并使```yylex()```函数返回```DELIMITER```

### 错误处理
1. lex的两个匹配原则：**最长匹配原则**和**最先匹配原则**。其中，**最长匹配原则**保证当有多个规则可以匹配缓冲区中的字符时，则选择在缓冲区中可以匹配最长字符串的规则；**最先匹配原则**保证若多条规则同时匹配且长度相等时，匹配在lex源程序中写在最前面的那一条规则。
2. 由以上的两条规则可知，对于任意只包含C语言中可能出现的字符的文件，不带错误处理的lex语法可以将其分割成众多记号。在C语言编写词法分析程序时，由于不存在以上两条规则，故在状态转换之前会出现只有一条非other路径的情况，此时需要进行错误处理。但lex保证此时不会陷入错误，而是将其识别为多个记号，对其错误的处理全部在语法分析程序进行。
3. 但由于题目的限制，此处将几个典型的错误以正则式的形式在lex源程序中写出，如下：
   ```
   num_err [+\-]?{digit}+(\.[^0-9])[^\t\n #$@\[\]\{\}\(\)\?:;\.,\+-/\%=&\|\^!<>*]*
   id_err1 {digit}({letter}|{digit})*{letter}({letter}|{digit})*
   id_err2 ({letter})({letter}|{digit})*{dlmt}+({letter}|{digit})*
   ```
   * 第一种情况对应在识别数字时，"."后只能跟数字而没有"other"输入状态时，词法分析程序的报错
   * 第二、三中情况对应非法标识符的错误检测
4. 检测到错误时，通过yylineno返回错误出现的位置，并通过error实例进行错误处理
5. 由于lex的两个匹配原则，故出现自动机不接受的词法错误的时候，不会报错，即可以直接完成错误状态的恢复

### 记号类的操作说明（以id类为例）
标识符类的声明如下：
```C++
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
        void disp_tbl();
};
```
变量和函数的作用及其实现方法：
```static int id_cnt```：用于记录标识符的个数
```static int id_id```：用于记录插入的标识符在标识符表中的入口地址（编码）
```static map<string, int> wordlist```：一个```map```数据类型，用来保存标识符表，以标识符作为索引（确保唯一），表项内容为标识符的编码
```C++
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
```
当lex程序检测到一个标识符时，调用id的此成员函数，判断此标识符是否已在标识符表中，若是，则返回其在表中的位置，若不是，则将其插入到标识符表中，之后再返回其再表中的位置
```C++
void id::add_id(string s){   //should be added first and then increase id_cnt
    wordlist[s] = id_id;
    id_id++;
}
```
将标识符插入表中的操作
```C++
void id::inc_id_cnt(){
    id_cnt++;
    token::count++;
}
```
将标识符的计数+1
```C++
void id::display(string s){
    if(argc == 3){
        outfile << left << setw(30) << "Identifier" << left << setw(30) << s << endl;
    }else{
        cout << left << setw(30) << "Identifier" << left << setw(30) << s << endl;
    }
}
```
将识别到的标识符打印到屏幕或输出到指定的文件中
```C++
void id::disp_tbl(){
    if (argc == 2){
        cout << endl << endl << "ID_TABLE" << endl << endl;
        map<string, int>::iterator iter = wordlist.begin();
        map<string ,int>::iterator end = wordlist.end();
        for (; iter != end; iter ++)
            cout << setw(5) << iter->second << setw(10) << " " << iter->first << endl;
    }else if (argc == 3){
        outfile << endl << endl << "ID_TABLE" << endl << endl;
        map<string, int>::iterator iter = wordlist.begin();
        map<string ,int>::iterator end = wordlist.end();
        for (; iter != end; iter ++)
            outfile << setw(5) << iter->second << setw(10) << " " << iter->first << endl;
    }else {
        ;
    }

}
```
打印出词法分析程序维护的标识符表

## 测试
### 测试用例1
#### C语言代码
```C
#include<stdio.h>

const int CNT_NUM = 5;

int as$aa;
int 3a;
double 3.ffff*2f;
char c == 'a';

int main(){
    int a[CNT_NUM];

    for (int i = 0; i < CNT_NUM; i++){
        a[i] = i;
    }
    for (int i = 0; i < CNT_NUM - 1; i++){
        printf("%d\"test\"\t", a[i]);
    }
    printf("%d\n", a[CNT_NUM - 1]);
}//mark

/*sdfjasdif*/
```
#### 输出结果
```
TOKEN_TYPE                    CONTENT                       

Delimiter                     #                             
Identifier                    include                       
Operator                      <                             
Identifier                    stdio                         
Delimiter                     .                             
Identifier                    h                             
Operator                      >                             
Keyword                       const                         
Keyword                       int                           
Identifier                    CNT_NUM                       
Operator                      =                             
Number                        5                             
Delimiter                     ;                             
Keyword                       int                           
Error                         as$aa                         Line: 5
Delimiter                     ;                             
Keyword                       int                           
Error                         3a                            Line: 6
Delimiter                     ;                             
Keyword                       double                        
Error                         3.ffff                        Line: 7
Operator                      *                             
Error                         2f                            Line: 7
Delimiter                     ;                             
Keyword                       char                          
Identifier                    c                             
Operator                      ==                            
Char                          a                             
Delimiter                     ;                             
Keyword                       int                           
Identifier                    main                          
Delimiter                     (                             
Delimiter                     )                             
Delimiter                     {                             
Keyword                       int                           
Identifier                    a                             
Delimiter                     [                             
Identifier                    CNT_NUM                       
Delimiter                     ]                             
Delimiter                     ;                             
Keyword                       for                           
Delimiter                     (                             
Keyword                       int                           
Identifier                    i                             
Operator                      =                             
Number                        0                             
Delimiter                     ;                             
Identifier                    i                             
Operator                      <                             
Identifier                    CNT_NUM                       
Delimiter                     ;                             
Identifier                    i                             
Operator                      ++                            
Delimiter                     )                             
Delimiter                     {                             
Identifier                    a                             
Delimiter                     [                             
Identifier                    i                             
Delimiter                     ]                             
Operator                      =                             
Identifier                    i                             
Delimiter                     ;                             
Delimiter                     }                             
Keyword                       for                           
Delimiter                     (                             
Keyword                       int                           
Identifier                    i                             
Operator                      =                             
Number                        0                             
Delimiter                     ;                             
Identifier                    i                             
Operator                      <                             
Identifier                    CNT_NUM                       
Operator                      -                             
Number                        1                             
Delimiter                     ;                             
Identifier                    i                             
Operator                      ++                            
Delimiter                     )                             
Delimiter                     {                             
Identifier                    printf                        
Delimiter                     (                             
String                        %d\"test\"\t                  
Delimiter                     ,                             
Identifier                    a                             
Delimiter                     [                             
Identifier                    i                             
Delimiter                     ]                             
Delimiter                     )                             
Delimiter                     ;                             
Delimiter                     }                             
Identifier                    printf                        
Delimiter                     (                             
String                        %d\n                          
Delimiter                     ,                             
Identifier                    a                             
Delimiter                     [                             
Identifier                    CNT_NUM                       
Operator                      -                             
Number                        1                             
Delimiter                     ]                             
Delimiter                     )                             
Delimiter                     ;                             
Delimiter                     }                             


STATISTICS

ITEM                          COUNT                         
Identifier                    16                            
Number                        5                             
Operator                      14                            
Delimiter                     41                            
Keyword                       12                            
Line                          22                            
Total character number        206                           


ID_TABLE

203            CNT_NUM
206            a
204            c
202            h
207            i
200            include
205            main
208            printf
201            stdio
```
### 测试用例2
#### C程序代码
```C
#include <stdio.h>
#include <string.h>

#include "protocol.h"
#include "datalink.h"

#define DATA_TIMER  2000

struct FRAME { 
    unsigned char kind; /* FRAME_DATA */
    unsigned char ack;
    unsigned char seq;
    unsigned char data[PKT_LEN]; 
    unsigned int  padding;
};

static unsigned char frame_nr = 0, buffer[PKT_LEN], nbuffered;
static unsigned char frame_expected = 0;
static int phl_ready = 0;

static void put_frame(unsigned char *frame, int len)
{
    *(unsigned int *)(frame + len) = crc32(frame, len);
    send_frame(frame, len + 4);
    phl_ready = 0;
}

static void send_data_frame(void)
{
    struct FRAME s;

    s.kind = FRAME_DATA;
    s.seq = frame_nr;
    s.ack = 1 - frame_expected;
    memcpy(s.data, buffer, PKT_LEN);

    dbg_frame("Send DATA %d %d, ID %d\n", s.seq, s.ack, *(short *)s.data);

    put_frame((unsigned char *)&s, 3 + PKT_LEN);
    start_timer(frame_nr, DATA_TIMER);
}

static void send_ack_frame(void)
{
    struct FRAME s;

    s.kind = FRAME_ACK;
    s.ack = 1 - frame_expected;

    dbg_frame("Send ACK  %d\n", s.ack);

    put_frame((unsigned char *)&s, 2);
}

int main(int argc, char **argv)
{
    int event, arg;
    struct FRAME f;
    int len = 0;

    protocol_init(argc, argv); 
    lprintf("Designed by Jiang Yanjun, build: " __DATE__"  "__TIME__"\n");

    disable_network_layer();

    for (;;) {
        event = wait_for_event(&arg);

        switch (event) {
        case NETWORK_LAYER_READY:
            get_packet(buffer);
            nbuffered++;
            send_data_frame();
            break;

        case PHYSICAL_LAYER_READY:
            phl_ready = 1;
            break;

        case FRAME_RECEIVED: 
            len = recv_frame((unsigned char *)&f, sizeof f);
            if (len < 5 || crc32((unsigned char *)&f, len) != 0) {
                dbg_event("**** Receiver Error, Bad CRC Checksum\n");
                break;
            }
            if (f.kind == FRAME_ACK) 
                dbg_frame("Recv ACK  %d\n", f.ack);
            if (f.kind == FRAME_DATA) {
                dbg_frame("Recv DATA %d %d, ID %d\n", f.seq, f.ack, *(short *)f.data);
                if (f.seq == frame_expected) {
                    put_packet(f.data, len - 7);
                    frame_expected = 1 - frame_expected;
                }
                send_ack_frame();
            } 
            if (f.ack == frame_nr) {
                stop_timer(frame_nr);
                nbuffered--;
                frame_nr = 1 - frame_nr;
            }
            break; 

        case DATA_TIMEOUT:
            dbg_event("---- DATA %d timeout\n", arg); 
            send_data_frame();
            break;
        }

        if (nbuffered < 1 && phl_ready)
            enable_network_layer();
        else
            disable_network_layer();
   }
}
```
#### 输出结果
```
TOKEN_TYPE                    CONTENT                       

Delimiter                     #                             
Identifier                    include                       
Operator                      <                             
Identifier                    stdio                         
Delimiter                     .                             
Identifier                    h                             
Operator                      >                             
Delimiter                     #                             
Identifier                    include                       
Operator                      <                             
Identifier                    string                        
Delimiter                     .                             
Identifier                    h                             
Operator                      >                             
Delimiter                     #                             
Identifier                    include                       
String                        protocol.h                    
Delimiter                     #                             
Identifier                    include                       
String                        datalink.h                    
Delimiter                     #                             
Identifier                    define                        
Identifier                    DATA_TIMER                    
Number                        2000                          
Keyword                       struct                        
Identifier                    FRAME                         
Delimiter                     {                             
Keyword                       unsigned                      
Keyword                       char                          
Identifier                    kind                          
Delimiter                     ;                             
Keyword                       unsigned                      
Keyword                       char                          
Identifier                    ack                           
Delimiter                     ;                             
Keyword                       unsigned                      
Keyword                       char                          
Identifier                    seq                           
Delimiter                     ;                             
Keyword                       unsigned                      
Keyword                       char                          
Identifier                    data                          
Delimiter                     [                             
Identifier                    PKT_LEN                       
Delimiter                     ]                             
Delimiter                     ;                             
Keyword                       unsigned                      
Keyword                       int                           
Identifier                    padding                       
Delimiter                     ;                             
Delimiter                     }                             
Delimiter                     ;                             
Keyword                       static                        
Keyword                       unsigned                      
Keyword                       char                          
Identifier                    frame_nr                      
Operator                      =                             
Number                        0                             
Delimiter                     ,                             
Identifier                    buffer                        
Delimiter                     [                             
Identifier                    PKT_LEN                       
Delimiter                     ]                             
Delimiter                     ,                             
Identifier                    nbuffered                     
Delimiter                     ;                             
Keyword                       static                        
Keyword                       unsigned                      
Keyword                       char                          
Identifier                    frame_expected                
Operator                      =                             
Number                        0                             
Delimiter                     ;                             
Keyword                       static                        
Keyword                       int                           
Identifier                    phl_ready                     
Operator                      =                             
Number                        0                             
Delimiter                     ;                             
Keyword                       static                        
Keyword                       void                          
Identifier                    put_frame                     
Delimiter                     (                             
Keyword                       unsigned                      
Keyword                       char                          
Operator                      *                             
Identifier                    frame                         
Delimiter                     ,                             
Keyword                       int                           
Identifier                    len                           
Delimiter                     )                             
Delimiter                     {                             
Operator                      *                             
Delimiter                     (                             
Keyword                       unsigned                      
Keyword                       int                           
Operator                      *                             
Delimiter                     )                             
Delimiter                     (                             
Identifier                    frame                         
Operator                      +                             
Identifier                    len                           
Delimiter                     )                             
Operator                      =                             
Identifier                    crc32                         
Delimiter                     (                             
Identifier                    frame                         
Delimiter                     ,                             
Identifier                    len                           
Delimiter                     )                             
Delimiter                     ;                             
Identifier                    send_frame                    
Delimiter                     (                             
Identifier                    frame                         
Delimiter                     ,                             
Identifier                    len                           
Operator                      +                             
Number                        4                             
Delimiter                     )                             
Delimiter                     ;                             
Identifier                    phl_ready                     
Operator                      =                             
Number                        0                             
Delimiter                     ;                             
Delimiter                     }                             
Keyword                       static                        
Keyword                       void                          
Identifier                    send_data_frame               
Delimiter                     (                             
Keyword                       void                          
Delimiter                     )                             
Delimiter                     {                             
Keyword                       struct                        
Identifier                    FRAME                         
Identifier                    s                             
Delimiter                     ;                             
Identifier                    s                             
Delimiter                     .                             
Identifier                    kind                          
Operator                      =                             
Identifier                    FRAME_DATA                    
Delimiter                     ;                             
Identifier                    s                             
Delimiter                     .                             
Identifier                    seq                           
Operator                      =                             
Identifier                    frame_nr                      
Delimiter                     ;                             
Identifier                    s                             
Delimiter                     .                             
Identifier                    ack                           
Operator                      =                             
Number                        1                             
Operator                      -                             
Identifier                    frame_expected                
Delimiter                     ;                             
Identifier                    memcpy                        
Delimiter                     (                             
Identifier                    s                             
Delimiter                     .                             
Identifier                    data                          
Delimiter                     ,                             
Identifier                    buffer                        
Delimiter                     ,                             
Identifier                    PKT_LEN                       
Delimiter                     )                             
Delimiter                     ;                             
Identifier                    dbg_frame                     
Delimiter                     (                             
String                        Send DATA %d %d, ID %d\n      
Delimiter                     ,                             
Identifier                    s                             
Delimiter                     .                             
Identifier                    seq                           
Delimiter                     ,                             
Identifier                    s                             
Delimiter                     .                             
Identifier                    ack                           
Delimiter                     ,                             
Operator                      *                             
Delimiter                     (                             
Keyword                       short                         
Operator                      *                             
Delimiter                     )                             
Identifier                    s                             
Delimiter                     .                             
Identifier                    data                          
Delimiter                     )                             
Delimiter                     ;                             
Identifier                    put_frame                     
Delimiter                     (                             
Delimiter                     (                             
Keyword                       unsigned                      
Keyword                       char                          
Operator                      *                             
Delimiter                     )                             
Operator                      &                             
Identifier                    s                             
Delimiter                     ,                             
Number                        3                             
Operator                      +                             
Identifier                    PKT_LEN                       
Delimiter                     )                             
Delimiter                     ;                             
Identifier                    start_timer                   
Delimiter                     (                             
Identifier                    frame_nr                      
Delimiter                     ,                             
Identifier                    DATA_TIMER                    
Delimiter                     )                             
Delimiter                     ;                             
Delimiter                     }                             
Keyword                       static                        
Keyword                       void                          
Identifier                    send_ack_frame                
Delimiter                     (                             
Keyword                       void                          
Delimiter                     )                             
Delimiter                     {                             
Keyword                       struct                        
Identifier                    FRAME                         
Identifier                    s                             
Delimiter                     ;                             
Identifier                    s                             
Delimiter                     .                             
Identifier                    kind                          
Operator                      =                             
Identifier                    FRAME_ACK                     
Delimiter                     ;                             
Identifier                    s                             
Delimiter                     .                             
Identifier                    ack                           
Operator                      =                             
Number                        1                             
Operator                      -                             
Identifier                    frame_expected                
Delimiter                     ;                             
Identifier                    dbg_frame                     
Delimiter                     (                             
String                        Send ACK  %d\n                
Delimiter                     ,                             
Identifier                    s                             
Delimiter                     .                             
Identifier                    ack                           
Delimiter                     )                             
Delimiter                     ;                             
Identifier                    put_frame                     
Delimiter                     (                             
Delimiter                     (                             
Keyword                       unsigned                      
Keyword                       char                          
Operator                      *                             
Delimiter                     )                             
Operator                      &                             
Identifier                    s                             
Delimiter                     ,                             
Number                        2                             
Delimiter                     )                             
Delimiter                     ;                             
Delimiter                     }                             
Keyword                       int                           
Identifier                    main                          
Delimiter                     (                             
Keyword                       int                           
Identifier                    argc                          
Delimiter                     ,                             
Keyword                       char                          
Operator                      *                             
Operator                      *                             
Identifier                    argv                          
Delimiter                     )                             
Delimiter                     {                             
Keyword                       int                           
Identifier                    event                         
Delimiter                     ,                             
Identifier                    arg                           
Delimiter                     ;                             
Keyword                       struct                        
Identifier                    FRAME                         
Identifier                    f                             
Delimiter                     ;                             
Keyword                       int                           
Identifier                    len                           
Operator                      =                             
Number                        0                             
Delimiter                     ;                             
Identifier                    protocol_init                 
Delimiter                     (                             
Identifier                    argc                          
Delimiter                     ,                             
Identifier                    argv                          
Delimiter                     )                             
Delimiter                     ;                             
Identifier                    lprintf                       
Delimiter                     (                             
String                        Designed by Jiang Yanjun, build: " __DATE__"  "__TIME__"\n
Delimiter                     )                             
Delimiter                     ;                             
Identifier                    disable_network_layer         
Delimiter                     (                             
Delimiter                     )                             
Delimiter                     ;                             
Keyword                       for                           
Delimiter                     (                             
Delimiter                     ;                             
Delimiter                     ;                             
Delimiter                     )                             
Delimiter                     {                             
Identifier                    event                         
Operator                      =                             
Identifier                    wait_for_event                
Delimiter                     (                             
Operator                      &                             
Identifier                    arg                           
Delimiter                     )                             
Delimiter                     ;                             
Keyword                       switch                        
Delimiter                     (                             
Identifier                    event                         
Delimiter                     )                             
Delimiter                     {                             
Keyword                       case                          
Identifier                    NETWORK_LAYER_READY           
Delimiter                     :                             
Identifier                    get_packet                    
Delimiter                     (                             
Identifier                    buffer                        
Delimiter                     )                             
Delimiter                     ;                             
Identifier                    nbuffered                     
Operator                      ++                            
Delimiter                     ;                             
Identifier                    send_data_frame               
Delimiter                     (                             
Delimiter                     )                             
Delimiter                     ;                             
Keyword                       break                         
Delimiter                     ;                             
Keyword                       case                          
Identifier                    PHYSICAL_LAYER_READY          
Delimiter                     :                             
Identifier                    phl_ready                     
Operator                      =                             
Number                        1                             
Delimiter                     ;                             
Keyword                       break                         
Delimiter                     ;                             
Keyword                       case                          
Identifier                    FRAME_RECEIVED                
Delimiter                     :                             
Identifier                    len                           
Operator                      =                             
Identifier                    recv_frame                    
Delimiter                     (                             
Delimiter                     (                             
Keyword                       unsigned                      
Keyword                       char                          
Operator                      *                             
Delimiter                     )                             
Operator                      &                             
Identifier                    f                             
Delimiter                     ,                             
Keyword                       sizeof                        
Identifier                    f                             
Delimiter                     )                             
Delimiter                     ;                             
Keyword                       if                            
Delimiter                     (                             
Identifier                    len                           
Operator                      <                             
Number                        5                             
Operator                      ||                            
Identifier                    crc32                         
Delimiter                     (                             
Delimiter                     (                             
Keyword                       unsigned                      
Keyword                       char                          
Operator                      *                             
Delimiter                     )                             
Operator                      &                             
Identifier                    f                             
Delimiter                     ,                             
Identifier                    len                           
Delimiter                     )                             
Operator                      !=                            
Number                        0                             
Delimiter                     )                             
Delimiter                     {                             
Identifier                    dbg_event                     
Delimiter                     (                             
String                        **** Receiver Error, Bad CRC Checksum\n
Delimiter                     )                             
Delimiter                     ;                             
Keyword                       break                         
Delimiter                     ;                             
Delimiter                     }                             
Keyword                       if                            
Delimiter                     (                             
Identifier                    f                             
Delimiter                     .                             
Identifier                    kind                          
Operator                      ==                            
Identifier                    FRAME_ACK                     
Delimiter                     )                             
Identifier                    dbg_frame                     
Delimiter                     (                             
String                        Recv ACK  %d\n                
Delimiter                     ,                             
Identifier                    f                             
Delimiter                     .                             
Identifier                    ack                           
Delimiter                     )                             
Delimiter                     ;                             
Keyword                       if                            
Delimiter                     (                             
Identifier                    f                             
Delimiter                     .                             
Identifier                    kind                          
Operator                      ==                            
Identifier                    FRAME_DATA                    
Delimiter                     )                             
Delimiter                     {                             
Identifier                    dbg_frame                     
Delimiter                     (                             
String                        Recv DATA %d %d, ID %d\n      
Delimiter                     ,                             
Identifier                    f                             
Delimiter                     .                             
Identifier                    seq                           
Delimiter                     ,                             
Identifier                    f                             
Delimiter                     .                             
Identifier                    ack                           
Delimiter                     ,                             
Operator                      *                             
Delimiter                     (                             
Keyword                       short                         
Operator                      *                             
Delimiter                     )                             
Identifier                    f                             
Delimiter                     .                             
Identifier                    data                          
Delimiter                     )                             
Delimiter                     ;                             
Keyword                       if                            
Delimiter                     (                             
Identifier                    f                             
Delimiter                     .                             
Identifier                    seq                           
Operator                      ==                            
Identifier                    frame_expected                
Delimiter                     )                             
Delimiter                     {                             
Identifier                    put_packet                    
Delimiter                     (                             
Identifier                    f                             
Delimiter                     .                             
Identifier                    data                          
Delimiter                     ,                             
Identifier                    len                           
Operator                      -                             
Number                        7                             
Delimiter                     )                             
Delimiter                     ;                             
Identifier                    frame_expected                
Operator                      =                             
Number                        1                             
Operator                      -                             
Identifier                    frame_expected                
Delimiter                     ;                             
Delimiter                     }                             
Identifier                    send_ack_frame                
Delimiter                     (                             
Delimiter                     )                             
Delimiter                     ;                             
Delimiter                     }                             
Keyword                       if                            
Delimiter                     (                             
Identifier                    f                             
Delimiter                     .                             
Identifier                    ack                           
Operator                      ==                            
Identifier                    frame_nr                      
Delimiter                     )                             
Delimiter                     {                             
Identifier                    stop_timer                    
Delimiter                     (                             
Identifier                    frame_nr                      
Delimiter                     )                             
Delimiter                     ;                             
Identifier                    nbuffered                     
Operator                      --                            
Delimiter                     ;                             
Identifier                    frame_nr                      
Operator                      =                             
Number                        1                             
Operator                      -                             
Identifier                    frame_nr                      
Delimiter                     ;                             
Delimiter                     }                             
Keyword                       break                         
Delimiter                     ;                             
Keyword                       case                          
Identifier                    DATA_TIMEOUT                  
Delimiter                     :                             
Identifier                    dbg_event                     
Delimiter                     (                             
String                        ---- DATA %d timeout\n        
Delimiter                     ,                             
Identifier                    arg                           
Delimiter                     )                             
Delimiter                     ;                             
Identifier                    send_data_frame               
Delimiter                     (                             
Delimiter                     )                             
Delimiter                     ;                             
Keyword                       break                         
Delimiter                     ;                             
Delimiter                     }                             
Keyword                       if                            
Delimiter                     (                             
Identifier                    nbuffered                     
Operator                      <                             
Number                        1                             
Operator                      &&                            
Identifier                    phl_ready                     
Delimiter                     )                             
Identifier                    enable_network_layer          
Delimiter                     (                             
Delimiter                     )                             
Delimiter                     ;                             
Keyword                       else                          
Identifier                    disable_network_layer         
Delimiter                     (                             
Delimiter                     )                             
Delimiter                     ;                             
Delimiter                     }                             
Delimiter                     }                             


STATISTICS

ITEM                          COUNT                         
Identifier                    104                           
Number                        18                            
Operator                      57                            
Delimiter                     230                           
Keyword                       69                            
Line                          115                           
Total character number        1867                          


ID_TABLE

249            DATA_TIMEOUT
205            DATA_TIMER
206            FRAME
230            FRAME_ACK
225            FRAME_DATA
244            FRAME_RECEIVED
241            NETWORK_LAYER_READY
243            PHYSICAL_LAYER_READY
211            PKT_LEN
208            ack
235            arg
232            argc
233            argv
214            buffer
221            crc32
210            data
246            dbg_event
227            dbg_frame
204            define
239            disable_network_layer
250            enable_network_layer
234            event
236            f
219            frame
216            frame_expected
213            frame_nr
242            get_packet
202            h
200            include
207            kind
220            len
238            lprintf
231            main
226            memcpy
215            nbuffered
212            padding
217            phl_ready
237            protocol_init
218            put_frame
247            put_packet
245            recv_frame
224            s
229            send_ack_frame
223            send_data_frame
222            send_frame
209            seq
228            start_timer
201            stdio
248            stop_timer
203            string
240            wait_for_event
```
## 源代码
### global.h
```C++
#pragma once

#include<vector>
#include<iomanip>
#include<map>
#include<fstream>
#include<string>
#include<stdio.h>
#include<iostream>

using namespace std;
```
### lex.h
```C++
#pragma once
#include"global.h"

extern int yyleng;
extern char* yytext;
extern FILE* yyin;
extern int yylineno;
int yylex();
```
### token.h
```C++
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
        void disp_tbl();
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

class strings{
    public:
        void display(char* yytext);
};

class characters{
    public:
        void display(char* yytext);
};

class error{
    public:
        void display(char* yytext, int yylineno);
};

extern int argc;
extern ofstream outfile;

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
#define KEYWORD 35

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

#define STRINGS 110
#define CHARACTERS 111

#define ERROR 199
```
### lex.l
```lex
%{
    #include"token.h"
    num g_num;
    id g_id;
    int yylval;
%}

%option yylineno

line    \n
blank   [ \t]
blanks  {blank}+
letter  [A-Za-z_]
digit   [0-9]
dlmt    [#$@]
id      {letter}({letter}|{digit})*
num     [+\-]?{digit}+(\.{digit}+)?(e[+\-]?{digit}+)?
charac  ['].[']
num_err [+\-]?{digit}+(\.[^0-9])[^\t\n #$@\[\]\{\}\(\)\?:;\.,\+-/\%=&\|\^!<>*]*
id_err1 {digit}({letter}|{digit})*{letter}({letter}|{digit})*
id_err2 ({letter})({letter}|{digit})*{dlmt}+({letter}|{digit})*

%%


{blanks}    ;
{line}      line::inc_cnt();
"/*".*"*/"  ;
"//".*      ;
("\"")(.)*("\"")  return STRINGS;
{charac}    return CHARACTERS;

"auto" 		{yylval = AUTO ; return KEYWORD;}
"break" 	{yylval = BREAK ; return KEYWORD;}
"case" 		{yylval = CASE ; return KEYWORD;}
"char" 		{yylval = CHAR ; return KEYWORD;}
"const" 	{yylval = CONST ; return KEYWORD;}
"continue" 	{yylval = CONTINUE ; return KEYWORD;}
"default" 	{yylval = DEFAULT ; return KEYWORD;}
"do" 		{yylval = DO ; return KEYWORD;}
"double" 	{yylval = DOUBLE ; return KEYWORD;}
"else" 		{yylval = ELSE ; return KEYWORD;}
"enum" 		{yylval = ENUM ; return KEYWORD;}
"extern" 	{yylval = EXTERN ; return KEYWORD;}
"float" 	{yylval = FLOAT ; return KEYWORD;}
"for" 		{yylval = FOR ; return KEYWORD;}
"goto" 		{yylval = GOTO ; return KEYWORD;}
"if" 		{yylval = IF ; return KEYWORD;}
"int" 		{yylval = INT ; return KEYWORD;}
"long" 		{yylval = LONG ; return KEYWORD;}
"register" 	{yylval = REGISTER ; return KEYWORD;}
"return" 	{yylval = RETURN ; return KEYWORD;}
"short" 	{yylval = SHORT ; return KEYWORD;}
"signed" 	{yylval = SIGNED ; return KEYWORD;}
"sizeof" 	{yylval = SIZEOF ; return KEYWORD;}
"static" 	{yylval = STATIC ; return KEYWORD;}
"struct" 	{yylval = STRUCT ; return KEYWORD;}
"switch" 	{yylval = SWITCH ; return KEYWORD;}
"typedef" 	{yylval = TYPEDEF ; return KEYWORD;}
"union" 	{yylval = UNION ; return KEYWORD;}
"unsigned" 	{yylval = UNSIGNED ; return KEYWORD;}
"void" 		{yylval = VOID ; return KEYWORD;}
"volatile" 	{yylval = VOLATILE ; return KEYWORD;}
"while" 	{yylval = WHILE ; return KEYWORD;}
"\\t"       {yylval = TAB; return KEYWORD;}
"\\n"       {yylval = NEWLINE; return KEYWORD;}

"+" 		{yylval = PLUS ;return RELOP;}
"-" 		{yylval = MINUS ;return RELOP;}
"*" 		{yylval = TIMES ;return RELOP;}
"/" 		{yylval = DIV ;return RELOP;}
"%" 		{yylval = MODULO ;return RELOP;}
"=" 		{yylval = EQUALS ;return RELOP;}
"&" 		{yylval = BITAND ;return RELOP;}
"|" 		{yylval = BITOR ;return RELOP;}
"^" 		{yylval = BITXOR ;return RELOP;}
"!" 		{yylval = NOT ;return RELOP;}
"<" 		{yylval = LESS ;return RELOP;}
">" 		{yylval = GREATER ;return RELOP;}
"+=" 		{yylval = PLUSEQ ;return RELOP;}
"-=" 		{yylval = MINUSEQ ;return RELOP;}
"*=" 		{yylval = TIMESEQ ;return RELOP;}
"/=" 		{yylval = DIVEQ ;return RELOP;}
"%=" 		{yylval = MODULOEQ ;return RELOP;}
"&&" 		{yylval = AND ;return RELOP;}
"||" 		{yylval = OR ;return RELOP;}
"==" 		{yylval = REQUALS ;return RELOP;}
"!=" 		{yylval = NOTEQUAL ;return RELOP;}
"<=" 		{yylval = LESSEQ ;return RELOP;}
">=" 		{yylval = GREATEREQ ;return RELOP;}
"++" 		{yylval = INCR ;return RELOP;}
"--" 		{yylval = DECR ;return RELOP;}
"<<" 		{yylval = LSHIFT ;return RELOP;}
">>" 		{yylval = RSHIFT ;return RELOP;}
"&=" 		{yylval = EMPEQ ;return RELOP;}
"|=" 		{yylval = OREQ ;return RELOP;}
"^=" 		{yylval = XOREQ ;return RELOP;}
"<<=" 		{yylval = LSHIFTEQ ;return RELOP;}
">>=" 		{yylval = RSHIFTEQ ;return RELOP;}
"->" 		{yylval = PTRREF ;return RELOP;}

"#" 		{yylval = HASH ; return DELIMITER;} 
"$" 		{yylval = DOLLAR ; return DELIMITER;} 
"@" 		{yylval = ATRATE ; return DELIMITER;} 
"[" 		{yylval = LS ; return DELIMITER;} 
"]" 		{yylval = RS ; return DELIMITER;} 
"{" 		{yylval = LB ; return DELIMITER;} 
"}" 		{yylval = RB ; return DELIMITER;} 
"(" 		{yylval = LP; return DELIMITER;} 
")" 		{yylval = RP ; return DELIMITER;} 
"?" 		{yylval = QMARK ; return DELIMITER;} 
":" 		{yylval = COLON ; return DELIMITER;} 
"." 		{yylval = DOT ; return DELIMITER;} 
";"         {yylval = SEMI ; return DELIMITER;} 
"," 		{yylval = COMMA ; return DELIMITER;} 

{id_err1}   {return ERROR;}
{id_err2}   {return ERROR;}
{num_err}   {return ERROR;}

{id}        {yylval = g_id.detected(); return IDENTIFIER;}
{num}       {yylval = g_num.detected(); return NUM;}


%%

int yywrap(void)
{
	return 1 ;
}
```
### flex编译生成的lex.c
```C++
#line 2 "lex.c"

#line 4 "lex.c"

#define  YY_INT_ALIGNED short int

/* A lexical scanner generated by flex */

#define FLEX_SCANNER
#define YY_FLEX_MAJOR_VERSION 2
#define YY_FLEX_MINOR_VERSION 6
#define YY_FLEX_SUBMINOR_VERSION 4
#if YY_FLEX_SUBMINOR_VERSION > 0
#define FLEX_BETA
#endif

/* First, we deal with  platform-specific or compiler-specific issues. */

/* begin standard C headers. */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

/* end standard C headers. */

/* flex integer type definitions */

#ifndef FLEXINT_H
#define FLEXINT_H

/* C99 systems have <inttypes.h>. Non-C99 systems may or may not. */

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L

/* C99 says to define __STDC_LIMIT_MACROS before including stdint.h,
 * if you want the limit (max/min) macros for int types. 
 */
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS 1
#endif

#include <inttypes.h>
typedef int8_t flex_int8_t;
typedef uint8_t flex_uint8_t;
typedef int16_t flex_int16_t;
typedef uint16_t flex_uint16_t;
typedef int32_t flex_int32_t;
typedef uint32_t flex_uint32_t;
#else
typedef signed char flex_int8_t;
typedef short int flex_int16_t;
typedef int flex_int32_t;
typedef unsigned char flex_uint8_t; 
typedef unsigned short int flex_uint16_t;
typedef unsigned int flex_uint32_t;

/* Limits of integral types. */
#ifndef INT8_MIN
#define INT8_MIN               (-128)
#endif
#ifndef INT16_MIN
#define INT16_MIN              (-32767-1)
#endif
#ifndef INT32_MIN
#define INT32_MIN              (-2147483647-1)
#endif
#ifndef INT8_MAX
#define INT8_MAX               (127)
#endif
#ifndef INT16_MAX
#define INT16_MAX              (32767)
#endif
#ifndef INT32_MAX
#define INT32_MAX              (2147483647)
#endif
#ifndef UINT8_MAX
#define UINT8_MAX              (255U)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX             (65535U)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX             (4294967295U)
#endif

#ifndef SIZE_MAX
#define SIZE_MAX               (~(size_t)0)
#endif

#endif /* ! C99 */

#endif /* ! FLEXINT_H */

/* begin standard C++ headers. */

/* TODO: this is always defined, so inline it */
#define yyconst const

#if defined(__GNUC__) && __GNUC__ >= 3
#define yynoreturn __attribute__((__noreturn__))
#else
#define yynoreturn
#endif

/* Returned upon end-of-file. */
#define YY_NULL 0

/* Promotes a possibly negative, possibly signed char to an
 *   integer in range [0..255] for use as an array index.
 */
#define YY_SC_TO_UI(c) ((YY_CHAR) (c))

/* Enter a start condition.  This macro really ought to take a parameter,
 * but we do it the disgusting crufty way forced on us by the ()-less
 * definition of BEGIN.
 */
#define BEGIN (yy_start) = 1 + 2 *
/* Translate the current start state into a value that can be later handed
 * to BEGIN to return to the state.  The YYSTATE alias is for lex
 * compatibility.
 */
#define YY_START (((yy_start) - 1) / 2)
#define YYSTATE YY_START
/* Action number for EOF rule of a given start state. */
#define YY_STATE_EOF(state) (YY_END_OF_BUFFER + state + 1)
/* Special action meaning "start processing a new file". */
#define YY_NEW_FILE yyrestart( yyin  )
#define YY_END_OF_BUFFER_CHAR 0

/* Size of default input buffer. */
#ifndef YY_BUF_SIZE
#ifdef __ia64__
/* On IA-64, the buffer size is 16k, not 8k.
 * Moreover, YY_BUF_SIZE is 2*YY_READ_BUF_SIZE in the general case.
 * Ditto for the __ia64__ case accordingly.
 */
#define YY_BUF_SIZE 32768
#else
#define YY_BUF_SIZE 16384
#endif /* __ia64__ */
#endif

/* The state buf must be large enough to hold one state per character in the main buffer.
 */
#define YY_STATE_BUF_SIZE   ((YY_BUF_SIZE + 2) * sizeof(yy_state_type))

#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif

#ifndef YY_TYPEDEF_YY_SIZE_T
#define YY_TYPEDEF_YY_SIZE_T
typedef size_t yy_size_t;
#endif

extern int yyleng;

extern FILE *yyin, *yyout;

#define EOB_ACT_CONTINUE_SCAN 0
#define EOB_ACT_END_OF_FILE 1
#define EOB_ACT_LAST_MATCH 2
    
    /* Note: We specifically omit the test for yy_rule_can_match_eol because it requires
     *       access to the local variable yy_act. Since yyless() is a macro, it would break
     *       existing scanners that call yyless() from OUTSIDE yylex.
     *       One obvious solution it to make yy_act a global. I tried that, and saw
     *       a 5% performance hit in a non-yylineno scanner, because yy_act is
     *       normally declared as a register variable-- so it is not worth it.
     */
    #define  YY_LESS_LINENO(n) \
            do { \
                int yyl;\
                for ( yyl = n; yyl < yyleng; ++yyl )\
                    if ( yytext[yyl] == '\n' )\
                        --yylineno;\
            }while(0)
    #define YY_LINENO_REWIND_TO(dst) \
            do {\
                const char *p;\
                for ( p = yy_cp-1; p >= (dst); --p)\
                    if ( *p == '\n' )\
                        --yylineno;\
            }while(0)
    
/* Return all but the first "n" matched characters back to the input stream. */
#define yyless(n) \
	do \
		{ \
		/* Undo effects of setting up yytext. */ \
        int yyless_macro_arg = (n); \
        YY_LESS_LINENO(yyless_macro_arg);\
		*yy_cp = (yy_hold_char); \
		YY_RESTORE_YY_MORE_OFFSET \
		(yy_c_buf_p) = yy_cp = yy_bp + yyless_macro_arg - YY_MORE_ADJ; \
		YY_DO_BEFORE_ACTION; /* set up yytext again */ \
		} \
	while ( 0 )
#define unput(c) yyunput( c, (yytext_ptr)  )

#ifndef YY_STRUCT_YY_BUFFER_STATE
#define YY_STRUCT_YY_BUFFER_STATE
struct yy_buffer_state
	{
	FILE *yy_input_file;

	char *yy_ch_buf;		/* input buffer */
	char *yy_buf_pos;		/* current position in input buffer */

	/* Size of input buffer in bytes, not including room for EOB
	 * characters.
	 */
	int yy_buf_size;

	/* Number of characters read into yy_ch_buf, not including EOB
	 * characters.
	 */
	int yy_n_chars;

	/* Whether we "own" the buffer - i.e., we know we created it,
	 * and can realloc() it to grow it, and should free() it to
	 * delete it.
	 */
	int yy_is_our_buffer;

	/* Whether this is an "interactive" input source; if so, and
	 * if we're using stdio for input, then we want to use getc()
	 * instead of fread(), to make sure we stop fetching input after
	 * each newline.
	 */
	int yy_is_interactive;

	/* Whether we're considered to be at the beginning of a line.
	 * If so, '^' rules will be active on the next match, otherwise
	 * not.
	 */
	int yy_at_bol;

    int yy_bs_lineno; /**< The line count. */
    int yy_bs_column; /**< The column count. */

	/* Whether to try to fill the input buffer when we reach the
	 * end of it.
	 */
	int yy_fill_buffer;

	int yy_buffer_status;

#define YY_BUFFER_NEW 0
#define YY_BUFFER_NORMAL 1
	/* When an EOF's been seen but there's still some text to process
	 * then we mark the buffer as YY_EOF_PENDING, to indicate that we
	 * shouldn't try reading from the input source any more.  We might
	 * still have a bunch of tokens to match, though, because of
	 * possible backing-up.
	 *
	 * When we actually see the EOF, we change the status to "new"
	 * (via yyrestart()), so that the user can continue scanning by
	 * just pointing yyin at a new input file.
	 */
#define YY_BUFFER_EOF_PENDING 2

	};
#endif /* !YY_STRUCT_YY_BUFFER_STATE */

/* Stack of input buffers. */
static size_t yy_buffer_stack_top = 0; /**< index of top of stack. */
static size_t yy_buffer_stack_max = 0; /**< capacity of stack. */
static YY_BUFFER_STATE * yy_buffer_stack = NULL; /**< Stack as an array. */

/* We provide macros for accessing buffer states in case in the
 * future we want to put the buffer states in a more general
 * "scanner state".
 *
 * Returns the top of the stack, or NULL.
 */
#define YY_CURRENT_BUFFER ( (yy_buffer_stack) \
                          ? (yy_buffer_stack)[(yy_buffer_stack_top)] \
                          : NULL)
/* Same as previous macro, but useful when we know that the buffer stack is not
 * NULL or when we need an lvalue. For internal use only.
 */
#define YY_CURRENT_BUFFER_LVALUE (yy_buffer_stack)[(yy_buffer_stack_top)]

/* yy_hold_char holds the character lost when yytext is formed. */
static char yy_hold_char;
static int yy_n_chars;		/* number of characters read into yy_ch_buf */
int yyleng;

/* Points to current character in buffer. */
static char *yy_c_buf_p = NULL;
static int yy_init = 0;		/* whether we need to initialize */
static int yy_start = 0;	/* start state number */

/* Flag which is used to allow yywrap()'s to do buffer switches
 * instead of setting up a fresh yyin.  A bit of a hack ...
 */
static int yy_did_buffer_switch_on_eof;

void yyrestart ( FILE *input_file  );
void yy_switch_to_buffer ( YY_BUFFER_STATE new_buffer  );
YY_BUFFER_STATE yy_create_buffer ( FILE *file, int size  );
void yy_delete_buffer ( YY_BUFFER_STATE b  );
void yy_flush_buffer ( YY_BUFFER_STATE b  );
void yypush_buffer_state ( YY_BUFFER_STATE new_buffer  );
void yypop_buffer_state ( void );

static void yyensure_buffer_stack ( void );
static void yy_load_buffer_state ( void );
static void yy_init_buffer ( YY_BUFFER_STATE b, FILE *file  );
#define YY_FLUSH_BUFFER yy_flush_buffer( YY_CURRENT_BUFFER )

YY_BUFFER_STATE yy_scan_buffer ( char *base, yy_size_t size  );
YY_BUFFER_STATE yy_scan_string ( const char *yy_str  );
YY_BUFFER_STATE yy_scan_bytes ( const char *bytes, int len  );

void *yyalloc ( yy_size_t  );
void *yyrealloc ( void *, yy_size_t  );
void yyfree ( void *  );

#define yy_new_buffer yy_create_buffer
#define yy_set_interactive(is_interactive) \
	{ \
	if ( ! YY_CURRENT_BUFFER ){ \
        yyensure_buffer_stack (); \
		YY_CURRENT_BUFFER_LVALUE =    \
            yy_create_buffer( yyin, YY_BUF_SIZE ); \
	} \
	YY_CURRENT_BUFFER_LVALUE->yy_is_interactive = is_interactive; \
	}
#define yy_set_bol(at_bol) \
	{ \
	if ( ! YY_CURRENT_BUFFER ){\
        yyensure_buffer_stack (); \
		YY_CURRENT_BUFFER_LVALUE =    \
            yy_create_buffer( yyin, YY_BUF_SIZE ); \
	} \
	YY_CURRENT_BUFFER_LVALUE->yy_at_bol = at_bol; \
	}
#define YY_AT_BOL() (YY_CURRENT_BUFFER_LVALUE->yy_at_bol)

/* Begin user sect3 */
typedef flex_uint8_t YY_CHAR;

FILE *yyin = NULL, *yyout = NULL;

typedef int yy_state_type;

extern int yylineno;
int yylineno = 1;

extern char *yytext;
#ifdef yytext_ptr
#undef yytext_ptr
#endif
#define yytext_ptr yytext

static yy_state_type yy_get_previous_state ( void );
static yy_state_type yy_try_NUL_trans ( yy_state_type current_state  );
static int yy_get_next_buffer ( void );
static void yynoreturn yy_fatal_error ( const char* msg  );

/* Done after the current pattern has been matched and before the
 * corresponding action - sets up yytext.
 */
#define YY_DO_BEFORE_ACTION \
	(yytext_ptr) = yy_bp; \
	yyleng = (int) (yy_cp - yy_bp); \
	(yy_hold_char) = *yy_cp; \
	*yy_cp = '\0'; \
	(yy_c_buf_p) = yy_cp;
#define YY_NUM_RULES 93
#define YY_END_OF_BUFFER 94
/* This struct is not used in this scanner,
   but its presence is necessary. */
struct yy_trans_info
	{
	flex_int32_t yy_verify;
	flex_int32_t yy_nxt;
	};
static const flex_int16_t yy_accept[231] =
    {   0,
        0,    0,   94,   93,    1,    2,   50,   93,   74,   75,
       45,   47,   93,   81,   82,   43,   41,   87,   42,   85,
       44,   92,   84,   86,   51,   46,   52,   83,   76,   91,
       77,   93,   78,   49,   91,   91,   91,   91,   91,   91,
       91,   91,   91,   91,   91,   91,   91,   91,   91,   79,
       48,   80,    1,   61,    0,    5,   57,   58,   68,    0,
       55,   64,   92,   53,   65,   54,   73,    0,    4,   56,
        0,   92,   88,   88,   66,   62,   60,   63,   67,   89,
       91,   40,   39,   70,   91,   91,   91,   91,   91,   91,
       14,   91,   91,   91,   91,   91,   91,   22,   91,   91,

       91,   91,   91,   91,   91,   91,   91,   91,   91,   69,
       59,    6,    0,    0,    0,    4,   90,   92,   88,   88,
        0,   88,   71,   72,   89,   91,   91,   91,   91,   91,
       91,   91,   91,   91,   91,   91,   20,   91,   23,   91,
       91,   91,   91,   91,   91,   91,   91,   91,   91,   91,
       91,   91,   91,   91,   92,    3,   90,    7,   91,    9,
       10,   91,   91,   91,   91,   16,   17,   91,   91,   21,
       24,   91,   91,   91,   91,   91,   91,   91,   91,   91,
       91,   91,   36,   91,   91,    8,   11,   91,   91,   91,
       91,   19,   91,   91,   27,   91,   91,   91,   91,   91,

       91,   34,   91,   91,   38,   91,   91,   15,   18,   91,
       26,   28,   29,   30,   31,   32,   91,   91,   91,   91,
       13,   91,   33,   91,   91,   12,   25,   35,   37,    0
    } ;

static const YY_CHAR yy_ec[256] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    2,    3,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    2,    4,    5,    6,    7,    8,    9,   10,   11,
       12,   13,   14,   15,   16,   17,   18,   19,   19,   19,
       19,   19,   19,   19,   19,   19,   19,   20,   21,   22,
       23,   24,   25,   26,   27,   27,   27,   27,   27,   27,
       27,   27,   27,   27,   27,   27,   27,   27,   27,   27,
       27,   27,   27,   27,   27,   27,   27,   27,   27,   27,
       28,   29,   30,   31,   27,    1,   32,   33,   34,   35,

       36,   37,   38,   39,   40,   27,   41,   42,   43,   44,
       45,   46,   27,   47,   48,   49,   50,   51,   52,   53,
       54,   55,   56,   57,   58,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,

        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1
    } ;

static const YY_CHAR yy_meta[59] =
    {   0,
        1,    2,    3,    2,    1,    4,    4,    2,    2,    1,
        2,    2,    2,    2,    2,    2,    5,    2,    6,    2,
        2,    2,    2,    2,    2,    4,    6,    2,    1,    2,
        2,    6,    6,    6,    6,    6,    6,    6,    6,    6,
        6,    6,    6,    6,    6,    6,    6,    6,    6,    6,
        6,    6,    6,    6,    6,    2,    2,    2
    } ;

static const flex_int16_t yy_base[241] =
    {   0,
        0,    0,  223,  802,  215,  802,  185,  194,  802,  802,
      161,   50,    0,  802,  802,  160,   46,  802,   47,  802,
       49,   57,  802,  802,   55,  156,   56,  802,  802,   75,
      802,   39,  802,  155,   78,   80,   84,   60,   80,   75,
       63,   93,   99,  100,  102,  111,   56,  116,  121,  802,
       92,  802,  175,  802,  169,  158,  802,  802,  802,  148,
      802,  802,  140,  802,  802,  802,  802,  139,    0,  802,
      121,  145,  107,  153,   98,  802,  802,  802,   95,  164,
      141,  802,  802,  802,  143,  155,  157,  165,  159,  169,
      165,  172,  177,  182,  197,  200,  201,  209,  212,  211,

      214,  227,  230,  239,  242,  244,  256,  258,  263,  802,
      802,  802,  179,  100,   76,    0,    0,  149,   92,   84,
       78,   56,  802,  802,  273,  274,  276,  278,  286,  290,
      295,  304,  309,  312,  314,  324,  326,  328,  329,  338,
      340,  343,  352,  345,  350,  358,  362,  372,  382,  378,
      388,  397,  398,  396,   49,   51,    0,  397,  403,  404,
      409,  410,  424,  423,  416,  428,  436,  440,  441,  444,
      454,  457,  462,  469,  472,  485,  487,  489,  497,  505,
      499,  507,  504,  512,  511,  524,  525,  528,  529,  543,
      542,  546,  549,  550,  562,  565,  568,  580,  581,  544,

      581,  596,  585,  600,  595,  601,  608,  605,  618,  623,
      622,  626,  630,  636,  640,  643,  644,  656,  657,  669,
      662,  670,  674,  684,  688,  687,  691,  692,  704,  802,
      757,  763,  765,  768,  774,  780,  786,   55,  789,  795
    } ;

static const flex_int16_t yy_def[241] =
    {   0,
      230,    1,  230,  230,  230,  230,  230,  231,  230,  230,
      230,  230,  232,  230,  230,  230,  230,  230,  230,  230,
      230,  233,  230,  230,  230,  230,  230,  230,  230,  234,
      230,  230,  230,  230,  234,  234,  234,   37,   37,   37,
       37,   39,   37,   37,   37,   37,   39,   37,   37,  230,
      230,  230,  230,  230,  231,  231,  230,  230,  230,  230,
      230,  230,  230,  230,  230,  230,  230,  235,  236,  230,
      237,  233,  238,  238,  230,  230,  230,  230,  230,  239,
       37,  230,  230,  230,   37,   37,   37,   37,   39,   39,
       39,   39,   39,   39,   37,   37,   37,   37,   37,   39,

       39,   37,   37,   37,   37,   37,   37,   39,   39,  230,
      230,  230,  230,  235,  235,  236,  240,  230,  238,  238,
      230,  238,  230,  230,  239,   37,   37,   37,   37,   37,
       37,   37,   37,   37,   37,   37,   37,   37,   37,   37,
       37,   37,   37,   39,   39,   39,   39,   39,   39,   37,
       37,   37,   37,   39,  230,  235,  240,   39,   39,   39,
       39,   39,   39,   39,   39,   39,   39,   39,   39,   39,
       39,   39,   39,   39,   39,   37,   37,   37,   37,   37,
       39,   39,   39,   39,   39,   39,   39,   39,   39,   39,
       39,   39,   39,   39,   39,   39,   39,   39,   39,   37,

       37,   37,   39,   39,   39,   39,   39,   39,   39,   39,
       39,   39,   39,   39,   39,   39,   39,   39,   39,   39,
       39,   39,   39,   39,   39,   39,   39,   39,   39,    0,
      230,  230,  230,  230,  230,  230,  230,  230,  230,  230
    } ;

static const flex_int16_t yy_nxt[861] =
    {   0,
        4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
       14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
       24,   25,   26,   27,   28,   29,   30,   31,   32,   33,
       34,   35,   36,   37,   38,   39,   40,   41,   30,   42,
       30,   43,   30,   30,   30,   30,   44,   45,   46,   47,
       48,   49,   30,   30,   30,   50,   51,   52,   58,   62,
      120,   68,   65,  115,   63,   63,   69,  155,   64,   66,
       67,   70,   59,   71,  122,   72,   75,   76,   78,   79,
       80,   80,   82,   80,   80,   80,   80,   83,  115,   80,
       80,   81,   74,  156,   81,   90,  155,   81,   81,  107,

       80,   81,  119,   80,   91,   80,   81,   97,   81,   80,
      119,   81,  115,   81,  110,   87,   95,  124,   81,   96,
      123,   92,   88,   93,   81,  119,   86,   85,   89,   98,
       81,   81,   94,   81,   81,  101,   99,   81,   81,  118,
      102,  103,   81,  100,   81,   81,   81,   81,  111,   81,
      104,  115,   81,  105,   81,   81,   71,  112,   63,  109,
      108,   71,   56,   72,  106,   81,  121,  118,  121,   80,
       80,  122,   81,   56,   81,  113,   53,   84,   77,   81,
       74,   81,   61,   57,  113,   81,   81,   81,   81,   80,
      127,  126,  121,   81,  121,   81,  129,  155,   56,   81,

       81,   81,  130,   81,  128,  131,   81,   54,   81,   81,
       81,   81,   81,   81,  132,   81,   53,   81,   81,  133,
       81,   81,  230,   81,   81,   81,  134,  230,   81,   81,
      135,   81,   81,  230,   81,   81,  230,  230,   81,   81,
       81,  136,  230,   81,   81,   81,  137,   81,  230,  138,
       81,  141,   81,   81,  140,   81,   81,   81,   81,  230,
      139,   81,  142,   81,  230,   81,   81,  144,   81,  230,
      146,  143,  230,   81,   81,   81,  230,   81,  230,  230,
       81,  148,   81,   81,  145,  147,   81,   81,   81,  149,
      230,  230,  230,  230,   81,  150,  230,  152,  230,  153,

       81,   81,  154,  151,   81,   81,   81,  159,  230,   81,
       81,  230,   81,  160,   81,   81,   81,   81,  158,  230,
       81,   81,   81,  230,   81,  230,  164,  230,   81,  230,
       81,  230,  161,   81,   81,   81,  165,  162,  163,   81,
       81,  230,   81,   81,  166,   81,  230,   81,   81,  168,
       81,  230,   81,   81,  167,  169,   81,   81,   81,   81,
       81,  230,   81,  230,   81,  230,   81,   81,   81,   81,
       81,   81,  170,   81,   81,  171,   81,  230,   81,  172,
      230,   81,   81,   81,   81,  176,   81,   81,  175,  230,
       81,   81,  173,   81,  230,  230,   81,   81,  174,   81,

      230,   81,   81,   81,  230,   81,  177,  230,  230,   81,
       81,  178,  230,   81,   81,   81,   81,  180,  230,   81,
      179,  230,  181,   81,   81,   81,   81,  182,   81,  184,
      230,  183,   81,  230,   81,   81,   81,  185,   81,   81,
       81,   81,   81,  186,   81,   81,   81,   81,   81,   81,
       81,   81,   81,   81,  230,   81,   81,  190,  187,   81,
      230,   81,   81,  188,   81,   81,   81,   81,   81,   81,
      230,   81,  189,  230,  230,   81,   81,   81,  230,   81,
       81,   81,   81,   81,   81,   81,  191,   81,   81,  192,
      230,  230,   81,   81,  230,   81,   81,   81,   81,  230,

       81,  230,  230,   81,  193,   81,   81,  196,  194,   81,
       81,  230,   81,   81,   81,   81,   81,  195,   81,  230,
       81,   81,  199,   81,   81,   81,  198,   81,   81,  197,
      200,   81,  230,   81,  230,   81,   81,  230,  230,  201,
       81,   81,  202,   81,  203,   81,  205,   81,   81,   81,
       81,   81,   81,   81,   81,   81,   81,  230,  230,   81,
      204,  230,  230,   81,   81,   81,   81,   81,   81,   81,
      207,  206,   81,  230,  230,   81,   81,   81,  208,  230,
       81,   81,  216,   81,   81,  209,   81,   81,   81,   81,
       81,   81,   81,  211,   81,   81,  230,  210,   81,  212,

      230,   81,   81,   81,  213,   81,   81,  230,   81,   81,
      230,   81,   81,  214,   81,  230,  217,   81,  230,   81,
       81,   81,   81,   81,   81,   81,   81,   81,  218,  215,
      230,  230,   81,   81,   81,  230,   81,   81,   81,  219,
       81,   81,   81,   81,   81,  230,   81,   81,   81,   81,
      220,   81,   81,   81,  230,  230,  221,   81,  222,   81,
       81,   81,  230,   81,   81,   81,   81,   81,  230,   81,
       81,   81,  230,   81,   81,   81,  230,   81,   81,   81,
      223,   81,   81,   81,   81,   81,   81,   81,   81,  230,
      230,  224,   81,  230,  230,   81,   81,   81,  225,   81,

       81,  230,  230,   81,  226,   81,  230,  230,   81,   81,
       81,   81,   81,   81,   81,   81,  227,   81,  228,  230,
      230,   81,   81,  229,  230,   81,   81,   81,   81,   81,
       81,   81,   81,   81,   81,   81,   81,  230,  230,   81,
       81,  230,  230,   81,   81,   81,  230,   81,  230,  230,
      230,  230,  230,  230,  230,  230,   81,   55,   55,  230,
       55,   55,   55,   60,   60,  230,   60,   60,   60,   73,
       73,   81,  230,   81,  114,  114,  230,  114,  114,  114,
      116,  116,  230,  116,  116,  116,  117,  117,  117,  117,
      117,  117,  125,  230,  125,  157,  230,  230,  230,  230,

      157,    3,  230,  230,  230,  230,  230,  230,  230,  230,
      230,  230,  230,  230,  230,  230,  230,  230,  230,  230,
      230,  230,  230,  230,  230,  230,  230,  230,  230,  230,
      230,  230,  230,  230,  230,  230,  230,  230,  230,  230,
      230,  230,  230,  230,  230,  230,  230,  230,  230,  230,
      230,  230,  230,  230,  230,  230,  230,  230,  230,  230
    } ;

static const flex_int16_t yy_chk[861] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,   12,   17,
      238,   21,   19,  156,   17,   19,   21,  155,   17,   19,
       19,   21,   12,   22,  122,   22,   25,   25,   27,   27,
       30,   30,   32,   35,   35,   36,   36,   32,  115,   37,
       37,   38,   22,  115,   41,   38,  121,   47,   38,   47,

       30,   41,  120,   35,   38,   36,   40,   41,   47,   37,
      119,   39,  114,   40,   51,   37,   40,   79,   39,   40,
       75,   39,   37,   39,   39,   73,   36,   35,   37,   42,
       43,   44,   39,   45,   42,   44,   42,   43,   44,   71,
       45,   45,   46,   43,   44,   42,   45,   48,   51,   46,
       45,   68,   49,   45,   48,   46,   63,   60,   63,   49,
       48,   72,   56,   72,   46,   49,   74,  118,   74,   80,
       80,   74,   81,   55,   85,   63,   53,   34,   26,   81,
       72,   85,   16,   11,  118,   81,   86,   85,   87,   80,
       86,   85,  113,   86,  113,   87,   88,  113,    8,   86,

       89,   87,   89,   88,   87,   90,   91,    7,   91,   88,
       90,   89,   90,   92,   91,   92,    5,   91,   93,   92,
       93,   90,    3,   94,   92,   94,   93,    0,   95,   93,
       94,   96,   97,    0,   94,   95,    0,    0,   96,   97,
       98,   95,    0,   99,   96,   97,   96,   98,    0,   97,
       99,  101,  100,   98,  100,  101,   99,  101,  102,    0,
       99,  103,  101,  100,    0,  102,  101,  103,  103,    0,
      104,  102,    0,  105,  103,  106,    0,  104,  125,  125,
      105,  105,  106,  104,  103,  104,  105,  107,  106,  106,
        0,    0,    0,    0,  107,  107,    0,  108,  125,  108,

      107,  108,  109,  107,  109,  126,  109,  127,    0,  128,
      108,    0,  126,  128,  127,  109,  128,  129,  126,    0,
      127,  130,  128,    0,  129,    0,  131,    0,  130,    0,
      129,    0,  129,  131,  130,  132,  132,  130,  130,  131,
      133,    0,  132,  134,  133,  135,    0,  133,  132,  135,
      134,    0,  135,  133,  134,  136,  134,  137,  135,  138,
      139,    0,  136,    0,  137,    0,  138,  139,  136,  140,
      137,  141,  138,  139,  142,  140,  140,    0,  141,  141,
        0,  142,  140,  143,  141,  145,  144,  142,  144,    0,
      143,  145,  142,  145,    0,    0,  143,  144,  143,  146,

        0,  146,  145,  147,    0,  147,  146,    0,    0,  150,
      146,  147,    0,  148,  147,  148,  150,  149,    0,  151,
      148,    0,  150,  149,  148,  149,  151,  151,  152,  153,
        0,  152,  151,    0,  149,  152,  153,  154,  158,  154,
      158,  152,  153,  159,  159,  160,  159,  160,  154,  158,
      161,  162,  161,  162,    0,  159,  160,  165,  162,  165,
        0,  161,  162,  163,  164,  163,  164,  163,  165,  166,
        0,  166,  164,    0,    0,  164,  163,  167,    0,  167,
      166,  168,  169,  168,  169,  170,  168,  170,  167,  169,
        0,    0,  168,  169,    0,  171,  170,  171,  172,    0,

      172,    0,    0,  173,  172,  173,  171,  175,  173,  172,
      174,    0,  174,  175,  173,  175,  176,  174,  177,    0,
      178,  174,  178,  176,  175,  177,  177,  178,  179,  176,
      179,  177,    0,  178,    0,  179,  180,    0,    0,  180,
      181,  179,  181,  180,  182,  183,  185,  183,  182,  180,
      182,  181,  185,  184,  185,  184,  183,    0,    0,  182,
      184,    0,    0,  185,  184,  186,  187,  186,  187,  188,
      189,  188,  189,    0,    0,  200,  186,  187,  190,    0,
      188,  189,  200,  191,  190,  191,  190,  192,  200,  192,
      193,  194,  193,  194,  191,  190,    0,  193,  192,  196,

        0,  193,  194,  195,  197,  195,  196,    0,  196,  197,
        0,  197,  201,  198,  195,    0,  201,  196,    0,  201,
      197,  198,  199,  198,  199,  201,  203,  202,  203,  199,
        0,    0,  198,  199,  202,    0,  205,  203,  205,  204,
      202,  204,  206,  204,  206,    0,  208,  205,  208,  207,
      206,  207,  204,  206,    0,    0,  207,  208,  210,  209,
      207,  209,    0,  211,  210,  211,  210,  212,    0,  212,
      209,  213,    0,  213,  211,  210,    0,  214,  212,  214,
      217,  215,  213,  215,  216,  217,  216,  217,  214,    0,
        0,  218,  215,    0,    0,  216,  217,  218,  219,  218,

      219,    0,    0,  221,  220,  221,    0,    0,  218,  219,
      220,  222,  220,  222,  221,  223,  222,  223,  224,    0,
        0,  220,  222,  225,    0,  224,  223,  224,  226,  225,
      226,  225,  227,  228,  227,  228,  224,    0,    0,  226,
      225,    0,    0,  227,  228,  229,    0,  229,    0,    0,
        0,    0,    0,    0,    0,    0,  229,  231,  231,    0,
      231,  231,  231,  232,  232,    0,  232,  232,  232,  233,
      233,  234,    0,  234,  235,  235,    0,  235,  235,  235,
      236,  236,    0,  236,  236,  236,  237,  237,  237,  237,
      237,  237,  239,    0,  239,  240,    0,    0,    0,    0,

      240,  230,  230,  230,  230,  230,  230,  230,  230,  230,
      230,  230,  230,  230,  230,  230,  230,  230,  230,  230,
      230,  230,  230,  230,  230,  230,  230,  230,  230,  230,
      230,  230,  230,  230,  230,  230,  230,  230,  230,  230,
      230,  230,  230,  230,  230,  230,  230,  230,  230,  230,
      230,  230,  230,  230,  230,  230,  230,  230,  230,  230
    } ;

/* Table of booleans, true if rule could match eol. */
static const flex_int32_t yy_rule_can_match_eol[94] =
    {   0,
0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,     };

static yy_state_type yy_last_accepting_state;
static char *yy_last_accepting_cpos;

extern int yy_flex_debug;
int yy_flex_debug = 0;

/* The intent behind this definition is that it'll catch
 * any uses of REJECT which flex missed.
 */
#define REJECT reject_used_but_not_detected
#define yymore() yymore_used_but_not_detected
#define YY_MORE_ADJ 0
#define YY_RESTORE_YY_MORE_OFFSET
char *yytext;
#line 1 "lex.l"
#line 2 "lex.l"
    #include"token.h"
    num g_num;
    id g_id;
    int yylval;
#line 740 "lex.c"
#line 741 "lex.c"

#define INITIAL 0

#ifndef YY_NO_UNISTD_H
/* Special case for "unistd.h", since it is non-ANSI. We include it way
 * down here because we want the user's section 1 to have been scanned first.
 * The user has a chance to override it with an option.
 */
#include <unistd.h>
#endif

#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE void *
#endif

static int yy_init_globals ( void );

/* Accessor methods to globals.
   These are made visible to non-reentrant scanners for convenience. */

int yylex_destroy ( void );

int yyget_debug ( void );

void yyset_debug ( int debug_flag  );

YY_EXTRA_TYPE yyget_extra ( void );

void yyset_extra ( YY_EXTRA_TYPE user_defined  );

FILE *yyget_in ( void );

void yyset_in  ( FILE * _in_str  );

FILE *yyget_out ( void );

void yyset_out  ( FILE * _out_str  );

			int yyget_leng ( void );

char *yyget_text ( void );

int yyget_lineno ( void );

void yyset_lineno ( int _line_number  );

/* Macros after this point can all be overridden by user definitions in
 * section 1.
 */

#ifndef YY_SKIP_YYWRAP
#ifdef __cplusplus
extern "C" int yywrap ( void );
#else
extern int yywrap ( void );
#endif
#endif

#ifndef YY_NO_UNPUT
    
    static void yyunput ( int c, char *buf_ptr  );
    
#endif

#ifndef yytext_ptr
static void yy_flex_strncpy ( char *, const char *, int );
#endif

#ifdef YY_NEED_STRLEN
static int yy_flex_strlen ( const char * );
#endif

#ifndef YY_NO_INPUT
#ifdef __cplusplus
static int yyinput ( void );
#else
static int input ( void );
#endif

#endif

/* Amount of stuff to slurp up with each read. */
#ifndef YY_READ_BUF_SIZE
#ifdef __ia64__
/* On IA-64, the buffer size is 16k, not 8k */
#define YY_READ_BUF_SIZE 16384
#else
#define YY_READ_BUF_SIZE 8192
#endif /* __ia64__ */
#endif

/* Copy whatever the last rule matched to the standard output. */
#ifndef ECHO
/* This used to be an fputs(), but since the string might contain NUL's,
 * we now use fwrite().
 */
#define ECHO do { if (fwrite( yytext, (size_t) yyleng, 1, yyout )) {} } while (0)
#endif

/* Gets input and stuffs it into "buf".  number of characters read, or YY_NULL,
 * is returned in "result".
 */
#ifndef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( YY_CURRENT_BUFFER_LVALUE->yy_is_interactive ) \
		{ \
		int c = '*'; \
		int n; \
		for ( n = 0; n < max_size && \
			     (c = getc( yyin )) != EOF && c != '\n'; ++n ) \
			buf[n] = (char) c; \
		if ( c == '\n' ) \
			buf[n++] = (char) c; \
		if ( c == EOF && ferror( yyin ) ) \
			YY_FATAL_ERROR( "input in flex scanner failed" ); \
		result = n; \
		} \
	else \
		{ \
		errno=0; \
		while ( (result = (int) fread(buf, 1, (yy_size_t) max_size, yyin)) == 0 && ferror(yyin)) \
			{ \
			if( errno != EINTR) \
				{ \
				YY_FATAL_ERROR( "input in flex scanner failed" ); \
				break; \
				} \
			errno=0; \
			clearerr(yyin); \
			} \
		}\
\

#endif

/* No semi-colon after return; correct usage is to write "yyterminate();" -
 * we don't want an extra ';' after the "return" because that will cause
 * some compilers to complain about unreachable statements.
 */
#ifndef yyterminate
#define yyterminate() return YY_NULL
#endif

/* Number of entries by which start-condition stack grows. */
#ifndef YY_START_STACK_INCR
#define YY_START_STACK_INCR 25
#endif

/* Report a fatal error. */
#ifndef YY_FATAL_ERROR
#define YY_FATAL_ERROR(msg) yy_fatal_error( msg )
#endif

/* end tables serialization structures and prototypes */

/* Default declaration of generated scanner - a define so the user can
 * easily add parameters.
 */
#ifndef YY_DECL
#define YY_DECL_IS_OURS 1

extern int yylex (void);

#define YY_DECL int yylex (void)
#endif /* !YY_DECL */

/* Code executed at the beginning of each rule, after yytext and yyleng
 * have been set up.
 */
#ifndef YY_USER_ACTION
#define YY_USER_ACTION
#endif

/* Code executed at the end of each rule. */
#ifndef YY_BREAK
#define YY_BREAK /*LINTED*/break;
#endif

#define YY_RULE_SETUP \
	YY_USER_ACTION

/** The main scanner function which does all the work.
 */
YY_DECL
{
	yy_state_type yy_current_state;
	char *yy_cp, *yy_bp;
	int yy_act;
    
	if ( !(yy_init) )
		{
		(yy_init) = 1;

#ifdef YY_USER_INIT
		YY_USER_INIT;
#endif

		if ( ! (yy_start) )
			(yy_start) = 1;	/* first start state */

		if ( ! yyin )
			yyin = stdin;

		if ( ! yyout )
			yyout = stdout;

		if ( ! YY_CURRENT_BUFFER ) {
			yyensure_buffer_stack ();
			YY_CURRENT_BUFFER_LVALUE =
				yy_create_buffer( yyin, YY_BUF_SIZE );
		}

		yy_load_buffer_state(  );
		}

	{
#line 23 "lex.l"



#line 962 "lex.c"

	while ( /*CONSTCOND*/1 )		/* loops until end-of-file is reached */
		{
		yy_cp = (yy_c_buf_p);

		/* Support of yytext. */
		*yy_cp = (yy_hold_char);

		/* yy_bp points to the position in yy_ch_buf of the start of
		 * the current run.
		 */
		yy_bp = yy_cp;

		yy_current_state = (yy_start);
yy_match:
		do
			{
			YY_CHAR yy_c = yy_ec[YY_SC_TO_UI(*yy_cp)] ;
			if ( yy_accept[yy_current_state] )
				{
				(yy_last_accepting_state) = yy_current_state;
				(yy_last_accepting_cpos) = yy_cp;
				}
			while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
				{
				yy_current_state = (int) yy_def[yy_current_state];
				if ( yy_current_state >= 231 )
					yy_c = yy_meta[yy_c];
				}
			yy_current_state = yy_nxt[yy_base[yy_current_state] + yy_c];
			++yy_cp;
			}
		while ( yy_base[yy_current_state] != 802 );

yy_find_action:
		yy_act = yy_accept[yy_current_state];
		if ( yy_act == 0 )
			{ /* have to back up */
			yy_cp = (yy_last_accepting_cpos);
			yy_current_state = (yy_last_accepting_state);
			yy_act = yy_accept[yy_current_state];
			}

		YY_DO_BEFORE_ACTION;

		if ( yy_act != YY_END_OF_BUFFER && yy_rule_can_match_eol[yy_act] )
			{
			int yyl;
			for ( yyl = 0; yyl < yyleng; ++yyl )
				if ( yytext[yyl] == '\n' )
					
    yylineno++;
;
			}

do_action:	/* This label is used only to access EOF actions. */

		switch ( yy_act )
	{ /* beginning of action switch */
			case 0: /* must back up */
			/* undo the effects of YY_DO_BEFORE_ACTION */
			*yy_cp = (yy_hold_char);
			yy_cp = (yy_last_accepting_cpos);
			yy_current_state = (yy_last_accepting_state);
			goto yy_find_action;

case 1:
YY_RULE_SETUP
#line 26 "lex.l"
;
	YY_BREAK
case 2:
/* rule 2 can match eol */
YY_RULE_SETUP
#line 27 "lex.l"
line::inc_cnt();
	YY_BREAK
case 3:
YY_RULE_SETUP
#line 28 "lex.l"
;
	YY_BREAK
case 4:
YY_RULE_SETUP
#line 29 "lex.l"
;
	YY_BREAK
case 5:
YY_RULE_SETUP
#line 30 "lex.l"
return STRINGS;
	YY_BREAK
case 6:
YY_RULE_SETUP
#line 31 "lex.l"
return CHARACTERS;
	YY_BREAK
case 7:
YY_RULE_SETUP
#line 33 "lex.l"
{yylval = AUTO ; return KEYWORD;}
	YY_BREAK
case 8:
YY_RULE_SETUP
#line 34 "lex.l"
{yylval = BREAK ; return KEYWORD;}
	YY_BREAK
case 9:
YY_RULE_SETUP
#line 35 "lex.l"
{yylval = CASE ; return KEYWORD;}
	YY_BREAK
case 10:
YY_RULE_SETUP
#line 36 "lex.l"
{yylval = CHAR ; return KEYWORD;}
	YY_BREAK
case 11:
YY_RULE_SETUP
#line 37 "lex.l"
{yylval = CONST ; return KEYWORD;}
	YY_BREAK
case 12:
YY_RULE_SETUP
#line 38 "lex.l"
{yylval = CONTINUE ; return KEYWORD;}
	YY_BREAK
case 13:
YY_RULE_SETUP
#line 39 "lex.l"
{yylval = DEFAULT ; return KEYWORD;}
	YY_BREAK
case 14:
YY_RULE_SETUP
#line 40 "lex.l"
{yylval = DO ; return KEYWORD;}
	YY_BREAK
case 15:
YY_RULE_SETUP
#line 41 "lex.l"
{yylval = DOUBLE ; return KEYWORD;}
	YY_BREAK
case 16:
YY_RULE_SETUP
#line 42 "lex.l"
{yylval = ELSE ; return KEYWORD;}
	YY_BREAK
case 17:
YY_RULE_SETUP
#line 43 "lex.l"
{yylval = ENUM ; return KEYWORD;}
	YY_BREAK
case 18:
YY_RULE_SETUP
#line 44 "lex.l"
{yylval = EXTERN ; return KEYWORD;}
	YY_BREAK
case 19:
YY_RULE_SETUP
#line 45 "lex.l"
{yylval = FLOAT ; return KEYWORD;}
	YY_BREAK
case 20:
YY_RULE_SETUP
#line 46 "lex.l"
{yylval = FOR ; return KEYWORD;}
	YY_BREAK
case 21:
YY_RULE_SETUP
#line 47 "lex.l"
{yylval = GOTO ; return KEYWORD;}
	YY_BREAK
case 22:
YY_RULE_SETUP
#line 48 "lex.l"
{yylval = IF ; return KEYWORD;}
	YY_BREAK
case 23:
YY_RULE_SETUP
#line 49 "lex.l"
{yylval = INT ; return KEYWORD;}
	YY_BREAK
case 24:
YY_RULE_SETUP
#line 50 "lex.l"
{yylval = LONG ; return KEYWORD;}
	YY_BREAK
case 25:
YY_RULE_SETUP
#line 51 "lex.l"
{yylval = REGISTER ; return KEYWORD;}
	YY_BREAK
case 26:
YY_RULE_SETUP
#line 52 "lex.l"
{yylval = RETURN ; return KEYWORD;}
	YY_BREAK
case 27:
YY_RULE_SETUP
#line 53 "lex.l"
{yylval = SHORT ; return KEYWORD;}
	YY_BREAK
case 28:
YY_RULE_SETUP
#line 54 "lex.l"
{yylval = SIGNED ; return KEYWORD;}
	YY_BREAK
case 29:
YY_RULE_SETUP
#line 55 "lex.l"
{yylval = SIZEOF ; return KEYWORD;}
	YY_BREAK
case 30:
YY_RULE_SETUP
#line 56 "lex.l"
{yylval = STATIC ; return KEYWORD;}
	YY_BREAK
case 31:
YY_RULE_SETUP
#line 57 "lex.l"
{yylval = STRUCT ; return KEYWORD;}
	YY_BREAK
case 32:
YY_RULE_SETUP
#line 58 "lex.l"
{yylval = SWITCH ; return KEYWORD;}
	YY_BREAK
case 33:
YY_RULE_SETUP
#line 59 "lex.l"
{yylval = TYPEDEF ; return KEYWORD;}
	YY_BREAK
case 34:
YY_RULE_SETUP
#line 60 "lex.l"
{yylval = UNION ; return KEYWORD;}
	YY_BREAK
case 35:
YY_RULE_SETUP
#line 61 "lex.l"
{yylval = UNSIGNED ; return KEYWORD;}
	YY_BREAK
case 36:
YY_RULE_SETUP
#line 62 "lex.l"
{yylval = VOID ; return KEYWORD;}
	YY_BREAK
case 37:
YY_RULE_SETUP
#line 63 "lex.l"
{yylval = VOLATILE ; return KEYWORD;}
	YY_BREAK
case 38:
YY_RULE_SETUP
#line 64 "lex.l"
{yylval = WHILE ; return KEYWORD;}
	YY_BREAK
case 39:
YY_RULE_SETUP
#line 65 "lex.l"
{yylval = TAB; return KEYWORD;}
	YY_BREAK
case 40:
YY_RULE_SETUP
#line 66 "lex.l"
{yylval = NEWLINE; return KEYWORD;}
	YY_BREAK
case 41:
YY_RULE_SETUP
#line 68 "lex.l"
{yylval = PLUS ;return RELOP;}
	YY_BREAK
case 42:
YY_RULE_SETUP
#line 69 "lex.l"
{yylval = MINUS ;return RELOP;}
	YY_BREAK
case 43:
YY_RULE_SETUP
#line 70 "lex.l"
{yylval = TIMES ;return RELOP;}
	YY_BREAK
case 44:
YY_RULE_SETUP
#line 71 "lex.l"
{yylval = DIV ;return RELOP;}
	YY_BREAK
case 45:
YY_RULE_SETUP
#line 72 "lex.l"
{yylval = MODULO ;return RELOP;}
	YY_BREAK
case 46:
YY_RULE_SETUP
#line 73 "lex.l"
{yylval = EQUALS ;return RELOP;}
	YY_BREAK
case 47:
YY_RULE_SETUP
#line 74 "lex.l"
{yylval = BITAND ;return RELOP;}
	YY_BREAK
case 48:
YY_RULE_SETUP
#line 75 "lex.l"
{yylval = BITOR ;return RELOP;}
	YY_BREAK
case 49:
YY_RULE_SETUP
#line 76 "lex.l"
{yylval = BITXOR ;return RELOP;}
	YY_BREAK
case 50:
YY_RULE_SETUP
#line 77 "lex.l"
{yylval = NOT ;return RELOP;}
	YY_BREAK
case 51:
YY_RULE_SETUP
#line 78 "lex.l"
{yylval = LESS ;return RELOP;}
	YY_BREAK
case 52:
YY_RULE_SETUP
#line 79 "lex.l"
{yylval = GREATER ;return RELOP;}
	YY_BREAK
case 53:
YY_RULE_SETUP
#line 80 "lex.l"
{yylval = PLUSEQ ;return RELOP;}
	YY_BREAK
case 54:
YY_RULE_SETUP
#line 81 "lex.l"
{yylval = MINUSEQ ;return RELOP;}
	YY_BREAK
case 55:
YY_RULE_SETUP
#line 82 "lex.l"
{yylval = TIMESEQ ;return RELOP;}
	YY_BREAK
case 56:
YY_RULE_SETUP
#line 83 "lex.l"
{yylval = DIVEQ ;return RELOP;}
	YY_BREAK
case 57:
YY_RULE_SETUP
#line 84 "lex.l"
{yylval = MODULOEQ ;return RELOP;}
	YY_BREAK
case 58:
YY_RULE_SETUP
#line 85 "lex.l"
{yylval = AND ;return RELOP;}
	YY_BREAK
case 59:
YY_RULE_SETUP
#line 86 "lex.l"
{yylval = OR ;return RELOP;}
	YY_BREAK
case 60:
YY_RULE_SETUP
#line 87 "lex.l"
{yylval = REQUALS ;return RELOP;}
	YY_BREAK
case 61:
YY_RULE_SETUP
#line 88 "lex.l"
{yylval = NOTEQUAL ;return RELOP;}
	YY_BREAK
case 62:
YY_RULE_SETUP
#line 89 "lex.l"
{yylval = LESSEQ ;return RELOP;}
	YY_BREAK
case 63:
YY_RULE_SETUP
#line 90 "lex.l"
{yylval = GREATEREQ ;return RELOP;}
	YY_BREAK
case 64:
YY_RULE_SETUP
#line 91 "lex.l"
{yylval = INCR ;return RELOP;}
	YY_BREAK
case 65:
YY_RULE_SETUP
#line 92 "lex.l"
{yylval = DECR ;return RELOP;}
	YY_BREAK
case 66:
YY_RULE_SETUP
#line 93 "lex.l"
{yylval = LSHIFT ;return RELOP;}
	YY_BREAK
case 67:
YY_RULE_SETUP
#line 94 "lex.l"
{yylval = RSHIFT ;return RELOP;}
	YY_BREAK
case 68:
YY_RULE_SETUP
#line 95 "lex.l"
{yylval = EMPEQ ;return RELOP;}
	YY_BREAK
case 69:
YY_RULE_SETUP
#line 96 "lex.l"
{yylval = OREQ ;return RELOP;}
	YY_BREAK
case 70:
YY_RULE_SETUP
#line 97 "lex.l"
{yylval = XOREQ ;return RELOP;}
	YY_BREAK
case 71:
YY_RULE_SETUP
#line 98 "lex.l"
{yylval = LSHIFTEQ ;return RELOP;}
	YY_BREAK
case 72:
YY_RULE_SETUP
#line 99 "lex.l"
{yylval = RSHIFTEQ ;return RELOP;}
	YY_BREAK
case 73:
YY_RULE_SETUP
#line 100 "lex.l"
{yylval = PTRREF ;return RELOP;}
	YY_BREAK
case 74:
YY_RULE_SETUP
#line 102 "lex.l"
{yylval = HASH ; return DELIMITER;} 
	YY_BREAK
case 75:
YY_RULE_SETUP
#line 103 "lex.l"
{yylval = DOLLAR ; return DELIMITER;} 
	YY_BREAK
case 76:
YY_RULE_SETUP
#line 104 "lex.l"
{yylval = ATRATE ; return DELIMITER;} 
	YY_BREAK
case 77:
YY_RULE_SETUP
#line 105 "lex.l"
{yylval = LS ; return DELIMITER;} 
	YY_BREAK
case 78:
YY_RULE_SETUP
#line 106 "lex.l"
{yylval = RS ; return DELIMITER;} 
	YY_BREAK
case 79:
YY_RULE_SETUP
#line 107 "lex.l"
{yylval = LB ; return DELIMITER;} 
	YY_BREAK
case 80:
YY_RULE_SETUP
#line 108 "lex.l"
{yylval = RB ; return DELIMITER;} 
	YY_BREAK
case 81:
YY_RULE_SETUP
#line 109 "lex.l"
{yylval = LP; return DELIMITER;} 
	YY_BREAK
case 82:
YY_RULE_SETUP
#line 110 "lex.l"
{yylval = RP ; return DELIMITER;} 
	YY_BREAK
case 83:
YY_RULE_SETUP
#line 111 "lex.l"
{yylval = QMARK ; return DELIMITER;} 
	YY_BREAK
case 84:
YY_RULE_SETUP
#line 112 "lex.l"
{yylval = COLON ; return DELIMITER;} 
	YY_BREAK
case 85:
YY_RULE_SETUP
#line 113 "lex.l"
{yylval = DOT ; return DELIMITER;} 
	YY_BREAK
case 86:
YY_RULE_SETUP
#line 114 "lex.l"
{yylval = SEMI ; return DELIMITER;} 
	YY_BREAK
case 87:
YY_RULE_SETUP
#line 115 "lex.l"
{yylval = COMMA ; return DELIMITER;} 
	YY_BREAK
case 88:
YY_RULE_SETUP
#line 117 "lex.l"
{return ERROR;}
	YY_BREAK
case 89:
YY_RULE_SETUP
#line 118 "lex.l"
{return ERROR;}
	YY_BREAK
case 90:
/* rule 90 can match eol */
YY_RULE_SETUP
#line 119 "lex.l"
{return ERROR;}
	YY_BREAK
case 91:
YY_RULE_SETUP
#line 121 "lex.l"
{yylval = g_id.detected(); return IDENTIFIER;}
	YY_BREAK
case 92:
YY_RULE_SETUP
#line 122 "lex.l"
{yylval = g_num.detected(); return NUM;}
	YY_BREAK
case 93:
YY_RULE_SETUP
#line 125 "lex.l"
ECHO;
	YY_BREAK
#line 1496 "lex.c"
case YY_STATE_EOF(INITIAL):
	yyterminate();

	case YY_END_OF_BUFFER:
		{
		/* Amount of text matched not including the EOB char. */
		int yy_amount_of_matched_text = (int) (yy_cp - (yytext_ptr)) - 1;

		/* Undo the effects of YY_DO_BEFORE_ACTION. */
		*yy_cp = (yy_hold_char);
		YY_RESTORE_YY_MORE_OFFSET

		if ( YY_CURRENT_BUFFER_LVALUE->yy_buffer_status == YY_BUFFER_NEW )
			{
			/* We're scanning a new file or input source.  It's
			 * possible that this happened because the user
			 * just pointed yyin at a new source and called
			 * yylex().  If so, then we have to assure
			 * consistency between YY_CURRENT_BUFFER and our
			 * globals.  Here is the right place to do so, because
			 * this is the first action (other than possibly a
			 * back-up) that will match for the new input source.
			 */
			(yy_n_chars) = YY_CURRENT_BUFFER_LVALUE->yy_n_chars;
			YY_CURRENT_BUFFER_LVALUE->yy_input_file = yyin;
			YY_CURRENT_BUFFER_LVALUE->yy_buffer_status = YY_BUFFER_NORMAL;
			}

		/* Note that here we test for yy_c_buf_p "<=" to the position
		 * of the first EOB in the buffer, since yy_c_buf_p will
		 * already have been incremented past the NUL character
		 * (since all states make transitions on EOB to the
		 * end-of-buffer state).  Contrast this with the test
		 * in input().
		 */
		if ( (yy_c_buf_p) <= &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] )
			{ /* This was really a NUL. */
			yy_state_type yy_next_state;

			(yy_c_buf_p) = (yytext_ptr) + yy_amount_of_matched_text;

			yy_current_state = yy_get_previous_state(  );

			/* Okay, we're now positioned to make the NUL
			 * transition.  We couldn't have
			 * yy_get_previous_state() go ahead and do it
			 * for us because it doesn't know how to deal
			 * with the possibility of jamming (and we don't
			 * want to build jamming into it because then it
			 * will run more slowly).
			 */

			yy_next_state = yy_try_NUL_trans( yy_current_state );

			yy_bp = (yytext_ptr) + YY_MORE_ADJ;

			if ( yy_next_state )
				{
				/* Consume the NUL. */
				yy_cp = ++(yy_c_buf_p);
				yy_current_state = yy_next_state;
				goto yy_match;
				}

			else
				{
				yy_cp = (yy_c_buf_p);
				goto yy_find_action;
				}
			}

		else switch ( yy_get_next_buffer(  ) )
			{
			case EOB_ACT_END_OF_FILE:
				{
				(yy_did_buffer_switch_on_eof) = 0;

				if ( yywrap(  ) )
					{
					/* Note: because we've taken care in
					 * yy_get_next_buffer() to have set up
					 * yytext, we can now set up
					 * yy_c_buf_p so that if some total
					 * hoser (like flex itself) wants to
					 * call the scanner after we return the
					 * YY_NULL, it'll still work - another
					 * YY_NULL will get returned.
					 */
					(yy_c_buf_p) = (yytext_ptr) + YY_MORE_ADJ;

					yy_act = YY_STATE_EOF(YY_START);
					goto do_action;
					}

				else
					{
					if ( ! (yy_did_buffer_switch_on_eof) )
						YY_NEW_FILE;
					}
				break;
				}

			case EOB_ACT_CONTINUE_SCAN:
				(yy_c_buf_p) =
					(yytext_ptr) + yy_amount_of_matched_text;

				yy_current_state = yy_get_previous_state(  );

				yy_cp = (yy_c_buf_p);
				yy_bp = (yytext_ptr) + YY_MORE_ADJ;
				goto yy_match;

			case EOB_ACT_LAST_MATCH:
				(yy_c_buf_p) =
				&YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)];

				yy_current_state = yy_get_previous_state(  );

				yy_cp = (yy_c_buf_p);
				yy_bp = (yytext_ptr) + YY_MORE_ADJ;
				goto yy_find_action;
			}
		break;
		}

	default:
		YY_FATAL_ERROR(
			"fatal flex scanner internal error--no action found" );
	} /* end of action switch */
		} /* end of scanning one token */
	} /* end of user's declarations */
} /* end of yylex */

/* yy_get_next_buffer - try to read in a new buffer
 *
 * Returns a code representing an action:
 *	EOB_ACT_LAST_MATCH -
 *	EOB_ACT_CONTINUE_SCAN - continue scanning from current position
 *	EOB_ACT_END_OF_FILE - end of file
 */
static int yy_get_next_buffer (void)
{
    	char *dest = YY_CURRENT_BUFFER_LVALUE->yy_ch_buf;
	char *source = (yytext_ptr);
	int number_to_move, i;
	int ret_val;

	if ( (yy_c_buf_p) > &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars) + 1] )
		YY_FATAL_ERROR(
		"fatal flex scanner internal error--end of buffer missed" );

	if ( YY_CURRENT_BUFFER_LVALUE->yy_fill_buffer == 0 )
		{ /* Don't try to fill the buffer, so this is an EOF. */
		if ( (yy_c_buf_p) - (yytext_ptr) - YY_MORE_ADJ == 1 )
			{
			/* We matched a single character, the EOB, so
			 * treat this as a final EOF.
			 */
			return EOB_ACT_END_OF_FILE;
			}

		else
			{
			/* We matched some text prior to the EOB, first
			 * process it.
			 */
			return EOB_ACT_LAST_MATCH;
			}
		}

	/* Try to read more data. */

	/* First move last chars to start of buffer. */
	number_to_move = (int) ((yy_c_buf_p) - (yytext_ptr) - 1);

	for ( i = 0; i < number_to_move; ++i )
		*(dest++) = *(source++);

	if ( YY_CURRENT_BUFFER_LVALUE->yy_buffer_status == YY_BUFFER_EOF_PENDING )
		/* don't do the read, it's not guaranteed to return an EOF,
		 * just force an EOF
		 */
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars) = 0;

	else
		{
			int num_to_read =
			YY_CURRENT_BUFFER_LVALUE->yy_buf_size - number_to_move - 1;

		while ( num_to_read <= 0 )
			{ /* Not enough room in the buffer - grow it. */

			/* just a shorter name for the current buffer */
			YY_BUFFER_STATE b = YY_CURRENT_BUFFER_LVALUE;

			int yy_c_buf_p_offset =
				(int) ((yy_c_buf_p) - b->yy_ch_buf);

			if ( b->yy_is_our_buffer )
				{
				int new_size = b->yy_buf_size * 2;

				if ( new_size <= 0 )
					b->yy_buf_size += b->yy_buf_size / 8;
				else
					b->yy_buf_size *= 2;

				b->yy_ch_buf = (char *)
					/* Include room in for 2 EOB chars. */
					yyrealloc( (void *) b->yy_ch_buf,
							 (yy_size_t) (b->yy_buf_size + 2)  );
				}
			else
				/* Can't grow it, we don't own it. */
				b->yy_ch_buf = NULL;

			if ( ! b->yy_ch_buf )
				YY_FATAL_ERROR(
				"fatal error - scanner input buffer overflow" );

			(yy_c_buf_p) = &b->yy_ch_buf[yy_c_buf_p_offset];

			num_to_read = YY_CURRENT_BUFFER_LVALUE->yy_buf_size -
						number_to_move - 1;

			}

		if ( num_to_read > YY_READ_BUF_SIZE )
			num_to_read = YY_READ_BUF_SIZE;

		/* Read in more data. */
		YY_INPUT( (&YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[number_to_move]),
			(yy_n_chars), num_to_read );

		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
		}

	if ( (yy_n_chars) == 0 )
		{
		if ( number_to_move == YY_MORE_ADJ )
			{
			ret_val = EOB_ACT_END_OF_FILE;
			yyrestart( yyin  );
			}

		else
			{
			ret_val = EOB_ACT_LAST_MATCH;
			YY_CURRENT_BUFFER_LVALUE->yy_buffer_status =
				YY_BUFFER_EOF_PENDING;
			}
		}

	else
		ret_val = EOB_ACT_CONTINUE_SCAN;

	if (((yy_n_chars) + number_to_move) > YY_CURRENT_BUFFER_LVALUE->yy_buf_size) {
		/* Extend the array by 50%, plus the number we really need. */
		int new_size = (yy_n_chars) + number_to_move + ((yy_n_chars) >> 1);
		YY_CURRENT_BUFFER_LVALUE->yy_ch_buf = (char *) yyrealloc(
			(void *) YY_CURRENT_BUFFER_LVALUE->yy_ch_buf, (yy_size_t) new_size  );
		if ( ! YY_CURRENT_BUFFER_LVALUE->yy_ch_buf )
			YY_FATAL_ERROR( "out of dynamic memory in yy_get_next_buffer()" );
		/* "- 2" to take care of EOB's */
		YY_CURRENT_BUFFER_LVALUE->yy_buf_size = (int) (new_size - 2);
	}

	(yy_n_chars) += number_to_move;
	YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] = YY_END_OF_BUFFER_CHAR;
	YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars) + 1] = YY_END_OF_BUFFER_CHAR;

	(yytext_ptr) = &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[0];

	return ret_val;
}

/* yy_get_previous_state - get the state just before the EOB char was reached */

    static yy_state_type yy_get_previous_state (void)
{
	yy_state_type yy_current_state;
	char *yy_cp;
    
	yy_current_state = (yy_start);

	for ( yy_cp = (yytext_ptr) + YY_MORE_ADJ; yy_cp < (yy_c_buf_p); ++yy_cp )
		{
		YY_CHAR yy_c = (*yy_cp ? yy_ec[YY_SC_TO_UI(*yy_cp)] : 1);
		if ( yy_accept[yy_current_state] )
			{
			(yy_last_accepting_state) = yy_current_state;
			(yy_last_accepting_cpos) = yy_cp;
			}
		while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
			{
			yy_current_state = (int) yy_def[yy_current_state];
			if ( yy_current_state >= 231 )
				yy_c = yy_meta[yy_c];
			}
		yy_current_state = yy_nxt[yy_base[yy_current_state] + yy_c];
		}

	return yy_current_state;
}

/* yy_try_NUL_trans - try to make a transition on the NUL character
 *
 * synopsis
 *	next_state = yy_try_NUL_trans( current_state );
 */
    static yy_state_type yy_try_NUL_trans  (yy_state_type yy_current_state )
{
	int yy_is_jam;
    	char *yy_cp = (yy_c_buf_p);

	YY_CHAR yy_c = 1;
	if ( yy_accept[yy_current_state] )
		{
		(yy_last_accepting_state) = yy_current_state;
		(yy_last_accepting_cpos) = yy_cp;
		}
	while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
		{
		yy_current_state = (int) yy_def[yy_current_state];
		if ( yy_current_state >= 231 )
			yy_c = yy_meta[yy_c];
		}
	yy_current_state = yy_nxt[yy_base[yy_current_state] + yy_c];
	yy_is_jam = (yy_current_state == 230);

		return yy_is_jam ? 0 : yy_current_state;
}

#ifndef YY_NO_UNPUT

    static void yyunput (int c, char * yy_bp )
{
	char *yy_cp;
    
    yy_cp = (yy_c_buf_p);

	/* undo effects of setting up yytext */
	*yy_cp = (yy_hold_char);

	if ( yy_cp < YY_CURRENT_BUFFER_LVALUE->yy_ch_buf + 2 )
		{ /* need to shift things up to make room */
		/* +2 for EOB chars. */
		int number_to_move = (yy_n_chars) + 2;
		char *dest = &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[
					YY_CURRENT_BUFFER_LVALUE->yy_buf_size + 2];
		char *source =
				&YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[number_to_move];

		while ( source > YY_CURRENT_BUFFER_LVALUE->yy_ch_buf )
			*--dest = *--source;

		yy_cp += (int) (dest - source);
		yy_bp += (int) (dest - source);
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars =
			(yy_n_chars) = (int) YY_CURRENT_BUFFER_LVALUE->yy_buf_size;

		if ( yy_cp < YY_CURRENT_BUFFER_LVALUE->yy_ch_buf + 2 )
			YY_FATAL_ERROR( "flex scanner push-back overflow" );
		}

	*--yy_cp = (char) c;

    if ( c == '\n' ){
        --yylineno;
    }

	(yytext_ptr) = yy_bp;
	(yy_hold_char) = *yy_cp;
	(yy_c_buf_p) = yy_cp;
}

#endif

#ifndef YY_NO_INPUT
#ifdef __cplusplus
    static int yyinput (void)
#else
    static int input  (void)
#endif

{
	int c;
    
	*(yy_c_buf_p) = (yy_hold_char);

	if ( *(yy_c_buf_p) == YY_END_OF_BUFFER_CHAR )
		{
		/* yy_c_buf_p now points to the character we want to return.
		 * If this occurs *before* the EOB characters, then it's a
		 * valid NUL; if not, then we've hit the end of the buffer.
		 */
		if ( (yy_c_buf_p) < &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] )
			/* This was really a NUL. */
			*(yy_c_buf_p) = '\0';

		else
			{ /* need more input */
			int offset = (int) ((yy_c_buf_p) - (yytext_ptr));
			++(yy_c_buf_p);

			switch ( yy_get_next_buffer(  ) )
				{
				case EOB_ACT_LAST_MATCH:
					/* This happens because yy_g_n_b()
					 * sees that we've accumulated a
					 * token and flags that we need to
					 * try matching the token before
					 * proceeding.  But for input(),
					 * there's no matching to consider.
					 * So convert the EOB_ACT_LAST_MATCH
					 * to EOB_ACT_END_OF_FILE.
					 */

					/* Reset buffer status. */
					yyrestart( yyin );

					/*FALLTHROUGH*/

				case EOB_ACT_END_OF_FILE:
					{
					if ( yywrap(  ) )
						return 0;

					if ( ! (yy_did_buffer_switch_on_eof) )
						YY_NEW_FILE;
#ifdef __cplusplus
					return yyinput();
#else
					return input();
#endif
					}

				case EOB_ACT_CONTINUE_SCAN:
					(yy_c_buf_p) = (yytext_ptr) + offset;
					break;
				}
			}
		}

	c = *(unsigned char *) (yy_c_buf_p);	/* cast for 8-bit char's */
	*(yy_c_buf_p) = '\0';	/* preserve yytext */
	(yy_hold_char) = *++(yy_c_buf_p);

	if ( c == '\n' )
		
    yylineno++;
;

	return c;
}
#endif	/* ifndef YY_NO_INPUT */

/** Immediately switch to a different input stream.
 * @param input_file A readable stream.
 * 
 * @note This function does not reset the start condition to @c INITIAL .
 */
    void yyrestart  (FILE * input_file )
{
    
	if ( ! YY_CURRENT_BUFFER ){
        yyensure_buffer_stack ();
		YY_CURRENT_BUFFER_LVALUE =
            yy_create_buffer( yyin, YY_BUF_SIZE );
	}

	yy_init_buffer( YY_CURRENT_BUFFER, input_file );
	yy_load_buffer_state(  );
}

/** Switch to a different input buffer.
 * @param new_buffer The new input buffer.
 * 
 */
    void yy_switch_to_buffer  (YY_BUFFER_STATE  new_buffer )
{
    
	/* TODO. We should be able to replace this entire function body
	 * with
	 *		yypop_buffer_state();
	 *		yypush_buffer_state(new_buffer);
     */
	yyensure_buffer_stack ();
	if ( YY_CURRENT_BUFFER == new_buffer )
		return;

	if ( YY_CURRENT_BUFFER )
		{
		/* Flush out information for old buffer. */
		*(yy_c_buf_p) = (yy_hold_char);
		YY_CURRENT_BUFFER_LVALUE->yy_buf_pos = (yy_c_buf_p);
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
		}

	YY_CURRENT_BUFFER_LVALUE = new_buffer;
	yy_load_buffer_state(  );

	/* We don't actually know whether we did this switch during
	 * EOF (yywrap()) processing, but the only time this flag
	 * is looked at is after yywrap() is called, so it's safe
	 * to go ahead and always set it.
	 */
	(yy_did_buffer_switch_on_eof) = 1;
}

static void yy_load_buffer_state  (void)
{
    	(yy_n_chars) = YY_CURRENT_BUFFER_LVALUE->yy_n_chars;
	(yytext_ptr) = (yy_c_buf_p) = YY_CURRENT_BUFFER_LVALUE->yy_buf_pos;
	yyin = YY_CURRENT_BUFFER_LVALUE->yy_input_file;
	(yy_hold_char) = *(yy_c_buf_p);
}

/** Allocate and initialize an input buffer state.
 * @param file A readable stream.
 * @param size The character buffer size in bytes. When in doubt, use @c YY_BUF_SIZE.
 * 
 * @return the allocated buffer state.
 */
    YY_BUFFER_STATE yy_create_buffer  (FILE * file, int  size )
{
	YY_BUFFER_STATE b;
    
	b = (YY_BUFFER_STATE) yyalloc( sizeof( struct yy_buffer_state )  );
	if ( ! b )
		YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );

	b->yy_buf_size = size;

	/* yy_ch_buf has to be 2 characters longer than the size given because
	 * we need to put in 2 end-of-buffer characters.
	 */
	b->yy_ch_buf = (char *) yyalloc( (yy_size_t) (b->yy_buf_size + 2)  );
	if ( ! b->yy_ch_buf )
		YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );

	b->yy_is_our_buffer = 1;

	yy_init_buffer( b, file );

	return b;
}

/** Destroy the buffer.
 * @param b a buffer created with yy_create_buffer()
 * 
 */
    void yy_delete_buffer (YY_BUFFER_STATE  b )
{
    
	if ( ! b )
		return;

	if ( b == YY_CURRENT_BUFFER ) /* Not sure if we should pop here. */
		YY_CURRENT_BUFFER_LVALUE = (YY_BUFFER_STATE) 0;

	if ( b->yy_is_our_buffer )
		yyfree( (void *) b->yy_ch_buf  );

	yyfree( (void *) b  );
}

/* Initializes or reinitializes a buffer.
 * This function is sometimes called more than once on the same buffer,
 * such as during a yyrestart() or at EOF.
 */
    static void yy_init_buffer  (YY_BUFFER_STATE  b, FILE * file )

{
	int oerrno = errno;
    
	yy_flush_buffer( b );

	b->yy_input_file = file;
	b->yy_fill_buffer = 1;

    /* If b is the current buffer, then yy_init_buffer was _probably_
     * called from yyrestart() or through yy_get_next_buffer.
     * In that case, we don't want to reset the lineno or column.
     */
    if (b != YY_CURRENT_BUFFER){
        b->yy_bs_lineno = 1;
        b->yy_bs_column = 0;
    }

        b->yy_is_interactive = file ? (isatty( fileno(file) ) > 0) : 0;
    
	errno = oerrno;
}

/** Discard all buffered characters. On the next scan, YY_INPUT will be called.
 * @param b the buffer state to be flushed, usually @c YY_CURRENT_BUFFER.
 * 
 */
    void yy_flush_buffer (YY_BUFFER_STATE  b )
{
    	if ( ! b )
		return;

	b->yy_n_chars = 0;

	/* We always need two end-of-buffer characters.  The first causes
	 * a transition to the end-of-buffer state.  The second causes
	 * a jam in that state.
	 */
	b->yy_ch_buf[0] = YY_END_OF_BUFFER_CHAR;
	b->yy_ch_buf[1] = YY_END_OF_BUFFER_CHAR;

	b->yy_buf_pos = &b->yy_ch_buf[0];

	b->yy_at_bol = 1;
	b->yy_buffer_status = YY_BUFFER_NEW;

	if ( b == YY_CURRENT_BUFFER )
		yy_load_buffer_state(  );
}

/** Pushes the new state onto the stack. The new state becomes
 *  the current state. This function will allocate the stack
 *  if necessary.
 *  @param new_buffer The new state.
 *  
 */
void yypush_buffer_state (YY_BUFFER_STATE new_buffer )
{
    	if (new_buffer == NULL)
		return;

	yyensure_buffer_stack();

	/* This block is copied from yy_switch_to_buffer. */
	if ( YY_CURRENT_BUFFER )
		{
		/* Flush out information for old buffer. */
		*(yy_c_buf_p) = (yy_hold_char);
		YY_CURRENT_BUFFER_LVALUE->yy_buf_pos = (yy_c_buf_p);
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
		}

	/* Only push if top exists. Otherwise, replace top. */
	if (YY_CURRENT_BUFFER)
		(yy_buffer_stack_top)++;
	YY_CURRENT_BUFFER_LVALUE = new_buffer;

	/* copied from yy_switch_to_buffer. */
	yy_load_buffer_state(  );
	(yy_did_buffer_switch_on_eof) = 1;
}

/** Removes and deletes the top of the stack, if present.
 *  The next element becomes the new top.
 *  
 */
void yypop_buffer_state (void)
{
    	if (!YY_CURRENT_BUFFER)
		return;

	yy_delete_buffer(YY_CURRENT_BUFFER );
	YY_CURRENT_BUFFER_LVALUE = NULL;
	if ((yy_buffer_stack_top) > 0)
		--(yy_buffer_stack_top);

	if (YY_CURRENT_BUFFER) {
		yy_load_buffer_state(  );
		(yy_did_buffer_switch_on_eof) = 1;
	}
}

/* Allocates the stack if it does not exist.
 *  Guarantees space for at least one push.
 */
static void yyensure_buffer_stack (void)
{
	yy_size_t num_to_alloc;
    
	if (!(yy_buffer_stack)) {

		/* First allocation is just for 2 elements, since we don't know if this
		 * scanner will even need a stack. We use 2 instead of 1 to avoid an
		 * immediate realloc on the next call.
         */
      num_to_alloc = 1; /* After all that talk, this was set to 1 anyways... */
		(yy_buffer_stack) = (struct yy_buffer_state**)yyalloc
								(num_to_alloc * sizeof(struct yy_buffer_state*)
								);
		if ( ! (yy_buffer_stack) )
			YY_FATAL_ERROR( "out of dynamic memory in yyensure_buffer_stack()" );

		memset((yy_buffer_stack), 0, num_to_alloc * sizeof(struct yy_buffer_state*));

		(yy_buffer_stack_max) = num_to_alloc;
		(yy_buffer_stack_top) = 0;
		return;
	}

	if ((yy_buffer_stack_top) >= ((yy_buffer_stack_max)) - 1){

		/* Increase the buffer to prepare for a possible push. */
		yy_size_t grow_size = 8 /* arbitrary grow size */;

		num_to_alloc = (yy_buffer_stack_max) + grow_size;
		(yy_buffer_stack) = (struct yy_buffer_state**)yyrealloc
								((yy_buffer_stack),
								num_to_alloc * sizeof(struct yy_buffer_state*)
								);
		if ( ! (yy_buffer_stack) )
			YY_FATAL_ERROR( "out of dynamic memory in yyensure_buffer_stack()" );

		/* zero only the new slots.*/
		memset((yy_buffer_stack) + (yy_buffer_stack_max), 0, grow_size * sizeof(struct yy_buffer_state*));
		(yy_buffer_stack_max) = num_to_alloc;
	}
}

/** Setup the input buffer state to scan directly from a user-specified character buffer.
 * @param base the character buffer
 * @param size the size in bytes of the character buffer
 * 
 * @return the newly allocated buffer state object.
 */
YY_BUFFER_STATE yy_scan_buffer  (char * base, yy_size_t  size )
{
	YY_BUFFER_STATE b;
    
	if ( size < 2 ||
	     base[size-2] != YY_END_OF_BUFFER_CHAR ||
	     base[size-1] != YY_END_OF_BUFFER_CHAR )
		/* They forgot to leave room for the EOB's. */
		return NULL;

	b = (YY_BUFFER_STATE) yyalloc( sizeof( struct yy_buffer_state )  );
	if ( ! b )
		YY_FATAL_ERROR( "out of dynamic memory in yy_scan_buffer()" );

	b->yy_buf_size = (int) (size - 2);	/* "- 2" to take care of EOB's */
	b->yy_buf_pos = b->yy_ch_buf = base;
	b->yy_is_our_buffer = 0;
	b->yy_input_file = NULL;
	b->yy_n_chars = b->yy_buf_size;
	b->yy_is_interactive = 0;
	b->yy_at_bol = 1;
	b->yy_fill_buffer = 0;
	b->yy_buffer_status = YY_BUFFER_NEW;

	yy_switch_to_buffer( b  );

	return b;
}

/** Setup the input buffer state to scan a string. The next call to yylex() will
 * scan from a @e copy of @a str.
 * @param yystr a NUL-terminated string to scan
 * 
 * @return the newly allocated buffer state object.
 * @note If you want to scan bytes that may contain NUL values, then use
 *       yy_scan_bytes() instead.
 */
YY_BUFFER_STATE yy_scan_string (const char * yystr )
{
    
	return yy_scan_bytes( yystr, (int) strlen(yystr) );
}

/** Setup the input buffer state to scan the given bytes. The next call to yylex() will
 * scan from a @e copy of @a bytes.
 * @param yybytes the byte buffer to scan
 * @param _yybytes_len the number of bytes in the buffer pointed to by @a bytes.
 * 
 * @return the newly allocated buffer state object.
 */
YY_BUFFER_STATE yy_scan_bytes  (const char * yybytes, int  _yybytes_len )
{
	YY_BUFFER_STATE b;
	char *buf;
	yy_size_t n;
	int i;
    
	/* Get memory for full buffer, including space for trailing EOB's. */
	n = (yy_size_t) (_yybytes_len + 2);
	buf = (char *) yyalloc( n  );
	if ( ! buf )
		YY_FATAL_ERROR( "out of dynamic memory in yy_scan_bytes()" );

	for ( i = 0; i < _yybytes_len; ++i )
		buf[i] = yybytes[i];

	buf[_yybytes_len] = buf[_yybytes_len+1] = YY_END_OF_BUFFER_CHAR;

	b = yy_scan_buffer( buf, n );
	if ( ! b )
		YY_FATAL_ERROR( "bad buffer in yy_scan_bytes()" );

	/* It's okay to grow etc. this buffer, and we should throw it
	 * away when we're done.
	 */
	b->yy_is_our_buffer = 1;

	return b;
}

#ifndef YY_EXIT_FAILURE
#define YY_EXIT_FAILURE 2
#endif

static void yynoreturn yy_fatal_error (const char* msg )
{
			fprintf( stderr, "%s\n", msg );
	exit( YY_EXIT_FAILURE );
}

/* Redefine yyless() so it works in section 3 code. */

#undef yyless
#define yyless(n) \
	do \
		{ \
		/* Undo effects of setting up yytext. */ \
        int yyless_macro_arg = (n); \
        YY_LESS_LINENO(yyless_macro_arg);\
		yytext[yyleng] = (yy_hold_char); \
		(yy_c_buf_p) = yytext + yyless_macro_arg; \
		(yy_hold_char) = *(yy_c_buf_p); \
		*(yy_c_buf_p) = '\0'; \
		yyleng = yyless_macro_arg; \
		} \
	while ( 0 )

/* Accessor  methods (get/set functions) to struct members. */

/** Get the current line number.
 * 
 */
int yyget_lineno  (void)
{
    
    return yylineno;
}

/** Get the input stream.
 * 
 */
FILE *yyget_in  (void)
{
        return yyin;
}

/** Get the output stream.
 * 
 */
FILE *yyget_out  (void)
{
        return yyout;
}

/** Get the length of the current token.
 * 
 */
int yyget_leng  (void)
{
        return yyleng;
}

/** Get the current token.
 * 
 */

char *yyget_text  (void)
{
        return yytext;
}

/** Set the current line number.
 * @param _line_number line number
 * 
 */
void yyset_lineno (int  _line_number )
{
    
    yylineno = _line_number;
}

/** Set the input stream. This does not discard the current
 * input buffer.
 * @param _in_str A readable stream.
 * 
 * @see yy_switch_to_buffer
 */
void yyset_in (FILE *  _in_str )
{
        yyin = _in_str ;
}

void yyset_out (FILE *  _out_str )
{
        yyout = _out_str ;
}

int yyget_debug  (void)
{
        return yy_flex_debug;
}

void yyset_debug (int  _bdebug )
{
        yy_flex_debug = _bdebug ;
}

static int yy_init_globals (void)
{
        /* Initialization is the same as for the non-reentrant scanner.
     * This function is called from yylex_destroy(), so don't allocate here.
     */

    /* We do not touch yylineno unless the option is enabled. */
    yylineno =  1;
    
    (yy_buffer_stack) = NULL;
    (yy_buffer_stack_top) = 0;
    (yy_buffer_stack_max) = 0;
    (yy_c_buf_p) = NULL;
    (yy_init) = 0;
    (yy_start) = 0;

/* Defined in main.c */
#ifdef YY_STDINIT
    yyin = stdin;
    yyout = stdout;
#else
    yyin = NULL;
    yyout = NULL;
#endif

    /* For future reference: Set errno on error, since we are called by
     * yylex_init()
     */
    return 0;
}

/* yylex_destroy is for both reentrant and non-reentrant scanners. */
int yylex_destroy  (void)
{
    
    /* Pop the buffer stack, destroying each element. */
	while(YY_CURRENT_BUFFER){
		yy_delete_buffer( YY_CURRENT_BUFFER  );
		YY_CURRENT_BUFFER_LVALUE = NULL;
		yypop_buffer_state();
	}

	/* Destroy the stack itself. */
	yyfree((yy_buffer_stack) );
	(yy_buffer_stack) = NULL;

    /* Reset the globals. This is important in a non-reentrant scanner so the next time
     * yylex() is called, initialization will occur. */
    yy_init_globals( );

    return 0;
}

/*
 * Internal utility routines.
 */

#ifndef yytext_ptr
static void yy_flex_strncpy (char* s1, const char * s2, int n )
{
		
	int i;
	for ( i = 0; i < n; ++i )
		s1[i] = s2[i];
}
#endif

#ifdef YY_NEED_STRLEN
static int yy_flex_strlen (const char * s )
{
	int n;
	for ( n = 0; s[n]; ++n )
		;

	return n;
}
#endif

void *yyalloc (yy_size_t  size )
{
			return malloc(size);
}

void *yyrealloc  (void * ptr, yy_size_t  size )
{
		
	/* The cast to (char *) in the following accommodates both
	 * implementations that use char* generic pointers, and those
	 * that use void* generic pointers.  It works with the latter
	 * because both ANSI C and C++ allow castless assignment from
	 * any pointer type to void*, and deal with argument conversions
	 * as though doing an assignment.
	 */
	return realloc(ptr, size);
}

void yyfree (void * ptr )
{
			free( (char *) ptr );	/* see yyrealloc() for (char *) cast */
}

#define YYTABLES_NAME "yytables"

#line 125 "lex.l"


int yywrap(void)
{
	return 1 ;
}
```
### token.cpp
```C++
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
void id::disp_tbl(){
    if (argc == 2){
        cout << endl << endl << "ID_TABLE" << endl << endl;
        map<string, int>::iterator iter = wordlist.begin();
        map<string ,int>::iterator end = wordlist.end();
        for (; iter != end; iter ++)
            cout << setw(5) << iter->second << setw(10) << " " << iter->first << endl;
    }else if (argc == 3){
        outfile << endl << endl << "ID_TABLE" << endl << endl;
        map<string, int>::iterator iter = wordlist.begin();
        map<string ,int>::iterator end = wordlist.end();
        for (; iter != end; iter ++)
            outfile << setw(5) << iter->second << setw(10) << " " << iter->first << endl;
    }else {
        ;
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

int line::count = 1;

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

void characters::display(char* yytext){
    string s_temp(yytext);
    string s = s_temp.substr(1, s_temp.length() - 2);
    if (argc == 3){
        outfile << left << setw(30) << "Char" << left << setw(30) << s << endl;
    }else{
        cout << left << setw(30) << "Char" << left << setw(30) << s << endl;
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
```
### main.cpp
```C++
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
    characters c;
    error e;
    id i;

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
        if (d == KEYWORD){
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
        }else if (d == CHARACTERS){
            c.display(yytext);
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
    i.disp_tbl();

    outfile.close();
    return 0;
}
```
### Makefile 文件
```Makefile
HEAD = global.h lex.h token.h
OBJ = main.o lex.o token.o
BIN = lexical_analysis

$(BIN): $(OBJ)
	g++ $(OBJ) -o $(BIN)

main.o: $(HEAD) main.cpp lex.cpp
	g++ -c main.cpp -o main.o
token.o: $(HEAD) token.cpp lex.cpp
	g++ -c token.cpp -o token.o
lex.o: $(HEAD) lex.cpp
	g++ -c lex.cpp -o lex.o

lex.cpp:lex.c
	mv lex.c lex.cpp

lex.c: lex.l
	flex -o lex.c lex.l

.PHONY: clean

clean:
	@- $(RM) *.o lex.cpp $(BIN)
```