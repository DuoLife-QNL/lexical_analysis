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
1. lex的两个匹配原则：最长匹配原则和最先匹配原则。其中，最先匹配原则保证当有多个规则可以匹配缓冲区中的字符时，则选择在缓冲区中可以匹配最长字符串的规则；最先匹配原则保证若多条规则同时匹配且长度相等时，匹配在lex源程序中写在最前面的那一条规则。
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