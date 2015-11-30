#ifndef __Tower_Defense__Connection__
	#define __Tower_Defense__Connection__
	
	//Includes
	///////////////////////////////////////////////////////////////////////
	#include <stdlib.h>
	#include <stdio.h>
	#include <strings.h>
	#include <time.h>
	
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
	
	//Structs
	///////////////////////////////////////////////////////////////////////
	
	typedef struct _host Host;
	typedef struct _client Client;
	typedef struct _server_status Server;
	typedef struct _match Match;
	typedef struct _adversary Adversary;
	typedef struct _user User;
	typedef struct _network Network;
	typedef struct _game_communication Communication;
	typedef struct _send_minion send_minion;
		
	struct _game_communication {
		int connection_lost;//Connection to server lost or not.
		Adversary *adversary;
		Match *match;
		Server *server;
	};
	
	struct _adversary {
		int id;
		int playing;
		int ready_to_play;
		int life;
		char *name;
		int pending_minions;
		int *minions_sent;
	};
	
	struct _match {
		int can_start;
		int finished;
		int winner_id;
		int lost;
		int error;
		int players;
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
		Host *host;
	};
		
	struct _user {
		int id;
		int is_server;
		int life;
		int ready_to_play;
		char *name;
	};
	
	struct _host {
		IPaddress ip;
		char name[SERVER_NAME];
	};
	
	struct _client {
		int id;
		int has_name;
		int alive;
		char name[SERVER_NAME];
		TCPsocket tcp_socket;
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
	Communication *init_communication();
	void remove_communication();
	void remove_client(int client);
	
	//Update Functions
	///////////////////////////////////////////////////////////////////////
	int update_list_servers(UDPpacket* package);
	
	
	//Check Functions
	///////////////////////////////////////////////////////////////////////
	
	UDPsocket establish_udp(int port);
	int find_servers();
	int establish_server(IPaddress *ip);
	void check_messages_udp();
	void check_connection_tcp();
	void check_messages_tcp();
	void game_status();
	
	//Connection Functions
	///////////////////////////////////////////////////////////////////////
	void close_connection();
	void close_socket(TCPsocket tcp_socket);
	void close_set(SDLNet_SocketSet activity);
	int connect_to_server(int server_choice);
	
	//Get Functions
	///////////////////////////////////////////////////////////////////////
	char *get_connected_server_name();
	char *get_host_name(int i);
	Host *get_host();
	TCPsocket get_socket_from_user_id(int user_id);
	
	//Messages Functions
	///////////////////////////////////////////////////////////////////////
	
	int send_message(char *message, int message_type, TCPsocket socket);
	void handle_message(char *buffer);
	int has_message_tcp(char *buffer, TCPsocket tcp_socket);
	int handle_message_pool(TCPsocket tcp_socket);
	
	//Server runner
	///////////////////////////////////////////////////////////////////////
	void run_server(void *data);

	void run_client(void *data);
	
	void kill_thread(SDL_Thread **thread);
	
#endif