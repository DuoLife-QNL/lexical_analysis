HEAD = global.h lex.h token.h
OBJ = main.o lex.o token.o
FINAL = lexical_analysis

$(FINAL): $(OBJ)
	$(CC) -o $(FINAL) $(OBJ)

main.o: $(HEAD) main.cpp
	$(CC) -o main main.cpp
token.o: $(HEAD) token.cpp
	$(CC) -o token token.cpp
lex.o: $(HEAD) lex.c
	$(CC) -o lex lex.c

lex.c: lex.l
	flex -o lex lex.l