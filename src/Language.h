#ifndef __Tower_Defense__Language__
	#define __Tower_Defense__Language__

	#include <dirent.h> 
	#include <stdio.h> 
	#include "Estruturas/Misc.h"

	#define ALPHABET 26
	
	typedef struct _phrase {
		char *string;
		char *var;
		struct _phrase *next;
	} Phrase;

	typedef struct _language {
		Phrase **hash;
	} Language;

	typedef struct _languages {
		int loaded;
		int current;
		char **files;
		char **names;
		Language **l;
	} Languages;

	Language *load_language(char *content, int index, char **name);
	Languages *init_languages(char *folder);
	char *_(char *var);

#endif