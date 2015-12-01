#include "Connection.h"

Host servers[MAX_SERVER];
Client clients[MAX_CLIENT];

UDPsocket server_udp_socket;       /* Socket descriptor */
UDPpacket server_udp_package;
TCPsocket server_tcp_socket = NULL;//Used for server side and client side.
TCPsocket server_ip = NULL;
SDLNet_SocketSet activity = NULL;//Used for TPC and UDP.

Host *main_server = NULL;

int connected_server = -1;
int connected_clients = 0;
int game_in_progress = 0;
int game_ended = 0;

int current_index_id = 0;

extern User *current_user;
extern Communication *comm;
extern int terminate_thread;
extern SDL_SpinLock lock;
extern SDL_SpinLock user_lock;

//Allocation Functions
///////////////////////////////////////////////////////////////////////

Communication *init_communication() {
	Communication *game = malloc(sizeof(Communication));
	
	//Connection information
	game->connection_lost = 0;
	
	//Match information
	game->match = calloc(1, sizeof(Match));
	
	//Adversary information - Must be allocated when there is some adversary 
	game->adversary = NULL;
	
	//Server information
	game->server = calloc(1, sizeof(Server));
	game->server->choosed = -1;
	game->server->host = NULL;//Host must be allocated later.

	//Reset user id
	current_user->id = 0;
	
	return game;
}

void remove_communication(){
	printf("Removing communication\n");
	
	if(comm){
		if(comm->match){
			free(comm->match);
			comm->match = NULL;
		}
		
		if(comm->adversary){
			if(comm->adversary->minions_sent){
				free(comm->adversary->minions_sent);
				comm->adversary->minions_sent = NULL;
			}
			free(comm->adversary);
			comm->adversary = NULL;
		}
		
		if(comm->server){
			if(comm->server->host){
				free(comm->server->host);
				comm->server->host = NULL;
			}
			free(comm->server);
			comm->server = NULL;
		}
		
		free(comm);
		comm = NULL;
	}
	return;
}

void remove_client(int client){
	int i, temp, connected;
	char buffer[3];
	
	//Send message to remove user to clients	
	temp = 0;
	connected = connected_clients;
	for(i=0; i < MAX_CLIENT; i++) {
		if(temp == connected){
			break;
		}
		if(clients[i].tcp_socket){
			if(i != client) {
				sprintf(buffer, "%c", (char) clients[i].id);
				//Send message to other players that user left.
				send_message(buffer, 1, clients[i].tcp_socket, 1);
			}
			temp++;
		}
    }

    SDLNet_TCP_DelSocket(activity, clients[client].tcp_socket);

    if(clients[client].tcp_socket){
        SDLNet_TCP_Close(clients[client].tcp_socket);
		clients[client].tcp_socket = NULL;
	}
	
	clients[client].id = 0;
	clients[client].alive = 0;
	clients[client].has_name = 0;
	
	connected_clients = connected_clients - 1;
	
	//Change player status to left.
	if(game_in_progress && !game_ended){
		temp = comm->match->players;
		for (i = 0; i < temp; i++){
			if(comm->adversary->id == clients[client].id){
				comm->adversary->playing = 0;
				break;
			}
			comm->adversary++;
		}
		//Reset location.
		comm->adversary-=i;
	}
	else {
		//If game not began remove from list.
		//Realloc
		if(comm->match->players > 0){
			temp = comm->match->players;
			Adversary *adversary = malloc(sizeof (Adversary) * (temp - 1));
			for (i = 0; i< temp; i++){
				if(comm->adversary[i].id != clients[client].id) {
					adversary->id = comm->adversary[i].id;
					adversary->playing = comm->adversary[i].playing;
					adversary->ready_to_play = comm->adversary[i].ready_to_play;
					adversary->name = comm->adversary[i].name;
					adversary->life = comm->adversary[i].life;
					adversary->pending_minions = comm->adversary[i].pending_minions;
					adversary->minions_sent = comm->adversary[i].minions_sent;
				}
				else {
<<<<<<< HEAD
					if(adversary->minions_sent){
						free(adversary->minions_sent);
						adversary->minions_sent = NULL;
					}
					if(adversary->name){
						//Free name
						free(adversary->name);
						adversary->name = NULL;
					}
=======
					//Free name
					free(adversary->name);
>>>>>>> 4b00cc2f950d883e8c70e6b6c33e5fc30223dcbf
				}
				adversary++;
			}
			//Reset location.
			adversary -= i;
			if(comm->adversary){
				free(comm->adversary);
			}
			comm->adversary = adversary;
			comm->match->players = temp - 1;
		}
	}
}

//Update Functions
///////////////////////////////////////////////////////////////////////
	
int update_list_servers(UDPpacket* package){
	int already_add = 0;
	char *name = NULL;
	char *prt = NULL;
	char port[SERVER_NAME];
	int i, use_default = 0;
	uint16_t port_number = 0;
	
	printf("Updating servers\n");
	
	//Check if already on list.
	for(i=0; i < MAX_SERVER && servers[i].ip.host != 0;i++) {
		//printf("Update %d\n", servers[i].ip.host);
		if(package->address.host == servers[i].ip.host){
			//printf("Here already\n");
			return 1;
		}
	}
	
	if(i < MAX_SERVER) {
		//printf("PORT: |%d|\n",package->address.port);
		name = strchr((char*)package->data, '\t');//Data is Uint8, so need cast.
		//printf("Message: |%s|\n", name);
		if(++name) {
			 strncpy(servers[i].name, name, SERVER_NAME);
			 //printf("Server name: |%s|\n", servers[i].name);
			 
			 name = strchr(servers[i].name, '\t');
			 
			 if(++name) {
				 strncpy(port, name, SERVER_NAME);
			 }
			 else {
				 use_default = 1;
			 }
			 
			 name = strchr(servers[i].name, '\t');
			 if(name) {
				  *name = '\0';
			 }
			 name = strchr(servers[i].name, '\n');
			 if(name) {
				  *name = '\0';
			 }
			 
			 prt = strchr(port, '\n');
			 if(prt) {
				  *prt = '\0';
			 }
			 
			 use_default = !str_to_uint16(port, &port_number);
		}
		else {
			return 0;
		}
		
		if(use_default){
			port_number = (uint16_t) DEFAULT_PORT_TCP;
		}
		servers[i].ip.host = package->address.host;
		servers[i].ip.port = port_number;
		printf("Server name: |%s|\n", servers[i].name);
		printf("Server port: |%d|\n", servers[i].ip.port);
		//Add this to next list number.
		i++;
		return 1;
	}	
	return 0;
}

//Check Functions
///////////////////////////////////////////////////////////////////////

UDPsocket establish_udp(int port){
	UDPsocket udp_socket = NULL;
	udp_socket = SDLNet_UDP_Open(port);
    if(!udp_socket)
    {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return NULL;
    }
	return udp_socket;
}

int find_servers() {
	UDPsocket udp_socket = NULL; 
	IPaddress ip;
	UDPpacket* output_package;
    UDPpacket* output_package_local;
    UDPpacket* input_package;
   
	int trying = 1;
	int number_found = 0;
	int attempts = 0;
	int sent = 0;
    int i = 0;
	int t1 = 0, t2, ignore_local = 0;
	
	//Port 0 because cannot open UDP socket to server if server is the same machine, so 0 is auto assigned.
	udp_socket = establish_udp(0);
    if(!udp_socket){
		printf("Cannot establish UDP socket.");
		return 0;
	}
	
	//Alloc memory.
	output_package = SDLNet_AllocPacket(BUFFER_LIMIT);
    output_package_local = SDLNet_AllocPacket(BUFFER_LIMIT);
    input_package = SDLNet_AllocPacket(BUFFER_LIMIT);
	
	//Mount package. 255.255.255.255 will broadcast to the entire LAN.
	//IP address is detect by the resolver from SDL.
    if(SDLNet_ResolveHost(&ip, "255.255.255.255", DEFAULT_PORT_UDP) != 0) {
		SDLNet_FreePacket(output_package); 
		SDLNet_FreePacket(output_package_local); 
		SDLNet_FreePacket(input_package); 
		SDLNet_UDP_Close(udp_socket); 
		printf("Could not resolve server name\n");
		return 0;
	}
	
    output_package->address.host = ip.host;
    output_package->address.port = ip.port;
    sprintf((char*) output_package->data, "GRADE_DEFENDER_CLIENT");
    output_package->len = strlen("GRADE_DEFENDER_CLIENT") + 1;

	//Package for localhost. This will be used on internal tests.
    if(SDLNet_ResolveHost(&ip, "localhost", DEFAULT_PORT_UDP) != 0) {
		ignore_local = 1;
	}
	
    output_package_local->address.host = ip.host;
    output_package_local->address.port = ip.port;
    sprintf((char*) output_package_local->data, "GRADE_DEFENDER_CLIENT");
    output_package_local->len = strlen("GRADE_DEFENDER_CLIENT") + 1;
	
	//Reset ip host
	for(i = 0; i < MAX_SERVER; i++) {
        servers[i].ip.host = 0;
	}
	
	while(trying){
		printf("Trying send package: %s\n", (char *) output_package->data);//Cast because data is save as Uint8 *
		
		//sent = SDLNet_UDP_Send(udp_socket, output_package->channel, output_package);
		sent = SDLNet_UDP_Send(udp_socket, -1, output_package);
		if (!sent) {
			printf("Broadcast failed!\nTrying Localhost...\n");
			
			if(ignore_local) {
				printf("Something really, really, I mean really wrong happened and Localhost was not accessed.\nThis was not suppost to happen.\n");
				SDLNet_FreePacket(output_package); 
				SDLNet_FreePacket(output_package_local); 
				SDLNet_FreePacket(input_package); 
				SDLNet_UDP_Close(udp_socket); 
				return 0;
			}
			//sent = SDLNet_UDP_Send(udp_socket, output_package_local->channel, output_package_local);
			sent = SDLNet_UDP_Send(udp_socket, -1, output_package_local);
			if (!sent) {
				return 0;
			}	
		}
		//Delay to give server chance to respond.
		SDL_Delay(CONNECTION_DELAY);
		
		while(SDLNet_UDP_Recv(udp_socket, input_package)){
			printf("Receiving Message\n");
            if(strncmp((char*)input_package->data, "GRADE_DEFENDER_SERVER", strlen("GRADE_DEFENDER_SERVER")) == 0){
                trying = 0;
                //add to list, checking for duplicates
                update_list_servers(input_package);
            }
        }

		for(number_found = 0; number_found < MAX_SERVER && servers[number_found].ip.host != 0;number_found++);
		printf("Found: %d\n", number_found);
		
		if (number_found != MAX_SERVER) {
			//Wait at least 100 ms.
			t2 = SDL_GetTicks();
			if (t2 < (t1 + 100)){
				t2 = (t1 + 100) - t2;
				if(t2 < 0) {
					t2 = 0;
				}
				if(t2 > 100) {
					t2 = 100;
				}
				SDL_Delay(t2);
			}
			t1 = SDL_GetTicks();
		
			//Will try more 4 times to see if found more servers.
			if(attempts < 5){
				trying = 1;
			}
			else if(attempts > 50) {
				trying = 0;
			}
		}
		else {
			trying = 0;
		}
		attempts++;
	}
	printf("Search server done!\n");

    SDLNet_FreePacket(output_package); 
    SDLNet_FreePacket(output_package_local); 
    SDLNet_FreePacket(input_package); 
    SDLNet_UDP_Close(udp_socket); 
    return number_found;
}

int establish_server(IPaddress *ip){
	time_t t;
<<<<<<< HEAD
	
=======
>>>>>>> 4b00cc2f950d883e8c70e6b6c33e5fc30223dcbf
	/* First create TCP socket that will be used to connect	*/
	
	//Null is to listen
<<<<<<< HEAD
	if(SDLNet_ResolveHost(ip, NULL, DEFAULT_PORT_TCP) < 0){
=======
	if(SDLNet_ResolveHost (ip, NULL, DEFAULT_PORT) < 0){
>>>>>>> 4b00cc2f950d883e8c70e6b6c33e5fc30223dcbf
		printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		return 0;
	}
	
	//Create TCP socket.
	server_tcp_socket = SDLNet_TCP_Open(ip);
	if (!server_tcp_socket){
        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return 0;
    }
	
	activity = SDLNet_AllocSocketSet(MAX_CLIENT);
	if(!activity){
		printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		close_socket(server_tcp_socket);
        return 0;
	}
	
	//Start UDP socket to listen for clients.
	server_udp_socket = establish_udp(DEFAULT_PORT_UDP);
	if (!server_udp_socket){
		close_socket(server_tcp_socket);
		close_set(activity);
		return 0;
	}
	
	srand((unsigned) time(&t));
	current_user->id = rand() % 50;
	if(current_user->id == 0) {
		current_user->id = 1;
	}
	current_index_id = current_user->id + 1;
	
	return 1;
}

void check_connection_tcp(){
	TCPsocket socket = NULL; 
	
	int sockets = 0;
	char buffer[BUFFER_LIMIT];
	int i, j, user_id, temp, connected;
	
	//Check pending connection. This is non-blocking
	socket = SDLNet_TCP_Accept(server_tcp_socket);
	if(!socket){
		return;
	}
	
	if(connected_clients == MAX_CLIENT) {
		//Inform client that connection cannot be made.
		send_message(NULL, 6, socket, 1);
		return;
	}
	//Check if there is slot avaliable
	for(i = 0; i < MAX_CLIENT && clients[i].tcp_socket;i++);
	//Repeat in case some went wrong in if above.
	if (i == MAX_CLIENT) {
		//Inform client that connection cannot be made.
		send_message(NULL, 6, socket, 1);
		return;
	}

	//Check if game is in progress
	if(game_in_progress) {
		//Informe client that game in progress
		send_message(NULL, 7, socket, 1);
		return;
	}

	//Add client socket to set 
	sockets = SDLNet_TCP_AddSocket(activity, socket);
	
	if(sockets == -1) {
		fprintf(stderr, "SDLNet_AddSocket: %s\n", SDLNet_GetError());
		comm->server->connecting = 0;
		comm->server->connection_failed = 1;
		return;
	}

	//Create id
	user_id = (current_index_id % 255);
	if(user_id == 0){
		user_id = 1;
	} 
	current_index_id = user_id + 1;
	
	//Send id
	sprintf(buffer, "%c\t%c", (char) user_id, (char) current_user->id);
	if(send_message(buffer, 10, socket, 1)){
		temp = 0;
		//Send other users to this user
		connected = connected_clients;
		for(j = 0; j < MAX_CLIENT;j++){
			if(temp == connected) {
				break;
			}
			if(clients[j].tcp_socket){
				if(j != i && clients[j].has_name) {
					sprintf(buffer, "%c\t%s", (char) clients[j].id, clients[j].name);
					if(!send_message(buffer,0, socket, 1)){
						//Connection failed.
						return;
					}
				}
				temp++;
			}
		}
		
		//Reset j.
		j = 0;
		
		//Add to adversary
		Adversary *adversary = malloc(sizeof (Adversary) * sockets);
		if(comm->adversary) {
			temp = comm->match->players;
			for (j = 0; j < temp; j++){
				adversary[j].id = comm->adversary[j].id;
				adversary[j].playing = comm->adversary[j].playing;
				adversary[j].name = comm->adversary[j].name;
				adversary[j].life = comm->adversary[j].life;
				adversary[j].ready_to_play = comm->adversary[j].ready_to_play;
				adversary[j].pending_minions = comm->adversary[j].pending_minions;
				adversary[j].minions_sent = comm->adversary[j].minions_sent;	
			}
			free(comm->adversary);
		}
	
		adversary[j].id = user_id;
		adversary[j].playing = 1;
		adversary[j].pending_minions = 0;
		adversary[j].life = DEFAULT_PLAYERS_LIFE;
		adversary[j].ready_to_play = 0;
		adversary[j].minions_sent = NULL;

		comm->adversary = adversary;
		comm->match->players = comm->match->players + 1;
		
		//Add to client list
		clients[i].tcp_socket = socket;
		clients[i].id = user_id;
		clients[i].alive = 1;
		connected_clients = sockets;
	}
	else {
		//Close connection
		SDLNet_TCP_DelSocket(activity, socket);
		close_socket(socket);
	}
	return;
}

void check_messages_tcp(){
	int i, temp;
	
	if(current_user->is_server){
		temp = 0;
		for(i = 0; i < MAX_CLIENT; i++){
			if(temp == connected_clients || connected_clients == 0){
				break;
			}
			if(clients[i].tcp_socket) {
				if(handle_message_pool(clients[i].tcp_socket) == 0) {
					//Remove client
					remove_client(i);
				}
				temp++;
			}
		}
	}
	else {
		if(handle_message_pool(server_tcp_socket) == 0){
			//Close socket
			close_socket(server_tcp_socket);
			//Close set
			close_set(activity);
			//Terminate thread
			comm->server->connection_failed = 1;
		}
	}
	return;
}

void check_messages_udp(){
	int received = 0;
	char buffer[BUFFER_LIMIT];
	
	UDPpacket* input = NULL;
	
	if(!server_udp_socket){
		printf("Error: Socket server_udp_socket is NULL.\n");
		return;
	}	
	
	input = SDLNet_AllocPacket(BUFFER_LIMIT);
	received = SDLNet_UDP_Recv(server_udp_socket, input);//Message will not be greater than buffer. 
	
	if(received == 1){
		if(strncmp((char*)input->data, "GRADE_DEFENDER_CLIENT", strlen("GRADE_DEFENDER_CLIENT")) == 0){
			//Send message to client.
			UDPpacket* output;
			output = SDLNet_AllocPacket(BUFFER_LIMIT);
			snprintf(buffer, BUFFER_LIMIT, "%s\t%s\t%d", "GRADE_DEFENDER_SERVER", current_user->name, DEFAULT_PORT_TCP);
			snprintf((char *)output->data, BUFFER_LIMIT, "%s", buffer);
			output->len = (int)strlen(buffer) + 1;
			output->address.host = input->address.host;
            output->address.port = input->address.port;
<<<<<<< HEAD
			sent = SDLNet_UDP_Send(server_udp_socket, -1, output);
			if (sent){
				printf("Package from server was sent\n");
			}
=======
			/*sent =*/ SDLNet_UDP_Send(server_udp_socket, -1, output);
>>>>>>> 4b00cc2f950d883e8c70e6b6c33e5fc30223dcbf
            SDLNet_FreePacket(output);
		}
	}
	SDLNet_FreePacket(input);
	return;
}

void game_status(){
	int i, j, temp, alive = 0, winner_id = 0;
	char buffer[BUFFER_LIMIT];
	
	if(current_user->is_server) {
		//Check if there is player connected. If not end game.
		
		if(game_in_progress){
			if(connected_clients <= 0){
				//Everyone left, and game with current_user as winner.
				comm->match->finished = 1;
				comm->match->lost = 0;
				comm->match->winner_id = current_user->id;
			}
			else {
				//Check if game was finished.
				if(current_user->life > 0){
					alive++;
					winner_id = current_user->id;
				}
				temp = 0;
				for(i = 0; i < MAX_CLIENT;i++){
					if(temp == connected_clients){
						break;
					}
					if(clients[i].tcp_socket && clients[i].has_name && clients[i].alive){
						//Send message
						alive++;
						winner_id = clients[i].id;
					}
				}
				if(alive == 1){
					game_ended = 1;
					//Send message of end game to users.
					temp = 0;
					j = connected_clients;
					for(i = 0; i < MAX_CLIENT;i++){
						if(temp == j){
							break;
						}
						if(clients[i].tcp_socket && clients[i].has_name && clients[i].alive){
							//Send message
							sprintf(buffer, "%c", (char) winner_id);
							//Send message to other players that user left.
							send_message(buffer, 4, clients[i].tcp_socket, 1);
							remove_client(i);
							temp++;
						}
					}
					
					//Set winner to server.
					comm->match->finished = 1;
					comm->match->winner_id = winner_id;
					if(winner_id != current_user->id){
						comm->match->lost = 1;
					}
					//Close connection.//Will be closed when thread is killed.
				}
			}
		}
		else {
			if(comm->match->players > 0){
				//Check if game can begin //Check if all users are ready
				temp = 0;
				for(i = 0; i < comm->match->players;i++){
					if(comm->adversary->ready_to_play){
						temp++;
					}
					comm->adversary++;
				}
				
				if(temp == comm->match->players){
					//This line can add a injust gameplay to server, because it will begin before others. 
					comm->match->can_start = 1;
					//Send message to users begin game.
					temp = 0;
					for(i = 0; i < MAX_CLIENT;i++){
						if(temp == connected_clients){
							break;
						}
						if(clients[i].tcp_socket && clients[i].has_name){
							//Send message
							if(send_message(NULL, 3, clients[i].tcp_socket, 1)){
								temp++;
							}
							else {
								remove_client(i);
							}
						}
					}
					game_in_progress = 1;
				}
			}
		}
	}
	else {
		//Check server connection.//This is detected by sending an receiving messages.
		if(comm->server->connection_failed){
			return;
		}
		
		if(game_in_progress && !comm->match->lost){
			//Check if game was finish.
			//Check if player lost. If lost, wait until the game is finished.
			if(current_user->life <= 0) {
				comm->match->lost = 1;
			}
		}
	}
	return;
}

//Connection Functions
///////////////////////////////////////////////////////////////////////

void close_connection(){
	if (server_tcp_socket) {
		close_socket(server_tcp_socket);
	}
	if (activity) {
		close_set(activity);
	}
	if(server_udp_socket){
		SDLNet_UDP_Close(server_udp_socket);
	}
	return;
}

void close_socket(TCPsocket tcp_socket){
	if (tcp_socket) {
		SDLNet_TCP_Close(tcp_socket);
        tcp_socket = NULL;
	}
	return;
}

void close_set(SDLNet_SocketSet activity) {
	if (activity) {
		SDLNet_FreeSocketSet(activity);
        activity = NULL;
	}
	return;
}

void close_clients(){
	int temp = 0, connected = connected_clients;
	
	for(int i = 0 ; i < MAX_CLIENT; i++) {
		if(temp == connected){
			break;
		}
		if(clients[i].tcp_socket){
			close_socket(clients[i].tcp_socket);
			temp++;
		}
	}
	return;
}

int connect_to_server(int server_choice){
	if(server_choice < 0 || server_choice > MAX_SERVER) {
		return 0;
	}
	
	server_tcp_socket = SDLNet_TCP_Open(&servers[server_choice].ip);
	
	if (!server_tcp_socket){
        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return 0;
    }

    activity = SDLNet_AllocSocketSet(1);
    if(!activity){
        printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        return 0;
    }

	//Add a socket to a socket set that will be watched
    if(SDLNet_TCP_AddSocket(activity, server_tcp_socket) == -1){
        printf("SDLNet_AddSocket: %s\n", SDLNet_GetError());
        // perhaps you need to restart the set and make it bigger...
    }

    // Success - record the index for future reference:
    connected_server = server_choice;
    return 1;
}

//Get Functions
///////////////////////////////////////////////////////////////////////
	
char *get_connected_server_name(){
	return servers[connected_server].name;
}

char *get_host_name(int i){
	return servers[i].name;
}

Host *get_host(){
	return &servers;
}

TCPsocket get_socket_from_user_id(int user_id, int *index){
	int i, temp = 0;
	
	for(i = 0; i< MAX_CLIENT; i++){
		if(temp == connected_clients){
			break;
		}
		if(clients[i].tcp_socket) {
			if(clients[i].id == user_id){
				*index = i;
				return clients[i].tcp_socket;
			}
			temp++;
		}
	}
	return NULL;
}

//Messages Functions
///////////////////////////////////////////////////////////////////////

int send_message(char *message, int message_type, TCPsocket socket, int incomplete_message){
	char msg[BUFFER_LIMIT];
	int next = 1;
	int sent;
	
	if(incomplete_message){
		switch(message_type){
			case 0://ADD_USER
				sprintf(msg, "ADD_USER\t%s", message);
				break;
			case 1://REMOVE_USER
				sprintf(msg, "REMOVE_USER\t%s", message);
				break;
			case 2://ADD_MINION
				sprintf(msg, "ADD_MINION\t%s", message);
				break;
			case 3://BEGIN_GAME
				sprintf(msg, "BEGIN_GAME");
				break;
			case 4://END_GAME
				sprintf(msg, "END_GAME\t%s", message);
				break;
			case 5://USER_READY
				sprintf(msg, "USER_READY\t%s", message);
				break;
			case 6://SERVER_FULL
				sprintf(msg, "SERVER_FULL");
				break;
			case 7://GAME_ALREADY_STARTED
				sprintf(msg, "GAME_ALREADY_STARTED");
				break;
			case 8://USER_MINION
				sprintf(msg, "USER_MINION\t%s", message);
				break;
			case 9://USER_STATUS
				sprintf(msg, "USER_STATUS\t%s", message);
				break;
			case 10://USER_ID
				sprintf(msg, "USER_ID\t%s", message);
				break;
			case 11://USER_LIFE
				sprintf(msg, "USER_LIFE\t%s", message);
				break;
			case 12://USER_STATUS_LIFE
				sprintf(msg, "USER_STATUS_LIFE\t%s", message);
				break;
			case 13://USER_NAME
				sprintf(msg, "USER_NAME\t%s", message);
				break;
			default:
				next = 0;
		}
	}

	if(next){
		if(incomplete_message)
			sent = SDLNet_TCP_Send(socket,msg,BUFFER_LIMIT);
		else
			sent = SDLNet_TCP_Send(socket,message,BUFFER_LIMIT);
		if(sent != BUFFER_LIMIT) {
			printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			// It may be good to disconnect sock because it is likely invalid now.
			return 0;
		}
	}
	return 1;
}

void handle_message(char *buffer, int handle_internal){
	char *pointer = NULL;
<<<<<<< HEAD
	int i, user_id, temp, life, connected;
	i = 0;
=======
	int i, user_id, temp, life;
>>>>>>> 4b00cc2f950d883e8c70e6b6c33e5fc30223dcbf
	//Client side
	//Check if game can begin
	if(strncmp(buffer, "BEGIN_GAME", strlen("BEGIN_GAME")) == 0) {
		comm->match->can_start = 1;
		game_in_progress = 1;
	}
	//Check user add
	else if(strncmp(buffer, "USER_ID", strlen("USER_ID")) == 0){
		pointer = strchr(buffer, '\t');
		pointer++;
		current_user->id = (int) *pointer;
		//Add server as adversary
		temp = comm->match->players;
		Adversary *adversary = malloc(sizeof (Adversary) * (temp + 1));
		if(comm->adversary) {
			for (i = 0; i < temp; i++){
				adversary->id = comm->adversary[i].id;
				adversary->playing = comm->adversary[i].playing;
				adversary->name = comm->adversary[i].name;
				adversary->life = comm->adversary[i].life;
				adversary->ready_to_play = comm->adversary[i].ready_to_play;
				adversary->pending_minions = comm->adversary[i].pending_minions;
				adversary->minions_sent = comm->adversary[i].minions_sent;
				adversary++;	
			}
			free(comm->adversary);
		}
		pointer+=2;
		
		adversary->id = (int) *pointer;
		adversary->playing = 1;
		adversary->pending_minions = 0;
		adversary->life = DEFAULT_PLAYERS_LIFE;
		adversary->ready_to_play = 0;
		adversary->name = get_connected_server_name();
		adversary->minions_sent = NULL;
		
		//Set pointer back
		adversary -= temp + 1;
		comm->adversary = adversary;
		comm->match->players = temp + 1;
		
	}
	//Check if must add new user
	else if(strncmp(buffer, "ADD_USER", strlen("ADD_USER")) == 0) {
		SDL_AtomicLock(&lock);
		temp = comm->match->players;
		Adversary *adversary = malloc(sizeof (Adversary) * (temp + 1));
		if(comm->adversary) {
			for (i = 0; i < temp; i++){
				adversary->id = comm->adversary[i].id;
				adversary->playing = comm->adversary[i].playing;
				adversary->name = comm->adversary[i].name;
				adversary->life = comm->adversary[i].life;
				adversary->ready_to_play = comm->adversary[i].ready_to_play;
				adversary->pending_minions = comm->adversary[i].pending_minions;
				adversary->minions_sent = comm->adversary[i].minions_sent;
				adversary++;		
			}
			free(comm->adversary);
		}
		pointer = strchr(buffer, '\t');
		pointer++;
		adversary->id = (int) *pointer;
		adversary->playing = 1;
		adversary->pending_minions = 0;
		adversary->life = DEFAULT_PLAYERS_LIFE;
		adversary->ready_to_play = 0;
		adversary->minions_sent = NULL;
		pointer = strchr(pointer, '\t');
		pointer++;
		if(!adversary->name){
			adversary->name = malloc(sizeof(char) * SERVER_NAME);
		}
		strncpy(adversary->name, (const char *) pointer, SERVER_NAME);
		
		//Set pointer back
		adversary -= temp + 1;
		
		comm->adversary = adversary;
		comm->match->players = temp + 1;
		
		SDL_AtomicUnlock(&lock);
	}			
	//Check if must remove user
	else if(strncmp(buffer, "REMOVE_USER", strlen("REMOVE_USER")) == 0) {
		pointer = strchr(buffer, '\t');
		pointer++;
		user_id = (int) *pointer;
		SDL_AtomicLock(&lock);
		if(game_in_progress){
			for (i = 0; i < comm->match->players; i++){
				if(comm->adversary->id == user_id){
					comm->adversary->playing = 0;
					break;
				}
				comm->adversary++;
			}
		}
		else {
			//If game not began remove from list.
			//Realloc
			if(comm->adversary){
				temp = comm->match->players;
				Adversary *adversary = malloc(sizeof (Adversary) * (temp - 1));
				for (i = 0; i< temp; i++){
					if(comm->adversary[i].id != user_id) {
						adversary->id = comm->adversary[i].id;
						adversary->playing = comm->adversary[i].playing;
						adversary->ready_to_play = comm->adversary[i].ready_to_play;
						adversary->name = comm->adversary[i].name;
						adversary->life = comm->adversary[i].life;
						adversary->pending_minions = comm->adversary[i].pending_minions;
						adversary->minions_sent = comm->adversary[i].minions_sent;
						adversary++;
					}
					else {
						if(adversary->minions_sent){
							free(adversary->minions_sent);
							adversary->minions_sent = NULL;
						}
						//Free name
						if(adversary->name){
							free(adversary->name);
							adversary->name = NULL;
						}
					}
				}
				free(comm->adversary);
				
				//Set pointer back
				adversary -= (i - 1);
				
				comm->adversary = adversary;
				comm->match->players = (i - 1);
			}
		}
		SDL_AtomicUnlock(&lock);
	}			
	//Check if must add minion
	else if(strncmp(buffer, "ADD_MINION", strlen("ADD_MINION")) == 0) {
		pointer = strchr(buffer, '\t');
		pointer++;
		user_id = (int) *pointer;
		pointer+=2;
		temp = (int) *pointer; //qtd.
		SDL_AtomicLock(&lock);
		//Check if user 
		for(i = 0; i < comm->match->players;i++){
			if(comm->adversary->id == user_id){
				int *minions_to_send = NULL;
				if(comm->adversary->pending_minions > 0 && comm->adversary->minions_sent){
					//Realloc
					minions_to_send = malloc(sizeof(int) * (comm->adversary->pending_minions + temp));
					for(int z = 0;z < comm->adversary->pending_minions;z++){
						*minions_to_send = comm->adversary->minions_sent[z];
						minions_to_send++;
					}
					pointer+=2;//Now pointer point to first type.
					//Add new ones.
					for(int z = 0; z < temp;z++){
						*minions_to_send = (int) *pointer;
						minions_to_send++;
						pointer+=2;
					}
					comm->adversary->pending_minions = temp + comm->adversary->pending_minions;
				}
				else {
					//Allocate new minions
					minions_to_send = malloc(sizeof(int) * (temp));
					pointer+=2;//Now pointer point to first type.
					for(int z = 0; z < temp;z++){
						*minions_to_send = (int) *pointer;
						minions_to_send++;
						pointer+=2;
					}
					comm->adversary->pending_minions = temp;
				}
				//Reset pointer location
				minions_to_send -= comm->adversary->pending_minions;
				if(comm->adversary->minions_sent){
					free(comm->adversary->minions_sent);
				}
				comm->adversary->minions_sent = minions_to_send;
				break;
			}
			comm->adversary++;
		}
		//Set pointer back
		comm->adversary -= i;
		SDL_AtomicUnlock(&lock);
	}			
	//Check USER_READY
	else if(strncmp(buffer, "USER_READY", strlen("USER_READY")) == 0) {
		pointer = strchr(buffer, '\t');
		pointer++;
		user_id = (int) *pointer;
		pointer+=2;
		for(i = 0; i < comm->match->players; i++){
			if(comm->adversary->id == user_id){
				comm->adversary->ready_to_play = (int) *pointer;
				break;
			}
			comm->adversary++;
		}
	}			
	//Check USER_LIFE
	else if(strncmp(buffer, "USER_LIFE", strlen("USER_LIFE")) == 0) {
		pointer = strchr(buffer, '\t');
		pointer++;
		user_id = (int) *pointer;
		pointer+=2;
		for(i = 0; i < comm->match->players; i++){
			if(comm->adversary->id == user_id){
				comm->adversary->life = (int) *pointer;
				break;
			}
			comm->adversary++;
		}
	}	
	//Check if game was ended
	else if(strncmp(buffer, "END_GAME", strlen("BEGIN_GAME")) == 0) {
		//comm->match->can_start = 1;
		comm->match->finished = 1;
		pointer = strchr(buffer, '\t');
		pointer++;
		comm->match->winner_id = (int) *pointer;
		if(comm->match->winner_id == current_user->id){
			comm->match->lost = 0;
		}
		else {
			comm->match->lost = 1;
		}
		game_in_progress = 0;
	}
	//Check SERVER_FULL
	else if(strncmp(buffer, "SERVER_FULL", strlen("SERVER_FULL")) == 0) {
		//Maximum number of clients connected.
		comm->match->error = 1;
	}
	//Check GAME_ALREADY_STARTED
	else if(strncmp(buffer, "GAME_ALREADY_STARTED", strlen("GAME_ALREADY_STARTED")) == 0) {
		//Game already started without you.
		comm->match->error = 2;
	}
	//---------------------
	//Server side
	//---------------------
	//Send minion
	else if(strncmp(buffer, "USER_MINION", strlen("USER_MINION")) == 0) {
		pointer = strchr(buffer, '\t');
		pointer++;
		user_id = (int)*pointer;
		temp = 0;
		if(user_id != current_user->id) {			
			TCPsocket sender_socket = get_socket_from_user_id(user_id, &temp);
			if(sender_socket){
				pointer+=2;
				//Send message
				if(!send_message(pointer, 2, sender_socket, 1)){
					//--Remove client if message not send
					//Get index for socket
					remove_client(temp);
				}
			}
		}
		//If current user is the server. This can be used without change com internal server making the action because will not drop by this else.
		else {
		//Update server game
			SDL_AtomicLock(&lock);
			temp = comm->adversary->pending_minions;
			//Realloc 
			int *minions_sent = malloc(sizeof(int) * (temp + (int) *pointer));
			if(comm->adversary->minions_sent){
				for(i = 0; i < temp; i++){
					*minions_sent = comm->adversary->minions_sent[i];
					minions_sent++;
				}
				free(comm->adversary->minions_sent);
			}
			temp = (int) *pointer;//pointer on qtd
			pointer+=2;
			for(i = 0; i < temp; i++){
				*minions_sent = (int) *pointer;
				minions_sent++;
				pointer+=2;
			}

			comm->adversary->minions_sent = minions_sent;
			comm->adversary->pending_minions = temp + comm->adversary->pending_minions;
			
			SDL_AtomicUnlock(&lock);
		}
	}
	//User USER_STATUS
	else if(strncmp(buffer, "USER_STATUS", strlen("USER_STATUS")) == 0) {
		pointer = strchr(buffer, '\t');
		pointer++;
		user_id = (int)*pointer;
	
		sprintf(buffer, "USER_READY\t%s", pointer);
		temp = 0;
		connected = connected_clients;
		for(i=0;i< MAX_CLIENT;i++){
			if(temp == connected) {
				break;
			}
			if(clients[i].tcp_socket) {
				if(clients[i].id != user_id){
				//Send message to all except who sent
				if(!send_message(buffer, 5, clients[i].tcp_socket, 0)){
					//Remove client
					remove_client(i);
					}
				}
				temp++;
			}
		}
		
		//Update server game. Only if action was not from server internal call.
		if(!handle_internal){
			temp = comm->match->players;
			pointer+=2;
			
			for (i = 0; i< temp; i++){
				if(comm->adversary->id == user_id){
					comm->adversary->ready_to_play = (int) *pointer;
					break;
				}
				comm->adversary++;
			}
			//Set pointer back
			comm->adversary -= i;
		}
	}
	//User USER_STATUS_LIFE
	else if(strncmp(buffer, "USER_STATUS_LIFE", strlen("USER_STATUS_LIFE")) == 0) {
		pointer = strchr(buffer, '\t');
		pointer++;
		user_id = (int)*pointer;
		pointer +=2;
		life = (int) *pointer;
		pointer -= 2;
		
		sprintf(buffer, "USER_LIFE\t%s", pointer);
		temp = 0;
		connected = connected_clients;
		for(i=0;i< MAX_CLIENT;i++){
			if(temp == connected) {
				break;
			}
			if(clients[i].tcp_socket) {
				if(clients[i].id != user_id){
					//Send message to all except who sent
					if(!send_message(buffer, 11, clients[i].tcp_socket, 0)) {
						//Remove client
						remove_client(i);
					}
				}
				else {
					//Update alive status
					if(life <= 0) {
						clients[i].alive = 0;
					}					
				}
				temp++;
			}
		}
		
		//Update server game
		if(!handle_internal){
			temp = comm->match->players;
			pointer+=2;
			
			for (i = 0; i< temp; i++){
				if(comm->adversary->id == user_id){
					comm->adversary->life = (int) *pointer;
					break;
				}
				comm->adversary++;
			}
			//Set pointer back
			comm->adversary -= i;
		}
	}
	//USER_NAME
	else if(strncmp(buffer, "USER_NAME", strlen("USER_NAME")) == 0) {
		pointer = strchr(buffer, '\t');
		pointer++;
		user_id = (int)*pointer;
		pointer+=2;
		char *name = pointer;
		//Update adversary
		for(i =0; i< comm->match->players;i++){
			if(comm->adversary->id == user_id){
				if(!comm->adversary->name){
					comm->adversary->name = malloc(sizeof(char) * SERVER_NAME);
				}
				strncpy(comm->adversary->name, name, SERVER_NAME);
				break;
			}
			comm->adversary++;
		}
		//Set pointer back
		comm->adversary -= i;
		//Send add user to clients
		pointer-=2;
		sprintf(buffer, "ADD_USER\t%s", pointer);
		temp = 0;
		for(i=0;i< MAX_CLIENT;i++){
			if(temp == connected_clients) {
				break;
			}
			if(clients[i].tcp_socket) {
				if(clients[i].id != user_id){
					//Send message to all except who sent//ADD_USER
					if(!send_message(buffer, 0, clients[i].tcp_socket, 0)){
						remove_client(i);
					}
				}
				else {
					clients[i].has_name = 1;
					strncpy(clients[i].name, name, SERVER_NAME);
				}
				temp++;
			}
		}
	}
}

int has_message_tcp(char *buffer, TCPsocket tcp_socket){
	int amount = 0;
	
	amount = SDLNet_CheckSockets(activity, 0);
	if (amount < 0) {
		printf("SDLNet_CheckSockets: %s\n", SDLNet_GetError());
		return -2;
	}
	else if (amount > 0) {
		if(SDLNet_SocketReady(tcp_socket)){
			if(SDLNet_TCP_Recv(tcp_socket, buffer, BUFFER_LIMIT) > 0){
				return 1;
			}
			else {
				SDLNet_TCP_DelSocket(activity, tcp_socket);
				close_socket(tcp_socket);
				return -1;
			}
		}
		else {
			SDLNet_TCP_DelSocket(activity, tcp_socket);
			close_socket(tcp_socket);
			return -1;
		}
	}

	return 0;
}

int handle_message_pool(TCPsocket tcp_socket){
	char *buffer = malloc(sizeof(char) * BUFFER_LIMIT);
	int has = 0;
	
	
	while(1) {
		has = has_message_tcp(buffer, tcp_socket);
		if(has == 1){
			//Message handle with success, give me the next one please.
			handle_message(buffer, 0);		}
		else if (has == -1){
			return 0;
		}
		else {
			return 1;
		}
	}
	
	free(buffer);
	return 1;
}

void process_action(){
	int i = 0, is_server;
	char buffer[BUFFER_LIMIT];
	SpawnMinion *minions = NULL;
	//Send message ready to play
	
	is_server = current_user->is_server;

	//Send message status
	if(current_user->process.message_status){
		SDL_AtomicLock(&lock);
		current_user->process.message_status--;
		sprintf(buffer, "USER_STATUS\t%c\t%c", (char) current_user->id, (char) current_user->ready_to_play);
		SDL_AtomicUnlock(&lock);
			
		if(is_server){
			//This use the internal server message handle to process the message without need to send.
			handle_message(buffer, 1);
		}
		else {
			if(!send_message(buffer, 9, server_tcp_socket, 0)){
				comm->connection_lost = 1;
			}
		}
	}
	//Send message life
	if(current_user->process.message_life){
		SDL_AtomicLock(&lock);
		current_user->process.message_life--;
		sprintf(buffer, "USER_STATUS_LIFE\t%c\t%c", (char) current_user->id, (char) current_user->life);
		SDL_AtomicUnlock(&lock);
		if(is_server){
			handle_message(buffer, 1);
		}
		else {
			if(send_message(buffer, 12, server_tcp_socket, 0) == 0){
				comm->connection_lost = 1;
			}
		}
	}
	//Send message minion
	if(current_user->process.message_minion){
		SDL_AtomicLock(&lock);
		current_user->process.message_minion--;
		if(current_user->spawn_amount && current_user->minions){
			minions = current_user->minions;
			current_user->minions = NULL;
			i = current_user->spawn_amount;
			current_user->spawn_amount = 0;
		}
		SDL_AtomicUnlock(&lock);
		//Process minions.
		if(minions) {
			for(int j = 0; j < i; j++){
				sprintf(buffer, "USER_MINION\t%c\t%c", buffer, (char) minions[j].client_id, (char) minions[j].amount);
				for(int z = 0; z < minions[j].amount;z++) {
					sprintf(buffer, "%s\t%c", buffer, (char) minions[j].type[z]);
				}
				//minions->type--;
				free(minions[j].type);
			
				//Send message to server
				if(is_server){
					handle_message(buffer, 1);
				}
				else {
					if(send_message(buffer, 8, server_tcp_socket, 0)){
						comm->connection_lost = 1;
						break;
					}
				}
			}
			//minions--;
			free(minions);
		}
	}
}

//Server runner
///////////////////////////////////////////////////////////////////////

void run_server(void *data){
	int created_server = 0;
	
	current_user->is_server = 1;
	game_in_progress = 0;
	game_ended = 0;
	
	
	//Initial setup 
	//------------------------------------
	main_server = malloc(sizeof(Host));
	
	created_server = establish_server(&main_server->ip);
	
	if(!created_server){
		printf("Could not create server.\n");
		comm->server->connecting = 0;
		comm->server->connection_failed = 1;
	}
	
	while(!terminate_thread){
		if(created_server && !comm->match->finished && !comm->server->connection_failed){
			//Dont allow anyone to connect if game is running.
			if(!game_in_progress) {
				//Check UDP messages
				check_messages_udp();
				//Check connection with client 
				check_connection_tcp();
			}
			
			//Update game status
			game_status();
			
			//Check TCP messages from clients connected. This also connect with a 
			check_messages_tcp();
			
			//Update game status
			game_status();
			
			//process player action.
			process_action();
			
			//Delay server
			
		}
	}
	//Close clients connection
	close_clients();
	//Close TCP connection.
	close_connection();
	
	//Free server
	if(main_server){
		//Free ip address
		main_server->ip;
		free(main_server);
	}
	terminate_thread = 0;
	current_user->is_server = 0;
	return;
}

void run_client(void *data){
	char buffer[BUFFER_LIMIT];
	
	int found = 0;
	int connected = 0;
	
	game_in_progress = 0;
	game_ended = 0;
	
	if(comm){
		SDL_AtomicLock(&lock);
		//Set searching network
		comm->server->searching = 1;
		SDL_AtomicUnlock(&lock);
		//Search server
		found = find_servers();
		printf("Servers found: %d\n",found);
		
		//Connect or choose server to connect.
		if(found == 0) {
			SDL_AtomicLock(&lock);
			comm->server->searching = 0;
			comm->server->searching_finished = 1;
			comm->server->connecting = 0;
			comm->server->search_result = 0;
			comm->server->avaliable = 0;
			SDL_AtomicUnlock(&lock);
			//Not connect.
		}
		else if(found == 1) {
			SDL_AtomicLock(&lock);
			comm->server->searching = 0;
			comm->server->searching_finished = 1;
			comm->server->connecting = 1;
			comm->server->search_result = 1;//Same as avaliable?
			comm->server->avaliable = 1;
			comm->server->host = malloc(sizeof(Host));
			comm->server->host = get_host();
			SDL_AtomicUnlock(&lock);
			//Connect to this server.
			printf("Only one found!\n");
			connected = connect_to_server(0);
		}
		else {
			SDL_AtomicLock(&lock);
			comm->server->searching = 0;
			comm->server->searching_finished = 1;
			comm->server->connecting = 0;
			comm->server->search_result = found;//Same as avaliable?
			comm->server->avaliable = found;//Maybe change to 1 and use for server avaliable or not.
			comm->server->host = malloc(sizeof(Host) * found);
			comm->server->host = get_host();
			
			comm->server->choosing = 1;
			SDL_AtomicUnlock(&lock);
			
			//Need to choose server.
			while(comm->server->choosing && !terminate_thread){
				//Wait until the user choose a server on main thread.
				SDL_Delay(SERVER_USER_RESPONSE_DELAY);
			}
			SDL_AtomicLock(&lock);
			comm->server->connecting = 1;
			SDL_AtomicUnlock(&lock);
			//Connect to selected server.
			connected = connect_to_server(comm->server->choosed);
		}
		if(!connected) {
			printf("Not able to connect with server.\n");
			SDL_AtomicLock(&lock);
			comm->server->connecting = 0;
			comm->server->connection_failed = 1;
			SDL_AtomicUnlock(&lock);
		}
		else {
			SDL_AtomicLock(&lock);
			comm->server->connecting = 0;
			comm->server->connected = 1;
			SDL_AtomicUnlock(&lock);
			
			//Send current name to server
			sprintf(buffer, "%c\t%s", (char) current_user->id, current_user->name);
			if(!send_message(buffer,13, server_tcp_socket, 1)){
				comm->server->connecting = 0;
				comm->server->connection_failed = 1;
				comm->server->connected = 0;
			}
		}
		
		while(!terminate_thread){
			//Running 
			if(connected && !comm->match->finished && !comm->server->connection_failed){
				printf("Connected\n");
				
				//Check if there is response to begin game.
				//Check tcp messages only.
				if(!comm->connection_lost){
					check_messages_tcp();
				}
				
				game_status();

				//Process player action
				if(!comm->connection_lost){
					process_action();
				}
			}
		}
		//Destroy game communication
		SDL_AtomicLock(&lock);
		remove_communication();
		SDL_AtomicUnlock(&lock);
	}
	printf("Thread exiting\n");
	terminate_thread = 0;
	return;
}

void kill_thread(SDL_Thread **thread){
	//Kill thread.
	terminate_thread = 1;
	SDL_DetachThread(*thread);
	*thread = NULL;
	return;
}

/**
This function was copied from deltheil on 
http://stackoverflow.com/questions/20019786/safe-and-portable-way-to-convert-a-char-to-uint16-t
*/
int str_to_uint16(const char *str, uint16_t *res){
	char *end;
	errno = 0;
	intmax_t val = strtoimax(str, &end, 10);
	if (errno == ERANGE || val < 0 || val > UINT16_MAX || end == str || *end != '\0')
		return 0;
	*res = (uint16_t) val;
	return 1;
}

