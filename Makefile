
CC := cc
OK := server
MAKE	:= make
LIBS	:=	-ll -ly 
#LIBS := -L/usr/local/mysql/lib/mysql -I/usr/local/mysql/include/mysql -DUNIV_LINUX
CFLAGS := $(LIBS)  -Wall -O -g -pedantic -Wshadow -Wunused -Wstrict-aliasing  

RM-F := rm -f
#CXXFLAGS := $(CFLAGS) -lmysqlclient_r -lpthread -lz -lcrypt -lnsl -lm -levent 
CXXFLAGS := $(CFLAGS) -lpthread -lz -lcrypt -lnsl -lm -levent  

EXECUTABLE :=  -o server $(CXXFLAGS)
SOURCE := $(wildcard *.c)
OBJS := $(patsubst %.c,%.o,$(SOURCE))

HASHDIR := ./hashtable/ ./deposit/
FILTER  := $(HASHDIR)test_hashtable.c
HASHSOUR :=  $(filter-out $(FILTER),$(wildcard $(HASHDIR)*.c))
HASHPROG := $(patsubst $(HASHDIR)%.c,$(HASHDIR)%.o, $(HASHSOUR))

PARDIR := ./parser/
LEXSOUR :=  $(wildcard $(PARDIR)*.l)
LEXPROG := $(patsubst $(PARDIR)%.l,$(PARDIR)%.o, $(LEXSOUR))
YACCSOUR :=  $(wildcard $(PARDIR)*.y)
YACCPROG := $(patsubst $(PARDIR)%.y,$(PARDIR)%.o, $(YACCSOUR))

.PHONY : deps everything objs clean 

$(OK) :   parsers hashtables $(OBJS) 
	@echo "======== ok ========="
	$(CC)  $(OBJS) $(PARPROG) $(HASHPROG) $(LEXPROG) $(YACCPROG)  $(HASHROG)  $(EXECUTABLE)

parsers :  
	@echo "======= parser ========="
	$(foreach c,$(PARDIR),$(MAKE) -C $(c) && ) true

hashtables :  
	@echo "======= hashtable and deposit ========="
	$(foreach c,$(HASHDIR),$(MAKE) -C $(c) && ) true

deps : $(OBJS)
		$(CC) $(OBJS) $(CFLAGS)  $(CXXFLAGS)

objs : $(OBJS)

clean :
	$(RM-F) *.o
	$(RM-F) *.gch
	$(RM-F) modules/*.o
	$(RM-F) modules/*.gch
	$(RM-F) hashtable/*.o	
	$(RM-F) hashtable/*.gch
	$(RM-F) deposit/*.o	
	$(RM-F) deposit/*.gch
	$(RM-F) parser/*.o
	$(RM-F) parser/*.c
	$(RM-F) parser/*.gch
	$(RM-F) $(OK)
	$(RM-F) core.*


# set noexpandtab
