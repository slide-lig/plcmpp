
# Set DEBUG to 0 for better optimization
DEBUG=0

CXX=g++
RM=rm -f
CPPFLAGS=-I. -Wall -c -fmessage-length=0 -std=gnu++11 -pthread -march=native
LDFLAGS=-std=gnu++11 -pthread -march=native
LDLIBS=

ifeq ($(DEBUG),1)
	CPPFLAGS += -g3 -Og -fno-omit-frame-pointer
	LDFLAGS += -g -Og -fno-omit-frame-pointer
else
	CPPFLAGS += -O3
	LDFLAGS += -O3
endif


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
