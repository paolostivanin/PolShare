#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "landnd.h"

void remove_char(char *str, const char garbage){
	char *src, *dst;
	for(src = dst = str; *src != '\0'; src++){
		*dst = *src;
		if (*dst != garbage) dst++;
	}
	*dst = '\0';
}