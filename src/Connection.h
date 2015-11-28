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
		#include <SDL2_net/SDL_net.h>
		#include <SDL2/SDL_thread.h>
	#endif
	
	//Constants
	///////////////////////////////////////////////////////////////////////
	#define DEFAULT_PORT 4242
	#define DEFAULT_PLAYERS_LIFE 10
	#define BUFFER_LIMIT 512
	#define SERVER_NAME 150
	#define MAX_SERVER 4//Change to get from config file.
	#define MAX_CLIENT 3//4 Players in total.
	#define SERVER_USER_RESPONSE_DELAY 300//Miliseconds.
	
	//Global variable
	///////////////////////////////////////////////////////////////////////
	static int terminate_thread;
	static SDL_SpinLock lock;

	//Structs
	///////////////////////////////////////////////////////////////////////
	
	typedef struct _host Host;
	typedef struct _server_status Server;
	typedef struct _game_communication game_comm;
	typedef struct _player Player;
	typedef struct _player_communication player_comm;
	typedef struct _send_minion send_minion;
	typedef struct _network NETWORK;
	
	struct _game_communication {
		int game_can_start;//Game can be started or not. Only if all players can start.
		int game_finished;//Game finished or not.
		int players_left;//Game finished or not.
		int connection_lost;//Connection to server lost or not.

		Server *server;
		player_comm **players;//[MAX_CLIENT];//Other players info.
		player_comm *current_player;
	};
	
	struct _server_status {
		int searching;
		int searching_finished;
		int connecting;
		int connection_failed;
		int connected;
		int choosing;
		int choosed;
		int search_result;
		int avaliable;
		Host **host;
	}; 
	
	struct _player_communication {
		int exited_game;
		int connection_lost;
		int ready_to_play;
		SDLNet_SocketSet activity;
		IPaddress ip;
		TCPsocket tcp_socket;
		//int lose_game //this is equal to info->life == 0
		Player *info;
	};
	
	struct _host {
		IPaddress ip;
		char name[SERVER_NAME];
	};
	
	struct _player {
		char *name;
		int life;
		int winner;
		int *minions_type_sent;
		//int left_game; Dont need, must update on game side.
	};
	
	struct _send_minion {
		int sent;
		int minion_type;
		int client_to_send_minion;
	};
	
	struct _network {
		int searching;
		int searched;
		int connecting;
		int connection_failed;
		int servers;
		char server_name[MAX_SERVER][SERVER_NAME];
		int choose_server;
		int server_choosed;
		
	};
	
	//Allocation Functions
	///////////////////////////////////////////////////////////////////////
	game_comm *init_communication(char *name);
	void remove_player(player_comm *player, int remove_name);
	void remove_communication(game_comm *comm);
	void remove_client(game_comm *game_communication, int client);
	
	//Update Functions
	///////////////////////////////////////////////////////////////////////
	int update_list_servers(UDPpacket* package);
	
	
	//Check Functions
	///////////////////////////////////////////////////////////////////////
	
	UDPsocket establish_udp(int port);
	int find_servers();
	int establish_server(IPaddress *ip);
	void check_messages_udp();
	void check_connection_tcp(game_comm *game_communication);
	void check_messages_tcp(game_comm *game_communication);
	void game_status_and_clients(game_comm *game_communication);
	
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
	
	void kill_thread(SDL_Thread **thread);
	
	int send_message(char *message, int message_type, TCPsocket socket);
	void terminate_server();
	void begin_game(game_comm * game_communication);
	void finish_game(game_comm *game_communication);
	void handle_message(int client_id, char* buffer, int type);
	
#endif