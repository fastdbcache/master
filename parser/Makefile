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
	$(CC) -c -o  $(OJY) $(OY)
	$(CC) -c -o  $(OJL) $(OL)
	$(CC) -c -o   $(EXOBJ)	$(SOURCE)
ifeq ($(DEBUG),y)
	$(CC)  -o $(OK) $(OJL) $(OJY) $(EXOBJ)  $(CFLAGS)
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
