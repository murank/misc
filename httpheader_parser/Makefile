CC = g++


.SUFFIXES: .o .cpp
.PHONY: all xp sp test clean

.cpp.o:
	$(CC) $(FLGS) -c $<

all: both
	
FLGS = -O0 -DRUN_TEST
test: main.o spirit_parser.o xpressive_parser.o
	$(CC) $(FLGS) -o a.out main.o spirit_parser.o xpressive_parser.o
	
FLGS = -O2 -DRUN_XP
xp: main.o xpressive_parser.o
	$(CC) $(FLGS) -o a.out main.o xpressive_parser.o

#FLGS = -O2 -DRUN_SP
sp: main.o spirit_parser.o
	override FLGS = -O2 -DRUN_SP
	$(CC) $(FLGS) -o a.out main.o spirit_parser.o

FLGS = -O2 -DRUN_XP -DRUN_SP
both: main.o spirit_parser.o xpressive_parser.o
	$(CC) $(FLGS) -o a.out main.o spirit_parser.o xpressive_parser.o


clean:
	rm a.out *.o

