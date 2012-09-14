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

landnd: src/main.c src/get_ipv4.c src/get_connected_ip.c src/do_send.c src/do_recv.c
	@echo "--> Using ${CC} to compile... <--"
	@echo "--> Compiling LanDnD..."
	@$(CC) $(CFLAGS) -o landnd src/main.c src/get_ipv4.c src/get_connected_ip.c src/do_send.c src/do_recv.c
	@echo "--> All done :)"
