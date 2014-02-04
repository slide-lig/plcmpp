
# Set DEBUG to 0 for better optimization
DEBUG=0
STATIC=1
NUMA=1
# TCMALLOC=[NONE|STANDARD|NUMA_AWARE]
TCMALLOC=NUMA_AWARE

GCC=g++ -Wall -std=gnu++11 -pthread -march=native
CXX=$(GCC)
RM=rm -f
CPPFLAGS=-I. -fmessage-length=0
LDFLAGS=
LDLIBS=
POST_PROCESSING=

ifeq ($(DEBUG),1)
	DEBUG_OR_OPTIM= -g3 -Og -fno-omit-frame-pointer
else
	DEBUG_OR_OPTIM= -O3
	POST_PROCESSING += strip PLCM
endif

CPPFLAGS += $(DEBUG_OR_OPTIM)
LDFLAGS += $(DEBUG_OR_OPTIM)

ifeq ($(NUMA),1)
	ifeq ($(STATIC),1)
		# workaround the fact that numa_init() and numa_fini()
		# would not be called if statically linked
		LDLIBS += -Wl,-whole-archive -lnuma -Wl,-no-whole-archive
	else
		LDLIBS += -lnuma
	endif
endif

ifeq ($(STATIC),1)
	# workaround the bogus detection of whether the pthread library
	# is necessary or not as the C++11 thread backend.
	LDLIBS += -Wl,-whole-archive -lpthread -Wl,-no-whole-archive
	LDFLAGS += -static
endif

ifneq ($(TCMALLOC),NONE)
	LDLIBS += -ltcmalloc_minimal
	# the NUMA-aware version of tcmalloc has a dependency on
	# libunwind.
	ifeq ($(TCMALLOC),NUMA_AWARE)
		LDLIBS += -lunwind
	endif
endif

SRCS=$(shell find . -iname '*.cpp')
OBJS=$(subst .cpp,.o,$(SRCS))

all: $(OBJS)
	$(GCC) $(LDFLAGS) -o PLCM $(OBJS) $(LDLIBS); $(POST_PROCESSING)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(GCC) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .depend PLCM

include .depend
