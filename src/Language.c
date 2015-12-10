#include "language.h"

extern Languages *lang;
extern Configuration *config;

Language *load_language(char *content, int index, char **name) {
	int previous = 0;
	int current_index = 0;
	Phrase *current_phrase = NULL;
	
	Language *l = malloc(sizeof(Language));
	l->hash = malloc(sizeof(Phrase*) * ALPHABET);
	for(int i =0; i < ALPHABET; i++){
		l->hash[i] = NULL;
	}

	for(int i = 0; content[i] != '\0'; i++){
		if(content[i] == '=' && *name) {
			//Get current index from first letter.
			current_index = (int) content[previous] % ALPHABET;
			//Add first part string
			if(!l->hash[current_index]){
				l->hash[current_index] = malloc(sizeof(Phrase));
				l->hash[current_index]->next = NULL;
				l->hash[current_index]->string = NULL;
				l->hash[current_index]->var = malloc(sizeof(char) * (i - previous + 1));
				content[i] = '\0';
				strncpy(l->hash[current_index]->var, &content[previous], (i - previous + 1));//Plus 1 because \0 wanst been copied.
				current_phrase = l->hash[current_index];
			}
			else {
				current_phrase = l->hash[current_index];
				//Go to last item.
				while(current_phrase->next)
					current_phrase = current_phrase->next;
				
				current_phrase->next = malloc(sizeof(Phrase));
				current_phrase->next->var = malloc(sizeof(char) * (i - previous + 1));
				current_phrase->next->string = NULL;
				current_phrase->next->next = NULL;
				content[i] = '\0';
				strncpy(current_phrase->next->var, &content[previous], (i - previous + 1));//Plus 1 because \0 wanst been copied. 
				current_phrase = current_phrase->next;
			}
			previous = i + 1;
		}
		else if(content[i] == '\n' || content[i + 1] == '\0'){
			if(content[i + 1] == '\0'){
				i++;
			}
			if(!*name) {
				content[i] = '\0';
				*name = calloc(i + 1, sizeof(char));//Plus 1 to allow \0 to be added by strncpy.
				strncpy(*name, &content[previous], i);
			}
			else {
				//Add second part of string.
				content[i] = '\0';
				current_phrase->string = malloc(sizeof(char) * (i - previous + 1));
				strncpy(current_phrase->string, &content[previous], (i - previous + 1));
			}
			previous = i + 1;
		}
	}

	return l;
}

Languages *init_languages(char *folder){
	DIR *d;
	struct dirent *dir;
	int founded = 0;
	int temp = 0;
	int len = (int) strlen(folder) + 3;
	
	Languages *l = malloc(sizeof(Languages));
	char *temp_content = NULL;
	
	d = opendir(folder);
	if (!d) {
		return NULL;
	}
	l->files = NULL;
	
	while ((dir = readdir(d)) != NULL){
		if(strstr(dir->d_name,".conf")){
			temp = (int) strlen(dir->d_name);
			//Load file.
			l->files = realloc(l->files, sizeof(char *) * (founded + 1));
			l->files[founded] = malloc(sizeof (char) * (temp + len));
			
			
			strncpy(l->files[founded], folder, len - 2);
			strncat(l->files[founded], "/", 1);
			strncat(l->files[founded], dir->d_name, temp);
			founded++;
		}
	}
	closedir(d);
	
	l->loaded = founded;
	l->current = 0;
	l->names = NULL;
	if(founded > 0) {
		//Null alloc names.
		l->names = calloc(founded, sizeof(char *));
		
		//Load language hashs
		l->l = malloc(sizeof(Language *) * l->loaded);
		for(int i = 0; i < l->loaded; i++){
			//Load phrases on hash.
			temp_content = load_file(l->files[i]);
			
			l->l[i] = NULL;
			l->l[i] = load_language(temp_content, i, &l->names[i]);

			if(temp_content)
				free(temp_content);
			
		}
	}
	
	return l;
}

char *_(char *var){
	int index;

	if(!lang){
		return var;
	}
	if(lang->loaded == 0){
		return var;
	}
	
	if(config->language_default){
		return var;
	}
	
	Phrase *current_phrase = NULL;
	//Get current lang from config.
	
	index = (int) var[0] % ALPHABET;
	current_phrase = lang->l[config->language]->hash[index];
	while(current_phrase){
		if(strcmp(current_phrase->var, var) == 0){
			return current_phrase->string;
		}
	}
	return var;
}
