#ifndef __Tower_Defense__Estruturas__Miscelanium__
	#define __Tower_Defense__Estruturas__Miscelanium__

	//Includes
	///////////////////////////////////////////////////////////////////////
	#include <SDL2/SDL.h>
	#include <stdio.h>
	#include <stdbool.h>
	#include <stdlib.h>
	#include <math.h>
	#include <string.h>
	
	#ifdef _WIN32 //Load lib for windows or mac.
		#define windows 1
		#include <SDL2/SDL_image.h>
		#include <SDL2/SDL_ttf.h>
		#include <SDL2/SDL_ttf.h>
		#include <SDL2/SDL_net.h>
	#else
		#define windows 0
		#include <SDL2_image/SDL_image.h>
		#include <SDL2_ttf/SDL_ttf.h>
		#include <SDL2_ttf/SDL_ttf.h>
		#include <SDL_net.h>
	#endif
	
	#include "../Path.h"
	
	//Constants
	///////////////////////////////////////////////////////////////////////
	#define TOP_LAYER_SPACING 83
	#define block_width 64
	#define block_height 43
	#define map_array_width 17
	#define map_array_height 13

	//Structs
	///////////////////////////////////////////////////////////////////////

	typedef struct _configuration CONFIGURATION;
	typedef struct _node node;
	
	struct _configuration {
		bool audio_sfx;
		bool audio_music;
		char *language;
	};
	
	//Node
	struct _node {
		double xPos;
		double yPos;
		SDL_Surface *sprite;
	};
	
	//Allocation Functions
	///////////////////////////////////////////////////////////////////////
	
	node *init_node(char *sprite_filename, double xPos, double yPos);
	void free_node(node *node);
	
	//Load functions
	///////////////////////////////////////////////////////////////////////
	
	char* load_file(char const *file_name);
	
	//Get functions
	///////////////////////////////////////////////////////////////////////
	
	int get_next_line(char* string);
	
#endif