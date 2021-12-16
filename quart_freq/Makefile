CC=g++
#-pedantic removed because of 128 bit stuff
CFLAGS=-c -Wall -ansi -Wno-long-long -m64 -O2 -std=c++11
LDFLAGS=-m64

SDIR=src
OUT_PREFIX=
OUT_POSTFIX=_bin
OUT=

ODIR_PREFIX=obj
ODIRS=

TOOLS=toolTreeGen

SOURCES=OutputHandler.cpp NewickParser.cpp NaiveImpl.cpp Soda13Impl.cpp RootedTree.cpp \
		HDTCountingLeaf.cpp HDTCountingCCToC.cpp HDTCountingIGToC.cpp HDTCountingCTransform.cpp HDTCountingG.cpp \
		HDT.cpp HDTFactory.cpp HDTListUtils.cpp RootedTreeFactory.cpp main.cpp Stopwatch.cpp \
		int_stuff.cpp

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	LDFLAGS += -lrt
endif

ifdef QUARTETS
	CFLAGS += -DquartetsToo
	OUT_PREFIX := quart
else
	OUT_PREFIX := trip
endif

ifndef NO_N4_128
	CFLAGS += -DN4INT128
else
	OUT_POSTFIX := $(OUT_POSTFIX)_64bitints
	ODIR_PREFIX := $(ODIR_PREFIX)_64bitints
endif

ifdef DEBUG
	CFLAGS += -D_DEBUG -g
	LDFLAGS += -D_DEBUG -g
	OUT_POSTFIX := $(OUT_POSTFIX)_debug
	ODIR_PREFIX := $(ODIR_PREFIX)_debug
	SOURCES := $(SOURCES) nvwa/debug_new.cpp
	ODIRS := $(ODIRS) nvwa
endif

ifdef PROF
	ifndef DEBUG
		CFLAGS += -g
		LDFLAGS += -g
	endif
	CFLAGS += -pg
	LDFLAGS += -pg
	OUT_POSTFIX := $(OUT_POSTFIX)_prof
	ODIR_PREFIX := $(ODIR_PREFIX)_prof
endif

ifdef SW
	CFLAGS += -DdoStopwatch -DdoCompactStopWatch
	OUT := $(OUT)_sw
endif

ifdef COUNTER
	CFLAGS += -DdoCounter
	OUT := $(OUT)_counter
endif

ifdef NOSWAP
	CFLAGS += -DquartetsNoSwap
	OUT := $(OUT)_noswap
endif

ifdef SWAPCHECK
	ifdef NOSWAP
		CFLAGS += -DquartetsSwapAnyways
	else
		CFLAGS += -DquartetsNoSwap -DquartetsSwapAnyways
	endif
	OUT := $(OUT)_swapcheck
endif

ifdef CALCE
	CFLAGS += -DcalcE
	OUT := $(OUT)_calcE
endif

ifndef NOEXTRACT
	CFLAGS += -DdoExtractAndContract
else
	OUT := $(OUT)_noextract
endif

ifdef CONTRACT_NUM
	CFLAGS += -DCONTRACT_MAX_EXTRA_SIZE=$(CONTRACT_NUM)
endif

ODIR := $(ODIR_PREFIX)_$(OUT_PREFIX)$(OUT)
ODIRS := $(foreach _ODIR,$(ODIRS),$(ODIR)/$(_ODIR))
ODIRS := $(ODIR) $(ODIRS)
OUT := $(OUT_PREFIX)$(OUT)$(OUT_POSTFIX)

_OBJECTS=$(SOURCES:.cpp=.o)
OBJECTS=$(patsubst %,$(ODIR)/%,$(_OBJECTS))

.PHONY: all clean tools $(TOOLS)

all: $(OUT)

clean:
	rm -rf obj_* *_bin *_bin_debug *_bin.exe *_bin_debug.exe

tools: $(TOOLS)

$(ODIRS):
	[ -e $(ODIR) ] || mkdir $(ODIRS)

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -o $@ $<

$(OUT): $(ODIR) $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)
	@echo 
	@echo --- Compiled binary $(OUT) ---

# Compile other tools
$(TOOLS):
	[ -e $@ ] && ( cd $@ ; make install )
