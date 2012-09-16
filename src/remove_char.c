/* Descrizione: Share your files with PCs that are in your LAN
 * Sviluppatore: Paolo Stivanin
 * Copyright: 2012
 * Licenza: GNU AGPL v3 <http://www.gnu.org/licenses/agpl-3.0.html>
 * Sito web: <https://github.com/polslinux/LanDND>
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "landnd.h"

void remove_char(char *str, char garbage){
	char *src, *dst;
	for(src = dst = str; *src != '\0'; src++){
		*dst = *src;
		if (*dst != garbage) dst++;
	}
	*dst = '\0';
}