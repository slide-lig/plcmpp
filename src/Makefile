#################################################################################
# Copyright (c) 2014 Etienne Dublé, Martin Kirchgessner, Vincent Leroy, Alexandre Termier, CNRS and Université Joseph Fourier.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#  
# or see the LICENSE.txt file joined with this program.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#################################################################################


# Set DEBUG to 0 for better optimization
DEBUG=0
STATIC=0
NUMA=0
# TCMALLOC=[NONE|STANDARD|NUMA_AWARE]
TCMALLOC=NONE

GCC=g++ -Wall -std=gnu++11 -pthread -march=native
CXX=$(GCC)
RM=rm -f
CPPFLAGS=-I. -fmessage-length=0 -D_GLIBCXX_USE_NANOSLEEP
LDFLAGS=
LDLIBS=-lrt
POST_PROCESSING=

ifeq ($(DEBUG),1)
	DEBUG_OR_OPTIM= -g3 -Og -fno-omit-frame-pointer
else
	DEBUG_OR_OPTIM= -O3
	POST_PROCESSING += strip pLCM++
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
	$(GCC) $(LDFLAGS) -o pLCM++ $(OBJS) $(LDLIBS); $(POST_PROCESSING)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(GCC) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .depend pLCM++

include .depend
