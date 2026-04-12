/********************************************************************
filename: 	ClientPacketSender
file path:	H:\sandboxol\client\blockmango-client\dev\client\Src\Network\ClientPacketSender.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/29
*********************************************************************/
#ifndef __CLIENTPACKETSENDER_H__
#define __CLIENTPACKETSENDER_H__

#include "Entity/Entity.h"
#include "Network/CommonPacketSender.h"
#include "Chat/ChatComponent.h"
#include "Enums/Enums.h"
#include "Game/Manor.h"
#include "Game/Ranch.h"

using namespace BLOCKMAN;

class ClientPacketSender : public CommonPacketSender
{
public:
	ClientPacketSender();
	~ClientPacketSender();

	void sendLogin();
	void sendRebirth();
	void sendAnimate(int32_t entityRuntimeId, NETWORK_DEFINE::PacketAnimateType animateType, int32_t value = 0);
	void sendAttack(int targetEntityRuntimeId, real32 targetX, real32 targetY, real32 targetZ);
	void sendPlayerMovement(bool onGround);
	void sendPlayerMovement(bool onGround, float yaw, float pitch);
	void sendPlayerMovement(bool onGround, float x, float y, float z);
	void sendPlayerMovement(bool onGround, float x, float y, float z, float yaw, float pitch);
	void sendVehicleMovement(const BLOCKMAN::EntityVehicle* vehicle);
	void sendCraftWithCraftingTable(int recipeId, Vector3i craftingTablePositoin);
	void sendCraftWithHand(int recipeId);
	void sendFormulationSubmit(int id , int count);
	void sendPlayerAction(NETWORK_DEFINE::PacketPlayerActionType action);
	void sendUseItem(NETWORK_DEFINE::C2SPacketUseItemUseType useType, const Vector3i& pos = Vector3i(0,0,0), int param0 = 0);
	void sendSwapItemPosition(int index1, int index2);
	void sendDropItemPosition(int inventoryIndex);
	void sendRemoveItemPosition(int inventoryIndex);
	void sendEquipArmor(int slotIndex);
	void sendUnequipArmor(int slotIndex);
	void sendChangeItemInHand(int slotIndex, int currentItemIndex);
	void sendStartBreakBlock(int x, int y, int z);
	void sendStartBreakBlock(const Vector3i& position)
	{
		sendStartBreakBlock(position.x, position.y, position.z);
	}
	void sendAbortBreakBlock();
	void sendFinishBreakBlock();
	void sendPlaceBlock(const Vector3i& blockPos, int face, const Vector3 & hisPos);
	void sendActivateBlock(const Vector3i& blockPos, int face, const Vector3 & hisPos);
	void sendExchangeItemStack(bool  isPlayerInventoryToChest, i32 touchTime, int index, Vector3i blockPos, int face, Vector3 hisPos, int targetIndex, bool isEnderChest);
	void sendChatString(String msg);
	void sendUseProp(String propId);
	void sendPing(ui32 currTime, ui32 lastRecvPingTime);
	void sendLaunchArrow(float cdTime, const Vector3& playerPos, float playerYaw, float playerPitch,int arrowID);
	void sendLaunchBullet(int bulletID, const Vector3& playerPos, float playerYaw, float playerPitch);
	void sendSpawnVehicle(int carID, const Vector3& pos, float yaw);
	void sendGunFireResult(const Vector3& pos, const Vector3& dir, int tracyType, const Vector3i& blockPos, const Vector3& hitpos, int hitEnttityID, bool isNeedConsumeBullet, bool isHeadshot);
	void sendSplashPotion(const Vector3& playerPos, float playerYaw, float playerPitch);
	void sendCloseContainer(const Vector3i& containerPos);
	void sendUseThrowableItem();
	void sendSetFlying(bool isFlying);
	void sendSetSpYaw(bool isSpYaw, float radian);
	void sendBuyCommodity(int merchantId, int groupIndex, int commodityIndex, int commodityId);
	void sendTakeVehicle(bool onoff, int carID);
	void sendReloadBullet();
	void sendBuyGoods(int groupIndex, int goodsIndex, int goodsId);
	void sendSelectRole(int role);
	void sendBuyVehicle(int carId);
	void sendLockVehicle(bool onoff, int vehicleId);
	void sendClientReady();
	void sendVehicleActionState(int vehicleId, int state);
	void syncVehicleInput(const BLOCKMAN::EntityVehicle* vehicle);
	void sendBuyRespawn(bool isRespawn, i32 uniqueId, i32 index);
	void sendKeepItem(bool isKeepItem, int reviveType);
	void sendEnchantmentQuick(int equipId, int effectIndex);
	void sendGoNpcMerchant(float x, float y, float z, float yaw);
	void sendClickTeleport();
	void sendLeaveAircraft();
	void sendChangeClothes(int actorId, String partName);
	void sendRestClothes();
	void sendFriendOperation(int operationType, ui64 userId);
	void sendSetParachute(bool isParachute);
	void sendSkillType(BLOCKMAN::SkillType skill , bool isRelease);
	void sendChangePlayerActor();
	void sendClickChangeActor();
	void sendCameraLockMovement(Vector3& pos);
	void sendCameraLock(bool bLock);
	void sendPickUpItemPay(i32 itemEntityId, i32 itemId);
	void sendAttackActorNpc(i32 entityId);

	void sendGetManor();
	void sendSellManor();
	void sendUpgradeManor();

	void sendBuyHouse(BLOCKMAN::ManorHouse* house);
	void sendBuyFurniture(BLOCKMAN::ManorFurniture* furniture);
	void sendDeleteManorMessage(BLOCKMAN::ManorMessage* message);

	void sendBuildHouse(BLOCKMAN::ManorHouse* house);
	void sendPutFurniture(i32 furnitureId, BLOCKMAN::PutFurnitureStatus status);
	void sendRecycleFurniture(BLOCKMAN::ManorFurniture* furniture);
	void sendManorTeleport();
	void sendCallOnManor(ui64 targetUserId, i32 from = 0, bool isFriend = false, i64 score = 0);
	void sendGiveRose(ui64 targetUserId);

	void sendRequestUpdateRealTimeRank();
	void sendBuySwitchableProp(String uniqueId);
	void sendBuyUpgradeProp(String uniqueId);
	void sendAttackCreatureEntity(i32 entityId);

	void sendCustomTipResult(String extra, bool bResult);
	void sendRelaseItemSkill(int skillItemId);

	void sendUpgradeResource(i32 resourceId);
	void sendWatchRespawn();
	void sendMovementInputChanged(float forward, float strafe);
	void sendJumpChanged(bool isJump);
	void sendPlayerRotationChanged(float yaw, float pitch);
	void sendDownChanged(bool isDown);
	void sendNeedUpdateMerchantCommodities(int merchantId);

	void sendBuildWarOpenShop();
	void sendGradeScore(int grade);
	void sendBuildWarOpenGuessResult();
	void sendBuildWarResult();
	void sendBuildWarGuessVisit(ui64 platformUserId);
	void sendBuildWarGuessSuc(int guessRoomId);
	void sendGetCommodity(int merchantId, int groupIndex, int commodityIndex, int commodityId);
	void sendUnlockCommodity(int merchantId, int groupIndex, int commodityIndex, int commodityId);
	
	void sendPlayerInteractionActionMesage(ui64 targetUserId, String langKey, LORD::vector<String>::type args);
	void sendAttackEntity(i32 entityId);
	void sendConsumeTipResult(String extra, bool bResult);
	void sendBuyEnchantmentProp(String uniqueId);
	void sendBuySuperProp(String uniqueId);
	void sendBuyCustomProp(String uniqueId);
	void sendClickBackHall();
	void sendLongHitEntity(i32 entityId);
	void sendUseFishingRod();
	void sendUseCannon(i32 entityId);
	void sendUpdateUserMoney();
	void sendBuyBullet();
	void sendBuyRanchItem(i32 itemId, i32 num, i32 totalPrice, i32 currencyType);
	void sendBuyRanchBuildItem(i32 itemId, i32 num, i32 type, i32 currencyType);
	void sendRanchOperation(BLOCKMAN::RanchOperationType type);
	void sendRanchReceive(ui64 referrer);
	void sendRanchExpand(i32 landCode);
	void sendRanchShortcut(i32 landCode, i32 moneyType);
	void sendRanchDestroyHouse();
	void sendRanchSellItem(i32 itemId, i32 num);
	void sendRanchBuyItems(const vector<RanchCommon>::type items);
	void sendRanchBuildTakeProduct(i32 entityId, i32 itemId, i32 index);
	void sendRanchBuildQueueOperation(i32 type, i32 entityId, i32 queueId, i32 productId);
	void sendRanchBuildRemove(i32 actorId);
	void sendRanchOrderOperate(i32 type, i32 orderId, i32 index);
	void sendRanchExBegin();
	void sendRanchExTaskFinish(int itemId, int itemNum);
	void sendRanchGiftsOrInvite(int type, i64 targetUserId);
	void sendRanchHelpFinish(i64 helpId, i32 type);
	void sendReceiveMailReward(i64 mailId);
	void sendReceiveRanchAchievementReward(i64 achievementId);
	void sendRanchGoExplore();
	void sendChangeCurrentItemInfo(int itemIndex);
	void buildNotification(i64 userId, const String& message) override;
	void sendSpeedUpCrop(Vector3i blockPos, i32 blockId);
	void sendUseCubeFinshOrder(i32 orderId, i32  index);
	void sendBuyFlyingPermissionResult();
	void sendResetCloseup(const Vector3& playerPos);
	void sendOpenEnchantMent();
	void sendEnchantmentEffectClick(int equipId, int consumeId, int effectIndex);
	void sendSelectLotteryItem(String uniqueId);
	void sendLotteryAgain();
	void sendHallLotteryAgain(int luck_times, int entityId);

	void sendEffectGunFire(bool isNeedConsumeBullet, const Vector3& endPos, bool hasBlockTarget, const Vector3i& blockTargetPos);
	void sendEffectGunFireResult(int hitEntityId, const Vector3& hitPos, const Vector3& dir, int gunId, int traceType, const Vector3i& blockPos, bool isHeadshot = false);
	void sendEffectGunFireExplosionResult(const Vector3& hitPos, int gunId);
	void sendStopLaserGun();
	void sendConsumeBullet();

	void sendBirdBagOperation(i64 birdId, i32 operationType);
	void sendBirdStoreOperation(i32 storeId, i32 goodsId, i32 operationType);
	void sendBirdPersonalStoreBuyGoods(i32 tabId, i32 goodsId);
	void sendBirdFuse(const vector<i64>::type& birdIds);
	void sendBirdOpenTreasureChest(i32 entityId);
	void sendBirdLottery(i32 entityId);
	void sendBirdTask(i32 entityId, i32 taskId);
	void sendBirdFeed(i64 birdId, i32 foodId, i32 num);
	void sendBirdSetDress(i64 birdId, i32 dressType, i32 dressId);

	void sendBuyQuicklyBuildBlock();

	void sendGlideOperation(bool isGlideWing);
	void sendBirdOpenEggChest(i32 entityId);
	void sendCheckin(i32 type);

	void sendPlayingSelectableAction(int actionId, String langKey);
	void sendUpdateUserSelectableAction();
	
	void sendGuideInfo(int guideId, int guideStatus);
	void sendWatchAdFinished(int type, String params, int code);

	void sendRechargeResult(i32 type, i32 result);
	void sendVehicleHornSoundSource(const Vector3 &pos);
	void sendBindEntity(int parentEntityId, int childEntityId, const String& nodeName, int idleActionId);
	void sendUnbindEntity(int childEntityId);
	void sendFlyingMountMovement(int entityId, const Vector3& pos, const Quaternion& bodyOrientation, const Quaternion& viewOrientation);
	void sendFloatingMountMovement(int entityId, const Vector3& pos, float bodyYaw, float viewYaw, float pitch);
	void sendPlayerSprint(bool isSprintInput);
};

#endif
