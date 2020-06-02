CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g -O3
LDFLAGS=-g -O3
LDLIBS=

SRCS=Simplified.cpp
SRCSHARDEST=SudokuSolverWithHardestEver.cpp
SRCSEPI=EPI_Simplified.cpp

OBJS=$(subst .cpp,.o,$(SRCS))
OBJSHARDEST=$(subst .cpp,.o,$(SRCSHARDEST))
OBJSEPI=$(subst .cpp,.o,$(SRCSEPI))


all: Simplified SudokuSolverWithHardestEver EPI_Simplified

SudokuSolverWithHardestEver: $(OBJSHARDEST)
	$(CXX) $(LDFLAGS) -o SudokuSolverWithHardestEver $(OBJSHARDEST) $(LDLIBS) 

Simplified: $(OBJS)
	$(CXX) $(LDFLAGS) -o Simplified $(OBJS) $(LDLIBS) 

EPI_Simplified: $(OBJSEPI)
	$(CXX) $(LDFLAGS) -o EPI_Simplified $(OBJSEPI) $(LDLIBS) 

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS) $(OBJSEPI) $(OBJSHARDEST)

distclean: clean
	$(RM) Simplified SudokuSolverWithHardestEver EPI_Simplified
