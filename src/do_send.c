#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <errno.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include "polshare.h"


static void cleanup (char *, char *, char *, int, int);

int
do_send (const char *ip)
{
	struct stat file_stat;
	struct sockaddr_in remote_server_addr;
	int fd, sockd = -1;
	unsigned int file_size = 0, size_to_send = 0;
	off_t offset = 0;
	ssize_t rc, tx = 0;
	char buffer[2], hash[33], bufname[512];
	char *file = NULL, *tmp_input = NULL, *input_file = NULL;
	const char *is_gnome = "'";
	
	/* Info sul server remoto */
	remote_server_addr.sin_family = AF_INET;
	remote_server_addr.sin_port = 15000;
	inet_pton(AF_INET, ip, &(remote_server_addr.sin_addr));
	
	if ((sockd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf (stderr, "%s\n", strerror (errno));
		return -1;
  	}

  	if (connect (sockd, (struct sockaddr *) &remote_server_addr, sizeof (struct sockaddr_in)) < 0)
  	{
  		fprintf (stderr, "%s\n", strerror (errno));
   		close (sockd);
   		return -1;
  	}

	printf ("Drag (or write) the file to send: ");
	fgets (bufname, 510, stdin);
	tmp_input = malloc (strlen (bufname));
	if (tmp_input == NULL)
	{
		printf ("malloc error (tmp_input)\n");
		close (sockd);
		return -1;
	}
	strncpy (tmp_input, bufname, strlen (bufname));
	tmp_input[strlen (bufname) - 1] = '\0';
	
	if (*tmp_input == *is_gnome)
		remove_char (tmp_input, '\''); /* se il primo carattere è ' allora lo tolgo */
	
	input_file = strdup (tmp_input);

	rc = offset = 0;
	if (tmp_input[0] == '.' && tmp_input[1] == '.')
	{
		printf ("You cannot use relative path\n");
		cleanup (tmp_input, input_file, NULL, sockd, 0);
		return -1;
	}
	if (strlen (tmp_input) > 5)
	{
		char *start = NULL;
		start = malloc (6);
		if (start == NULL)
		{
			printf ("memory allocation error\n");
			cleanup (tmp_input, input_file, NULL, sockd, 0);
			return -1;
		}
		memcpy (start, tmp_input, 5);
		start[5] = '\0';
		if (strcmp(start, "/home") != 0)
		{
			printf ("%s\n", start);
			printf ("You cannot send file from outside your home directory\n");
			cleanup (tmp_input, start, input_file, sockd, 0);
			return -1;
		}
		free (start);
	}
	if (stat (tmp_input, &file_stat) < 0)
	{
		fprintf (stderr, "%s\n", strerror (errno));
		cleanup (tmp_input, input_file, NULL, sockd, 0);
		return -1;
	}
	if (S_ISDIR (file_stat.st_mode))
	{
		fprintf(stderr, "ERROR: This is a directory not file, exiting...\n");
		cleanup (tmp_input, input_file, NULL, sockd, 0);
		return -1;
	}
	
	file = basename (input_file); /* basename potrebbe modificare input_file così ne passo una copia e sono al sicuro */
	file_size = strlen (file) + 1;
	
	if (send (sockd, &file_size, sizeof (file_size), 0) < 0)
	{
		fprintf (stderr, "%s\n", strerror (errno));
		cleanup (tmp_input, input_file, NULL, sockd, 0);
		return -1;
	}
	if (send (sockd, file, file_size, 0) < 0)
	{
		fprintf (stderr, "%s\n", strerror (errno));
	    	cleanup (tmp_input, input_file, NULL, sockd, 0);
		return -1;
	}
	file_size = file_stat.st_size;
	if (send (sockd, &file_size, sizeof (file_size), 0) < 0)
	{
		fprintf (stderr, "%s\n", strerror (errno));
    		cleanup (tmp_input, input_file, NULL, sockd, 0);
		return -1;
	}
	printf ("Waiting for server confirmation...\n");
	if (recv (sockd, buffer, 2, 0) < 0)
	{
		fprintf (stderr, "%s\n", strerror (errno));
    		cleanup (tmp_input, input_file, NULL, sockd, 0);
		return -1;		
	}
	if (strcmp (buffer, "N") == 0)
	{
		printf ("The server has aborted the file transfer\n");
    		cleanup (tmp_input, input_file, NULL, sockd, 0);
		return -2;
	}
  	fd = open (tmp_input, O_RDONLY | O_NOFOLLOW);
  	if (fd < 0)
  	{
		fprintf (stderr, "%s\n", strerror (errno));
   		cleanup (tmp_input, input_file, NULL, sockd, 0);
   		return -1;
  	}

  	tx = 0;
	for (size_to_send = file_size; size_to_send > 0; )
	{
		rc = sendfile (sockd, fd, &offset, size_to_send);
    		if (rc <= 0)
    		{
	    		fprintf (stderr, "%s\n", strerror (errno));
      			cleanup (tmp_input, input_file, NULL, sockd, fd);
      			return -1;
    		}
    	    	tx += rc;
   		printf ("\r%zd%%", (tx * 100 / file_size));
    		fflush (NULL);
    		size_to_send -= rc;
  	}
  	memset (buffer, 0, sizeof (buffer));
  	char *file_md5 = check_md5 (tmp_input);
  	strcpy (hash, file_md5);
  	hash[32] = '\0';
  	if (send (sockd, hash, 33, 0) < 0)
  	{
		fprintf (stderr, "%s\n", strerror (errno));
    		cleanup (tmp_input, input_file, file_md5, sockd, fd);
		return -1;
	}
  	if (recv (sockd, buffer, 2, 0) < 0)
  	{
		fprintf (stderr, "%s\n", strerror (errno));
    		cleanup (tmp_input, input_file, file_md5, sockd, fd);
		return -1;
	}
	if (strcmp (buffer, "Y") == 0)
		printf ("\n--> md5sum OK, file successfully transferred\n");

	else
		printf("\n--> md5sum NOT OK, file transfer FAILED\n");
		
	cleanup (tmp_input, input_file, file_md5, sockd, fd);
	
	return 0;
}


static void
cleanup (	char *f1,
		char *f2,
		char *f3,
		int c1,
		int c2)
{
	if (f1)
		free (f1);

	if (f2)
		free (f2);
		
	if (f3)
		free (f3);
		
	if (c1)
		close (c1);
		
	if (c2)
		close (c2);
}
