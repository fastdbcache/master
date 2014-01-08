
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
FILTER  := $(firstword $(HASHDIR))test_hashtable.o
HASHSOUR := $(foreach cd,$(HASHDIR), $(patsubst $(cd)%.c,$(cd)%.o, $(wildcard $(cd)*.c)))
HASHPROG := $(filter-out $(FILTER),$(HASHSOUR))

PARDIR := ./parser/
LEXSOUR :=  $(wildcard $(PARDIR)*.l)
LEXPROG := $(patsubst $(PARDIR)%.l,$(PARDIR)%.o, $(LEXSOUR))
YACCSOUR :=  $(wildcard $(PARDIR)*.y)
YACCPROG := $(patsubst $(PARDIR)%.y,$(PARDIR)%.o, $(YACCSOUR))
EXSOURCE := $(PARDIR)Expression.c 
EXOBJ	:=	$(patsubst %.c,%.o,$(EXSOURCE))

.PHONY : deps everything objs clean 

$(OK) :   parsers hashtables $(OBJS) 
	@echo "======== ok ========="
	$(CC)  $(OBJS) $(PARPROG) $(HASHPROG) $(LEXPROG) $(YACCPROG) $(EXOBJ)  $(HASHROG)  $(EXECUTABLE)

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
	$(RM-F) $(OK)
	$(RM-F) core.*
	$(foreach c,$(PARDIR),$(MAKE) clean -C $(c) && ) true
	$(foreach c,$(HASHDIR),$(MAKE) clean -C $(c) && ) true

# set noexpandtab
