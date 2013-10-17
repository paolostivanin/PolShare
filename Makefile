CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-result -Wformat-security -O2 -D_FORTIFY_SOURCE=2
LDFLAGS = -lgcrypt

all: landnd

landnd: src/main.c src/check_md5.c src/get_ipv4.c src/get_connected_ip.c src/do_send.c src/do_recv.c src/remove_char.c src/replace_space_with_vbar.c
	@echo "--> Compiling LanDnD..."
	@$(CC) $(CFLAGS) -o landnd src/main.c src/check_md5.c src/get_ipv4.c src/get_connected_ip.c src/do_send.c src/do_recv.c src/remove_char.c src/replace_space_with_vbar.c $(LDFLAGS)
	@echo "--> All done :)"
