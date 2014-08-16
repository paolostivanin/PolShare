#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nettle/md5.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>

#define MIN_FILE_SIZE 16777216


char *
check_md5 (const char *filename)
{
	int i, fd, ret;
	off_t done_size = 0, diff = 0, offset = 0, file_size = 0;
	unsigned char *addr = NULL, digest[MD5_DIGEST_SIZE];
	char *hash;
	struct stat fd_stat;
	struct md5_ctx ctx;
	
	md5_init (&ctx);

	hash = malloc ((MD5_DIGEST_SIZE * 2) + 1);
	if(hash == NULL)
	{
		fprintf (stderr, "Error during memory allocation (hash)\n");
		return NULL;
	}
	
	fd = open (filename, O_RDONLY);
	if (fd == -1)
	{
		fprintf (stderr, "%s\n", strerror (errno));
		return NULL;
	}
	
  	if (fstat (fd, &fd_stat) == -1)
  	{
  		fprintf (stderr, "%s\n", strerror (errno));
  		close (fd);
		return NULL;
  	}
  	
  	file_size = fd_stat.st_size;

	if (file_size < MIN_FILE_SIZE)
	{
		addr = mmap (NULL, file_size, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
		if (addr == MAP_FAILED)
		{
			fprintf (stderr, "check_md5: %s\n", strerror (errno));
			return NULL;
		}
		
		md5_update (&ctx, file_size, addr);
		ret = munmap (addr, file_size);
		if (ret == -1)
		{
			fprintf (stderr, "md5: munmap error\n");
			return NULL;
		}
		goto nowhile;
	}
	
	while (file_size > done_size)
	{
		addr = mmap(NULL, MIN_FILE_SIZE, PROT_READ, MAP_FILE | MAP_SHARED, fd, offset);
		if (addr == MAP_FAILED)
		{
			fprintf (stderr, "check_md5: %s\n", strerror (errno));
			return NULL;
		}
		
		md5_update (&ctx, MIN_FILE_SIZE, addr);
		
		done_size += MIN_FILE_SIZE;
		diff = file_size - done_size;
		offset += MIN_FILE_SIZE;
		
		if (diff < MIN_FILE_SIZE && diff > 0)
		{
			addr = mmap (NULL, diff, PROT_READ, MAP_FILE | MAP_SHARED, fd, offset);
			if (addr == MAP_FAILED)
			{
				fprintf (stderr, "check_md5: %s\n", strerror (errno));
				return NULL;
			}
			
			md5_update (&ctx, diff, addr);
			
			ret = munmap (addr, diff);
			if (ret == -1)
			{
				fprintf (stderr, "check_md5: munmap error\n");
				return NULL;
			}
			break;
		}
		
		ret = munmap (addr, MIN_FILE_SIZE);
		if (ret == -1)
		{
			fprintf (stderr, "check_md5: munmap error\n");
			return NULL;
		}
	}
	
	nowhile:
	md5_digest (&ctx, MD5_DIGEST_SIZE, digest);
 	for (i = 0; i < MD5_DIGEST_SIZE; i++)
 		sprintf (hash+(i*2), "%02x", digest[i]);
 	
 	hash[MD5_DIGEST_SIZE * 2] = '\0';
 	
 	close (fd);
 	
	return hash;
}
