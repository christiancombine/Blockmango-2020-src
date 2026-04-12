/********************************************************************
filename: 	EntityPlayerMP.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-07-10
*********************************************************************/
#ifndef __ENTITY_PLAYER_MP_HEADER__
#define __ENTITY_PLAYER_MP_HEADER__

#include "Entity/EntityPlayer.h"
#include "Inventory/Crafting.h"
#include "Item/Potion.h"
#include "Util/SubscriptionGuard.h"
#include "Util/AboutShopping.h"
#include "TileEntity/TileEntityInventory.h"
#include <vector>
#include "SynBlockStrategy.h"

namespace BLOCKMAN
{
class Container;
class ItemStack;
class ChatMessageComponent;
class TileEntityChest;

struct ServerMovementInput 
{
	float moveForward = 0.0f;
	float moveStrafe = 0.0f;
};

class EntityPlayerMP : public EntityPlayer ,public ICrafting
{
	RTTI_DECLARE(EntityPlayerMP);
private:
	SubscriptionGuard m_subscriptionGuard;
	TileEntityChest* m_openChest = nullptr;
	bool m_isElevatorArrive = false;
	TileEntityFurnace* m_openFurnace = nullptr;
	
public:
	/** player X position as seen by PlayerManager */
	float managedPosX = 0.0f;

	/** player Z position as seen by PlayerManager */
	float managedPosZ = 0.0f;

	/** LinkedList that holds the loaded chunks. */
	// List loadedChunks = new LinkedList();

	/** entities added to this list will  be packet29'd to the player */
	// List destroyedItemsNetCache = new LinkedList();
	
	/** poor mans concurency flag, lets hope the jvm doesn't re-order the setting of this flag wrt the inventory change on the next line */
	bool playerInventoryBeingManipulated = false;
	int ping = 0;

	/** Set when a player beats the ender dragon, used to respawn the player at the spawn point while retaining inventory and XP */
	bool playerConqueredTheEnd = false;

	/** identity whether the player inventory changed */
	bool m_inventoryChanged = true;
	bool m_inventoryForceUpdate = false;
protected:
	Vector3 oldPos;
	float oldMinY = 0.f;

	/** de-increments onUpdate, attackEntityFrom is ignored if this >0 */
	int initialInvulnerability = 60;	// = 60;

	/** must be between 3>x>15 (strictly between) */
	int chatVisibility = 0;
	bool chatColours = true; // = true;

	/** The currently in use window ID. Incremented every time a window is opened. */
	int currentWindowId = 0;

	/** the ranksid */
	ui64 m_raknetID = 0;

	/** identity whether the player property changed */
	bool m_propertyChanged = true;
	bool m_itemInHandChanged = true;

	bool m_lastTickIsFlying = false;

	AttributeModifierMap m_attackModifiers;

	AttributeModifierMap m_hpModifiers;

	SyncBlockContext *mSynBlockContext = nullptr;

	int m_movePlayerUpdateTick = 0;

	bool m_isForceTeleport = false;

	bool m_isCraftingChanged = false;

	bool m_isClientReady = false;

public:
	EntityPlayerMP(World* pWorld, const String& worldName, const Vector3i& spawnPos);
	virtual ~EntityPlayerMP();
	void doConstruct(const Vector3i& spawnPos);
	void doDestruct();

	void reconstruct(const Vector3i& spawnPos);
	void propertyChanged() { m_propertyChanged = true; };
	void addMoveSpeedPotionEffect(int seconds);
	void addJumpPotionEffect(int seconds);
	void addNightVisionPotionEffect(int seconds);
	void addPoisonPotionEffect(int seconds);
	void addEffect(int id, int seconds, int amplifier);
	void removeEffect(int id);
	void clearEffects();
	virtual void setDead();
	void specialJump(float yFactor = 0.0f, float xFactor = 1.0f, float zFactor = 1.0f);
    void dropItemIntoChest(int itemID, int itemDamage, int itemCount);
	/** implement override functions from EntityLivingBase */
	virtual void updatePotionMetadata();
	virtual void readEntityFromNBT(NBTTagCompound* pNbtCompound);
	virtual void writeEntityToNBT(NBTTagCompound* pNbtCompound);
	virtual float getEyeHeight() { return 1.62F; }
	virtual void onUpdate();
	virtual void triggerEntityUpdate();
	virtual void onDeath(DamageSource* pSource);
	virtual bool attackEntityFrom(DamageSource* pSource, float amount);
	virtual bool canAttackPlayer(EntityPlayer* pPlayer);
	virtual void onItemPickup(Entity* pEntity, int size);
	virtual void closeScreen();
	virtual void addChatMessage(const String& msg);
	virtual void setPositionAndUpdate(float x, float y, float z);
	virtual bool setToLastPosition(float x, float y, float z);
	virtual void onCriticalHit(Entity* pEntity);
	virtual void jump() override;
	virtual void changeCurrentItem(int slot);
	virtual void moveEntity(const Vector3& vel) override;
	virtual void moveEntityWithHeading(float strafe, float forward) override;
	virtual void updateEntityActionState() override;
	
	/** implement override functions from EntityPlayer */
	virtual void addExperienceLevel(int exp);
	virtual SLEEP_RESULT sleepInBedAt(const BlockPos& pos);
	virtual void wakeUpPlayer(bool immediately, bool update, bool setSpawn);
	virtual void setItemInUse(ItemStackPtr pStack, int duration);
	virtual void clonePlayer(EntityPlayer* pPlayer, bool all);
	virtual void onEnchantmentCritical(Entity* pEntity);
	virtual void sendPlayerAbilities();
	virtual void setGameType(GAME_TYPE type);
	virtual void notifyClientReloadBullet();
	virtual void sendGetBulletSound();
	virtual bool takeOnAircraft(EntityAircraft* pAircraft);
	virtual bool takeOffAircraft();
	virtual bool getOnVehicle(EntityVehicle* vehicle, bool bForce, bool bIsDriver) override;
	virtual bool getOffVehicle() override;
	virtual bool getOnFlyingMount(EntityFlyingMount* mount) override;
	virtual bool getOffFlyingMount() override;
	virtual bool getOnFloatingMount(EntityFloatingMount* mount) override;
	virtual bool getOffFloatingMount() override;

	virtual void setOpenChest(TileEntityChest* chest);
	virtual void closeOpenChest();

	void setOpenFurnace(TileEntityFurnace* furnace);
	void closeOpenFurnace();

	virtual void knockBack(Entity* pEntity, float strength, float xRatio, float zRatio);

	/** implement override functions from ICrafting */
	virtual void sendSlotContents(Container* pContainer, int par2, ItemStackPtr pStack);
	virtual void sendContainerAndContentsToPlayer(Container* pContainer, ItemStackArr& stacks);
	virtual void sendProgressBarUpdate(Container* pContainer, int toUpdate, int val);

	void addSelfToInternalCraftingInventory();

	/** Teleports the entity to another dimension. Params: Dimension number to teleport to */
	void travelToDimension(int dimension);
	//void doFlyingByPulledCollision();
	/** likeUpdateFallState, but called from updateFlyingState, rather than moveEntity */
	void updateFlyingState(float y, bool onGroundIn) { EntityPlayer::updateFallState(y, onGroundIn); }

	void sendContainerToPlayer(Container* pContainer);
	
	/*** updates item held by mouse */
	void updateHeldItem();

	/** Closes the container the player currently has open. */
	void closeContainer();

	void sendChatToPlayer(ChatMessageComponent* chatComponent);
	
	/** Returns true if the command sender is allowed to use the given command. */
	bool canCommandSenderUseCommand(int par1, const String& par2Str);
	
	/** Gets the player's IP address. Used in /banip. */
	String getPlayerIP();

	// todo.
	// void updateClientInfo(Packet204ClientInfo par1Packet204ClientInfo)
	
	int getChatVisibility() { return chatVisibility; }

	/** on recieving this message the client (if permission is given) will download the requested textures */
	void requestTexturePackLoad(const String& par1Str, int par2);
	
	/** Return the position for this command sender. */
	BlockPos getPlayerCoordinates();

	void setRaknetID(ui64 id) { m_raknetID = id; }
	ui64 getRaknetID() const { return m_raknetID; }

	bool useItem(NETWORK_DEFINE::C2SPacketUseItemUseType useType, const Vector3i& pos, int param0);
	
	bool addItem(int id, int num, int damage, bool isReverse = false, int maxDamage = 0);
	void replaceItem(int id, int num, int damage, int stackIndex, int clipBullet = 0);
	void addGunItem(int id, int num, int damage, int clipBullet, bool isReverse = false);
	void replaceArmorItemWithEnchant(int newId, int num, int damage, std::vector<std::vector<int>>& enchantments);

	void addItemToEnderChest(int slot, int id, int num, int damage, std::vector<std::vector<int>> enchantments);
	void addGunItemToEnderChest(int slot, int id, int num, int damage, int clipBullet);

	void addEchantmentItem(int id, int num, int damage, std::vector<std::vector<int>> enchantments);
	void addItemPotion(int index, int num);
	void buy(const Commodity& commodity, bool isConsumeCoin, bool isAddGoods);

	void setAllowFlying(bool allowFlying) override;
	void setWatchMode(bool isWatchMode) override;
	virtual void changeMaxHealth(float health, bool unChangeCurrent = false) override;
	void changeMaxHealthSingle(float maxHealth);
	bool equipArmor(int id, int damage);
	bool setCameraLocked(bool bLocked);
	void changeInvisible(ui64 rakssId, bool isVisible);
	void changeNamePerspective(ui64 rakssId, bool isPerspective);
	void setHoldItemId(int id);
	
	void changeHeart(int hp, int maxHp);
	void showBossStripWithIcon(String name, int curProgress, int maxProgress, String iconPath);
	void changeDefense(int defense, int maxDefense);
	void changeAttack(int attack, int maxAttack);
	void changeApple(int cur, int max);
	void appleFresh(int time);
	virtual void setScale(float s) override;
	void setPersonalShopArea(Vector3 startPos, Vector3 endPos);
	void changeSwitchableProps(String data);
	void changeUpgradeProps(String data);
	void changeEnchantmentProps(String data);
	void changeSuperProps(String data);
	void changeCustomProps(String data);
	void updateCustomProps(String data);

	void initUpdateBlocks();
	void sendUpdateBlocks();
	void addUpdateBlocks(std::vector<BlockPos>& blocks);
	void addSingleUpdateBlock(BlockPos pos, int block_id, int meta);
	int getSynblockNum();
	void setIsCrafting(bool sign) { m_isCraftingChanged = sign; }
	void setExtraInventoryCount(int count);
	virtual void markInventoryDirty()
	{
		m_inventoryChanged = true;
	};
	void inventoryForceUpdate() { m_inventoryForceUpdate = true; };
	bool canPickupItem(Entity* pEntity, int size);
	void setCurrency(i64 currency);
	void addCurrency(i32 currency);
	void subCurrency(i32 currency);
	i64 getCurrency();
	void resetClothes(ui64 userId);
	bool tryBuyVehicle(int vehicleId);
	void syncOwnVehicle();

	void addBackpackCapacity(int capacity);
	void subBackpackCapacity(int capacity);
	void resetBackpack(int capacity, int maxCapacity);
	void setChangePlayerActor(bool isCanChangeActor, int changeActorCount, int needMoneyCount);

	/*set player knock back Coefficient when gun shoot */
	void SetKnockBackCoefficient(float knockBackCoefficient);
	
	
	void setSpeedAdditionLevel(int level);
	void updateExp(int level, float exp, int maxExp);

	/** attack ActorNpc */
	virtual void attackActorNpc(Entity* entity, PLAYER_ATTACK_TYPE damageType);
	/** attack CreatureEntity */
	virtual void attackCreatureEntity(EntityCreature* entityCreature, int attackType);

	void upgradeResource(int resourceId);
	void playSkillEffect(String name, float duration, int range, Vector3 color);
	void sendSkillEffect(int effectId);
	void playAnimate();
	void setOccupation(int occupation) override;
	int getSlotByItemId(int ItemId);
	void setArmItem(int itemId);
	void setUnlockedCommodities(std::vector<std::vector<int>> unlockedCommodities);

	void addGunBulletNum(int gunId, int bulletNum);
	void subGunRecoil(int gunId, float recoil);
	void subGunFireCd(int gunId, float fireCd);

	void setOnFrozen(float frozenTime);
	void setOnHurt(float hurtTime);
	void setOnColorful(float colorfulTime);
	void setDisableMove(float disableMoveTime);
	void setOnFire(int seconds);

	virtual void attackEntity(Entity* entity) override;
	bool isForceTeleport() { return m_isForceTeleport; }
	void forceTeleportStart() { m_isForceTeleport = true; }
	void forceTeleportEnd() { m_isForceTeleport = false; }
	void enableMovement();
	void checkForceMovement();
	bool isPlayerMoved(bool precise = false);
	void recordOldPos();
	Ranch* getRanch() { return m_ranch; }
	BirdSimulator* getBirdSimulator() { return m_birdSimulator; }
	BlockCity* getBlockCity() { return m_blockCity; }
	BlockFort* getBlockFort() { return m_blockFort; }
	InventoryPlayer* getInventoryPlayer() { return inventory; }
	void setBagInfo(int curCapacity, int maxCapacity);
	void setBirdConvert(bool isConvert);
	void setFlyWing(bool isEquipWing, float glideMoveAddition, float glideDropResistance) override;
	void setGlide(bool isOpenGlide) override;
	void setElevator(bool isUse, float speed, float arriveHeight, i32 arriveFloor, i32 elevatorId) override;
	void setFlying(bool isFlying);
	int getClothesId(String partName);
	i64 getDiamond();
	i64 getGold();

	void placeBlock(Vector3i selectedBlockPos, Vector3 hitPos, int face);
	bool canSyncEntity();
	bool isClientReady() { return m_isClientReady; }
	void setClientReady() { m_isClientReady = true; }
	int getBulletNum(int stackIndex);

protected:
	void initPlayerInventory();
	void monitorPropertyChanged();
	bool onPlayerHealthChanged(float old_health, float new_health);
	bool onPlayerAirChanged(int old_air, int new_air);
	bool onPlayerFoodLevelChanged(int old_foodLevel, int new_foodLevel);
	bool onPlayerWaterLevelChange(int old_WaterLevel, int new_WaterLevel);
	bool onPlayerFoodSaturationLevelChanged(float old_saturationLevel, float new_saturationLevel);
	void monitorInventoryChanged();

	/** implement override functions from EntityPlayer EntityLivingBase */
	virtual void resetHeight() { yOffset = 0.0F; }
	virtual void onItemUseFinish() override;
	virtual void onNewPotionEffect(PotionEffect* pEff) override;
	virtual void onChangedPotionEffect(PotionEffect* pEff, bool apply) override;
	virtual void onFinishedPotionEffect(PotionEffect* pEff) override;

	/** called from onUpdate for all tileEntity in specific chunks */
	void sendTileEntityToPlayer(TileEntity* pTileEntity);
	int onEnchantmentModifier(Entity * target, int  enchantmentModifier) override;

private:
	Vector3 lastMoveEventInvokePos;
	void updateMoveEventInvoke();
};

}

#endif
