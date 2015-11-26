#ifndef __Tower_Defense__Connection__
	#define __Tower_Defense__Connection__
	
	//Includes
	///////////////////////////////////////////////////////////////////////
	#include <stdlib.h>
	#include <stdio.h>
	#include <strings.h>
	
	#ifdef _WIN32 //Load lib for windows or mac.
		#include <SDL2/SDL_net.h>
		#include <SDL2/SDL_thread.h>
	#else
		#include <SDL2/SDL_net.h>
		#include <SDL2/SDL_thread.h>
	#endif
	
	//Constants
	///////////////////////////////////////////////////////////////////////
	#define DEFAULT_PORT 4242
	#define BUFFER_LIMIT 512
	#define SERVER_NAME 150
	#define MAX_SERVER 4//Change to get from config file.
	#define MAX_CLIENT 3//4 Players in total.
	
	//Global variable
	///////////////////////////////////////////////////////////////////////
	int terminate_thread;
	
	//Structs
	///////////////////////////////////////////////////////////////////////
	
	typedef struct _server server_register;
	typedef struct _game_communication game_comm;
	typedef struct _player Player;
	typedef struct _player_communication player_comm;
	typedef struct _send_minion send_minion;
	
	struct _game_communication {
		int game_can_start;//Game can be started or not. Only if all players can start.
		int game_finished;//Game finished or not.
		int connection_lost;//Connection to server lost or not.
		int server_found;//Server found or not found.
		int server_choose;//Server need to be choosed.
		int server_connecting;//Server connected.
		int server_connected;//Server connected.
		player_comm **other_players;//Other players info.
		player_comm *current_player;
	};
	
	struct _player_communication {
		int exited_game;
		int connection_lost;
		//int lose_game //this is equal to info->life == 0
		Player *info;
	};
	
	struct _server {
		IPaddress ip;
		char name[SERVER_NAME];
	};
	
	struct _player {
		char *name;
		int life;
		int winner;
		int *minions_type_sent;
	};
	
	struct _send_minion {
		int sent;
		int minion_type;
		int client_to_send_minion;
	};
	
	//Allocation Functions
	///////////////////////////////////////////////////////////////////////
	game_comm *init_communication();
	void remove_communication(game_comm *comm);
	
	//Update Functions
	///////////////////////////////////////////////////////////////////////
	int update_list_servers(UDPpacket* package);
	
	
	//Check Functions
	///////////////////////////////////////////////////////////////////////
	UDPsocket check_network_avaliable();
	int find_servers();
	
	//Connection Functions
	///////////////////////////////////////////////////////////////////////
	void close_connection();
	int connect_to_server(int server_choice);
	
	//Get Functions
	///////////////////////////////////////////////////////////////////////
	char *get_connected_server_name();
	
	//Server runner
	///////////////////////////////////////////////////////////////////////
	
	void run_server(void *data);

	void run_client(void *data);
	
#endif