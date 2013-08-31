#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gcrypt.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define GCRYPT_VER "1.5.0"

char *check_md5(char *filename){
	if(!gcry_check_version(GCRYPT_VER)){
		fputs("libgcrypt version mismatch\n", stderr);
		exit(2);
	}
	gcry_control(GCRYCTL_INIT_SECMEM, 16384, 0);
	gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
	int algo, i, fd;
	off_t donesize = 0, diff = 0, fsize = 0;
	FILE *fp;
	char *buffer, *hashed = malloc(32);
	struct stat fileStat;
	const char *name = gcry_md_algo_name(GCRY_MD_MD5);

	if(hashed == NULL){
		printf("Error during memory allocation\n");
		return (char *)1;
	}
	
	algo = gcry_md_map_name(name);
	fd = open(filename, O_RDONLY);
  	if(fstat(fd, &fileStat) < 0){
  		perror("Fstat error");
    	close(fd);
    	return (char *)1;
  	}
  	fsize = fileStat.st_size;
  	close(fd);
  	
	fp = fopen(filename, "r");
	gcry_md_hd_t hd;
	gcry_md_open(&hd, algo, 0);
	if(fsize < 16){ //se il file è più piccolo di 16 allora in una chiamata ho finito;
		buffer = malloc(fsize);
  		if(buffer == NULL){
  			printf("malloc error\n");
  			exit(1);
  		}
		fread(buffer, 1, fsize, fp);
		gcry_md_write(hd, buffer, fsize);
		goto nowhile;
	}
	buffer = malloc(16); // altrimenti alloco 16 byte al buffer...
  	if(buffer == NULL){
  		printf("malloc error\n");
  		exit(1);
  	}
	while(fsize > donesize){ //...e finchè la grandezza del file è maggiore di quello letto...
		fread(buffer, 1, 16, fp); //...leggo 16 byte alla volta...
		gcry_md_write(hd, buffer, 16); //...scrivo 16 byte....
		donesize+=16; //...aumento i byte letti di 16...
		diff=fsize-donesize; //...calcolo la differenza...
		if(diff < 16){ //...e se la differenza è minore di 16 allora termino con l'ultima chiamata...
			fread(buffer, 1, diff, fp); //...che legge soltanto i byte necessari...
			gcry_md_write(hd, buffer, diff); //...e li scrive!
			break;
		}
	}
	nowhile:
	gcry_md_final(hd);
	unsigned char *md5 = gcry_md_read(hd, algo);
 	for(i=0; i<16; i++){
 		sprintf(hashed+(i*2), "%02x", md5[i]);
 	}
 	free(buffer);
 	fclose(fp);
	gcry_md_close(hd);
	return hashed;
}
