
CC := cc
CFLAGS :=  -Wall -O -g -pedantic -Wshadow -Wunused -Wstrict-aliasing
FLAG	:=	-ll -ly -lm
OK	:= test_hashtable
DEBUG	:= n
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
ifeq ($(DEBUG),y)
	$(CC)  -o $(OK) $(OBJS) $(EXOBJ)  $(CFLAGS)
endif	
#	$(CC)   $(OBJS) $(CONF) $(CONN)  $(TIME)  $(PARSOUR) $(FLAG)

# set noexpandtab
