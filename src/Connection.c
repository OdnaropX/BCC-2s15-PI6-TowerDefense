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
int is_server = 0;

int current_index_id = 0;

extern Threads *thread_control;
extern ShareData *data_shared;

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
	data_shared->current_user->id = 0;
	
	return game;
}

void remove_communication(){
	//printf("Removing communication\n");
	
	if(data_shared->current_comm){
		SDL_AtomicLock(&thread_control->lock.comm);
		if(data_shared->current_comm->match){
			free(data_shared->current_comm->match);
			data_shared->current_comm->match = NULL;
		}
		
		if(data_shared->current_comm->adversary){
			if(data_shared->current_comm->adversary->minions_sent){
				free(data_shared->current_comm->adversary->minions_sent);
				data_shared->current_comm->adversary->minions_sent = NULL;
			}
			free(data_shared->current_comm->adversary);
			data_shared->current_comm->adversary = NULL;
		}
		
		if(data_shared->current_comm->server){
			////printf("Here %d]n\n", comm->server->avaliable);
			if(data_shared->current_comm->server->host && data_shared->current_comm->server->avaliable){
				free(data_shared->current_comm->server->host);
				data_shared->current_comm->server->host = NULL;
			}
			free(data_shared->current_comm->server);
			data_shared->current_comm->server = NULL;
		}
		
		free(data_shared->current_comm);
		data_shared->current_comm = NULL;
		SDL_AtomicUnlock(&thread_control->lock.comm);
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
				snprintf(buffer, BUFFER_LIMIT, "%c", (char) clients[i].id);
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
		SDL_AtomicLock(&thread_control->lock.comm);
		temp = data_shared->current_comm->match->players;
		for (i = 0; i < temp; i++){
			if(data_shared->current_comm->adversary[i].id == clients[client].id){
				data_shared->current_comm->adversary[i].playing = 0;
				break;
			}
		}
		SDL_AtomicUnlock(&thread_control->lock.comm);
	}
	else {
		SDL_AtomicLock(&thread_control->lock.comm);
		//If game not began remove from list.
		//Realloc
		if(data_shared->current_comm->match->players > 0){
			temp = data_shared->current_comm->match->players;
			Adversary *adversary = malloc(sizeof (Adversary) * (temp - 1));
			for (i = 0; i< temp; i++){
				if(data_shared->current_comm->adversary[i].id != clients[client].id) {
					adversary[i].id = data_shared->current_comm->adversary[i].id;
					adversary[i].playing = data_shared->current_comm->adversary[i].playing;
					adversary[i].ready_to_play = data_shared->current_comm->adversary[i].ready_to_play;
					adversary[i].name = data_shared->current_comm->adversary[i].name;
					adversary[i].life = data_shared->current_comm->adversary[i].life;
					adversary[i].pending_minions = data_shared->current_comm->adversary[i].pending_minions;
					adversary[i].minions_sent = data_shared->current_comm->adversary[i].minions_sent;
				}
				else {
					if(adversary[i].minions_sent){
						free(adversary[i].minions_sent);
						adversary[i].minions_sent = NULL;
					}
					if(adversary[i].name){
						//Free name
						free(adversary[i].name);
						adversary[i].name = NULL;
					}

				}
			}
			if(data_shared->current_comm->adversary){
				free(data_shared->current_comm->adversary);
			}
			data_shared->current_comm->adversary = adversary;
			data_shared->current_comm->match->players = temp - 1;
		}
		SDL_AtomicUnlock(&thread_control->lock.comm);
	}
}

//Update Functions
///////////////////////////////////////////////////////////////////////
	
int update_list_servers(UDPpacket* package){
	char *name = NULL;
	char *prt = NULL;
	char port[SERVER_NAME];
	int i, use_default = 0;
	Uint16 port_number = 0;
	
	//printf("Updating servers\n");
	
	//Check if already on list.
	for(i=0; i < MAX_SERVER && servers[i].ip.host != 0;i++) {
		////printf("Update %d\n", servers[i].ip.host);
		if(package->address.host == servers[i].ip.host){
			////printf("Here already\n");
			return 1;
		}
	}
	
	if(i < MAX_SERVER) {
		////printf("PORT: |%d|\n",package->address.port);
		name = strchr((char*)package->data, '\t');//Data is Uint8, so need cast.
		////printf("Message: |%s|\n", name);
		if(++name) {
			 strncpy(servers[i].name, name, SERVER_NAME);
			 ////printf("Server name: |%s|\n", servers[i].name);
			 
			 name = strchr(servers[i].name, '\t');
			 
			 if(++name) {
				 strncpy(port, name, SERVER_NAME);
			 }
			 else {
				 //use_default = 1;
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
			port_number = (Uint16) DEFAULT_PORT_TCP;
		}
		servers[i].ip.host = package->address.host;
		servers[i].ip.port = port_number;
		////printf("Server name: |%s|\n", servers[i].name);
		////printf("Server port: |%d|\n", servers[i].ip.port);
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
        //printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
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
		//printf("Cannot establish UDP socket.");
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
		//printf("Could not resolve server name\n");
		return 0;
	}
	
    output_package->address.host = ip.host;
    output_package->address.port = ip.port;
    snprintf((char*) output_package->data, BUFFER_LIMIT, "GRADE_DEFENDER_CLIENT");
    output_package->len = strlen("GRADE_DEFENDER_CLIENT") + 1;

	//Package for localhost. This will be used on internal tests.
    if(SDLNet_ResolveHost(&ip, "localhost", DEFAULT_PORT_UDP) != 0) {
		ignore_local = 1;
	}
	
    output_package_local->address.host = ip.host;
    output_package_local->address.port = ip.port;
    snprintf((char*) output_package_local->data, BUFFER_LIMIT, "GRADE_DEFENDER_CLIENT");
    output_package_local->len = strlen("GRADE_DEFENDER_CLIENT") + 1;
	
	//Reset ip host
	for(i = 0; i < MAX_SERVER; i++) {
        servers[i].ip.host = 0;
	}
	
	while(trying && !thread_control->client.terminate){
		////printf("Trying send package: %s\n", (char *) output_package->data);//Cast because data is save as Uint8 *
		
		//sent = SDLNet_UDP_Send(udp_socket, output_package->channel, output_package);
		sent = SDLNet_UDP_Send(udp_socket, -1, output_package);
		if (!sent) {
			//printf("Broadcast failed!\nTrying Localhost...\n");
			
			if(ignore_local) {
				//printf("Something really, really, I mean really wrong happened and Localhost was not accessed.\nThis was not suppost to happen.\n");
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
			//printf("Receiving Message\n");
            if(strncmp((char*)input_package->data, "GRADE_DEFENDER_SERVER", strlen("GRADE_DEFENDER_SERVER")) == 0){
                trying = 0;
                //add to list, checking for duplicates
                update_list_servers(input_package);
            }
        }

		for(number_found = 0; number_found < MAX_SERVER && servers[number_found].ip.host != 0; number_found++);
		
		//printf("Found: %d\n", number_found);
		
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
				if(!thread_control->client.terminate)
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
	//printf("Search server done!\n");

    SDLNet_FreePacket(output_package); 
    SDLNet_FreePacket(output_package_local); 
    SDLNet_FreePacket(input_package); 
    SDLNet_UDP_Close(udp_socket); 
    return number_found;
}

int establish_server(IPaddress *ip){
	time_t t;
	/* First create TCP socket that will be used to connect	*/
	//Null is to listen
	if(SDLNet_ResolveHost(ip, NULL, DEFAULT_PORT_TCP) < 0){

		//printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		return 0;
	}

	////printf("Server host |%s| port |%d|", inet_ntoa(ip->host), ip->port);
	
	//Create TCP socket.
	server_tcp_socket = SDLNet_TCP_Open(ip);
	
	if (!server_tcp_socket){
        //printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return 0;
    }
	
	activity = SDLNet_AllocSocketSet(MAX_CLIENT);
	if(!activity){
		//printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
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
	SDL_AtomicLock(&thread_control->lock.user);
	data_shared->current_user->id = rand() % 50;
	if(data_shared->current_user->id == 0) {
		data_shared->current_user->id = 1;
	}
	current_index_id = data_shared->current_user->id + 1;
	SDL_AtomicUnlock(&thread_control->lock.user);
	
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
	
	//printf("Not returned 1\n");
	
	if(connected_clients == MAX_CLIENT) {
		//Inform client that connection cannot be made.
		send_message(NULL, 6, socket, 1);
		return;
	}
	
	//printf("Not returned 2\n");
	
	//Check if there is slot avaliable
	for(i = 0; i < MAX_CLIENT && clients[i].tcp_socket;i++);
	
	//printf("Not returned 3\n");
	
	//Repeat in case some went wrong in if above.
	if (i == MAX_CLIENT) {
		//Inform client that connection cannot be made.
		send_message(NULL, 6, socket, 1);
		return;
	}
	
	//printf("Not returned 4\n");

	//Check if game is in progress
	if(game_in_progress) {
		//Informe client that game in progress
		send_message(NULL, 7, socket, 1);
		return;
	}
	
	//printf("Not returned 5\n");

	//Add client socket to set 
	sockets = SDLNet_TCP_AddSocket(activity, socket);
	
	//printf("Not returned 6\n");
	
	if(sockets == -1) {
		//printf("SDLNet_AddSocket: %s\n", SDLNet_GetError());
		SDL_AtomicLock(&thread_control->lock.comm);
		data_shared->current_comm->server->connecting = 0;
		data_shared->current_comm->server->connection_failed = 1;
		SDL_AtomicUnlock(&thread_control->lock.comm);
		return;
	}
	
	//printf("Not returned 7\n");

	//Create id
	user_id = (current_index_id % 255);
	if(user_id == 0){
		user_id = 1;
	} 
	current_index_id = user_id + 1;
	
	//printf("Not returned 8\n");
	
	//Send id
	SDL_AtomicLock(&thread_control->lock.user);
	snprintf(buffer, BUFFER_LIMIT, "%c\t%c", (char) user_id, (char) data_shared->current_user->id);
    SDL_AtomicUnlock(&thread_control->lock.user);
	printf("Buffer |%s|\n", buffer);
	if(send_message(buffer, 10, socket, 1)){
		printf("Send user iddddd \n");
		temp = 0;
		//Send other users to this user
		connected = connected_clients;
		for(j = 0; j < MAX_CLIENT;j++){
			if(temp == connected) {
				break;
			}
			if(clients[j].tcp_socket){
				if(j != i && clients[j].has_name) {
					snprintf(buffer, BUFFER_LIMIT, "%c\t%s", (char) clients[j].id, clients[j].name);
					if(!send_message(buffer, 0, socket, 1)){
						//Connection failed.
						printf("Connection failed need to check this\n");
						return;
					}
				}
				temp++;
			}
		}
		
		//Reset j.
		//j = 0;
		
		//Add to adversary
		SDL_AtomicLock(&thread_control->lock.comm);
		data_shared->current_comm->adversary = realloc(data_shared->current_comm->adversary, sizeof (Adversary) * sockets);
		temp = data_shared->current_comm->match->players;
	
		data_shared->current_comm->adversary[temp].id = user_id;
		data_shared->current_comm->adversary[temp].playing = 1;
		data_shared->current_comm->adversary[temp].pending_minions = 0;
		data_shared->current_comm->adversary[temp].life = DEFAULT_PLAYERS_LIFE;
		data_shared->current_comm->adversary[temp].ready_to_play = 0;
		data_shared->current_comm->adversary[temp].minions_sent = NULL;

		data_shared->current_comm->match->players = data_shared->current_comm->match->players + 1;
		SDL_AtomicUnlock(&thread_control->lock.comm);
		
		//Add to client list
		clients[i].tcp_socket = socket;
		clients[i].id = user_id;
		clients[i].alive = 1;
		connected_clients = sockets;
		printf("User id connected %d\n", user_id);
	}
	else {
		//Close connection
		SDLNet_TCP_DelSocket(activity, socket);
		close_socket(socket);
		printf("Not connected\n");
	}

	return;
}

void check_messages_tcp(){
	int i, temp;
	
	if(is_server){
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
			SDL_AtomicLock(&thread_control->lock.comm);
			data_shared->current_comm->server->connection_failed = 1;
			SDL_AtomicUnlock(&thread_control->lock.comm);
		}
	}
	return;
}

void check_messages_udp(){
	int sent, received = 0;
	char buffer[BUFFER_LIMIT];
	
	UDPpacket* input = NULL;
	
	if(!server_udp_socket){
		//printf("Error: Socket server_udp_socket is NULL.\n");
		return;
	}	
	
	input = SDLNet_AllocPacket(BUFFER_LIMIT);
	received = SDLNet_UDP_Recv(server_udp_socket, input);//Message will not be greater than buffer. 
	
	if(received == 1){
		if(strncmp((char*)input->data, "GRADE_DEFENDER_CLIENT", strlen("GRADE_DEFENDER_CLIENT")) == 0){
			//Send message to client.
			UDPpacket* output;
			output = SDLNet_AllocPacket(BUFFER_LIMIT);
			snprintf(buffer, BUFFER_LIMIT, "%s\t%s\t%d", "GRADE_DEFENDER_SERVER", data_shared->current_user->name, DEFAULT_PORT_TCP);
			snprintf((char *)output->data, BUFFER_LIMIT, "%s", buffer);
			output->len = (int)strlen(buffer) + 1;
			output->address.host = input->address.host;
            output->address.port = input->address.port;
			sent = SDLNet_UDP_Send(server_udp_socket, -1, output);
			if (sent){
				////printf("Package from server was sent\n");
			}
            SDLNet_FreePacket(output);
		}
	}
	SDLNet_FreePacket(input);
	return;
}

void game_status(){
	int i, j, temp, alive = 0, winner_id = 0;
	char buffer[BUFFER_LIMIT];
	
	if(is_server) {
		//Check if there is player connected. If not end game.
		
		if(game_in_progress){
			if(connected_clients <= 0){
				printf("Game step 1\n");
				SDL_AtomicLock(&thread_control->lock.comm);
				//Everyone left, and game with data_shared->current_user as winner.
				data_shared->current_comm->match->finished = 1;
				data_shared->current_comm->match->lost = 0;
				data_shared->current_comm->match->winner_id = data_shared->current_user->id;
				SDL_AtomicUnlock(&thread_control->lock.comm);
			}
			else {
				printf("Game step 2\n");
				//Check if game was finished.
				SDL_AtomicLock(&thread_control->lock.user);
				printf("Game step alive %d\n", alive);
				if(data_shared->current_user->life > 0){
					alive++;
					winner_id = data_shared->current_user->id;
				}
				SDL_AtomicUnlock(&thread_control->lock.user);
				
				temp = 0;
				for(i = 0; i < MAX_CLIENT; i++){
					if(temp == connected_clients){
						break;
					}
					if(clients[i].tcp_socket && clients[i].has_name && clients[i].alive){
						//Send message
						alive++;
						winner_id = clients[i].id;
					}
				}
				
				printf("Game step alive %d\n", alive);
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
							snprintf(buffer, BUFFER_LIMIT, "%c", (char) winner_id);
							//Send message to other players that user left.
							send_message(buffer, 4, clients[i].tcp_socket, 1);
							remove_client(i);
							temp++;
						}
					}
					
					SDL_AtomicLock(&thread_control->lock.comm);
					//Set winner to server.
					data_shared->current_comm->match->finished = 1;
					data_shared->current_comm->match->winner_id = winner_id;
					if(winner_id != data_shared->current_user->id){
						data_shared->current_comm->match->lost = 1;
					}
					SDL_AtomicUnlock(&thread_control->lock.comm);
					//Close connection.//Will be closed when thread is killed.
				}
				printf("Game step alive %d\n", alive);
			}
		}
		else {
			SDL_AtomicLock(&thread_control->lock.comm);
			if(data_shared->current_comm->match->players > 0){
				//Check if game can begin //Check if all users are ready
				temp = 0;
				for(i = 0; i < data_shared->current_comm->match->players;i++){
					if(data_shared->current_comm->adversary[i].ready_to_play){
						temp++;
					}
				}
				
				if(temp == data_shared->current_comm->match->players){
					//This line can add a injust gameplay to server, because it will begin before others. 
					data_shared->current_comm->match->can_start = 1;
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
			SDL_AtomicUnlock(&thread_control->lock.comm);
		}
	}
	else {
		//Check server connection.//This is detected by sending an receiving messages.
		if(data_shared->current_comm->server->connection_failed){
			return;
		}
		
		if(game_in_progress && !data_shared->current_comm->match->lost){
			//Check if game was finish.
			//Check if player lost. If lost, wait until the game is finished.
			if(data_shared->current_user->life <= 0) {
				SDL_AtomicLock(&thread_control->lock.comm);
				data_shared->current_comm->match->lost = 1;
				SDL_AtomicUnlock(&thread_control->lock.comm);
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
	IPaddress ip;
	
	if(server_choice < 0 || server_choice > MAX_SERVER) {
		return 0;
	}
	//Fix for ip address.
	//SDLNet_ResolveHost(&ip, servers[server_choice].ip_name, servers[server_choice].ip.port);
	SDLNet_ResolveHost(&ip, NULL, servers[server_choice].ip.port);

	ip.host = servers[server_choice].ip.host;
	server_tcp_socket = SDLNet_TCP_Open(&ip);
	//ip.port = servers[server_choice].ip.port;
	//server_tcp_socket = SDLNet_TCP_Open(&servers[server_choice].ip);
	if (!server_tcp_socket){
        //printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return 0;
    }
    activity = SDLNet_AllocSocketSet(1);
    if(!activity){
		close_socket(server_tcp_socket);
		close_set(activity);
        //printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        return 0;
    }
	//Add a socket to a socket set that will be watched
    if(SDLNet_TCP_AddSocket(activity, server_tcp_socket) == -1){
        //printf("SDLNet_AddSocket: %s\n", SDLNet_GetError());
        // perhaps you need to restart the set and make it bigger...
		close_socket(server_tcp_socket);
		close_set(activity);
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

Host* get_host(){
	return servers;
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
				snprintf(msg, BUFFER_LIMIT, "ADD_USER\t%s", message);
				break;
			case 1://REMOVE_USER
				snprintf(msg, BUFFER_LIMIT, "REMOVE_USER\t%s", message);
				break;
			case 2://ADD_MINION
				snprintf(msg, BUFFER_LIMIT, "ADD_MINION\t%s", message);
				break;
			case 3://BEGIN_GAME
				snprintf(msg, BUFFER_LIMIT, "BEGIN_GAME");
				break;
			case 4://END_GAME
				snprintf(msg, BUFFER_LIMIT, "END_GAME\t%s", message);
				break;
			case 5://USER_READY
				snprintf(msg, BUFFER_LIMIT, "USER_READY\t%s", message);
				break;
			case 6://SERVER_FULL
				snprintf(msg, BUFFER_LIMIT, "SERVER_FULL");
				break;
			case 7://GAME_ALREADY_STARTED
				snprintf(msg, BUFFER_LIMIT, "GAME_ALREADY_STARTED");
				break;
			case 8://USER_MINION
				snprintf(msg, BUFFER_LIMIT, "USER_MINION\t%s", message);
				break;
			case 9://USER_STATUS
				snprintf(msg, BUFFER_LIMIT, "USER_STATUS\t%s", message);
				break;
			case 10://USER_ID
				snprintf(msg, BUFFER_LIMIT, "USER_ID\t%s", message);
				break;
			case 11://USER_LIFE
				snprintf(msg, BUFFER_LIMIT, "USER_LIFE\t%s", message);
				break;
			case 12://USER_STATUS_LIFE
				snprintf(msg, BUFFER_LIMIT, "USER_STATUS_LIFE\t%s", message);
				break;
			case 13://USER_NAME
				snprintf(msg, BUFFER_LIMIT, "USER_NAME\t%s", message);
				break;
			default:
				next = 0;
		}
	}

	if(next){
		if(incomplete_message) {
			sent = SDLNet_TCP_Send(socket,msg,BUFFER_LIMIT);
			printf("Send buffer |%s|\n",msg);
		}
		else {
			sent = SDLNet_TCP_Send(socket,message,BUFFER_LIMIT);
			printf("Send buffer |%s|\n",message);
		}
			
		if(sent != BUFFER_LIMIT) {
			//printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			// It may be good to disconnect sock because it is likely invalid now.
			return 0;
		}
	}
	return 1;
}

void handle_message(char *buffer, int handle_internal){
	printf("Handling message\n");
	printf("Receive buffer |%s|\n",buffer);
	char *pointer = NULL;
	int i, j, user_id, user_from, temp, life, connected, status;
	//i = 0;

	//Client side
	//Check if game can begin
	if(strncmp(buffer, "BEGIN_GAME", strlen("BEGIN_GAME")) == 0) {
		//printf("Handling BEGIN_GAME\n");
		SDL_AtomicLock(&thread_control->lock.comm);
		data_shared->current_comm->match->can_start = 1;
		game_in_progress = 1;
		SDL_AtomicUnlock(&thread_control->lock.comm);
	}
	//Check user add
	else if(strncmp(buffer, "USER_ID", strlen("USER_ID")) == 0){
		printf("Handling USER_ID\n");
        
		pointer = strchr(buffer, '\t');
		pointer++;
		
		SDL_AtomicLock(&thread_control->lock.user);
		data_shared->current_user->id = (int) *pointer;
		SDL_AtomicUnlock(&thread_control->lock.user);
		
		//Send user name to server.
		char *buffer_name = malloc(sizeof(char) * BUFFER_LIMIT);
		//Send current name to server
		snprintf(buffer_name, BUFFER_LIMIT, "%c\t%s", (char) data_shared->current_user->id, data_shared->current_user->name);
		
		printf("Buffer |%c|%s|\n", (char) data_shared->current_user->id, data_shared->current_user->name);
		
			if(!send_message(buffer_name,13, server_tcp_socket, 1)){
				SDL_AtomicLock(&thread_control->lock.comm);
				data_shared->current_comm->server->connecting = 0;
				data_shared->current_comm->server->connection_failed = 1;
				data_shared->current_comm->server->connected = 0;
				SDL_AtomicUnlock(&thread_control->lock.comm);
				//printf("Failed\n");
			}
		free(buffer_name);

		//Add server as adversary
		SDL_AtomicLock(&thread_control->lock.comm);
		
		temp = data_shared->current_comm->match->players;
		data_shared->current_comm->adversary = realloc(data_shared->current_comm->adversary, sizeof (Adversary) * (temp + 1));
		if(data_shared->current_comm->adversary){
			data_shared->current_comm->adversary[temp].id = (int) *pointer;
			data_shared->current_comm->adversary[temp].playing = 1;
			data_shared->current_comm->adversary[temp].pending_minions = 0;
			data_shared->current_comm->adversary[temp].life = DEFAULT_PLAYERS_LIFE;
			data_shared->current_comm->adversary[temp].ready_to_play = 0;
			data_shared->current_comm->adversary[temp].name = get_connected_server_name();
			data_shared->current_comm->adversary[temp].minions_sent = NULL;
			data_shared->current_comm->match->players = temp + 1;
			//printf("Successfully realloced adversary\n");
		}
		else {
			//printf("Error on realloc data_shared->current_comm->adversary\n");
		}
		SDL_AtomicUnlock(&thread_control->lock.comm);
		
		
	}
	//Check if must add new user
	else if(strncmp(buffer, "ADD_USER", strlen("ADD_USER")) == 0) {
		//printf("Handling ADD_USER\n");
		
		//Add user as adversary
		
		pointer = strchr(buffer, '\t');
		pointer++;
		user_id = (int) *pointer;
		
		pointer = strchr(pointer, '\t');
		pointer++;//This now is name.
		
		//Realloc adversary
		SDL_AtomicLock(&thread_control->lock.comm);
		temp = data_shared->current_comm->match->players;
		data_shared->current_comm->adversary = realloc(data_shared->current_comm->adversary, sizeof (Adversary) * (temp + 1));
		if(data_shared->current_comm->adversary){
			data_shared->current_comm->adversary[temp].id = user_id;
			data_shared->current_comm->adversary[temp].playing = 1;
			data_shared->current_comm->adversary[temp].pending_minions = 0;
			data_shared->current_comm->adversary[temp].life = DEFAULT_PLAYERS_LIFE;
			data_shared->current_comm->adversary[temp].ready_to_play = 0;
			
			data_shared->current_comm->adversary[temp].minions_sent = NULL;
			data_shared->current_comm->adversary[temp].name = get_connected_server_name();
			//data_shared->current_comm->adversary[temp].name = realloc(data_shared->current_comm->adversary[temp].name, sizeof(char) * SERVER_NAME);
			
			if(!data_shared->current_comm->adversary[temp].name){
				data_shared->current_comm->adversary[temp].name = malloc(sizeof(char) * SERVER_NAME);
			}
			strncpy(data_shared->current_comm->adversary[temp].name, (const char *) pointer, SERVER_NAME);
		
			data_shared->current_comm->match->players = temp + 1;
			//printf("Successfully realloced adversary\n");
		}
		else {
			//printf("Error on realloc data_shared->current_comm->adversary\n");
		}
		
		SDL_AtomicUnlock(&thread_control->lock.comm);
	}			
	//Check if must remove user
	else if(strncmp(buffer, "REMOVE_USER", strlen("REMOVE_USER")) == 0) {
		//printf("Handling REMOVE_USER\n");
		pointer = strchr(buffer, '\t');
		pointer++;
		user_id = (int) *pointer;
		if(game_in_progress){
			SDL_AtomicLock(&thread_control->lock.comm);
			temp = data_shared->current_comm->match->players;
			for (i = 0; i < temp; i++){
				if(data_shared->current_comm->adversary[i].id == user_id){
					data_shared->current_comm->adversary[i].playing = 0;
					break;
				}
			}
			SDL_AtomicUnlock(&thread_control->lock.comm);
		}
		else {
			//If game not began remove from list.
			//Realloc
			if(data_shared->current_comm->adversary){
				j = 0;
				
				SDL_AtomicLock(&thread_control->lock.comm);
				temp = data_shared->current_comm->match->players;
				Adversary *adversary = malloc(sizeof (Adversary) * (temp - 1));
				for (i = 0; i< temp; i++){
					if(data_shared->current_comm->adversary[i].id != user_id) {
						adversary[j].id = data_shared->current_comm->adversary[i].id;
						adversary[j].playing = data_shared->current_comm->adversary[i].playing;
						adversary[j].ready_to_play = data_shared->current_comm->adversary[i].ready_to_play;
						adversary[j].name = data_shared->current_comm->adversary[i].name;
						adversary[j].life = data_shared->current_comm->adversary[i].life;
						adversary[j].pending_minions = data_shared->current_comm->adversary[i].pending_minions;
						adversary[j].minions_sent = data_shared->current_comm->adversary[i].minions_sent;
						j++;
					}
					else {
						if(adversary[i].minions_sent){
							free(adversary[i].minions_sent);
							adversary[i].minions_sent = NULL;
						}
						//Free name
						if(adversary[i].name){
							free(adversary[i].name);
							adversary[i].name = NULL;
						}
					}
				}
				free(data_shared->current_comm->adversary);

				data_shared->current_comm->adversary = adversary;
				data_shared->current_comm->match->players = (temp - 1);
				SDL_AtomicUnlock(&thread_control->lock.comm);
			}
		}
	}			
	//Check if must add minion
	else if(strncmp(buffer, "ADD_MINION", strlen("ADD_MINION")) == 0) {
		//printf("Handling ADD_MINION\n");
		pointer = strchr(buffer, '\t');
		pointer++;
		//user_from = (int) *pointer;
		pointer+=2;
		user_id = (int) *pointer;
		pointer+=2;
		temp = (int) *pointer; //qtd.
		SDL_AtomicLock(&thread_control->lock.comm);
		//Check if user 
		for(i = 0; i < data_shared->current_comm->match->players;i++){
			if(data_shared->current_comm->adversary[i].id == user_id){
				int *minions_to_send = NULL;
				if(data_shared->current_comm->adversary[i].pending_minions > 0 && data_shared->current_comm->adversary[i].minions_sent){
					//Realloc
					minions_to_send = malloc(sizeof(int) * (data_shared->current_comm->adversary[i].pending_minions + temp));
					for(int z = 0; z < data_shared->current_comm->adversary[i].pending_minions; z++){
						minions_to_send[z] = data_shared->current_comm->adversary[i].minions_sent[z];
					}
					pointer+=2;//Now pointer point to first type.
					//Add new ones.
					for(int z = 0; z < temp;z++){
						minions_to_send[z] = (int) *pointer;
						pointer+=2;
					}
					data_shared->current_comm->adversary[i].pending_minions = temp + data_shared->current_comm->adversary[i].pending_minions;
				}
				else {
					//Allocate new minions
					minions_to_send = malloc(sizeof(int) * (temp));
					pointer+=2;//Now pointer point to first type.
					for(int z = 0; z < temp;z++){
						minions_to_send[z] = (int) *pointer;
						pointer+=2;
					}
					data_shared->current_comm->adversary[i].pending_minions = temp;
				}
				if(data_shared->current_comm->adversary[i].minions_sent){
					free(data_shared->current_comm->adversary[i].minions_sent);
				}
				data_shared->current_comm->adversary[i].minions_sent = minions_to_send;
				break;
			}
		}
		SDL_AtomicUnlock(&thread_control->lock.comm);
	}	
	//Check USER_READY
	else if(strncmp(buffer, "USER_READY", strlen("USER_READY")) == 0) {
		//printf("Handling USER_READY\n");
		pointer = strchr(buffer, '\t');
		pointer++;
		user_id = (int) *pointer;
		pointer+=2;
		status = (int) *pointer;
		//Fix null char.
		status--;
		SDL_AtomicLock(&thread_control->lock.comm);
		for(i = 0; i < data_shared->current_comm->match->players; i++){
			if(data_shared->current_comm->adversary[i].id == user_id){
				data_shared->current_comm->adversary[i].ready_to_play = status;
				break;
			}
		}
		SDL_AtomicUnlock(&thread_control->lock.comm);
	}			
	//Check USER_LIFE
	else if(strncmp(buffer, "USER_LIFE", strlen("USER_LIFE")) == 0) {
		//printf("Handling USER_LIFE\n");
		pointer = strchr(buffer, '\t');
		pointer++;
		user_id = (int) *pointer;
		pointer+=2;
		status = (int) *pointer;
		//Fix null char.
		status--;
		SDL_AtomicLock(&thread_control->lock.comm);
		for(i = 0; i < data_shared->current_comm->match->players; i++){
			if(data_shared->current_comm->adversary[i].id == user_id){
				data_shared->current_comm->adversary[i].life = status;
				break;
			}
		}
		SDL_AtomicUnlock(&thread_control->lock.comm);
	}	
	//Check if game was ended
	else if(strncmp(buffer, "END_GAME", strlen("BEGIN_GAME")) == 0) {
		//printf("Handling END_GAME\n");
		//data_shared->current_comm->match->can_start = 1;
		data_shared->current_comm->match->finished = 1;
		pointer = strchr(buffer, '\t');
		pointer++;
		SDL_AtomicLock(&thread_control->lock.comm);
		data_shared->current_comm->match->winner_id = (int) *pointer;
		if(data_shared->current_comm->match->winner_id == data_shared->current_user->id){
			data_shared->current_comm->match->lost = 0;
		}
		else {
			data_shared->current_comm->match->lost = 1;
		}
		SDL_AtomicUnlock(&thread_control->lock.comm);
		game_in_progress = 0;
	}
	//Check SERVER_FULL
	else if(strncmp(buffer, "SERVER_FULL", strlen("SERVER_FULL")) == 0) {
		//printf("Handling SERVER_FULL\n");
		//Maximum number of clients connected.
		SDL_AtomicLock(&thread_control->lock.comm);
		data_shared->current_comm->match->error = 1;
		SDL_AtomicUnlock(&thread_control->lock.comm);
	}
	//Check GAME_ALREADY_STARTED
	else if(strncmp(buffer, "GAME_ALREADY_STARTED", strlen("GAME_ALREADY_STARTED")) == 0) {
		//printf("Handling GAME_ALREADY_STARTED\n");
		//Game already started without you.
		SDL_AtomicLock(&thread_control->lock.comm);
		data_shared->current_comm->match->error = 2;
		SDL_AtomicUnlock(&thread_control->lock.comm);
	}
	//---------------------
	//Server side
	//---------------------
	//Send minion
	else if(strncmp(buffer, "USER_MINION", strlen("USER_MINION")) == 0) {
		//printf("Handling USER_MINION\n");
		pointer = strchr(buffer, '\t');
		pointer++;
		user_from = (int)*pointer;
		pointer+=2;
		user_id = (int)*pointer;
		temp = 0;
		if(user_id != data_shared->current_user->id) {			
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
			SDL_AtomicLock(&thread_control->lock.comm);
			for(i = 0; i < data_shared->current_comm->match->players; i++){
				if(data_shared->current_comm->adversary[i].id == user_from){
					j = 0;
					//Realloc 
					int *minions_sent = malloc(sizeof(int) * (data_shared->current_comm->adversary[i].pending_minions + (int) *pointer));
					if(data_shared->current_comm->adversary[i].minions_sent){
						for(; j < data_shared->current_comm->adversary[i].pending_minions; j++){
							minions_sent[j] = data_shared->current_comm->adversary[j].minions_sent[j];
						}
						//free(data_shared->current_comm->adversary[i].minions_sent); //Agora tem realloc abaixo.
					}
					//Realloc 
					data_shared->current_comm->adversary[i].minions_sent = realloc(data_shared->current_comm->adversary[i].minions_sent, sizeof(int) * (data_shared->current_comm->adversary[i].pending_minions + (int) *pointer));
					if(data_shared->current_comm->adversary[i].minions_sent){
						temp = (int) *pointer;//pointer on qtd
						pointer+=2;
						for(; j < temp + data_shared->current_comm->adversary[i].pending_minions; j++){
							minions_sent[j] = (int) *pointer;
							pointer+=2;
						}
						//printf("Successfully realloced data_shared->current_comm->adversary[i].minions_sent\n");
					}
					else {
						//printf("Error on realloc data_shared->current_comm->adversary[i].minions_sent\n");
					}
					data_shared->current_comm->adversary[i].pending_minions = temp + data_shared->current_comm->adversary[i].pending_minions;
					break;
				}
			}
			SDL_AtomicUnlock(&thread_control->lock.comm);
		}
	}
	//User USER_STATUS
	else if(strncmp(buffer, "USER_STATUS", strlen("USER_STATUS")) == 0) {
		//printf("Handling USER_STATUS\n");
		pointer = strchr(buffer, '\t');
		pointer++;
		user_id = (int)*pointer;
	
		snprintf(buffer, BUFFER_LIMIT, "USER_READY\t%s", pointer);
		printf("Message to send |%s|\n", buffer);
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
		printf("current handle %d\n", handle_internal);
		//Update server game. Only if action was not from server internal call.
		if(!handle_internal){
			temp = data_shared->current_comm->match->players;
			pointer+=2;
			status = (int) *pointer;
			//Fix null char.
			status--;
			printf("User %d new status %d\n", user_id, status);
			SDL_AtomicLock(&thread_control->lock.comm);
			for (i = 0; i < temp; i++){
				if(data_shared->current_comm->adversary[i].id == user_id){
					data_shared->current_comm->adversary[i].ready_to_play = status;
					break;
				}
			}
			SDL_AtomicUnlock(&thread_control->lock.comm);
		}
	}
	//User USER_STATUS_LIFE
	else if(strncmp(buffer, "USER_STATUS_LIFE", strlen("USER_STATUS_LIFE")) == 0) {
		//printf("Handling USER_STATUS_LIFE\n");
		pointer = strchr(buffer, '\t');
		pointer++;
		user_id = (int)*pointer;
		pointer +=2;
		life = (int) *pointer;
		pointer -= 2;
		
		snprintf(buffer, BUFFER_LIMIT, "USER_LIFE\t%s", pointer);
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
			pointer+=2;
			status = (int) *pointer;
			//Fix null char.
			status--;
			SDL_AtomicLock(&thread_control->lock.comm);
			temp = data_shared->current_comm->match->players;
			for (i = 0; i< temp; i++){
				if(data_shared->current_comm->adversary[i].id == user_id){
					data_shared->current_comm->adversary[i].life = status;
					break;
				}
			}
			SDL_AtomicUnlock(&thread_control->lock.comm);
		}
	}
	//USER_NAME
	else if(strncmp(buffer, "USER_NAME", strlen("USER_NAME")) == 0) {
		//printf("Handling USER_NAME\n");
		pointer = strchr(buffer, '\t');
		pointer++;
		user_id = (int)*pointer;
		pointer+=2;
		char *name = pointer;
        printf("User name %s\n", name);
		//Update adversary
		SDL_AtomicLock(&thread_control->lock.comm);
		for(i =0; i < data_shared->current_comm->match->players; i++){
			if(data_shared->current_comm->adversary[i].id == user_id){
                data_shared->current_comm->adversary[i].name = NULL;
				data_shared->current_comm->adversary[i].name = realloc(data_shared->current_comm->adversary[i].name, sizeof(char) * BUFFER_LIMIT);
				if(!data_shared->current_comm->adversary[i].name){
					data_shared->current_comm->adversary[i].name = malloc(sizeof(char) * SERVER_NAME);
				}
				strncpy(data_shared->current_comm->adversary[i].name, name, SERVER_NAME);
				break;
			}
		}
		SDL_AtomicUnlock(&thread_control->lock.comm);
		//Send add user to clients
		pointer-=2;
		snprintf(buffer, BUFFER_LIMIT, "ADD_USER\t%s", pointer);
		temp = 0;
		for(i = 0; i < MAX_CLIENT; i++){
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
	//printf("Message Handled\n");
}

int has_message_tcp(char *buffer, TCPsocket tcp_socket){
	int amount = 0;
	
	amount = SDLNet_CheckSockets(activity, 0);
	if (amount < 0) {
		printf("SDLNet_CheckSockets: %s\n", SDLNet_GetError());
		return -2;
	}
	else if (amount > 0) {
		printf("Has activity\n");
		if(SDLNet_SocketReady(tcp_socket)){
			if(SDLNet_TCP_Recv(tcp_socket, buffer, BUFFER_LIMIT) > 0){
				printf("Buffer received has |%s|\n", buffer);
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
			//printf("Has Buffer |%s|\n", buffer);
			handle_message(buffer, 0);
		}
		else if (has == -1){
            free(buffer);
			return 0;
		}
		else {
            free(buffer);
			return 1;
		}
	}
}

void process_action(){
	int i = 0;
	int lost_connection = 0;
	char buffer[BUFFER_LIMIT];
	SpawnMinion *minions = NULL;
	//Send message ready to play
	is_server = data_shared->current_user->is_server;

	//Send message status
	if(data_shared->current_user->process.message_status){
		SDL_AtomicLock(&thread_control->lock.user);
		data_shared->current_user->process.message_status--;
		snprintf(buffer, BUFFER_LIMIT, "USER_STATUS\t%c\t%c", (char) data_shared->current_user->id, (char) (data_shared->current_user->ready_to_play + 1));
		SDL_AtomicUnlock(&thread_control->lock.user);
			
		if(is_server){
			//This use the internal server message handle to process the message without need to send.
			handle_message(buffer, 1);
		}
		else {
			if(!send_message(buffer, 9, server_tcp_socket, 0)){
				SDL_AtomicLock(&thread_control->lock.comm);
				data_shared->current_comm->connection_lost = 1;
				SDL_AtomicUnlock(&thread_control->lock.comm);
			}
		}
	}
	//Send message life
	if(data_shared->current_user->process.message_life){
		SDL_AtomicLock(&thread_control->lock.user);
		data_shared->current_user->process.message_life--;
		snprintf(buffer, BUFFER_LIMIT, "USER_STATUS_LIFE\t%c\t%c", (char) data_shared->current_user->id, (char) (data_shared->current_user->life + 1));
		SDL_AtomicUnlock(&thread_control->lock.user);
		if(is_server){
			handle_message(buffer, 1);
		}
		else {
			if(send_message(buffer, 12, server_tcp_socket, 0) == 0){
				SDL_AtomicLock(&thread_control->lock.comm);
				data_shared->current_comm->connection_lost = 1;
				SDL_AtomicUnlock(&thread_control->lock.comm);
			}
		}
	}
	//Send message minion
	if(data_shared->current_user->process.message_minion){
		SDL_AtomicLock(&thread_control->lock.user);
		data_shared->current_user->process.message_minion--;
		if(data_shared->current_user->spawn_amount && data_shared->current_user->minions){
			minions = data_shared->current_user->minions;
			data_shared->current_user->minions = NULL;
			i = data_shared->current_user->spawn_amount;
			data_shared->current_user->spawn_amount = 0;
		}
		SDL_AtomicUnlock(&thread_control->lock.user);
		//Process minions.
		if(minions) {
			for(int j = 0; j < i; j++){
				SDL_AtomicLock(&thread_control->lock.user);
				snprintf(buffer, BUFFER_LIMIT, "USER_MINION\t%c\t%c\t%c", (char) data_shared->current_user->id, (char) minions[j].client_id, (char) minions[j].amount);
				for(int z = 0; z < minions[j].amount;z++) {
					snprintf(buffer, BUFFER_LIMIT, "%s\t%c", buffer, (char) minions[j].type[z]);
				}
				free(minions[j].type);
				SDL_AtomicUnlock(&thread_control->lock.user);
			
				//Send message to server
				if(is_server){
					handle_message(buffer, 1);
				}
				else {
					if(send_message(buffer, 8, server_tcp_socket, 0)){
						lost_connection = 1;
						break;
					}
				}
			}
			if(lost_connection){
				SDL_AtomicLock(&thread_control->lock.comm);
				data_shared->current_comm->connection_lost = 1;
				SDL_AtomicUnlock(&thread_control->lock.comm);
			}
			free(minions);
		}
	}
}

//Server runner
///////////////////////////////////////////////////////////////////////

void thread_check_messages_udp(void *data){
	thread_control->udp.alive = 1;
	int check, t1, t2;
	int delay = SERVER_USER_RESPONSE_DELAY;//Aprox. de 1000ms/60
	
	while(!thread_control->udp.terminate){
		t1 = SDL_GetTicks();
		
		SDL_AtomicLock(&thread_control->lock.comm);
		check = !game_in_progress && !data_shared->current_comm->server->connection_failed;
		SDL_AtomicUnlock(&thread_control->lock.comm);
		if(check){
			//Check UDP messages
			//printf("Checking UDP\n");
			check_messages_udp();
		}
		else {
			thread_control->udp.terminate = 1;
		}
		//FPS Handling
        t2 = SDL_GetTicks() - t1;
        if(t2 < delay){
            SDL_Delay(delay - t2);
        }
	}
	
	thread_control->udp.alive = 0;
	thread_control->udp.terminate = 0;
	return;
}

void run_server(void *data){
	int created_server = 0, thread_begin = 0, t1, t2;
	int delay = SERVER_USER_RESPONSE_DELAY;//Aprox. de 1000ms/60
	
	data_shared->current_user->is_server = 1;
	is_server = 1;
	game_in_progress = 0;
	game_ended = 0;

	//Initial setup 
	//------------------------------------
	main_server = malloc(sizeof(Host));
	
	created_server = establish_server(&main_server->ip);
	
	if(!created_server){
		//printf("Could not create server.\n");
		SDL_AtomicLock(&thread_control->lock.comm);
		data_shared->current_comm->server->connecting = 0;
		data_shared->current_comm->server->connection_failed = 1;
		SDL_AtomicUnlock(&thread_control->lock.comm);
		
		thread_control->server.terminate = 1;
	}
	
	//printf("Here 3b %d\n", thread_control->lock.comm);

	
	////printf("Here 4b\n");
	
	thread_control->udp.pointer = NULL;
	thread_control->udp.terminate = 0;
	thread_control->udp.alive = 0;
		
	//Create thread to UDP.
	thread_control->udp.pointer = SDL_CreateThread((SDL_ThreadFunction) thread_check_messages_udp, "udp", (void *)NULL);
	if (thread_control->udp.pointer == NULL) {
		//printf("UDP thread not created\n");
		thread_control->server.terminate = 1;
		SDL_AtomicLock(&thread_control->lock.comm);
		data_shared->current_comm->server->connection_failed = 1;
		SDL_AtomicUnlock(&thread_control->lock.comm);
	}
	
	//printf("Here 5b\n");
	
	
	while(!thread_control->server.terminate){
		t1 = SDL_GetTicks();
		SDL_AtomicLock(&thread_control->lock.comm);
		thread_begin = created_server && !data_shared->current_comm->match->finished && !data_shared->current_comm->server->connection_failed && !data_shared->current_comm->connection_lost;
		SDL_AtomicUnlock(&thread_control->lock.comm);
		
		if(thread_begin){
			//Dont allow anyone to connect if game is running.
			if(!game_in_progress) {
				//printf("Begin check TCP\n");
				if(!thread_control->server.terminate)
					check_connection_tcp();
			}
			
			if(!data_shared->current_comm->connection_lost && !thread_control->server.terminate){
				//printf("Begin game status\n");
				//Update game status
				game_status();
			}
			
			if(!data_shared->current_comm->connection_lost && !thread_control->server.terminate){
				//printf("Begin message handler\n");
				//Check TCP messages from clients connected. This also connect with a 
				check_messages_tcp();
			}
			
			if(!data_shared->current_comm->connection_lost && !thread_control->server.terminate){
				//printf("Begin game status again\n");
				//Update game status
				game_status();
			}
			
			//printf("Begin to process user action\n");
			//process player action.
			if(!data_shared->current_comm->connection_lost && !thread_control->server.terminate){
				process_action();
			}
			
		}
		//FPS Handling
        t2 = SDL_GetTicks() - t1;
        if(t2 < delay){
            SDL_Delay(delay - t2);
        }
	}
	
	//printf("Here 6b\n");
	
	thread_control->udp.terminate = 1;
	//Close UDP thread
	while(!thread_control->udp.alive){
		//printf("Thread UDP not finished\n");
	}
	thread_control->udp.pointer = NULL;
	
	//Close clients connection
	close_clients();
	//Close TCP connection.
	close_connection();
	
	//Free server
	if(main_server){
		//Free ip address
		//main_server->ip;
		free(main_server);
	}
	
	data_shared->current_user->is_server = 0;
	is_server = 0;
	SDL_AtomicLock(&thread_control->lock.control);
	thread_control->server.terminate = 0;
	thread_control->server.alive = 0;
	thread_control->server.pointer = NULL;
	SDL_AtomicUnlock(&thread_control->lock.control);
	return;
}

void run_client(void *data){	
	int found = 0;
	int connected = 0;
	int thread_begin = 0;
	int t1, t2;
	int delay = SERVER_USER_RESPONSE_DELAY;//Aprox. de 1000ms/60
	
	game_in_progress = 0;
	game_ended = 0;
	
	is_server = 0;
	
	if(data_shared->current_comm){
		SDL_AtomicLock(&thread_control->lock.comm);
		//Set searching network
		data_shared->current_comm->server->searching = 1;
		SDL_AtomicUnlock(&thread_control->lock.comm);
		//Search server
		found = find_servers();
		//printf("Servers found: %d\n",found);
		
		//Connect or choose server to connect.
		if(found == 0) {
			SDL_AtomicLock(&thread_control->lock.comm);
			data_shared->current_comm->server->searching = 0;
			data_shared->current_comm->server->searching_finished = 1;
			data_shared->current_comm->server->connecting = 0;
			data_shared->current_comm->server->search_result = 0;
			data_shared->current_comm->server->avaliable = 0;
			SDL_AtomicUnlock(&thread_control->lock.comm);
			//Not connect.
		}
		else {
			SDL_AtomicLock(&thread_control->lock.comm);
			data_shared->current_comm->server->searching = 0;
			data_shared->current_comm->server->searching_finished = 1;
			data_shared->current_comm->server->connecting = 0;
			data_shared->current_comm->server->search_result = found;//Same as avaliable?
			data_shared->current_comm->server->avaliable = found;//Maybe change to 1 and use for server avaliable or not.
			data_shared->current_comm->server->host = malloc(sizeof(Host) * found);
			data_shared->current_comm->server->host = get_host();
			
			data_shared->current_comm->server->choosing = 1;
			SDL_AtomicUnlock(&thread_control->lock.comm);
			
			//Need to choose server.
			while(data_shared->current_comm->server->choosing && !thread_control->client.terminate){
				t1 = SDL_GetTicks();
				//Wait until the user choose a server on main thread.
				//printf("Waiting user choose server\n");
				//FPS Handling
				t2 = SDL_GetTicks() - t1;
				if(t2 < delay && !thread_control->client.terminate){
					SDL_Delay(delay - t2);
				}
			}
			
			printf("Choosedddd!\n");
			
			if(!thread_control->client.terminate){
				SDL_AtomicLock(&thread_control->lock.comm);
				data_shared->current_comm->server->connecting = 1;
				SDL_AtomicUnlock(&thread_control->lock.comm);
				//Connect to selected server.
				connected = connect_to_server(data_shared->current_comm->server->choosed);
			}
		}
		
		if(!thread_control->client.terminate){
			printf("Not terminated\n");
			if(!connected) {
				//printf("Not able to connect with server.\n");
				SDL_AtomicLock(&thread_control->lock.comm);
				data_shared->current_comm->server->connecting = 0;
				data_shared->current_comm->server->connection_failed = 1;
				SDL_AtomicUnlock(&thread_control->lock.comm);
			}
			else {
				printf("Connected--\n");
				SDL_AtomicLock(&thread_control->lock.comm);
				data_shared->current_comm->server->connecting = 0;
				data_shared->current_comm->server->connected = 1;
				SDL_AtomicUnlock(&thread_control->lock.comm);

			}
			
			while(!thread_control->client.terminate){
				t1 = SDL_GetTicks();
				
				//Running 
				SDL_AtomicLock(&thread_control->lock.comm);
				thread_begin = connected && !data_shared->current_comm->match->finished && !data_shared->current_comm->server->connection_failed && !data_shared->current_comm->connection_lost;
				SDL_AtomicUnlock(&thread_control->lock.comm);
				
				if(thread_begin){
					////printf("Connected\n");
					//Check if there is response to begin game.
					//Check tcp messages only.
					if(!data_shared->current_comm->connection_lost){
						//printf("Begin checking TCP\n");
						check_messages_tcp();
					}
					
					if(!data_shared->current_comm->connection_lost){
						//printf("Begin checking game status\n");
						game_status();
					}
					
					//Process player action
					if(!data_shared->current_comm->connection_lost){
						process_action();
					}
				}
				//FPS Handling
				t2 = SDL_GetTicks() - t1;
				if(t2 < delay){
					SDL_Delay(delay - t2);
				}
			}
		}
	}
	
	//Destroy game communication
	remove_communication();
	printf("destroy\n");
	//printf("Thread exiting\n");
	SDL_AtomicLock(&thread_control->lock.control);
	thread_control->client.terminate = 0;
	thread_control->client.alive = 0;
	thread_control->client.pointer = NULL;
	SDL_AtomicUnlock(&thread_control->lock.control);
	printf("here end\n");
	return;
}


/**
This function was copied from deltheil on 
http://stackoverflow.com/questions/20019786/safe-and-portable-way-to-convert-a-char-to-uint16-t
*/
int str_to_uint16(const char *str, Uint16 *res){
	char *end;
	errno = 0;
	intmax_t val = strtoimax(str, &end, 10);
	if (errno == ERANGE || val < 0 || val > UINT16_MAX || end == str || *end != '\0')
		return 0;
	*res = (Uint16) val;
	return 1;
}
