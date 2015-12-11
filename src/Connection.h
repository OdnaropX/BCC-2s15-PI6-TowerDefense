#ifndef __Tower_Defense__Connection__
	#define __Tower_Defense__Connection__
	
	//Includes
	///////////////////////////////////////////////////////////////////////
	#include <stdlib.h>
	#include <stdio.h>
	#include <strings.h>
	#include <time.h>
	#include <inttypes.h>
	#include <errno.h>

	#ifdef _WIN32 //Load lib for windows or mac.
		#include <SDL2/SDL_net.h>
		#include <SDL2/SDL_thread.h>
	#else
		#include <SDL2_net/SDL_net.h>
		#include <SDL2/SDL_thread.h>
	#endif
	
	//Constants
	///////////////////////////////////////////////////////////////////////
	#define DEFAULT_PORT_UDP 7777
	#define DEFAULT_PORT_TCP 7776
	#define DEFAULT_PLAYERS_LIFE 10
	#define CONNECTION_DELAY 50
	#define BUFFER_LIMIT 512
	#define SERVER_NAME 150
	#define MAX_SERVER 4//Change to get from config file.
	#define MAX_CLIENT 3//4 Players in total.
	#define SERVER_USER_RESPONSE_DELAY 30//Aprox. de 1000ms/60
	
	//Structs
	///////////////////////////////////////////////////////////////////////
	
	typedef struct _host Host;
	typedef struct _client Client;
	typedef struct _server_status Server;
	typedef struct _match Match;
	typedef struct _action Action;
	typedef struct _adversary Adversary;
	typedef struct _user User;
	typedef struct _game_communication Communication;
	typedef struct _send_minion SpawnMinion;
	typedef struct _thread Thread;
	typedef struct _threads Threads;
	typedef struct _locks Locks;
	typedef struct _share_data ShareData;
		
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
		
	struct _send_minion {
		int client_id;
		int amount;
		int *type;
	};
		
	struct _action {
		int message_status;
		int message_life;
		int message_minion;
	};
	
	struct _user {
		int id;
		int is_server;
		int life;
		int ready_to_play;
		int spawn_amount;
		char *name;
		Action process;
		SpawnMinion *minions;
	};
	
	struct _host {
		IPaddress ip;
		char name[SERVER_NAME];
		//char ip_name[16];
	};
	
	struct _client {
		int id;
		int has_name;
		int alive;
		char name[SERVER_NAME];
		TCPsocket tcp_socket;
	};
	
	struct _thread {
		int alive;
		int terminate;
		SDL_Thread *pointer;
		SDL_ThreadPriority priority; //SDL_THREAD_PRIORITY_LOW //SDL_THREAD_PRIORITY_NORMAL//SDL_THREAD_PRIORITY_HIGH
	};
	
	struct _locks {
		SDL_SpinLock user;
		SDL_SpinLock comm;
		SDL_SpinLock control;
	};
	
		
	struct _share_data {
		Communication *current_comm;
		User *current_user;
	};
	
	struct _threads {
		Thread udp; 
		Thread client; 
		Thread server; 
		Locks lock;
		//ShareData data;
	};
	

	
	//Allocation Functions
	///////////////////////////////////////////////////////////////////////
	Communication *init_communication();
	void remove_communication();
	void remove_client(int client, int send);
	
	//Update Functions
	///////////////////////////////////////////////////////////////////////
	int update_list_servers(UDPpacket* package);
	void update_server_minion(int user_from, char *pointer);
	
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
	void close_clients();
	void close_socket(TCPsocket tcp_socket);
	void close_set(SDLNet_SocketSet activity);
	int connect_to_server(int server_choice);
	
	//Get Functions
	///////////////////////////////////////////////////////////////////////
	char *get_connected_server_name();
	char *get_host_name(int i);
	Host *get_host();
	TCPsocket get_socket_from_user_id(int user_id, int *index);
	
	//Messages Functions
	///////////////////////////////////////////////////////////////////////
	
	int send_message(char *message, int message_type, TCPsocket socket, int incomplete_message);
	void handle_message(char *buffer, int handle_internal);
	int has_message_tcp(char *buffer, TCPsocket tcp_socket);
	int handle_message_pool(TCPsocket tcp_socket);
	
	//Server runner
	///////////////////////////////////////////////////////////////////////
	void thread_check_messages_udp(void *data);
	
	void run_server(void *data);

	void run_client(void *data);
	
	void kill_thread(SDL_Thread **thread);
	int str_to_uint16(const char *str, uint16_t *res);
#endif