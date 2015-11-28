#include "Connection.h"

Host servers[MAX_SERVER];
TCPsocket tcp;

UDPsocket server_udp_socket;       /* Socket descriptor */
UDPpacket server_udp_package;
	

TCPsocket server_tcp_socket = NULL;
TCPsocket server_ip = NULL;
	
SDLNet_SocketSet activity = NULL;//Used to TPC and UDP.

Host *main_server = NULL;

int connected_server = -1;
int connected_clients = 0;
int game_in_progress = 0;
char server_name[SERVER_NAME];

//Allocation Functions
///////////////////////////////////////////////////////////////////////

game_comm *init_communication(char *name) {
	game_comm *game = malloc(sizeof(game_comm));
	
	game->game_can_start = 0;
	game->game_finished = 0;
	game->players_left = 0;
	game->connection_lost = 0;
	
	game->server = malloc(sizeof(Server));
	game->server->searching = 0;
	game->server->searching_finished = 0;
	game->server->connecting = 0;
	game->server->connection_failed = 0;
	game->server->connected = 0;
	game->server->choosing = 0;
	game->server->choosed = -1;
	game->server->search_result = 0;
	game->server->avaliable = 0;
	game->server->host = NULL;
	
	game->current_player = malloc(sizeof(player_comm));
	game->current_player->tcp_socket = NULL;
	game->current_player->exited_game = 0;
	game->current_player->connection_lost = 0;
	game->current_player->ready_to_play = 0;
	
	game->current_player->info = malloc(sizeof(Player));
	game->current_player->info->life = DEFAULT_PLAYERS_LIFE;
	game->current_player->info->winner = 0;
	game->current_player->info->minions_type_sent = NULL;
    game->current_player->activity = NULL;
	if(name){
		game->current_player->info->name = name;
	}
	else {
		game->current_player->info->name = malloc(sizeof(char) * SERVER_NAME);
		strncpy(game->current_player->info->name, "Unknown", SERVER_NAME);
	}
	
	game->players = malloc(sizeof(player_comm*) * MAX_CLIENT);
	for(int i = 0; i < MAX_CLIENT; i++) {
		game->players[i] = malloc(sizeof(player_comm));
		
		game->players[i]->tcp_socket = NULL;
		game->players[i]->exited_game = 0;
		game->players[i]->connection_lost = 0;
		game->players[i]->ready_to_play = 0;
		
		game->players[i]->info = malloc(sizeof(Player));
		game->players[i]->info->life = DEFAULT_PLAYERS_LIFE;
		game->players[i]->info->winner = 0;
		game->players[i]->info->minions_type_sent = NULL;
		printf("herea1aa\n");
		game->players[i]->info->name = malloc(sizeof(char) * SERVER_NAME);
		strncpy(game->players[i]->info->name, "No name", SERVER_NAME);
		printf("herea1aa\n");
        game->players[i]->activity = NULL;
	}
	printf("hereaaa\n");
	return game;
}

void remove_player(player_comm *player, int remove_name){
	if(player){
		if(player->info){
			if(remove_name && player->info->name){
				free(player->info->name);
				player->info->name = NULL;
			}
			if(player->info->minions_type_sent){
				free(player->info->minions_type_sent);
				player->info->minions_type_sent = NULL;
			}
			free(player->info);
			player->info = NULL;
		}
		if(player->activity){
			SDLNet_FreeSocketSet(player->activity);
			player->activity = NULL;
		}
		//player->ip
		if(player->tcp_socket) {
			SDLNet_TCP_Close(player->tcp_socket);
			player->tcp_socket = NULL;
		}
		free(player);
		player = NULL;
	}
	return;
}

void remove_communication(game_comm *comm){
	printf("Removing communication\n");
	if (comm) {
		for(int i = 0; i < MAX_CLIENT; i++) {
			remove_player(comm->players[i], 1);
		}
		if(comm->players) {
			free(comm->players);
			comm->players = NULL;
		}
		if(comm->current_player){
			remove_player(comm->current_player, 1);
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


void remove_client(game_comm *game_communication, int client){
	int i;
	char buffer[2];
	
	for(i=0; i <MAX_CLIENT; i++) {
        if(i != client && game_communication->players[i]->tcp_socket) {
			sprintf(buffer, "%d\0", i);
			//Send message to other players that user left.
			send_message(buffer, 2, game_communication->players[i]->tcp_socket);
            //SDLNet_TCP_Send(slave_thread[thread_id_no].client[j].sock, buffer, strlen(buffer) + 1);
        }
    }

    SDLNet_TCP_DelSocket(activity, game_communication->players[client]->tcp_socket);

    if(game_communication->players[client]->tcp_socket != NULL)
        SDLNet_TCP_Close(game_communication->players[client]->tcp_socket);

	game_communication->players[client]->tcp_socket = NULL;
	game_communication->players[client]->ready_to_play = 0;  
	if(game_communication->players[client]->info->name) {
		free(game_communication->players[client]->info->name);
		game_communication->players[client]->info->name = NULL;
	}
	
	//Change player status to left.
	
	
	
}

//Update Functions
///////////////////////////////////////////////////////////////////////
	
int update_list_servers(UDPpacket* package){
	int already_add = 0;
	char *name = NULL;
	int i;
	
	//Check if already on list.
	for(i=0; i < MAX_SERVER && servers[i].ip.host != 0;i++) {
		if(package->address.host == servers[i].ip.host){
			already_add = 1;
			break;
		}
	}
	
	if(!already_add && i < MAX_SERVER) {
		servers[i].ip.host = package->address.host;
		servers[i].ip.port = package->address.port;
		
		name = strchr((char*)package->data, '\t');//Data is Uint8, so need cast.
		if(++name) {
			 strncpy(servers[i].name, name, SERVER_NAME);
			 //Remove \t
			 name = strchr(servers[i].name, '\t');
			 if(name) {
				  *name = '\0';
			 }
			 //Remove \n
			 name = strchr(servers[i].name, '\n');
			 if(name) {
				  *name = '\0';
			 }
		}
		//Add this to next list number.
		i++;
	}
	return i;
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
	int t1, t2;
	t1 = 0;
	
	//Port 0 because cannot open UDP socket to server if server is the same machine, so 0 is auto assigned.
	udp_socket = establish_udp(0);
    if(!udp_socket){
		return 0;
	}
	
	//Alloc memory.
	output_package = SDLNet_AllocPacket(BUFFER_LIMIT);
    output_package_local = SDLNet_AllocPacket(BUFFER_LIMIT);
    input_package = SDLNet_AllocPacket(BUFFER_LIMIT);
	
	//Mount package. 255.255.255.255 will broadcast to the entire LAN.
	//IP address is detect by the resolver from SDL.
    SDLNet_ResolveHost(&ip, "255.255.255.255", DEFAULT_PORT);
    output_package->address.host = ip.host;
    output_package->address.port = ip.port;
    sprintf((char*) output_package->data, "GRADE_DEFENDER_CLIENT");
    output_package->len = strlen("GRADE_DEFENDER_CLIENT") + 1;

	//Package for localhost. This will be used on internal tests.
    SDLNet_ResolveHost(&ip, "255.255.255.255", DEFAULT_PORT);
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
			
			//sent = SDLNet_UDP_Send(udp_socket, output_package_local->channel, output_package_local);
			sent = SDLNet_UDP_Send(udp_socket, -1, output_package_local);
			
			if (!sent) {
				printf("Something really, really, I mean really wrong happened and Localhost was not accessed.\nThis was not suppost to happen.\n");
				return 0;
			}	
		}
		//Delay to give server chance to respond.
		SDL_Delay(30);
		
		printf("Here\n");
		while(SDLNet_UDP_Recv(udp_socket, input_package))
        {
            if(strncmp((char*)input_package->data, "GRADE_DEFENDER_SERVER", strlen("GRADE_DEFENDER_SERVER")) == 0)
            {
                trying = 0;
                //add to list, checking for duplicates
                number_found = update_list_servers(input_package);
            }
        }
		printf("Here 2\n");

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
			else if(attempts > 15) {
				trying = 0;
			}
		}
		else {
			trying = 0;
		}
		printf("Here 3\n");
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
	/* First create TCP socket that will be used to connect	*/
	
	//Null is to listen
	if(SDLNet_ResolveHost(ip, NULL, DEFAULT_PORT) < 0){
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
        return 0;
	}
	
	//Start UDP socket to listen for clients.
	server_udp_socket = establish_udp(DEFAULT_PORT);
	if (!server_udp_socket){
		return 0;
	}
	
	return 1;
}

void check_messages_udp(){
	int received = 0;
	int sent = 0;
	char buffer[BUFFER_LIMIT];
	
	UDPpacket* input = NULL;
	
	if(!server_udp_socket){
		printf("Error: Socket server_udp_socket is NULL.\n");
		return;
	}	
	
	input = SDLNet_AllocPacket(BUFFER_LIMIT);
	received = SDLNet_UDP_Recv(server_udp_socket, input);//Message will not be greater than buffer. 
	
	if(received > 0 && received <= BUFFER_LIMIT){
		if(strncmp((char*)input->data, "GRADE_DEFENDER_CLIENT", strlen("GRADE_DEFENDER_CLIENT")) == 0){
			//Send message to client.
			UDPpacket* output;
			output = SDLNet_AllocPacket(BUFFER_LIMIT);
			snprintf(buffer, BUFFER_LIMIT, "%s\t%s", "GRADE_DEFENDER_SERVER", server_name);
			snprintf((char *)output->data, BUFFER_LIMIT, "%s", buffer);
			output->len = strlen(buffer) + 1;
			output->address.host = input->address.host;
            output->address.port = input->address.port;
			sent = SDLNet_UDP_Send(server_udp_socket, -1, output);
            SDLNet_FreePacket(output);
		}
	}
	SDLNet_FreePacket(input);
	return;
}

void check_connection_tcp(game_comm *game_communication){
	TCPsocket socket = NULL; 
	
	int sockets = 0;
	char buffer[BUFFER_LIMIT];
	int i;
	
	//Check pending connection. This is non-blocking
	socket = SDLNet_TCP_Accept(server_tcp_socket);
	if(!socket){
		return;
	}
	
	//Check if there is slot avaliable
	for(i = 0; i < MAX_CLIENT && game_communication->players[i]->tcp_socket;i++);
	if (i == MAX_CLIENT) {
		//Inform client that connection cannot be made.
		send_message("Maximum number of clients connected.", 1, socket);
		return;
	}
	
	//Update game status
	game_status_and_clients(game_communication);
	
	//Check if game is in progress
	if(game_in_progress) {
		//Informe client that game in progress
		send_message("Game already started without you.", 1, socket);
		return;
	}
	//Now make new connection. This is non-blocking.
	//Add to client list
	game_communication->players[i]->tcp_socket = socket;
	
	//Add client socket to set 
	sockets = SDLNet_TCP_AddSocket(activity, game_communication->players[i]->tcp_socket);
	if(sockets == -1) {
		fprintf(stderr, "SDLNet_AddSocket: %s\n", SDLNet_GetError());
		terminate_server();
		return;
	}
    
	connected_clients = sockets;
	//Send message to client informing of connection made with success
	return;
}

void check_messages_tcp(game_comm *game_communication){
	int action, i;
	int clients_ready = 0;
	char buffer[BUFFER_LIMIT];
	
	i = action = 0;
	
	action = SDLNet_CheckSockets(activity, 0);
	
	if(action == -1){
		printf("SDLNet_CheckSockets: %s\n", SDLNet_GetError());
	}
	else if(action){
		for(i = 0; i < MAX_CLIENT; i++){
			if(game_communication->players[i]->tcp_socket != NULL && SDLNet_SocketReady(game_communication->players[i]->tcp_socket)){ 
                clients_ready++;

                if (SDLNet_TCP_Recv(game_communication->players[i]->tcp_socket, buffer, BUFFER_LIMIT) > 0){
                    if(game_in_progress){
                        handle_message(i, buffer, 0);
                    }
                    else{
                        handle_message(i, buffer, 1);
                    }
                    // See if game is ended because everyone has left:
                    game_status_and_clients(game_communication); 
                }
                else {
                    remove_client(game_communication, i);
                }
            }
		}
		game_status_and_clients(game_communication); 
		
		if(action > clients_ready){
			printf("Warning: SDLNet_CheckSockets() reported %d active sockets,\nbut only %d detected by SDLNet_SocketReady()\n", action, clients_ready);
		}
	}
	return;
}


/**
Based on Tuxmath (by  Akash Gangil, David Bruce) check game clients function.
*/
void game_status_and_clients(game_comm *game_communication){
	int i;

	if(game_in_progress){
		int someone_still_playing = 0;
		for(i = 0; i < MAX_CLIENT; i++){
			if(game_communication->players[i]->tcp_socket != NULL && game_communication->players[i]->ready_to_play){
                someone_still_playing = 1;
                break;
            }
		}
		
		if(!someone_still_playing){
			for(i = 0; i < MAX_CLIENT; i++){
				//Check if all clients are closed.
				SDLNet_TCP_Close(game_communication->players[i]->tcp_socket);
				game_communication->players[i]->tcp_socket = NULL;
				game_communication->players[i]->ready_to_play = 0;
			}
			game_in_progress = 0;
			//Finish game
			finish_game(game_communication);
		}
	}
	else {
		int someone_connected = 0;
		int someone_not_ready = 0;
        for(i = 0; i < MAX_CLIENT; i++){
            if(game_communication->players[i]->tcp_socket != NULL){ 
                someone_connected = 1;
                if (!game_communication->players[i]->ready_to_play){
                    someone_not_ready = 1;
                }
            }
        }
        if(someone_connected && !someone_not_ready){
			begin_game(game_communication); 
		}
	}
}

//Connection Functions
///////////////////////////////////////////////////////////////////////

void close_connection(){
	if (tcp) {
		SDLNet_TCP_Close(tcp);
        tcp = NULL;
	}
	if (activity) {
		SDLNet_FreeSocketSet(activity);
        activity = NULL;
	}
	return;
}

int connect_to_server(int server_choice){
	if(server_choice < 0 || server_choice > MAX_SERVER) {
		return 0;
	}
	
	tcp = SDLNet_TCP_Open(&servers[server_choice].ip);
	
	if (!tcp){
        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return 0;
    }

    activity = SDLNet_AllocSocketSet(1);
    if(!activity){
        printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        return 0;
    }

	//Add a socket to a socket set that will be watched
    if(SDLNet_TCP_AddSocket(activity, tcp) == -1){
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

Host *get_host(int i){
	return &servers[i];
}

//Server runner
///////////////////////////////////////////////////////////////////////


int send_message(char *message, int message_type, TCPsocket socket){
	
	switch(message_type){
		case 0:
			break;
		case 1:
		
			break;
		case 2:
		
			break;
		
	}
	return 0;
}

void terminate_server(){
	
}


void begin_game(game_comm * game_communication){
	int sent, i;
	
	for(i = 0; i < MAX_CLIENT; i++){
        // Only check sockets that aren't null:
        if(game_communication->players[i]->ready_to_play != 1 && game_communication->players[i]->tcp_socket != NULL){
            return;      
        }
    }
	connected_clients = 0;
	
	
	for(i = 0; i < MAX_CLIENT; i++){
        if(game_communication->players[i]->ready_to_play == 1 && game_communication->players[i]->tcp_socket != NULL){
            sent = send_message("START_GAME", 1, game_communication->players[i]->tcp_socket);
			//(SDLNet_TCP_Send(slave_thread[thread_id_no].client[j].sock, buf, NET_BUF_LEN) == NET_BUF_LEN)
            if(sent) {
				connected_clients++;
			}
            else {
                remove_client(game_communication, i);
            }
        }
    }
	if (connected_clients == 0){
		game_communication->players_left = 1;
		return;
	}
	
	game_in_progress = 1;
	
	//Initialize data
	
	
	//Send info to all clients
	
	
	
}

void finish_game(game_comm *game_communication){
	int alive;
	int i;
	//Transmit message to all
	for(i = 0; i < MAX_CLIENT;i++){
		alive = 0;
		alive = send_message("GAME_STOPED", 0, game_communication->players[i]->tcp_socket);
		if(!alive){
			remove_client(game_communication, i);
		}
	}
	for(i = 0; i < MAX_CLIENT; i++){
		if( game_communication->players[i]->tcp_socket) {
			SDLNet_TCP_Close(game_communication->players[i]->tcp_socket);
			 game_communication->players[i]->tcp_socket = NULL;
			  game_communication->players[i]->ready_to_play = 0;
		}
    }
	
	game_in_progress = 0;
}




void handle_message(int client_id, char* buffer, int type){
	
}

void run_server(void *data){
	int created_server = 0;
	game_comm *game_communication = (game_comm *) data;

	//Initial setup 
	//------------------------------------
	main_server = malloc(sizeof(Host));
	
	created_server = establish_server(&main_server->ip);
	if(!created_server){
		printf("Could not create server.\n");
		terminate_thread = 1;
	}

	while(!terminate_thread){
		//Check UDP messages
		check_messages_udp(game_communication);
		//Check connection with client 
		check_connection_tcp(game_communication);
		//Check TCP messages from clients connected. This also connect with a 
		check_messages_tcp(game_communication);
		
		//Handle game events
		
		
		
		
		//Delay server
		
		printf("Connected %d\n", connected_clients);
		
		
	}
	
	
	//Free server
	if(main_server){
		//Free ip address
		
		main_server->ip;
		free(main_server);
	}
	return;
}


void run_client(void *data){
	game_comm *game_communication = (game_comm *) data;
	
	int found = 0;
	int connected = 0;
	
	//Set searching network
	game_communication->server->searching = 1;
	
	//Search server
	found = find_servers();
	
	//Connect or choose server to connect.
	if(found == 0) {
		game_communication->server->searching = 0;
		game_communication->server->searching_finished = 1;
		game_communication->server->connecting = 0;
		game_communication->server->search_result = 0;
		game_communication->server->avaliable = 0;

		//Not connect.
		printf("No server found");
	}
	else if(found == 1) {
		game_communication->server->searching = 0;
		game_communication->server->searching_finished = 1;
		game_communication->server->connecting = 1;
		game_communication->server->search_result = 1;//Same as avaliable?
		game_communication->server->avaliable = 1;
		game_communication->server->host = malloc(sizeof(Host *));
		game_communication->server->host[0] = get_host(0);
		
		//Connect to this server.
		connected = connect_to_server(0);
	}
	else {
		game_communication->server->searching = 0;
		game_communication->server->searching_finished = 1;
		game_communication->server->connecting = 0;
		game_communication->server->search_result = found;//Same as avaliable?
		game_communication->server->avaliable = found;//Maybe change to 1 and use for server avaliable or not.
		game_communication->server->host = malloc(sizeof(Host *) * found);
		for(int i; i < found; i ++){
			game_communication->server->host[i] = get_host(i);
		}
		
		game_communication->server->choosing = 1;
		
		//Need to choose server.
		while(game_communication->server->choosing && !terminate_thread){
			//Wait until the user choose a server on main thread.
			SDL_Delay(SERVER_USER_RESPONSE_DELAY);
		}

		game_communication->server->connecting = 1;
		
		//Connect to selected server.
		connected = connect_to_server(game_communication->server->choosed);
	}
	
	if(!connected) {
		printf("Not able to connect with server.\n");
		game_communication->server->connecting = 0;
		game_communication->server->connection_failed = 1;
	}
	else {
		game_communication->server->connecting = 0;
		game_communication->server->connected = 1;
	}
	printf("hereq\n");
	while(!terminate_thread){
		//Running 
		if(connected){
			printf("Connected\n");
			
			//Check if there is response to begin game.
			//Check tcp messages only.
			
			//Check if connection was not lost.
			//Get server 
			
			//Send message to server that can start game
			
			
			//Send message to server that exit game
			
			
			
			
		}
	}
	printf("Destroctor\n");
	//Destroy game communication
	remove_communication(game_communication);
	return;
}

void kill_thread(SDL_Thread *thread){
	//Kill thread.
	terminate_thread = 1;
	SDL_DetachThread(thread);
	thread = NULL;
}
