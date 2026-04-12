#ifndef __S2C_COMMON_DATA_PACKET_HANDLES_H__
#define __S2C_COMMON_DATA_PACKET_HANDLES_H__

#include <memory>
#include "Network/protocol/S2CPackets.h"

class ClientCommonDataHelper
{
public:
	void init();
};

class S2CCommonDataPacketHandles
{
public:
	static void handlePacket(std::shared_ptr<S2CPacketServerCommonData>& packet);

	static void onLuaCommonEvent(ui64 userId, String& name, String& key, String& data);
	static void onDispatchInfo(ui64 userId, String& name, String& key, String& data);

	static void onGameAnalyticsData(ui64 userId, String& name, String& key, String& data);
	static void onGetShopData(ui64 userId, String& name, String& key, String& data);
	static void onGetAppShopData(ui64 userId, String& name, String& key, String& data);
	static void onGetVideoAdTipData(ui64 userId, String& name, String& key, String& data);
	static void onGetMainControlData(ui64 userId, String& name, String& key, String& data);
	static void onGetToolBarData(ui64 userId, String& name, String& key, String& data);
    static void onOpenMovingEnderChest(ui64 userId, String& name, String& key, String& data);
	static void onGetPlayerInfoData(ui64 userId, String& name, String& key, String& data);

	static void onGetBedWarData(ui64 userId, String& name, String& key, String& data);
	static void onGetTycoonData(ui64 userId, String& name, String& key, String& data);
	static void onGetBedWarHallData(ui64 userId, String& name, String& key, String& data);
	static void onGetPixelGunHallData(ui64 userId, String& name, String& key, String& data);
	static void onGetPixelGunGameData(ui64 userId, String& name, String& key, String& data);

	static void onBlockCityStorageOperation(ui64 userId, String& name, String& key, String& data);
	static void onPlayingPraiseAction(ui64 userId, String& name, String& key, String& data);
	static void onSyncBlockCityCDRewardData(ui64 userId, String& name, String& key, String& data);
	static void onGetBlockCityData(ui64 userId, String& name, String& key, String& data);
	static void onGetBirdSimulatorData(ui64 userId, String& name, String& key, String& data);
	static void onGetShellInterfaceDataReport(ui64 userId, String& name, String& key, String& data);
	static void onGetBlockCityTigerAdShowUI(ui64 userId, String& name, String& key, String& data);
	//static void onShowDiamondBlueLackTips(ui64 userId, String& name, String& key, String& data);
    static void onGetPixelAdData(ui64 userId, String& name, String& key, String& data);
	static void onGetSkyBlockMiningAreaData(ui64 userId, String& name, String& key, String& data);
	static void onGetSkyBlockProductData(ui64 userId, String& name, String& key, String& data);
	static void onGetSkyBlockMainData(ui64 userId, String& name, String& key, String& data);
	static void onGetTipDialogData(ui64 userId, String& name, String& key, String& data);
	static void onDoHurtMsg(ui64 userId, String& name, String& key, String &data);
	static void onDoAttackMsg(ui64 userId, String& name, String& key, String& data);
    static void onSyncChestInfoMsg(ui64 userId, String& name, String& key, String& data);
	static void onSyncDiePlayerMsg(ui64 userId, String& name, String& key, String& data);
	static void onGetWalkingDeadGameData(ui64 userId, String& name, String& key, String& data);
	static void onBlockCityToGetOnCarOrNotRace(ui64 userId, String& name, String& key, String& data);
	static void onBlockCityRaceVehicleResulr(ui64 userId, String name, String key, String data);
	static void onShowBlockCityRaceFailMsg(ui64 uiserId, String& name, String& key, String& data);
	static void onGetBlockData(ui64 userId, String& name, String& key, String& data);
	static void onGetSkyWarLuckyBlockData(ui64 userId, String& name, String& key, String& data);
};

#endif
