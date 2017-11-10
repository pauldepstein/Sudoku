CC=gcc
CXX=g++
RM=del
CPPFLAGS=-g -O3
LDFLAGS=-g -O3
LDLIBS=

SRCS=Simplified.cpp
SRCSHARDEST=SudokuSolverWithHardestEver.cpp
SRCSEPI=EPI_Simplified.cpp

OBJS=$(subst .cc,.o,$(SRCS))
OBJSHARDEST=$(subst .cc,.o,$(SRCSHARDEST))


all: Simplified SudokuSolverWithHardestEver EPI_Simplified

SudokuSolverWithHardestEver: $(OBJSHARDEST)
	$(CXX) $(LDFLAGS) -o SudokuSolverWithHardestEver $(OBJSHARDEST) $(LDLIBS) 

Simplified: $(OBJS)
	$(CXX) $(LDFLAGS) -o Simplified $(OBJS) $(LDLIBS) 

EPI_Simplified: $(OBJS)
	$(CXX) $(LDFLAGS) -o EPI_Simplified $(OBJS) $(LDLIBS) 

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) tool