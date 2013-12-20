
CC := gcc
CFLAGS :=  -Wall -O -g -pedantic -Wshadow -Wunused -Wstrict-aliasing
FLAG	:=	-ll -ly -lm

RM-F := rm -f
EXECUTABLE :=  $(CFLAGS)
FILTER	:= test_hashtable.c
SOURCE := $(filter-out $(FILTER),$(wildcard *.c)) 
OBJS := $(patsubst %.c,%.o,$(SOURCE))


objs : $(EXECUTABLE)

clean :
	$(RM-F) *.o
	$(RM-F) *.gch
	$(RM-F) $(OK)
	$(RM-F) core.*

$(EXECUTABLE) : $(OBJS)
#	$(CC)   $(OBJS) $(CONF) $(CONN)  $(TIME)  $(PARSOUR) $(FLAG)

# set noexpandtab
