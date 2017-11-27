CC = g++
AR = ar cru
RM = /bin/rm -f

CUR_INCLUDE1 = ../Mota_Need/Mota_h
OBJS_HOME1 = ../Mota_Need/Mota_Lib
BIN_DIR = ../../Gui/V3_bin
ORACLE_HOME=/home/tt/oracle_cli/oracle
CUR_INCLUDE = ../include
OBJS_HOME= ../wxlib/u12
#--------------------COMPILE FLAGS
   
 
ADD_INCL =-I$(ORACLE_HOME)/rdbms/demo -I$(CUR_INCLUDE) -I$(CUR_INCLUDE)/lua5.1  -I$(CUR_INCLUDE1) -I$(CUR_INCLUDE1)/lua5.1  

CFLAGS = -Wall -g -DLINUX  -funsigned-char -DOCI_IMPORT_LINKAGE -DOCI_CHARSET_ANSI     
 			
LDFLAGS =  -L$(ORACLE_HOME) -L$(ORACLE_HOME)/lib   -L$(OBJS_HOME)  -llua5.1\
		 $(OBJS_HOME)/libhiredis.a -locilib  -lclntsh   -lstdc++   -L$(OBJS_HOME1)  -lm  -lstdc++ -pthread -llua5.1 -ldl

CFLAGS  += $(ADD_INCL)
#-------------------------------------------

SRC= BMG_S.cpp util.cpp OraMeta.cpp 
SRCOBJ=${SRC:.cpp=.o}

#-------------------------------------------
TRP_PGMSRC = BMG_S.cpp 
PGMSRCOBJ=${BMG_S:.cpp=.o}
#--------------------------------------------------------------------

TARGET = BMG_S
#--------------------EXE PGM  AND LIB -------------------------------

all:  ${TARGET} 

###MAKE RULE
.cpp.o:
	$(CC) $(CFLAGS) -c -o $@  $< 	
.c.o:
	$(CC) $(CFLAGS) -c -o $@  $< 	
###MAKE RULE 	

BMG_S: $(SRCOBJ)  BMG_S.o  	
	$(CC) $^ $(LDFLAGS) -o $@  $(EXT_LIB)
	@cp $@ ${BIN_DIR}
	@echo "build BMG_S Ok!"
	@( $(RM) *.o core core.* $(SRCOBJ) )
#db2Test: $(SRCOBJ) db2Test.o  	
#	$(CC) $^ $(LDFLAGS) -o $@
#	@mv $@ ${BIN_DIR}/$@
#	@echo "build $@ Ok! Move It to ${BIN_DIR} Ok!"	
		
clean:
	@( $(RM) *.o core core.* $(TARGET) $(SRCOBJ) )
