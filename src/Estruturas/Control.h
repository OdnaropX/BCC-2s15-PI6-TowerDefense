#ifndef __Tower_Defense__Estruturas__Control__
	#define __Tower_Defense__Estruturas__Control__

	//Enum
	///////////////////////////////////////////////////////////////////////
	
	typedef enum _screens {MAIN, CONFIG, GAME_RUNNING, GAME_PAUSED, CREDITS, SCORE, END_GAME, GAME_MULTIPLAY_SERVER, MINIONS_INFO, MULTIPLAYER_CHOOSE_ROOM, MULTIPLAYER_ROOM} screen;
	typedef enum _tab_location{TOP_MENU, LEFT_MENU, BOTTOM_MENU, GAME_AREA} TAB;
	typedef enum _main_options {OPT_PLAY, OPT_MULTIPLAYER, OPT_CONFIG, OPT_SCORE, OPT_EXIT, OPT_CREDIT, OPT_NONE} main_options;
	typedef enum _pause_options {OPT_P_RESUME, OPT_P_CONFIG, OPT_P_SCORE, OPT_P_EXIT, OPT_P_MAIN, OPT_P_CREDITS, OPT_P_NONE} pause_options;
	typedef enum _config_options{AUDIO_SFX, AUDIO_MUSIC, LANGUAGE, BACK, NONE} config_options;
	typedef enum _end_game_options{EG_NEW_GAME, EG_MAIN, EG_QUIT, EG_NONE} end_game_options;

	typedef enum _multiplayer_menu_options{MP_CREATE_ROOM, MP_SEARCH_ROOM, MP_BACK_TO_MAIN, MP_START, MP_TOGGLE_READY, MP_LEAVE, MP_ROOM_1, MP_ROOM_2, MP_ROOM_3, MP_ROOM_4, MP_NONE} multiplayer_menu_options;
	typedef enum _multiplayer_status{MPS_WAIT_FOR_PLAYER, MPS_SEARCHING_ROOM, MPS_WAIT_READY, MPS_ENTERED_ROOM, MPS_CAN_START, MPS_STARTED_GAME, MPS_NONE} multiplayer_status;
    typedef enum _end_game_status{EGS_WIN, EGS_LOSE, EGS_DC, EGS_OPLEFT, EGS_NONE}end_game_status;

	typedef enum _running_top_option {OPT_R_T_PAUSE, OPT_R_T_RESUME, OPT_R_T_NONE} RUNNING_TOP_OPTIONS;
	typedef enum _running_left_option {OPT_R_L_GOLD, OPT_R_L_MANA, OPT_R_L_LIFE, OPT_R_L_NONE} RUNNING_LEFT_OPTIONS;
	typedef enum _running_area_right_option {OPT_R_A_R_MINION_1, OPT_R_A_R_MINION_2, OPT_R_A_R_MINION_3, OPT_R_A_R_NONE} RUNNING_AREA_RIGHT_OPTIONS;
	typedef enum _running_area_left_option {OPT_R_A_L_TOWER_1, OPT_R_A_L_TOWER_2, OPT_R_A_L_TOWER_3, OPT_R_A_L_NONE} RUNNING_AREA_LEFT_OPTIONS;

	
	//Structs
	///////////////////////////////////////////////////////////////////////
	
	typedef struct _select_game_running_inner GAME_AREA_OPTIONS;
	typedef struct _select_game_running GAME_RUNNING_OPTIONS;
	
	struct _select_game_running_inner {
		RUNNING_AREA_LEFT_OPTIONS left;
		RUNNING_AREA_RIGHT_OPTIONS right;
	};
	
	struct _select_game_running {
		TAB current_tab;
		RUNNING_TOP_OPTIONS top;
		RUNNING_LEFT_OPTIONS left;
		GAME_AREA_OPTIONS game_area;
	};
	
	
#endif