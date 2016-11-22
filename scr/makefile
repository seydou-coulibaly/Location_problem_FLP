SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic

#------------------------------------------------------------
#
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#
#------------------------------------------------------------

CPLEXDIR      = /home/csey/ibm/cplex
EXEC= executable
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

# ---------------------------------------------------------------------
# Compiler selection
# ---------------------------------------------------------------------

CC  = gcc -O0

# ---------------------------------------------------------------------
# Compiler options
# ---------------------------------------------------------------------

COPT  = -m64 -fPIC -fno-strict-aliasing

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CLNDIRS   = -L$(CPLEXLIBDIR)
CLNFLAGS  = -lcplex -lm -lpthread


all:$(EXEC)

CPLEXINCDIR   = $(CPLEXDIR)/include

EXDIR         = $(CPLEXDIR)/examples
EXINC         = $(EXDIR)/include
EXDATA        = $(EXDIR)/data
EXSRCC        = $(EXDIR)/src/c

CFLAGS  = $(COPT)  -I$(CPLEXINCDIR) -W -Wall -std=c99 -g
LDFLAGS =


#------------------------------------------------------------
#  make all      : to compile the examples.
#  make execute  : to compile and execute the examples.
#------------------------------------------------------------

C_EX = $(SRC)

executable: $(OBJ)
	@$(CC) -o $@ $^ $(LDFLAGS) $(CLNDIRS) $(CLNFLAGS)

%.o: %.c
		@$(CC) -o $@ -c -I/. $< $(CFLAGS)
# ------------------------------------------------------------
.PHONY: clean mrproper
clean:
	@rm -rf *.o
mrproper: clean
		@rm -rf $(EXEC)

# Local Variables:
# mode: makefile
# End:
