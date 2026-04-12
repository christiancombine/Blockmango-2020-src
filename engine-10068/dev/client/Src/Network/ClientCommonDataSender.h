#pragma once
#ifndef __CLIENT_COMMON_DATA_SENDER_H__
#define __CLIENT_COMMON_DATA_SENDER_H__

#include "Core.h"

using namespace LORD;

class ClientCommonDataSender
{
public:
	ClientCommonDataSender() {}
	~ClientCommonDataSender() {}

	void sendClientCommonData(String name = "", String key = "", String data = "");
	void sendLuaCommonData(String key = "", String data = "");

	void sendBlockCityOpenTigerLottery(i32 entityId);
	void sendBlockCitySelectTigerLottery(String uniqueId);
	void sendBlockCityModOpenLimit(i32 openType);
	void sendInteractionWithDecoration(i32 entityId, String node);
	void sendBlockCityOperation(i32 operationType, i32 flyId = 0);
	void sendBlockCityBuyGoods(i32 tabId, i32 goodsId, i32 buyType, i32 moneyType);
	void sendBlockCityChangeEditMode(bool isEditMode);
	void sendBlockCityStorageOperation(bool isAdd, i32 hotbarIndex, i32 itemId, i32 meta, bool isEditMode);
	void sendPlayerOperation(i32 type, ui64 userId, String url, bool isFriend = false, i32 from = 0);
	//type: 1 entity , 2 template ,id: entityId or templateId, direction: 1 left 2 right
	void sendRotateOperation(i32 type, i32 id, i32 direction);
	void sendBlockCityBuyLackingItems(i32 buyType, i32 moneyType);
	void sendBlockCityManorBuyOperation(i32 operationType, i32 moneyType);
	void sendBlockCityChooseProp(i32 guideId, i32 propId);
	void sendBlockCityGetFreeFly();
	void sendBlockCityHouseShopBuyGoods(i32 shopId, i32 goodsId, i32 moneyType, i32 index);
	void sendBlockCityChangeToolStatus(i32 classifyId, i32 itemId, i32 index);
	void sendMoveElevator(i32 floorId);
	void sendBlockCityTeleportToShop();
	void sendBlockCityOperateArchiveData(i32 index, i32 type);
	void sendBlockCitylUnlockNewArchive();
	void sendBlockCityChangeArchiveName(i32 index, String name);
	void sendBlockCityTossBack(i64 targetId);
	void sendReceiveChickenWeapon();


	void sendGameOperation(i32 operationType);
	void sendBlockFortOperation(i32 operationType, i32 id, i32 index);
	//type: 1 buy 2 sell
	void sendGameTrade(i32 id, i32 num = 1, i32 type = 1);
	void sendCloseVideoAdTip(int type, String params, bool isClickSure);
	void sendRefreshProps();

	void sendPixelGunReceiveTaskActiveReward(int activeId);
	void sendPixelGunReceiveTaskReward(int taskId);
	void sendPixelGunFirstShowStoreGun(int gunId);
	void sendPixelGunUseStoreGun(int gunId);
	void sendPixelGunBuyStoreGun(int gunId);
	void sendPixelGunUpgradeStoreGun(int gunId, bool useTicket);
	void sendPixelGunQuickUpgradeStoreGun(int gunId);
	void sendPixelGunFirstShowStoreProp(int propId);
	void sendPixelGunUseStoreProp(int propId);
	void sendPixelGunUnloadStoreProp(int propId);
	void sendPixelGunBuyStoreProp(int propId);
	void sendPixelGunUpgradeStoreProp(int propId);
	void sendPixelGunQuickUpgradeStoreProp(int propId);
	void sendPixelGunFirstShowStoreModule(int moduleId);
	void sendPixelGunUseStoreModule(int moduleId);
	void sendPixelGunUnloadStoreModule(int moduleId);
	void sendPixelGunBuyStoreModule(int moduleId);
	void sendPixelGunUnlockMap(int mapId);
	void sendPixelGunOpenLotteryChest(int chestId);
	void sendPixelGunUpgradeArmor(int operateType);
	void sendPixelGunReceiveSignIn(int signInId);
	void sendPixelGunBuyGiftBag(int giftId);
	void sendPixelGunRefreshGiftBag();
	void sendPixelGunStartGame(String gameType, String mapName);
	void sendPixelGunSelect1v1Box(int index);
	void sendPixelGunBackHall();
	void sendPixelGunPlayAgain();
	void sendPixelGun1v1Revenge();

	void sendBedWarBuyGood(int goodId);
	void sendBedWarOpenRune();
	void sendBedWarChangeRune(int troughId, int curRuneId, int targetRuneId);
	void sendBedWarOneKeyUnloadRune(int troughType);
	void sendBedWarOneKeyEquipRune(int troughType);
	void sendBedWarOneKeyReclaimRune(int runeType);
	void sendBedWarReclaimRune(int runeId);
	void sendBedWarUnlockRune(int runeType);

	void sendBlockCityPortal(int uId, int id);
	void sendBlockCityToRaceOrNot(ui64 targetUserId, bool isAgree = true);

	void sendClickBuyUpdate(int type, int id);
	void sendWatchAdUpdateInfo(int id, int level);
	void sendSkyBlockBuyGoods(int num, int indexId, int type);
	void sendSkyBlockReceiveTaskReward(int taskId, int status);
	void sendSellItem(i32 selectIndex, i32 itemId, i32 itemMeta, i32 num);
	void sendClickResourceIsland();
	void sendClickExtendArea();
	void sendSkyBlockAddHome();
	void sendSkyBlockBackHome();
	void sendSkyBlockReceiveSignInReward(int rewardId);
	void sendSkyBlockDareTaskOperate(i32 type, bool has_ad);
	void sendSkyBlocReciveNewPri();

	void sendWalkingDeadBuyStoreGoods(int goodsid);
	void sendWalkingDeadBuyStoreSupply(int supplyId);
	void sendWalkingDeadDesignationChange(int titleId, int titleStatus);
	void sendAbandonItem(i32 selectIndex, i32 num);
    void sendOpenMovingEnderChest();
};
#endif