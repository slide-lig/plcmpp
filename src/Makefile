CXX=g++
RM=rm -f
CPPFLAGS=-I. -O0 -g3 -Wall -c -fmessage-length=0 -std=gnu++11 -pthread -march=native
LDFLAGS=-g 
LDLIBS=

SRCS=$(shell find . -iname '*.cpp')
OBJS=$(subst .cpp,.o,$(SRCS))

all: $(OBJS)
	g++ $(LDFLAGS) -o PLCM $(OBJS) $(LDLIBS) 

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .depend PLCM

include .depend
