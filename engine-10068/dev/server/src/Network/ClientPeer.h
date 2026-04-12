/********************************************************************
filename: 	ServerNetPlayer
file path:	H:\sandboxol\client\blockmango-client\dev\server\src\ServerNetPlayer.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/23
*********************************************************************/
#ifndef _SERVERPLAYER_H__
#define _SERVERPLAYER_H__

#pragma once

#include "Global.h"
#include "Server.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "World/Shop.h"
#include "Setting/GunSetting.h"
#include "Any/VarList.h"

class ClientPeer :public ObjectAlloc
{
public:
	enum PlayerState
	{
		emConnected,
		emLogined,
		emPlaying,
		emDisconnected
	};

	enum InterruptType
	{
		InterruptType_None,
		InterruptType_doPay,
		InterruptType_doBuyGoods,
		InterruptType_doManorPay,
	};

private:
	class OrderPayResult
	{
	public:
		i32 code = 0;
		String orderId = "";
		bool isConsume = true;
	};

public:
	ClientPeer(ui64 rakssid, const LORD::String& clientIp);
	virtual ~ClientPeer();

	static std::shared_ptr<ClientPeer> createPlayer(ui64 rakssid, const LORD::String& clientIp)
	{
		auto player = LORD::make_shared<ClientPeer>(rakssid, clientIp);
		return player;
	}

	void rebirth();

	void onTick();

	void removePlayerEntity();

	int getEntityRuntimeID();

	bool hasLogon() { return m_logon; }

	void setTeamId(int teamId) { m_teamId = teamId; }

	int getTeamId() { return m_teamId; }

	void setTeamName(String teamName) { m_teamName = teamName; }

	String getTeamName() { return m_teamName; }

	void setTeamInfo(int teamId, String teamName);

	bool isVisitor();

	void setClientReady(i32 uiStatus) { clientUiStatus = uiStatus; }

	ui64 getPlatformUserId() { return m_platformUserId; }

	ui64 getRakssid() { return m_rakSsid; }

	String getName() { return m_name; }

	String getLanguage() { return m_Language; };

	String getClientIp() { return m_clientIp; };

	void   setLanguage(String lan) { m_Language = lan; };

	void setModuleBlockData(String data) {
		updateModuleBlockData(data);
	};

	void   setShowName(String name, ui64 targetId = 0);

	String getShowName() { return m_showName; }

	void setRakssid(int rakssid) { m_rakSsid = rakssid; }

	void setPlayerLocation(const Vector3& pos, float yaw, float pitch);

	void setPlatformUserId(ui64 platformUserId);

	void setRequestIdFromClient(String requestId);

	void onSpawn(String name, EntityPlayerMP *mpPlayer);

	void login(String clientInfo, bool enableIndie, int gameTimeToday);

	void logout();

	void reconnect(std::shared_ptr<ClientPeer> clientPeer);

	void resetPos(float x, float y, float z, float yaw = 361);

	void movePlayer(float x, float y, float z, float yaw, float pitch, bool onGround, bool toMoving, bool rotating, bool fromServer = false);

	void doAttack(int targetEngityRuntimeId, float targetX, float targetY, float targetZ);

	void sendLoginResult(bool suc, int teamId, String teamName, int curPlayer, int maxPlayer);

	void doGunShootTarget(ui32 tracyType, ui32 hitEntityID, const PACKET_DETAIL::Vector3& hitpos,
		bool isHeadshot, const GunSetting* pGunSetting);

	void reloadBullet();

	void loadUserMoney();

	BLOCKMAN::EntityPlayerMP * getEntityPlayer() { return m_entityPlayer; }

	void buyGoods(i32 groupIndex, i32 goodsIndex, i32 goodsId);

	String getHttpUrl();

	bool isPioneer();

	bool clientUiIsReady() { return clientUiStatus == 1; }

	void buyRespawn(bool isRespawn, i32 uniqueId, i32 index);

	void buyChangeActor(bool isCanChangeActor, int changeActorCount, int needMoneyCount);

	void pickupItemPay(i32 itemId, i32 itemMeta, i32 addNum, i32 price, i32 entityItemId, i32 currencyType);

	bool checkParam(String url, i32 currencyType, i32 price);

	void upgradeManor();

	void buyHouse(i32 houseId);

	void buyFurniture(i32 furnitureId);

	void consumeDiamonds(i32 uniqueId, i32 diamonds, String remark, bool isConsume);

	void consumeGolds(i32 uniqueId, i32 golds, String remark, bool isConsume);

	void buyRanchItem(i32 itemId, i32 addNum, i32 totalPrice, i32 currencyType);

	void buyRanchBuildItem(i32 itemId, i32 addNum, i32 type, i32 currencyType);

	void setRespawnPos(Vector3i position) { respawnPos = position; };

	void ranchShortcut(i32 landCode, i32 currencyType, i32 totalPrice);

	void buyRanchItems(vector<RanchCommon>::type items);

	void ranchBuildQueueOperation(i32 entityId, i32 type, i32 queueId, i32 productId);

	void speedUpCrop(Vector3i blockPos, i32 blockId, i32 totalPrice);

	void useCubeFinshOrder(i32 orderId, i32 index);

	void buyQuicklyBuildBlock(i32 cost);

	void updateUserSelectableAction();

	void tryAskVehicle(int entityId, bool onOrOff, bool isAdReward = false);

	void doCacheRequest();

	String getLogString() const;

public:
	Vector3i getRespawnPos() { return respawnPos; }
	std::map<std::string, std::string> getModuleBlockData() {
		return m_mapModuleBlockData;
	};
	void stopLaserGun();
	void setNolongerRemind(bool bNolongerRemind) { m_bNolongerRemind = bNolongerRemind; };
	bool getNolongerRemind() { return m_bNolongerRemind; };
	bool checkDiamondBlues(int price);
	String getPackageName() { return m_packageName; }
	String getVersionCode() { return m_versionCode; }

private:
	bool payResult(i64 platformUserId, String result, bool isSuccess);
	OrderPayResult payResultByConsume(i64 platformUserId, bool isSuccess , bool isConsume ,String result);
	void updateModuleBlockData(String data);
	bool resetLogicSpeedUpCheatIfNeed(float moveLenSqr, bool onGround, bool fromServer);
	bool checkLogicSpeedUpCheat(float moveLenSqr, bool onGround, bool fromServer);
	void doConsumeDiamonds(i32 uniqueId, i32 diamonds, String remark, bool isConsume, String url);
	bool resetInTheAirCheatIfNeed(bool onGround, bool fromServer);
	bool checkInTheAirCheat(bool onGround, bool fromServer);
	bool inTheAirCheatIgnoreState();
	void saveCacheRequest(InterruptType interruptType, VarList& args);
	void doPickupItemPay(i32 itemId, i32 itemMeta, i32 addNum, i32 price, i32 entityItemId, i32 currencyType);
	void doPay(i32 currencyType, i32 uniqueId, i32 price, std::function<void(i64, String, bool)> fun, bool skipRemind);
	void doBuyGoods(i32 uniqueId, i32 coinId, i32 price, std::function<void(i64, String, bool)> fun);
	void doBuyRespawn(i32 uniqueId, i32 price, std::function<void(i64, String, bool)> fun);
	void doBuyChangeActor(i32 uniqueId, i32 price, std::function<void(i64, String, bool)> fun);
	void doManorPay(String webPayUrl, String dataServerPayUrl, i32 currencyType, i32 uniqueId, i32 price, ui64 platformUserId, std::function<void(String, bool)> fun);
	void checkUserRelation();

private:
	bool m_logon = false;
	int m_teamId = 0;
	ui64 m_rakSsid = 0;
	ui64 m_platformUserId = 0;
	String m_requestIdFromClient = "";
	String m_name = "noname";
	String m_showName = "noshowname";
	String m_teamName = "noteamname";
	PlayerState m_state = PlayerState::emDisconnected;
	String m_Language = "en_US";
	std::map<std::string, std::string> m_mapModuleBlockData;
	BLOCKMAN::EntityPlayerMP * m_entityPlayer = nullptr;
	i64 currentTicks = 0;
	int keepAliveRandomID = 0;
	i32 clientUiStatus = 0;
	i64 keepAliveTimeSent = 0;
	i64 ticksOfLastKeepAlive = 0;
	int chatSpamThresholdCount = 0;
	int creativeItemCreationSpamThresholdTally = 0;
	Vector3 lastPos{0,0,0};
	bool hasMoved = false;
	Vector3i respawnPos;
	bool m_bNolongerRemind = true;
	LORD::String m_clientIp;
	VarList m_requestCache{};
	//std::function<void(i64, String, bool)> m_requestFunc{};
	int m_inTheAirCnt = 0;
	ui32 m_inTheAirBeginTime = 0;
	InterruptType m_interruptType{ InterruptType_None }; //�ж�����
	int m_longMoveCnt = 0;
	ui32 m_longMoveBeginTime = 0;
	String m_packageName = "com.sandboxol.blockymods";
	String m_versionCode = "1";
};

#endif