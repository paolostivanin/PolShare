#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv){
	if(argc != 2){
		printf("Usage: %s <file-to-send>\n", argv[0]);
		return EXIT_FAILURE;
	}
	char *input_file = NULL;
	size_t len_input;
	len_input = strlen(argv[1])+1;
	input_file = (char *)malloc(len_input);
	if(input_file == NULL){
		printf("Error during memory allocation, exiting...\n");
		return EXIT_FAILURE;
	}
	strcpy(input_file, argv[1]);
	printf("%s\n", argv[1]);
	free(input_file);
	return 0;
}
