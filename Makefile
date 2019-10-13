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
