/* Descrizione: Share your files with PCs that are in your LAN
 * Sviluppatore: Paolo Stivanin
 * Copyright: 2012
 * Licenza: GNU AGPL v3 <http://www.gnu.org/licenses/agpl-3.0.html>
 * Sito web: <https://github.com/polslinux/LanDND>
 */

#ifndef LANDND_H_INCLUDED
#define LANDND_H_INCLUDED
int do_send(void);
int do_recv(void);
int get_ipv4(void);
int get_connected_ip(void);
char *get_ip_from_file(char *);
void remove_char(char *, char);
#endif
