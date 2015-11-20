#ifndef __Tower_Defense__Connection__
	#define __Tower_Defense__Connection__
	
	#include <stdlib.h>
	
	#ifdef _WIN32 //Load lib for windows or mac.
		#include <SDL2/SDL_net.h>
		#include <SDL2/SDL_thread.h>
	#else
		#include <SDL2/SDL_net.h>
		#include <SDL2/SDL_thread.h>
	#endif
	
	int terminate_thread;
	
	
	void run_server(void *data);

	void run_client(void *data);
	
#endif