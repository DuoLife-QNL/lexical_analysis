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
