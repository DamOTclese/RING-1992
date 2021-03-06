# This file creates the ring.exe program.
# Copyright (c) 1992 Fredric L. Rice
#
# The MODEL variable is used to describe
# the memory model to use.
#
# Compiler SWITCHES:
#    -G 	Generate for speed rather than size
#    -O 	Optimizes jumps
#    -d 	Merge duplicate strings
#    -Z 	This would optimize registers
#    -K		Default char is considered unsigned
#    -g1        Stop after first warning
#    -j1	Stop after first error
#    -v         Symbol table
#    -y         Line number
#    -w         Enable all warnings
#    -S         Produce assembly output

MODEL = l

SWITCHES1 = -Ic:\tc\include -G -O -d -Z -K -g1 -j1

# Describe the files that are used to
# create the executable module.

ring.exe : ring.obj

# Here is the command to execute if
# ring.exe needs to be linked.

     tlink /x /i @ring.res

# The following are the commands that create the object module

ring.obj : ring.c
     tcc -c $(SWITCHES1) -m$(MODEL) ring.c

