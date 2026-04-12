#pragma once
#ifndef __SERVER_COMMON_DATA_SENDER_H__
#define __SERVER_COMMON_DATA_SENDER_H__

#include "Core.h"

using namespace LORD;

class ServerCommonDataSender
{
public:
	ServerCommonDataSender() {}
	~ServerCommonDataSender() {}

	void sendServerCommonData(ui64 rakssId, String name = "", String key = "", String data = "");
	void broadCastServerCommonData(String name = "", String key = "", String data = "");
	void sendServerCommonDataToTrackingPlayers(int entityId, String name = "", String key = "", String data = "");

	void sendBlockCityStorageOperation(ui64 rakssId, i32 hotbarIndex, i32 itemId, i32 meta, bool isEditMode);
	void sendPlayingPraiseAction(ui64 rakssId, i32 entityId, int duration);
	void sendOpenMovingEnderChest(ui64 rakssId);
};
#endif