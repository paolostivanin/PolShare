AC_INIT([LanDND], [1.1.0-exp], [admin@polslinux.it])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CHECK_HEADERS([stdio.h \
stdlib.h \
string.h \
unistd.h \
sys/types.h \
ctype.h \
stdint.h \
inttypes.h \
signal.h \
dirent.h \
wait.h \
sys/stat.h \
sys/utsname.h \
netdb.h \
netinet/in.h \
arpa/inet.h \
fcntl.h \
sys/socket.h \
sys/sendfile.h
],,AC_MSG_ERROR[
required header file missing])
AC_CHECK_FUNCS([gethostbyname \
socket \
htons \
connect \
recv \
send \
write \
read \
atoi \
strlen \
strdup \
strcmp \
strtok \
strcpy \
memset \
free \
malloc \
sendfile \
stat \
rename \ 
fopen \
open
],,AC_MSG_ERROR([
required standard library function missing]))
AC_CONFIG_FILES([
Makefile
src/Makefile
])
AC_OUTPUT

