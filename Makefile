CC = clang

CFLAGS = -Wall -Wextra -O2 -Wformat=2 -fstack-protector-all -fPIE -Wstrict-prototypes -Wunreachable-code  -Wwrite-strings -Wpointer-arith -Wbad-function-cast -Wcast-align -Wcast-qual $(shell pkg-config --cflags gtk+-3.0)
NOFLAGS = -Wno-unused-result -Wno-format-nonliteral
DFLAGS = -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2

LDFLAGS = -Wl,-z,now -Wl,-z,relro

LIBS = -lnettle

SOURCES = $(wildcard src/*.c)
OBJS = ${SOURCES:.c=.o}

PROG = polshare

.SUFFIXES:.c .o

.c.o:
	$(CC) -c $(CFLAGS) $(NOFLAGS) $(DFLAGS) $< -o $@

all: $(PROG)


$(PROG) : $(OBJS)
	$(CC) $(CFLAGS) $(NOFLAGS) $(DFLAGS) $(OBJS) -o $@ $(LIBS)


.PHONY: clean

clean :
	rm -f $(PROG) $(OBJS)
