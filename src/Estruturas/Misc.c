#include "Misc.h"
	
//Allocation Functions
///////////////////////////////////////////////////////////////////////

//Node
/**
	Function used to allocate struct node.
*/
node *init_node(char *sprite_filename, double xPos, double yPos){
    node *new_node = malloc(sizeof(node));
    
    new_node->sprite = IMG_Load(sprite_filename);
    if(!new_node->sprite) {
        printf("Erro ao carregar sprite! %s\n", IMG_GetError());
        free(new_node);
        return NULL;
    }
    
    new_node->xPos = xPos;
    new_node->yPos = yPos;
    
    return new_node;
}

/**
	Function used to free the struct node.
*/
void free_node(node *node){
    if(node){
        if(node->sprite)
            SDL_FreeSurface(node->sprite);
        
        node->sprite = NULL;
        
        free(node);
        node = NULL;
    }
}

//Load functions
///////////////////////////////////////////////////////////////////////
	
char* load_file(char const *file_name){
	SDL_RWops *rw = SDL_RWFromFile(file_name, "rb");
    if (rw == NULL) return NULL;

    Sint64 res_size = SDL_RWsize(rw);
    char* res = (char*) malloc(res_size + 1);

    Sint64 nb_read_total = 0, nb_read = 1;
    char* buf = res;
    while (nb_read_total < res_size && nb_read != 0) {
		nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
        nb_read_total += nb_read;
        buf += nb_read;
	}
    SDL_RWclose(rw);
    if (nb_read_total != res_size) {
		free(res);
		return NULL;
    }

    res[nb_read_total] = '\0';
	
    return res;
}

//Get functions
///////////////////////////////////////////////////////////////////////

int get_next_line(char* string) {
	char line[255];
	sscanf(string, "%[^\n]\n", line);
	return (int)strlen (line);
}