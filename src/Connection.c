#include "Connection.h"

server_register servers[MAX_SERVER];
TCPsocket tcp;
SDLNet_SocketSet activity;
int connected_server = -1;

//Allocation Functions
///////////////////////////////////////////////////////////////////////

game_comm *init_communication() {
	game_comm *game = malloc(sizeof(game_comm));
	
	return game;
}

void remove_communication(game_comm *comm){
	if (comm) {
		
		
		
		free(comm);
	}
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
		if(name) {
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

UDPsocket check_network_avaliable(){
	UDPsocket udp_socket = NULL; 
	//Port 0 because cannot open UDP socket to server if server is the same machine, so 0 is auto assigned.
	udp_socket = SDLNet_UDP_Open(0);
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
	
	udp_socket = check_network_avaliable();
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
		
		sent = SDLNet_UDP_Send(udp_socket, output_package->channel, output_package);
		if (!sent) {
			printf("Broadcast failed!\nTrying Localhost...\n");
			
			sent = SDLNet_UDP_Send(udp_socket, output_package_local->channel, output_package_local);
			
			if (!sent) {
				printf("Something really, really, I mean really wrong happened and Localhost was not accessed.\nThis was not suppost to happen.\n");
				//return 0;
			}	
		}
		//Delay to give server chance to respond.
		SDL_Delay(30);
		
		while(SDLNet_UDP_Recv(udp_socket, input_package))
        {
            if(strncmp((char*)input_package->data, "GRADE_DEFENDER_SERVER", strlen("GRADE_DEFENDER_SERVER")) == 0)
            {
                trying = 0;
                //add to list, checking for duplicates
                number_found = update_list_servers(input_package);
            }
        }

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
		
			//Will try more 4 times to see se found more servers.
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
	}
	printf("Search server done!\n");

    SDLNet_FreePacket(output_package); 
    SDLNet_FreePacket(output_package_local); 
    SDLNet_FreePacket(input_package); 
    SDLNet_UDP_Close(udp_socket); 
    return number_found;
}

/*
void encrypt(){
	
}

void decrypt(){
	
}

void send_private_key(){
	
}

void send_public_key(){
	
}

void send_minion(){
	/* Data to send:
	MinionType
	PLAYER SENDER ID
	
}

void response_minion(){
	/*
	Minion Alives Number
	
}
void send_alive(){
	/* Data to send:
	ALIVE
	PLAYER SENDER ID
	
}

void response_alive(){
	/* Data:
	Ok 
}

void send_lives(){
	/*
	Live Number
	PLAYER SENDER ID
	
}

void send_request(){
	/*
	PLAYER SENDER NAME
	
	
}

void send_(){}

*/



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
}

int connect_to_server(int server_choice){
	if(server_choice < 0 || server_choice > MAX_SERVER) {
		return 0;
	}
	
	if (!(tcp = SDLNet_TCP_Open(&servers[server_choice].ip))){
		//Add () to remove warning when assining variable inside boolean.
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

//Server runner
///////////////////////////////////////////////////////////////////////

void run_server(void *data){
	game_comm *game_communication = (game_comm *) data;
	
	
	printf("hererere %d\n", terminate_thread);
	while(!terminate_thread){
		printf("Test server\n");
	}

	return;
}


void run_client(void *data){
	game_comm *game_communication = (game_comm *) data;
	
	int found = 0;
	int connected = 0;
	
	//Search server
	found = find_servers();
	
	if(found == 0) {
		game_communication->server_found = found;
	}
	else if(found == 1) {
		game_communication->server_found = 1;
	}
	else {
		game_communication->server_found = 1;
		game_communication->server_choose = 1;
		
		//Need to choose server.
		while(game_communication->server_choose == -1 && !terminate_thread){
			//Wait until the user choose a server on main thread.
			SDL_Delay(10);
		}
		//Connect to selected server.
		connected = connect_to_server(game_communication->server_choose);
		if (!connected){
			printf("Not able to connect with server.\n");
			//Set not connect status and terminate 
			game_communication->server_connected = 0;
			game_communication->server_choose = 0;
		}
	}
	
	while(!terminate_thread && found > 0){
		printf("Test client\n");
	}
	return;
}
