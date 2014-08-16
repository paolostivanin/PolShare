#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <signal.h>
#include "polshare.h"


static void signal_handler (int);
static void cleanup (void);
static void init_signals (void);
char *make_message (const char *, ...);


struct descriptors
{
	int fd;
	int sockd, newsockd;
} fsd;

struct sigaction sigact;


int
do_recv (const char *answer)
{
	if (get_ipv4 () < 0)
	{
		fprintf (stderr, "ERROR on getting IPv4 address, exiting...\n");
		return -1;
	}
	
	struct sockaddr_in my_addr, peer_addr;
	
	unsigned int file_size = 0, total_bytes_read = 0, tmp_file_size;
	ssize_t nread = 0, tx = 0;
	size_t socket_len = 0;
	void *filebuffer = NULL;
	char *filename = NULL;
	char yes_or_no[2], hash[33];
	
	/* Info sul server locale */
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = 15000;
	my_addr.sin_addr.s_addr = INADDR_ANY;
	
	init_signals ();

	if ((fsd.sockd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf (stderr, "Error on socket creation\n");
		return -1;
	}

	if (bind (fsd.sockd, (struct sockaddr *) &my_addr, sizeof (struct sockaddr_in)) < 0)
	{
		fprintf (stderr, "Bind error\n");
		return -1;
	}
	
	if (listen (fsd.sockd, 10) < 0)
	{
		fprintf (stderr, "Error on listen");
		return -1;
	}
	
	new_conn:
	socket_len = sizeof (peer_addr);
	if ((fsd.newsockd = accept (fsd.sockd, (struct sockaddr *) &peer_addr, (socklen_t *) &socket_len)) < 0)
	{
		fprintf (stderr, "Connection error (accept)\n");
		return -1;
  	}
	
	file_size = total_bytes_read = nread = 0;
	memset( yes_or_no, 0, sizeof (yes_or_no));
	if (recv (fsd.newsockd, &file_size, sizeof (file_size), 0) < 0)
	{
		fprintf(stderr, "Error on receiving the file name length\n");
   		return -1;
	}
	
	filename = (char *) malloc (file_size);
	if (filename == NULL)
	{
		fprintf(stderr, "Error during filename memory allocation\n");
		return -1;
	}
	if (recv (fsd.newsockd, filename, file_size, 0) < 0)
	{
   		fprintf(stderr, "Error on receiving the file name\n");
   		free(filename);
   		return -1;
	}
	file_size = 0;
	if (recv (fsd.newsockd, &file_size, sizeof(file_size), 0) < 0)
	{
		fprintf (stderr, "Error on receiving the file size\n");
   		free (filename);
		return -1;
	}
    
	if (strcmp (answer, "y") == 0)
	{
		strcpy (yes_or_no, "Y");
		goto auto_accept;
	}

	printf ("Do you want to receive the file '%s' which size is '%"PRIu32"' bytes? (Y or N)\n", filename, file_size);
	
	another_yorn:
	if(scanf("%1s", yes_or_no) == EOF)
	{
	   	fprintf (stderr, "Scanf error\n");
		free (filename);
		return -1;
	}
	yes_or_no[0] = toupper (yes_or_no[0]);
	if (strcmp (yes_or_no, "Y") != 0)
	{
	   	if (strcmp (yes_or_no, "N") != 0)
	   	{
			printf ("You have to write Y or N, try again: ");
			memset (yes_or_no, 0, sizeof (yes_or_no));
			goto another_yorn;
		}
	}
	auto_accept:
	if (strcmp (yes_or_no, "N") == 0)
	{
	   	printf ("Transfer aborted\n");
		if (send (fsd.newsockd, yes_or_no, 2, 0) < 0)
		{
			printf ("Error on sending N\n");
			free (filename);
			return -1;   
		}
		close (fsd.newsockd);
		free (filename);
		goto new_conn;
	}
	else
	{
		if (send (fsd.newsockd, yes_or_no, 2, 0) < 0)
		{
			fprintf (stderr, "Error on sending Y\n");
			free (filename);
			return -1;
		}
	}
	
	tmp_file_size = file_size;
	filebuffer = malloc (file_size);
	if (filebuffer == NULL)
	{
		fprintf (stderr, "Error during filebuffer memory allocation\n");
		free (filename);
		return -1;
	}
	
	fsd.fd = open (filename, O_CREAT | O_WRONLY, 0644);
	if (fsd.fd  < 0)
	{
		fprintf (stderr, "Error during file opening\n");
		free (filename);
		free (filebuffer);
   	  	return -1;
	}
	tx = 0;
	while((total_bytes_read != file_size) && ((nread = read(fsd.newsockd, filebuffer, tmp_file_size)) > 0))
	{
		tx += nread;
		printf ("\r%zd%%", (tx * 100 / file_size));
   	  	if (write (fsd.fd, filebuffer, nread) != nread)
   	  	{
			fprintf (stderr, "Write error\n");
			free (filename);
  		  	free (filebuffer);
			return -1;
   		}
   	  	total_bytes_read += nread;
   	  	tmp_file_size -= nread;
	}
	char *file_md5 = check_md5 (filename);
	if (recv (fsd.newsockd, hash, 33, 0) < 0)
	{
		fprintf (stderr, "Error on receiving file md5\n");
		free (filename);
		free (file_md5);
		return -1;
	}
	if (strcmp (file_md5, hash) == 0)
	{
		memset (yes_or_no, 0, sizeof(yes_or_no));
		strcpy (yes_or_no, "Y");
		if (send (fsd.newsockd, yes_or_no, 2, 0) < 0)
		{
			free (filename);
			free (file_md5);
			return -1;
		}
		printf ("\n--> File successfully received\n");
	}
	else
	{
	  	memset (yes_or_no, 0, sizeof (yes_or_no));
		strcpy (yes_or_no, "N");
		if (send (fsd.newsockd, yes_or_no, 2, 0) < 0)
		{
			close (fsd.newsockd);
			close (fsd.fd);
			free (filename);
			free (file_md5);
			goto new_conn;
		}
		printf ("\n--> File transfer FAILED, md5sum doesn't match\n");
	}
	
	free (file_md5);
	close (fsd.fd);
	free (filename);
	free (filebuffer);
	close (fsd.newsockd);
	goto new_conn;
}


static void
init_signals (void)
{
	sigact.sa_handler = signal_handler;
	sigemptyset (&sigact.sa_mask);
	sigact.sa_flags = 0;
   	sigaction (SIGINT, &sigact, (struct sigaction *)NULL);
}


static void
signal_handler (int sig)
{
	
	if (sig == SIGINT)
	{
		char *s = make_message ("\nCtrl-C caught, exiting...\n");
		if (s != NULL)
		{
			printf("%s\n");
			free (s);
		}
		cleanup ();
	}
}


char *
make_message (const char *fmt, ...)
{
	
	int n;
	int size = 100;
    
	char *p, *np;
	va_list ap;
    
	p = malloc (size);
	if (p == NULL)
		return NULL;
	
	while (1)
	{
		va_start (ap, fmt);
		n = vsnprintf (p, size, fmt, ap);
		va_end (ap);
		
		if (n < 0)
		{
			free (p);
			return NULL;
		}
		
		if (n < size)
			return p;
			
		size = n + 1;
		np = realloc (p, size);
		if (np == NULL)
		{
			free (p);
			return NULL;
		}
		else
			p = np;
	}
}


static void
cleanup (void)
{
	
	sigemptyset (&sigact.sa_mask);
   
	if (fsd.fd)
		close (fsd.fd);
	
	if (fsd.newsockd)
		close (fsd.newsockd);
    
	if (fsd.sockd)
		close (fsd.sockd);
}
