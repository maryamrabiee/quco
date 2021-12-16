CC=g++
CFLAGS=-c -Wall -pedantic -ansi -m64
LDFLAGS=-m64

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	LDFLAGS += -lrt
endif

OUT=tool_treegen
ODIR=obj_tools
SOURCES=main.cpp
_OBJECTS=$(SOURCES:.cpp=.o)
OBJECTS=$(patsubst %,$(ODIR)/%,$(_OBJECTS))

.PHONY: install clean

install: $(OUT)
	if [ -e $(OUT).exe ] ; \
	then \
		strip $(OUT).exe ; \
		mv $(OUT).exe .. ; \
	else \
		strip $(OUT) ; \
		mv $(OUT) .. ; \
	fi;

clean:
	[ ! -e $(ODIR) ] || rm -rf $(ODIR)
	[ ! -e $(OUT) ] || rm $(OUT)

$(OUT): $(ODIR) $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

$(ODIR):
	[ -e $(ODIR) ] || mkdir $(ODIR)

$(ODIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ $<
