#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char **argv){
	if(argc != 2){
		printf("Usage: %s <file-to-send>\n", argv[0]);
		return EXIT_FAILURE;
	}
	struct stat fileStat;
	char *input_file = NULL, *token = NULL, *file = NULL;
	size_t len_input;
	len_input = strlen(argv[1])+1;
	input_file = (char *)malloc(len_input);
	if(input_file == NULL){
		printf("Memory allocation ERROR, exiting...\n");
		return EXIT_FAILURE;
	}
	strcpy(input_file, argv[1]);
	if(stat(input_file, &fileStat) < 0){
		printf("stat ERROR, exiting...");
		free(input_file);
		return EXIT_FAILURE;
	}
	if(S_ISDIR(fileStat.st_mode)){
		printf("ERROR: This is a directory not file, exiting...\n");
		free(input_file);
		return EXIT_FAILURE;
	}
	printf("%s\n", argv[1]);
	token = strdup(input_file);
	file = basename(token); /* basename potrebbe modificare input_file così ne passo una copia e sono al sicuro */
	printf("%s\n", file);
	free(input_file);
	free(token);
	return 0;
}
