
CC := gcc
OK := server
#LIBS := -L/usr/local/mysql/lib/mysql -I/usr/local/mysql/include/mysql -DUNIV_LINUX
CFLAGS := $(LIBS) -Wall -O -g -pedantic -Wshadow -Wunused -Wstrict-aliasing  

RM-F := rm -f
#CXXFLAGS := $(CFLAGS) -lmysqlclient_r -lpthread -lz -lcrypt -lnsl -lm -levent 
CXXFLAGS := $(CFLAGS) -lpthread -lz -lcrypt -lnsl -lm -levent  

EXECUTABLE := -o server $(CXXFLAGS) 
SOURCE := $(wildcard *.c)
OBJS := $(patsubst %.c,%.o,$(SOURCE))

MODPROG := $(wildcard ./modules/*.o)
HASHROG := $(wildcard ./hashtable/*.o)


.PHONY : deps everything objs clean 

everything : $(EXECUTABLE)

deps : $(OBJS)
		$(CC) $(OBJS) $(CFLAGS) $(LIBS) $(CXXFLAGS)

objs : $(OBJS)

clean :
	$(RM-F) *.o
	$(RM-F) *.gch
	$(RM-F) modules/*.o
	$(RM-F) modules/*.gch
	$(RM-F) hashtable/*.o
	$(RM-F) hashtable/*.gch
	$(RM-F) $(OK)
	$(RM-F) core.*


$(EXECUTABLE) : $(OBJS)
	$(CC)  $(EXECUTABLE) $(OBJS) $(MODPROG) $(HASHROG) $(LIBS) $(CFLAGS) 

# set noexpandtab
