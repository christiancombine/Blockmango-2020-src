#pragma once

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN {

	class ClientDataReport
	{

	public: 
		const static String ENTER_GAME_STATUS;
		const static String ENTER_GAME_SUCCESS;
		const static String QUIT_GAME;
		const static String GAME_ERROR_CODE;
		const static String GAME_DISCONNECT;
		const static String CLICK_ACT_BUTTON;
		const static String SEND_ACT_SUCCESS;
		const static String GAME_RETRY_CONNECT;
		const static String DOWNLOAD_MAP_FAIL;
		const static String GAME_FRIEND_ADD;
		const static String GAME_FRIEND_AGREE;
		const static String GAME_FRIEND_IGNORE;
		const static String ENTER_GAME_TIME;

		const static String ERROR_CODE_VALIDATION_FAILS;
		const static String ERROR_CODE_VALIDATION_TIMEOUT;
		const static String ERROR_CODE_MAP_DOWNLOAD_FAILS;
		const static String ERROR_CODE_NETWORK_TIMEOUT;
		const static String ERROR_CODE_NETWORK_CONNECT_ERROR;
		const static String ERROR_CODE_NETWORK_CONNECT_FAILS;
		const static String ERROR_CODE_GAME_OVER;
		const static String ERROR_CODE_ENTER_GAME_SUCCESS;
		const static String ERROR_CODE_BE_KICKED;
		const static String ERROR_CODE_UNKNOWN;
		const static String ERROR_CODE_ENGINE_VERSION_VALIDATION_FAILS;
		const static String ERROR_CODE_ENGINE_VERSION_VALIDATION_TIMEOUT;
		const static String ERROR_CODE_ROOM_FULL;
		const static String ERROR_CODE_GAME_START;
		const static String ERROR_CODE_GAME_STOP;
		const static String ERROR_CODE_ENGINE_GET_USER_ATTR_FAIL;
		const static String ERROR_CODE_NOT_RECV_GAME_INFO;
		const static String ERROR_CODE_CONNECTION_LOST;
		const static String ERROR_CODE_SERVER_LOAD_DB_ERROR;
		const static String ERROR_CODE_ENGINE_WAIT_ROLE_ATTR_FAIL;
		const static String ERROR_CODE_ROOM_FULL_RETRY;
		const static String ERROR_CODE_GAME_START_RETRY;
		const static String ERROR_CODE_SPEED_UP_CHEAT_DETECTED;
		const static String ERROR_CODE_SLOW_DOWN_CHEAT_DETECTED;
		const static String ERROR_CODE_IN_THE_AIR_CHEAT_DETECTED;
		const static String ERROR_CODE_LOGIC_SPEED_UP_CHEAT_DETECTED;
		const static String ERROR_CODE_CLIENT_ON_GROUND_CHEAT_DETECTED;
		const static String ERROR_CODE_ALLOCATION_GAME_MAP_FAILURE;
		const static String ERROR_CODE_ALLOCATION_GAME_FAILURE;
		const static String ERROR_CODE_SERVER_QUITTING;
		const static String ERROR_CODE_KICK_OUT_DUPLICATE_ENTER;
		const static String ERROR_CODE_KICK_OUT_TIMEOUT_ENTER;
		const static String ERROR_CODE_KICK_OUT_MANUAL_KICK;
		const static String ERROR_CODE_ATTACK_PLAYER_CHEAT_DETECTED;

		const static String GAME_DISCONNECT_NORMAL;
		const static String GAME_DISCONNECT_TIMEOUT;
		const static String GAME_DISCONNECT_UNKNOWN;


		const static String GAME_RETRY_CONNECT_START;
		const static String GAME_RETRY_CONNECT_FAILS;

		const static String GAME_PROCESS_EVENT_DOWNLOAD_MAP;
		const static String GAME_PROCESS_EVENT_UNZIP_MAP;
		const static String GAME_PROCESS_EVENT_MD5_CHECK;
		const static String GAME_PROCESS_EVENT_CONNECT_DISPATCH;
		const static String GAME_PROCESS_EVENT_CONNECT_SERVER;
		const static String GAME_PROCESS_EVENT_ENTER_SUCCESS;
	};

}
