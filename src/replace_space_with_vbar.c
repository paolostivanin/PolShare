#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "polshare.h"

void
replace_space_with_vbar (char *str)
{
	char *src = NULL, *dst = NULL;
	
	for (src = dst = str; *src != '\0'; src++)
	{
		*dst = *src;
		if(*dst == ' ')
		{
			if( *(++dst) == '/' || *dst == '@')
				*(--dst) = '|';
			
			else --dst; 
		}
		
		dst++;
		
		if (*dst == '\0')
			break;
	}
	
	*dst = '\0';
}
