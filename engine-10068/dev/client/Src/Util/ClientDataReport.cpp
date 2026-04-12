#include "ClientDataReport.h"



namespace BLOCKMAN {
	const String ClientDataReport::ENTER_GAME_STATUS = "enter_game_status";
	const String ClientDataReport::ENTER_GAME_SUCCESS = "enter_game_success";
	const String ClientDataReport::QUIT_GAME = "quit_game";
	const String ClientDataReport::GAME_ERROR_CODE = "game_error_code";
	const String ClientDataReport::GAME_DISCONNECT = "game_disconnect";
	const String ClientDataReport::CLICK_ACT_BUTTON = "click_act_button";
	const String ClientDataReport::SEND_ACT_SUCCESS = "send_act_success";
	const String ClientDataReport::GAME_RETRY_CONNECT = "game_retry_connect";
	const String ClientDataReport::DOWNLOAD_MAP_FAIL = "download_map_fail";

	const String ClientDataReport::GAME_FRIEND_ADD = "game_friend_add";
	const String ClientDataReport::GAME_FRIEND_AGREE = "game_friend_agree";
	const String ClientDataReport::GAME_FRIEND_IGNORE = "game_friend_ignore";
	
	const String ClientDataReport::ENTER_GAME_TIME = "enter_game_time";

	const String ClientDataReport::ERROR_CODE_ENTER_GAME_SUCCESS = "0";
	const String ClientDataReport::ERROR_CODE_VALIDATION_FAILS = "1";
	const String ClientDataReport::ERROR_CODE_VALIDATION_TIMEOUT = "2";
	const String ClientDataReport::ERROR_CODE_MAP_DOWNLOAD_FAILS = "3";
	const String ClientDataReport::ERROR_CODE_NETWORK_TIMEOUT = "4";
	const String ClientDataReport::ERROR_CODE_NETWORK_CONNECT_ERROR = "5";
	const String ClientDataReport::ERROR_CODE_NETWORK_CONNECT_FAILS = "6";
	const String ClientDataReport::ERROR_CODE_GAME_OVER = "7";
	const String ClientDataReport::ERROR_CODE_BE_KICKED = "8";
	const String ClientDataReport::ERROR_CODE_UNKNOWN = "9";
	const String ClientDataReport::ERROR_CODE_ENGINE_VERSION_VALIDATION_FAILS = "10";
	const String ClientDataReport::ERROR_CODE_ENGINE_VERSION_VALIDATION_TIMEOUT = "11";
	const String ClientDataReport::ERROR_CODE_ROOM_FULL = "12";
	const String ClientDataReport::ERROR_CODE_GAME_START = "13";
	const String ClientDataReport::ERROR_CODE_GAME_STOP = "14";
	const String ClientDataReport::ERROR_CODE_ENGINE_GET_USER_ATTR_FAIL = "15";
	const String ClientDataReport::ERROR_CODE_NOT_RECV_GAME_INFO = "16";
	const String ClientDataReport::ERROR_CODE_CONNECTION_LOST = "17";
	const String ClientDataReport::ERROR_CODE_SERVER_LOAD_DB_ERROR = "18";
	const String ClientDataReport::ERROR_CODE_ENGINE_WAIT_ROLE_ATTR_FAIL = "19";
	const String ClientDataReport::ERROR_CODE_ROOM_FULL_RETRY = "20";
	const String ClientDataReport::ERROR_CODE_GAME_START_RETRY = "21";
	const String ClientDataReport::ERROR_CODE_SPEED_UP_CHEAT_DETECTED = "22";
	const String ClientDataReport::ERROR_CODE_SLOW_DOWN_CHEAT_DETECTED = "23";
	const String ClientDataReport::ERROR_CODE_IN_THE_AIR_CHEAT_DETECTED = "24";
	const String ClientDataReport::ERROR_CODE_LOGIC_SPEED_UP_CHEAT_DETECTED = "25";
	const String ClientDataReport::ERROR_CODE_CLIENT_ON_GROUND_CHEAT_DETECTED = "26";
	const String ClientDataReport::ERROR_CODE_ALLOCATION_GAME_MAP_FAILURE = "27";
	const String ClientDataReport::ERROR_CODE_ALLOCATION_GAME_FAILURE = "28";
	const String ClientDataReport::ERROR_CODE_SERVER_QUITTING = "29";
	const String ClientDataReport::ERROR_CODE_KICK_OUT_DUPLICATE_ENTER = "30";
	const String ClientDataReport::ERROR_CODE_KICK_OUT_TIMEOUT_ENTER = "31";
	const String ClientDataReport::ERROR_CODE_KICK_OUT_MANUAL_KICK = "32";
	const String ClientDataReport::ERROR_CODE_ATTACK_PLAYER_CHEAT_DETECTED = "33";

	const String ClientDataReport::GAME_DISCONNECT_NORMAL	= "1";
	const String ClientDataReport::GAME_DISCONNECT_TIMEOUT	= "2";
	const String ClientDataReport::GAME_DISCONNECT_UNKNOWN  = "3";

	const String ClientDataReport::GAME_RETRY_CONNECT_START = "1";
	const String ClientDataReport::GAME_RETRY_CONNECT_FAILS = "2";


	const String ClientDataReport::GAME_PROCESS_EVENT_DOWNLOAD_MAP = "new_game_download_map_%s";
	const String ClientDataReport::GAME_PROCESS_EVENT_UNZIP_MAP = "new_game_release_%s";
	const String ClientDataReport::GAME_PROCESS_EVENT_MD5_CHECK = "new_game_MD5_%s";
	const String ClientDataReport::GAME_PROCESS_EVENT_CONNECT_DISPATCH = "new_connect_dispatch_success_%s";
	const String ClientDataReport::GAME_PROCESS_EVENT_CONNECT_SERVER = "new_connect_%s_server_success";
	const String ClientDataReport::GAME_PROCESS_EVENT_ENTER_SUCCESS = "new_game_enter_success_%s";
}
