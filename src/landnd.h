#ifndef LANDND_H_INCLUDED
#define LANDND_H_INCLUDED

#define VERSION "1.1.1"

int do_send(const char *);
int do_recv(const char *);
int get_ipv4(void);
int get_connected_ip(const char *);
int do_action(int, const char *);
char *check_md5(char *);
void remove_char(char *, const char);
void replace_space_with_vbar(char *);

#endif
