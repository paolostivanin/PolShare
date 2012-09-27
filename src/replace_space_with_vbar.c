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

void replace_space_with_vbar(char *str){
	char *src = NULL, *dst = NULL;
	for(src = dst = str; *src != '\0'; src++){
		*dst = *src;
		if(*dst == ' '){ /* Se trovo uno spazio... */
			if( *(++dst) == '/' || *dst == '@'){ /* ...e il prossimo carattere è uno... / */
				*(--dst) = '|'; /* ...allora sostituisco lo spazio con | */
			}
			else --dst; /* ...altrimenti torno indietro. Se non torno indietro il programma è "troppo avanti" a causa
						 * del controllo ++dst == '\' */
		}
		dst++;
		if(*dst == '\0')break; /* Se trovo il carattere di terminazione allora mi fermo */
	}
	*dst = '\0';
}