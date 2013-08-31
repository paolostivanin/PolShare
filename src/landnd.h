#ifndef LANDND_H_INCLUDED
#define LANDND_H_INCLUDED

#define VERSION "1.1.0-alpha"

int do_send(void);
int do_recv(const int);
int get_ipv4(void);
int get_connected_ip(void);
char *get_ip_from_file(char *);
char *check_md5(char *);
void remove_char(char *, const char);
void replace_space_with_vbar(char *);

#endif
