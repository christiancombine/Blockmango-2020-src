#pragma once
#include "Core.h"
#include "game.h"
#include "Util/PlayerPrefs.h"
#include "Setting/MultiLanTipSetting.h"
#include "ShellInterface.h"
#include "cWorld/Blockman.h"
#include "cEntity/PlayerControlller.h"
#include "Setting/FunctionSetting.h"
#include "Audio/SoundSystem.h"

using namespace LORD;

struct HostApi
{
	static void log(const char *log, int level = 0)
	{
		if (level == 0)
			LOG(G3LOG_DEBUG) << log;
		if (level == 1)
			LOG(G3LOG_WARNING) << log;
		if (level == 2)
			LOG(G3LOG_ERROR) << log;
	}

	static void sendMsg(ui8 type, i32 time, const char* msg)
	{
		switch ((ChatComponentNotification::Type)type)
		{
		case ChatComponentNotification::Type::NATURE:
			ChatMessageReceiveEvent::emit(msg, false);
			break;
		case ChatComponentNotification::Type::TIMING_TOP:
			TopSystemNotificationEvent::emit(msg, time);
			break;
		case ChatComponentNotification::Type::TIMING_BOTTOM:
			BottomSystemNotificationEvent::emit(msg, time);
			break;
		case ChatComponentNotification::Type::TIMING_CENTER:
			CenterSystemNotificationEvent::emit(msg, time);
			break;
		case ChatComponentNotification::Type::TEAM_RESOURCES_NOTIFICATION:
			TeamResourcesShowNotificationEvent::emit(msg, time);
			break;
		}
	}

	static void sendMsgByType(ui8 type, i32 time, int msgType, const char* args)
	{
		String lanType = LanguageManager::Instance()->getCurrentLanguage();
		String msgContext = LanguageManager::Instance()->getString(args);
		String msg = MultiLanTipSetting::getMessage(lanType, msgType, msgContext.c_str()).c_str();
		sendMsg(type, time, msg.c_str());
	}
	
	static void dataReport(String main, String child)
	{
		GameClient::CGame::Instance()->getShellInterface()->dataReport(main, child);
	}

	static void onRecharge(int type)
	{
		GameClient::CGame::Instance()->getShellInterface()->onRecharge(type);
	}

	static void putBoolForKey(String key, bool value)
	{
		GameClient::CGame::Instance()->getShellInterface()->putBoolForKey(key, value);
	}

	static bool getBoolForKey(String key, bool defaultValue)
	{
		return GameClient::CGame::Instance()->getShellInterface()->getBoolForKey(key, defaultValue);
	}

	static void putStringForKey(String key, String value)
	{
		GameClient::CGame::Instance()->getShellInterface()->putStringForKey(key, value);
	}

	static String getStringForKey(String key, String defaultValue)
	{
		return GameClient::CGame::Instance()->getShellInterface()->getStringForKey(key, defaultValue);
	}

	static void onWatchAd(int type, String params)
	{
		GameClient::CGame::Instance()->getShellInterface()->onWatchAd(type, params);
	}

	static void reportAdVideoShow()
	{
		GameClient::CGame::Instance()->getShellInterface()->reportAdVideoShow();
	}

	static String getClientInfo()
	{
		return GameClient::CGame::Instance()->getShellInterface()->getClientInfo();
	}

	static void onAppActionTrigger(int type, String params)
	{
		GameClient::CGame::Instance()->getShellInterface()->onAppActionTrigger(type, params);
	}

	static void putBoolPrefs(String key, bool value)
	{
		PlayerPrefs::Instance()->putBoolPrefs(key, value);
	}
	
	static bool getBoolPrefs(String key)
	{
		return PlayerPrefs::Instance()->getBoolPrefs(key);
	}

	static void putIntPrefs(String key, int value)
	{
		PlayerPrefs::Instance()->putIntPrefs(key, value);
	}

	static int getIntPrefs(String key)
	{
		return PlayerPrefs::Instance()->getIntPrefs(key.c_str());
	}

	static void putFloatPrefs(String key, float value)
	{
		PlayerPrefs::Instance()->putFloatPrefs(key, value);
	}

	static float getFloatPrefs(String key)
	{
		return PlayerPrefs::Instance()->getFloatPrefs(key);
	}

	static void putStringPrefs(String key, String value)
	{
		PlayerPrefs::Instance()->putStringPrefs(key, value);
	}

	static String getStringPrefs(String key)
	{
		return PlayerPrefs::Instance()->getStringPrefs(key);
	}

	static void showNotification(const String& content)
	{
		ReceiveNotificationEvent::emit(1, content);
	}

	static void setClientAction(EntityPlayer* pPlayer, int holdItemId, int animateType, int actionState)
	{
		NULL_RETURN_VOID(pPlayer);

		pPlayer->m_holdItemId = holdItemId != 0 ? holdItemId : pPlayer->m_holdItemId;

		switch (animateType)
		{
		case NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_ATTACK:
			pPlayer->swingItem();
			break;

		case NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_BASE_ACTION:
			pPlayer->setBaseAction(static_cast<ActionState>(actionState));
			break;

		case NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_UPPER_ACTION:
			pPlayer->setUpperAction(static_cast<ActionState>(actionState));
			break;

		default:
			LordLogError("recv animate packet with unknown animate type %d\n", static_cast<int>(animateType));
			return;
		}
	}

	static void addFunctionConfig(const String & key, bool value)
	{
		FunctionSetting::Instance()->add(key, value);
	}

	static void notifyGetMoney(int count)
	{
		LORD::SoundSystem::Instance()->playEffectByType(ST_GetCoin);
		BLOCKMAN::GetMoneyEvent::emit(count);
	}

	static void notifyGetItem(int itemId, int meta, int count)
	{
		LORD::SoundSystem::Instance()->playEffectByType(ST_GetItem);
		BLOCKMAN::GetItemEvent::emit(itemId, meta, count);
	}

	static void notifyGetGoods(String icon, int count)
	{
		LORD::SoundSystem::Instance()->playEffectByType(ST_GetItem);
		BLOCKMAN::GetGoodsEvent::emit(icon, count);
	}

	static void followEnterGame(LORD::ui64 targetId, const LORD::String& gameType)
	{
		GameClient::CGame::Instance()->getShellInterface()->followEnterGame(targetId, gameType);
	}

};