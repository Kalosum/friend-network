#
# Created by gmakemake (Ubuntu Jul 25 2014) on Fri Dec  1 02:03:40 2023
#

#
# Definitions
#

.SUFFIXES:
.SUFFIXES:	.a .o .c .C .cpp .s .S
.c.o:
		$(COMPILE.c) $<
.C.o:
		$(COMPILE.cc) $<
.cpp.o:
		$(COMPILE.cc) $<
.S.s:
		$(CPP) -o $*.s $<
.s.o:
		$(COMPILE.cc) $<
.c.a:
		$(COMPILE.c) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.C.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.cpp.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%

CC =		gcc
CXX =		g++

RM = rm -f
AR = ar
LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)
LINK.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS)
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) -c
COMPILE.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c
CPP = $(CPP) $(CPPFLAGS)
########## Flags from header.mak

#
# Define any additional compilation flags you want to use while compiling
# and testing your code.  For instance, you could use "OPTS=-DDEBUG" as a
# command-line option to 'make' to predefine the CPP macro symbol 'DEBUG',
# which could be used to conditionally compile in some debugging code.
# Another use might be "OPTS=-Werror", which would pass the -Werror option
# to GCC, telling it to treat all warning messages as if they were fatal
# coompilation errors.
#
OPTS =

#
# Compilation options
#
CFLAGS = -ggdb -std=c99 -Wall -Wextra -pedantic $(OPTS)

#
# Library options
#
# The first version of CLIBFLAGS tells the linker to look in the
# course account for a library file named "libhash.a" and link in 
# precompiled code from that library.  The library contains a working
# implementation of the HashADT module which you can use during
# development of your solution.
#
# To use the supplied version, DO NOT copy your HashADT.c file into the
# project source directory; create your Makefile with gmakemake, and
# compile and link the program.
#
# Once you are ready to test your program with your own HashADT
# implementation, do the following:
#
#   1. Copy your HashADT.c file into the project directory
#   2. Edit header.mak; comment out the first CLIBFLAGS statement
#      and uncomment the second one
#   3. Re-run gmakemake to create a new Makefile
#   4  Run 'make realclean' and then 'make'.
#

#
# This version links your code against the precompiled HashADT library
#
# CLIBFLAGS = -L/home/course/csci243/pub/projects/02 -lhash -lm 

#
# This version doesn't use the precompiled HashADT library; instead,
# your implementation will be used.
#
# CLIBFLAGS = -lm

########## End of flags from header.mak


CPP_FILES =	
C_FILES =	HashADT.c Support.c amici.c
PS_FILES =	
S_FILES =	
H_FILES =	HashADT.h Support.h
SOURCEFILES =	$(H_FILES) $(CPP_FILES) $(C_FILES) $(S_FILES)
.PRECIOUS:	$(SOURCEFILES)
OBJFILES =	HashADT.o Support.o 

#
# Main targets
#

all:	amici 

amici:	amici.o $(OBJFILES)
	$(CC) $(CFLAGS) -o amici amici.o $(OBJFILES) $(CLIBFLAGS)

#
# Dependencies
#

HashADT.o:	HashADT.h
Support.o:	Support.h
amici.o:	HashADT.h

#
# Housekeeping
#

Archive:	archive.tgz

archive.tgz:	$(SOURCEFILES) Makefile
	tar cf - $(SOURCEFILES) Makefile | gzip > archive.tgz

clean:
	-/bin/rm -f $(OBJFILES) amici.o core

realclean:        clean
	-/bin/rm -f amici 
