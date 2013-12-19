
CC := gcc
OK := server
MAKE	:= make
#LIBS := -L/usr/local/mysql/lib/mysql -I/usr/local/mysql/include/mysql -DUNIV_LINUX
CFLAGS := $(LIBS) -Wall -O -g -pedantic -Wshadow -Wunused -Wstrict-aliasing  

RM-F := rm -f
#CXXFLAGS := $(CFLAGS) -lmysqlclient_r -lpthread -lz -lcrypt -lnsl -lm -levent 
CXXFLAGS := $(CFLAGS) -lpthread -lz -lcrypt -lnsl -lm -levent  

EXECUTABLE := -o server $(CXXFLAGS) 
SOURCE := $(wildcard *.c)
OBJS := $(patsubst %.c,%.o,$(SOURCE))

#UBDIRS := ./modules/
#MODSOUR :=  $(wildcard $(SUBDIRS)*.c)
#MODPROG := $(patsubst $(SUBDIRS)%.c,$(SUBDIRS)%.o, $(MODSOUR))

HASHDIR := ./hashtable/
HASHSOUR :=  $(wildcard $(HASHDIR)*.c)
HASHPROG := $(patsubst $(HASHDIR)%.c,$(HASHDIR)%.o, $(HASHSOUR))

PARDIR := ./parser/
PARSOUR :=  $(wildcard $(PARDIR)*.c)
#PARPROG := $(patsubst $(PARDIR)%.c,$(PARDIR)%.o, $(PARSOUR))
PARPROG	:=	$(PARDIR)ly.o

.PHONY : deps everything objs clean 

$(OK) : $(OBJS) hashtables parsers 
	@echo "======== ok ========="
	$(CC)  $(EXECUTABLE) $(OBJS) $(PARPROG) $(HASHPROG) $(LEXYACC)  $(HASHROG) $(LIBS) $(CFLAGS) 

hashtables :  
	@echo "======= hashtable ========="
	$(foreach c,$(HASHDIR),$(MAKE) -C $(c) || ) true

parsers :  
	@echo "======= parser ========="
	$(foreach c,$(PARDIR),$(MAKE) -C $(c) || ) true

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
	$(RM-F) parser/*.o
	$(RM-F) parser/*.gch
	$(RM-F) $(OK)
	$(RM-F) core.*


# set noexpandtab
