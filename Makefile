CC = clang
CFLAGS = -std=c23 -g -O2
LIBS = ncurses
LDFLAGS = $(shell pkg-config $(LIBS) --cflags)
LDLIBS = $(shell pkg-config $(LIBS) --libs)

PROG = mode
SOURCES = $(shell ls *.c)

.SILENT:

.PHONY: all

all: $(PROG)

$(PROG): $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(PROG) $(LDLIBS)