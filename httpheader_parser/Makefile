
MAKE = make --no-print-directory


.PHONY: all xp sp test clean

all: both
	
test: FLGS = -O0 -DRUN_TEST -Wall
both: FLGS = -O2 -DRUN_XP -DRUN_SP -Wall
both test: main.o spirit_parser.o xpressive_parser.o
	$(CXX) $(FLGS) -o a.out main.o spirit_parser.o xpressive_parser.o
	
xp:	FLGS = -O2 -DRUN_XP -Wall
xp: main.o xpressive_parser.o
	$(CXX) $(FLGS) -o a.out main.o xpressive_parser.o

sp:	FLGS = -O2 -DRUN_SP -Wall
sp: main.o spirit_parser.o
	$(CXX) $(FLGS) -o a.out main.o spirit_parser.o

.cpp.o:
	$(CXX) $(FLGS) -c $<

clean:
	rm a.out *.o

