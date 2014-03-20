OK := lex
CC := cc
CFLAGS := -Wall -O -g -pedantic -Wshadow -Wunused -Wstrict-aliasing -ll -ly -lm  
DEBUG := y
LEX := flex
YACC := bison

RM-F := rm -f
EXECUTABLE :=  $(CFLAGS)

SY := $(wildcard *.y) 
OY := $(patsubst %.y,%.c,$(SY))

SL := $(wildcard *.l) 
OL := $(patsubst %.l,%.c,$(SL))

OJY := $(patsubst %.y,%.o,$(SY))
OJL	:=	$(patsubst %.l,%.o,$(SL))

SOURCE := Expression.c 
EXOBJ	:=	$(patsubst %.c,%.o,$(SOURCE))

ly :  oy ol 
	@echo "+++ build lex and yacc +++"
	$(CC) -c -g -o $(OJY) $(OY)
	$(CC) -c -g -o $(OJL) $(OL)
	$(CC) -c -g -o $(EXOBJ)	$(SOURCE)
ifeq ($(DEBUG),y)
	$(CC) -g  -o $(OK) $(OJL) $(OJY) $(EXOBJ)  $(CFLAGS) ../log_lib.c
endif	

oy	:	$(SY) 
	$(YACC) -d -o  $(OY) $(SY)
	

ol	: $(SL)
	$(LEX) -o $(OL) $(SL)
	
clean :
	$(RM-F) *.o
	$(RM-F) Lexer.c Lexer.h Parser.c Parser.h
	$(RM-F) *.gch
	$(RM-F) $(OK)
	$(RM-F) core.*


# set noexpandtab
