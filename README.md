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
### lex词法分析程序设计说明
#### 使用的lex内置变量
* ```yylineno```：记录当前程序读取到哪一行，以便报错时输出错误的位置
* ```yytext```：字符数组指针，指向当前接收的记号
* ```yyleng```：当前读入记号的长度
* ```yyin```：```FILE*```类型变量，是词法分析程序的文件读入指针，程序开始时指向用户输入的待识别文件：
    ```C++
    if(!(yyin = fopen(argv[1], "r"))){
            perror(argv[1]);
            return 1;
        }
    ```
    若没有如上设置，则```yyin```默认指向标准输入
#### 对于各种记号的定义及操作
1. 定义标识符和常数的规则如下（对于常数的定义参照PASCAL的规则扩充了指数）
```
letter  [A-Za-z_]
digit   [0-9]
id      {letter}({letter}|{digit})*
num     [+\-]?{digit}+(\.{digit}+)?(e[+\-]?{digit}+)?
```
2. 对于关键字的定义及操作：
```C++
"auto" 		return AUTO ;
"break" 	return BREAK ;
"case" 		return CASE ;
"char" 		return CHAR ;
"const" 	return CONST ;
"continue" 	return CONTINUE ;
"default" 	return DEFAULT ;
"do" 		return DO ;
"double" 	return DOUBLE ;
"else" 		return ELSE ;
"enum" 		return ENUM ;
"extern" 	return EXTERN ;
"float" 	return FLOAT ;
"for" 		return FOR ;
"goto" 		return GOTO ;
"if" 		return IF ;
"int" 		return INT ;
"long" 		return LONG ;
"register" 	return REGISTER ;
"return" 	return RETURN ;
"short" 	return SHORT ;
"signed" 	return SIGNED ;
"sizeof" 	return SIZEOF ;
"static" 	return STATIC ;
"struct" 	return STRUCT ;
"switch" 	return SWITCH ;
"typedef" 	return TYPEDEF ;
"union" 	return UNION ;
"unsigned" 	return UNSIGNED ;
"void" 		return VOID ;
"volatile" 	return VOLATILE ;
"while" 	return WHILE ;
```

1. 对于运算符的定义和操作如下：
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

4. 对于分界符的定义和操作如下：
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
