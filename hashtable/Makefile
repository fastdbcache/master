
CC := gcc
CFLAGS :=  -Wall -O -g -pedantic -Wshadow -Wunused -Wstrict-aliasing  

RM-F := rm -f
EXECUTABLE :=  $(CFLAGS)
SOURCE := $(wildcard *.c) 
OBJS := $(patsubst %.c,%.o,$(SOURCE))

objs : $(OBJS)

clean :
	$(RM-F) *.o
	$(RM-F) *.gch
	$(RM-F) $(OK)
	$(RM-F) core.*

$(EXECUTABLE) : $(OBJS)
	$(CC)   $(OBJS)  

# set noexpandtab
