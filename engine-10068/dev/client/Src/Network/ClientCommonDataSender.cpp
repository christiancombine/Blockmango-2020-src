#include "ClientCommonDataSender.h"
#include "Network/protocol/C2SPackets.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "Network/ClientNetwork.h"

using namespace LORD;

void ClientCommonDataSender::sendClientCommonData(String name, String key, String data)
{
	auto packet = LORD::make_shared<C2SPacketClientCommonData>();
	packet->name = name;
	packet->key = key;
	packet->data = data;
	ClientNetwork::Instance()->sendPacket(packet);
}

void ClientCommonDataSender::sendLuaCommonData(String key, String data)
{
	sendClientCommonData("Lua", key, data);
}

void ClientCommonDataSender::sendBlockCityOpenTigerLottery(i32 entityId)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto s_entityId = dataCache->getServerId(entityId);
	String data = CommonDataBuilder().addIntParam("entityId", s_entityId).getData();
	sendLuaCommonData("BlockCityOpenTigerLottery", data);
}

void ClientCommonDataSender::sendBlockCitySelectTigerLottery(String uniqueId)
{
	String data = CommonDataBuilder().addStringParam("uniqueId", uniqueId).getData();
	sendLuaCommonData("BlockCitySelectTigerLottery", data);
}

void ClientCommonDataSender::sendBlockCityModOpenLimit(i32 openType)
{
	String data = CommonDataBuilder().addIntParam("openType", openType).getData();
	sendLuaCommonData("BlockCityModOpenLimit", data);
}

void ClientCommonDataSender::sendInteractionWithDecoration(i32 entityId, String node)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto s_entityId = dataCache->getServerId(entityId);
	String data = CommonDataBuilder().addIntParam("entityId", s_entityId)
		.addStringParam("node", node).getData();
	sendClientCommonData("InteractionWithDecoration", "", data);
}

void ClientCommonDataSender::sendBlockCityOperation(i32 operationType, i32 flyId)
{
	String data = CommonDataBuilder().addIntParam("operationType", operationType)
		.addIntParam("flyId", flyId).getData();
	sendLuaCommonData("BlockCityOperation", data);
}

void ClientCommonDataSender::sendBlockCityBuyGoods(i32 tabId, i32 goodsId, i32 buyType, i32 moneyType)
{
	String data = CommonDataBuilder().addIntParam("tabId", tabId)
		.addIntParam("goodsId", goodsId)
		.addIntParam("buyType", buyType)
		.addIntParam("moneyType", moneyType).getData();
	sendLuaCommonData("BlockCityBuyGoods", data);
}

void ClientCommonDataSender::sendBlockCityChangeEditMode(bool isEditMode)
{
	String data = CommonDataBuilder().addBoolParam("isEditMode", isEditMode).getData();
	sendLuaCommonData("BlockCityChangeEditMode", data);
}

void ClientCommonDataSender::sendBlockCityStorageOperation(bool isAdd, i32 hotbarIndex, i32 itemId, i32 meta, bool isEditMode)
{
	String data = CommonDataBuilder().addBoolParam("isAdd", isAdd)
		.addIntParam("hotbarIndex", hotbarIndex)
		.addIntParam("itemId", itemId)
		.addIntParam("meta", meta)
		.addBoolParam("isEditMode", isEditMode).getData();
	sendClientCommonData("BlockCityStorageOperation", "", data);
}

void ClientCommonDataSender::sendPlayerOperation(i32 type, ui64 userId, String url, bool isFriend, i32 from)
{
	String data = CommonDataBuilder().addIntParam("type", type)
		.addI64Param("userId", (i64)userId)
		.addStringParam("url", url)
		.addBoolParam("isFriend", isFriend)
		.addIntParam("from", from).getData();
	sendClientCommonData("PlayerOperation", "", data);
}

void ClientCommonDataSender::sendRotateOperation(i32 type, i32 id, i32 direction)
{
	String data = CommonDataBuilder()
		.addIntParam("type", type)
		.addIntParam("id", id)
		.addIntParam("direction", direction).getData();
	sendLuaCommonData("RotateOperation", data);
}

void ClientCommonDataSender::sendBlockCityBuyLackingItems(i32 buyType, i32 moneyType)
{
	String data = CommonDataBuilder().addIntParam("buyType", buyType).addIntParam("moneyType", moneyType).getData();
	sendLuaCommonData("BlockCityBuyLackingItems", data);
}

void ClientCommonDataSender::sendBlockCityManorBuyOperation(i32 operationType, i32 moneyType)
{
	String data = CommonDataBuilder().addIntParam("operationType", operationType).addIntParam("moneyType", moneyType).getData();
	sendLuaCommonData("BlockCityManorBuyOperation", data);
}

void ClientCommonDataSender::sendBlockCityChooseProp(i32 guideId, i32 propId)
{
	String data = CommonDataBuilder().addIntParam("guideId", guideId).addIntParam("propId", propId).getData();
	sendLuaCommonData("BlockCityChooseProp", data);
}

void ClientCommonDataSender::sendBlockCityGetFreeFly()
{
	sendLuaCommonData("BlockCityGetFreeFly");
}

void ClientCommonDataSender::sendBlockCityHouseShopBuyGoods(i32 shopId, i32 goodsId, i32 moneyType, i32 index)
{
	String data = CommonDataBuilder()
		.addIntParam("shopId", shopId)
		.addIntParam("goodsId", goodsId)
		.addIntParam("moneyType", moneyType)
		.addIntParam("index", index).getData();
	sendLuaCommonData("BlockCityHouseShopBuyGoods", data);
}

void ClientCommonDataSender::sendBlockCityChangeToolStatus(i32 classifyId, i32 itemId, i32 index)
{
	String data = CommonDataBuilder()
		.addIntParam("classifyId", classifyId)
		.addIntParam("itemId", itemId)
		.addIntParam("index", index).getData();
	sendLuaCommonData("BlockCityChangeToolStatus", data);
}

void ClientCommonDataSender::sendMoveElevator(i32 floorId)
{
	String data = CommonDataBuilder().addIntParam("floorId", floorId).getData();
	sendLuaCommonData("MoveElevator", data);
}

void ClientCommonDataSender::sendBlockCityTeleportToShop()
{
	sendLuaCommonData("BlockCityTeleportToShop");
}

void ClientCommonDataSender::sendBlockCityOperateArchiveData(i32 index, i32 type)
{
	String data = CommonDataBuilder().addIntParam("index", index).addIntParam("type", type).getData();
	sendLuaCommonData("BlockCityOperateArchiveData", data);
}

void ClientCommonDataSender::sendBlockCitylUnlockNewArchive()
{
	sendLuaCommonData("BlockCityUnlockNewArchive");
}

void ClientCommonDataSender::sendBlockCityChangeArchiveName(i32 index, String name)
{
	String data = CommonDataBuilder()
		.addIntParam("index", index)
		.addStringParam("name", name).getData();
	sendLuaCommonData("BlockCityChangeArchiveName", data);
}

void ClientCommonDataSender::sendBlockCityTossBack(i64 targetId)
{
	String data = CommonDataBuilder().addI64Param("targetId", targetId).getData();
	sendLuaCommonData("BlockCityTossBack", data);
}

void ClientCommonDataSender::sendGameOperation(i32 operationType)
{
	String data = CommonDataBuilder().addIntParam("operationType", operationType).getData();
	sendLuaCommonData("GameOperation", data);
}

void ClientCommonDataSender::sendBlockFortOperation(i32 operationType, i32 id, i32 index)
{
	String data = CommonDataBuilder()
		.addIntParam("operationType", operationType)
		.addIntParam("id", id)
		.addIntParam("index", index).getData();
	sendLuaCommonData("BlockFortOperation", data);
}

void ClientCommonDataSender::sendGameTrade(i32 id, i32 num, i32 type)
{
	String data = CommonDataBuilder()
		.addIntParam("id", id)
		.addIntParam("num", num)
		.addIntParam("type", type).getData();
	sendLuaCommonData("GameTrade", data);
}

void ClientCommonDataSender::sendCloseVideoAdTip(int type, String params, bool isClickSure)
{
	String data = CommonDataBuilder()
		.addIntParam("Type", type)
		.addStringParam("Params", params)
		.addBoolParam("IsClickSure", isClickSure)
		.getData();
	sendLuaCommonData("CloseVideoAdTip", data);
}

void ClientCommonDataSender::sendRefreshProps()
{
	sendLuaCommonData("RefreshProps");
}

void ClientCommonDataSender::sendPixelGunReceiveTaskActiveReward(int activeId)
{
	String data = CommonDataBuilder().addIntParam("activeId", activeId).getData();
	sendLuaCommonData("ReceiveTaskActiveReward", data);
}

void ClientCommonDataSender::sendPixelGunReceiveTaskReward(int taskId)
{
	String data = CommonDataBuilder().addIntParam("taskId", taskId).getData();
	sendLuaCommonData("ReceiveTaskReward", data);
}

void ClientCommonDataSender::sendPixelGunFirstShowStoreGun(int gunId)
{
	String data = CommonDataBuilder().addIntParam("gunId", gunId).getData();
	sendLuaCommonData("FirstShowStoreGun", data);
}

void ClientCommonDataSender::sendPixelGunUseStoreGun(int gunId)
{
	String data = CommonDataBuilder().addIntParam("gunId", gunId).getData();
	sendLuaCommonData("UseStoreGun", data);
}

void ClientCommonDataSender::sendPixelGunBuyStoreGun(int gunId)
{
	String data = CommonDataBuilder().addIntParam("gunId", gunId).getData();
	sendLuaCommonData("BuyStoreGun", data);
}

void ClientCommonDataSender::sendPixelGunUpgradeStoreGun(int gunId, bool useTicket)
{
	String data = CommonDataBuilder()
		.addIntParam("gunId", gunId)
		.addBoolParam("useTicket", useTicket)
		.getData();
	sendLuaCommonData("UpgradeStoreGun", data);
}

void ClientCommonDataSender::sendPixelGunQuickUpgradeStoreGun(int gunId)
{
	String data = CommonDataBuilder().addIntParam("gunId", gunId).getData();
	sendLuaCommonData("QuickUpgradeStoreGun", data);
}

void ClientCommonDataSender::sendPixelGunFirstShowStoreProp(int propId)
{
	String data = CommonDataBuilder().addIntParam("propId", propId).getData();
	sendLuaCommonData("FirstShowStoreProp", data);
}

void ClientCommonDataSender::sendPixelGunUseStoreProp(int propId)
{
	String data = CommonDataBuilder().addIntParam("propId", propId).getData();
	sendLuaCommonData("UseStoreProp", data);
}

void ClientCommonDataSender::sendPixelGunUnloadStoreProp(int propId)
{
	String data = CommonDataBuilder().addIntParam("propId", propId).getData();
	sendLuaCommonData("UnloadStoreProp", data);
}

void ClientCommonDataSender::sendPixelGunBuyStoreProp(int propId)
{
	String data = CommonDataBuilder().addIntParam("propId", propId).getData();
	sendLuaCommonData("BuyStoreProp", data);
}

void ClientCommonDataSender::sendPixelGunUpgradeStoreProp(int propId)
{
	String data = CommonDataBuilder().addIntParam("propId", propId).getData();
	sendLuaCommonData("UpgradeStoreProp", data);
}

void ClientCommonDataSender::sendPixelGunQuickUpgradeStoreProp(int propId)
{
	String data = CommonDataBuilder().addIntParam("propId", propId).getData();
	sendLuaCommonData("QuickUpgradeStoreProp", data);
}

void ClientCommonDataSender::sendPixelGunFirstShowStoreModule(int moduleId)
{
	String data = CommonDataBuilder().addIntParam("moduleId", moduleId).getData();
	sendLuaCommonData("FirstShowStoreModule", data);
}

void ClientCommonDataSender::sendPixelGunUseStoreModule(int moduleId)
{
	String data = CommonDataBuilder().addIntParam("moduleId", moduleId).getData();
	sendLuaCommonData("UseStoreModule", data);
}

void ClientCommonDataSender::sendPixelGunUnloadStoreModule(int moduleId)
{
	String data = CommonDataBuilder().addIntParam("moduleId", moduleId).getData();
	sendLuaCommonData("UnloadStoreModule", data);
}

void ClientCommonDataSender::sendPixelGunBuyStoreModule(int moduleId)
{
	String data = CommonDataBuilder().addIntParam("moduleId", moduleId).getData();
	sendLuaCommonData("BuyStoreModule", data);
}

void ClientCommonDataSender::sendPixelGunUnlockMap(int mapId)
{
	String data = CommonDataBuilder().addIntParam("mapId", mapId).getData();
	sendLuaCommonData("UnlockMap", data);
}

void ClientCommonDataSender::sendPixelGunOpenLotteryChest(int chestId)
{
	String data = CommonDataBuilder().addIntParam("chestId", chestId).getData();
	sendLuaCommonData("OpenLotteryChest", data);
}

void ClientCommonDataSender::sendPixelGunUpgradeArmor(int operateType)
{
	String data = CommonDataBuilder().addIntParam("operateType", operateType).getData();
	sendLuaCommonData("UpgradeArmor", data);
}

void ClientCommonDataSender::sendPixelGunReceiveSignIn(int signInId)
{
	String data = CommonDataBuilder().addIntParam("signInId", signInId).getData();
	sendLuaCommonData("ReceiveSignIn", data);
}

void ClientCommonDataSender::sendPixelGunBuyGiftBag(int giftId)
{
	String data = CommonDataBuilder().addIntParam("giftId", giftId).getData();
	sendLuaCommonData("BuyGiftBag", data);
}

void ClientCommonDataSender::sendPixelGunRefreshGiftBag()
{
	sendLuaCommonData("RefreshGiftBag");
}

void ClientCommonDataSender::sendPixelGunStartGame(String gameType, String mapName)
{
	String data = CommonDataBuilder()
		.addStringParam("gameType", gameType)
		.addStringParam("mapName", mapName)
		.getData();
	sendLuaCommonData("StartGame", data);
}

void ClientCommonDataSender::sendPixelGunSelect1v1Box(int index)
{
	String data = CommonDataBuilder().addIntParam("index", index).getData();
	sendLuaCommonData("Select1v1Box", data);
}

void ClientCommonDataSender::sendPixelGunBackHall()
{
	sendLuaCommonData("BackHall");
}

void ClientCommonDataSender::sendPixelGunPlayAgain()
{
	sendLuaCommonData("PlayAgain");
}

void ClientCommonDataSender::sendPixelGun1v1Revenge()
{
	sendLuaCommonData("1v1Revenge");
}

void ClientCommonDataSender::sendReceiveChickenWeapon()
{
	sendLuaCommonData("ReceiveChickenWeapon");
}

void ClientCommonDataSender::sendBedWarBuyGood(int goodId)
{
	String data = CommonDataBuilder().addIntParam("goodId", goodId).getData();
	sendLuaCommonData("BuyGood", data);
}

void ClientCommonDataSender::sendBedWarOpenRune()
{
	sendLuaCommonData("OpenRune");
}

void ClientCommonDataSender::sendBedWarChangeRune(int troughId, int curRuneId, int targetRuneId)
{
	String data = CommonDataBuilder()
		.addIntParam("troughId", troughId)
		.addIntParam("curRuneId", curRuneId)
		.addIntParam("targetRuneId", targetRuneId)
		.getData();
	sendLuaCommonData("ChangeRune", data);
}

void ClientCommonDataSender::sendBedWarOneKeyUnloadRune(int troughType)
{
	String data = CommonDataBuilder().addIntParam("troughType", troughType).getData();
	sendLuaCommonData("OneKeyUnloadRune", data);
}

void ClientCommonDataSender::sendBedWarOneKeyEquipRune(int troughType)
{
	String data = CommonDataBuilder().addIntParam("troughType", troughType).getData();
	sendLuaCommonData("OneKeyEquipRune", data);
}

void ClientCommonDataSender::sendBedWarOneKeyReclaimRune(int runeType)
{
	String data = CommonDataBuilder().addIntParam("runeType", runeType).getData();
	sendLuaCommonData("OneKeyReclaimRune", data);
}

void ClientCommonDataSender::sendBedWarReclaimRune(int runeId)
{
	String data = CommonDataBuilder().addIntParam("runeId", runeId).getData();
	sendLuaCommonData("ReclaimRune", data);
}

void ClientCommonDataSender::sendBedWarUnlockRune(int runeType)
{
	String data = CommonDataBuilder().addIntParam("runeType", runeType).getData();
	sendLuaCommonData("UnlockRune", data);
}

void ClientCommonDataSender::sendBlockCityPortal(int uId, int id)
{
	String data = CommonDataBuilder()
		.addIntParam("uId", uId)
		.addIntParam("id", id).getData();
	sendLuaCommonData("BlockCityPortal", data);
}

void ClientCommonDataSender::sendBlockCityToRaceOrNot(ui64 targetUserId, bool isAgree/* = true*/)
{
	String data = CommonDataBuilder()
		.addI64Param("targetUserId", i64(targetUserId))
		.addBoolParam("isAgree", isAgree).getData();
	sendLuaCommonData("BlockCityToRaceOrNot", data);
}

void ClientCommonDataSender::sendClickBuyUpdate(int type, int id)
{
	String data = CommonDataBuilder()
		.addIntParam("type", type)
		.addIntParam("id", id)
		.getData();
	sendLuaCommonData("ClickBuyUpdate", data);
}

void ClientCommonDataSender::sendWatchAdUpdateInfo(int id, int level)
{
	String data = CommonDataBuilder()
		.addIntParam("id", id)
		.addIntParam("level", level)
		.getData();
	sendLuaCommonData("WatchAdUpdateInfo", data);
}

void ClientCommonDataSender::sendSkyBlockBuyGoods(int num, int indexId, int type)
{
	String data = CommonDataBuilder()
		.addIntParam("num", num)
		.addIntParam("indexId", indexId)
		.addIntParam("type", type)
		.getData();
	sendLuaCommonData("SkyBlocShopOperate", data);
}

void ClientCommonDataSender::sendSkyBlockReceiveTaskReward(int taskId, int status)
{
	String data = CommonDataBuilder()
		.addIntParam("taskId", taskId)
		.addIntParam("status", status)
		.getData();
	sendLuaCommonData("SkyBlockReceiveTask", data);
}

void ClientCommonDataSender::sendSellItem(i32 selectIndex, i32 itemId, i32 itemMeta, i32 num)
{
	String data = CommonDataBuilder()
		.addIntParam("selectIndex", selectIndex)
		.addIntParam("itemId", itemId)
		.addIntParam("itemMeta", itemMeta)
		.addIntParam("num", num)
		.getData();
	sendLuaCommonData("SellItemEvent", data);
}

void ClientCommonDataSender::sendClickResourceIsland()
{
	sendLuaCommonData("ClickResourceIsland");
}

void ClientCommonDataSender::sendClickExtendArea()
{
	sendLuaCommonData("ExtendArea");
}

void ClientCommonDataSender::sendSkyBlockAddHome()
{
	sendLuaCommonData("SkyBlockSetHome");
}

void ClientCommonDataSender::sendSkyBlockBackHome()
{
	sendLuaCommonData("SkyBlockBackHome");
}

void ClientCommonDataSender::sendSkyBlockReceiveSignInReward(int rewardId)
{
	String data = CommonDataBuilder().addIntParam("rewardId", rewardId).getData();
	sendLuaCommonData("ReceiveSignInReward", data);
}

void ClientCommonDataSender::sendSkyBlockDareTaskOperate(i32 type, bool has_ad)
{
	String data = CommonDataBuilder()
		.addIntParam("type", type)
		.addBoolParam("has_ad", has_ad)
		.getData();
	sendLuaCommonData("DareTaskOperate", data);
}

void ClientCommonDataSender::sendSkyBlocReciveNewPri()
{
	sendLuaCommonData("ReciveNewPri");
}


void ClientCommonDataSender::sendWalkingDeadBuyStoreGoods(int goodsId)
{
	String data = CommonDataBuilder().addIntParam("goodsId", goodsId).getData();
	sendLuaCommonData("WalkingDeadBuyGoods",data);
}

void ClientCommonDataSender::sendWalkingDeadBuyStoreSupply(int supplyId)
{
	String data = CommonDataBuilder().addIntParam("supplyId", supplyId).getData();
	sendLuaCommonData("WalkingDeadBuySupply", data);
}

void ClientCommonDataSender::sendWalkingDeadDesignationChange(int titleId,int titleStatus)
{
	String data = CommonDataBuilder()
		.addIntParam("titleId", titleId)
		.addIntParam("titleStatus", titleStatus).getData();
	sendLuaCommonData("DesignationChange", data);
}
void ClientCommonDataSender::sendOpenMovingEnderChest()
{
	sendClientCommonData("OpenMovingEnderChest");
}

void ClientCommonDataSender::sendAbandonItem(i32 selectIndex, i32 num)
{
	String data = CommonDataBuilder()
		.addIntParam("selectIndex", selectIndex)
		.addIntParam("num", num)
		.getData();
	sendLuaCommonData("PlayerAbandonItem", data);
}

