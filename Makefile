CC = gcc
CFLAGS = -Wall -Wextra -Wformat-security -O2 -D_FORTIFY_SOURCE=2 -fstack-protector

CLANG_OR_GCC := $(shell which clang)
ifeq "$(CLANG_OR_GCC)" "/usr/bin/clang"
 VER := $(shell clang --version | clang --version | grep -o 3.1 | awk '0; NR == 1 { print $0 } ')
 ifeq "$(VER)" "3.1"
  CC = clang
 endif
endif

all: landnd

landnd: src/landnd.c
	@echo "--> Using ${CC} to compile... <--"
	@echo "--> Compiling LanDnD..."
	@$(CC) $(CFLAGS) -o landnd src/landnd.c
	@echo "--> All done :)"
