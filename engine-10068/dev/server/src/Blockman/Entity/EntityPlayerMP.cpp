#include "EntityPlayerMP.h"
#include "Entity/DamageSource.h"
#include "Entity/EntityArrow.h"
#include "Entity/EntityBullet.h"
#include "Entity/EntityMerchant.h"
#include "EntityTracker.h"

#include "World/World.h"
#include "Inventory/Container.h"
#include "Inventory/InventoryPlayer.h"
#include "Inventory/InventoryEnderChest.h"
#include "Inventory/Slot.h"
#include "Inventory/CoinManager.h"
#include "Item/Item.h"
#include "Item/ItemStack.h"
#include "Item/PotionManager.h"
#include "Item/Items.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "Blockman/World/ServerWorld.h"
#include "Util/LanguageKey.h"
#include "Script/GameServerEvents.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Network/ClientPeer.h"
#include "Block/BlockManager.h"
#include "Entity/Entity.h"
#include "Setting/CarSetting.h"
#include "Setting/SkillSetting.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityAircraft.h"
#include "Entity/EntityItem.h"
#include "Entity/EntityDecoration.h"
#include "Blockman/Entity/EntityCreatureAI.h"
#include "Blockman/World/BlockChangeRecorderServer.h"
#include "Network/ManorRequest.h"
#include "Network/protocol/S2CPackets2.h"
#include "Util/StringUtil.h"
#include "Setting/LogicSetting.h"
#include "Entity/EntityFlyingMount.h"
#include "Entity/EntityFloatingMount.h"
#include "Setting/CustomArmorSetting.h"
#include "Util/PlayerPrefs.h"
#include "CommodityManager.h"

namespace BLOCKMAN
{
	
EntityPlayerMP::EntityPlayerMP(World* pWorld, const String& worldName, const Vector3i& spawnPos)
	: EntityPlayer(pWorld, worldName)
{
	doConstruct(spawnPos);
	mSynBlockContext = LordNew SyncBlockContext(LordNew SyncBlockStrategySection());
}

EntityPlayerMP::~EntityPlayerMP() 
{
	doDestruct();
	LordSafeDelete(mSynBlockContext);
}

void EntityPlayerMP::doConstruct(const Vector3i& spawnPos)
{
	oldPos = Vector3::ZERO;
	oldMinY = 0.f;
	managedPosX = 0.f;
	managedPosZ = 0.f;
	playerInventoryBeingManipulated = false;
	ping = 0;
	playerConqueredTheEnd = false;
	initialInvulnerability = 60;
	chatVisibility = false;
	chatColours = true;
	currentWindowId = 0;
	m_raknetID = 0;

	//stepHeight = 0.0F;
	yOffset = 0.0F;
	setLocationAndAngles(Vector3(spawnPos) + Vector3(0.5f, 0.f, 0.5f), 0.0F, 0.0F);
	while (!world->getCollidingBoundingBoxes(this, boundingBox).empty())
	{
		setPosition(position + Vector3::UNIT_Y);
	}

	monitorInventoryChanged();
	monitorPropertyChanged();

	if (!Server::Instance()->isScriptLoaded()) {
		initPlayerInventory();
	}

	// for debug, added by wanglei
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	foodStats->setFoodLevel(10);
	data_Health = 20.0f;
#else
	data_Health = 20.0f;
#endif

	setGameType(Server::Instance()->getWorld()->getWorldInfo().getGameType());

	m_inventoryChanged = true;
	m_propertyChanged = true;
	m_itemInHandChanged = true;
	isDead = false;
	movementInput = LordNew MovementInput();
	lastMoveEventInvokePos = position;
	m_movePlayerUpdateTick = 0;
}

void EntityPlayerMP::doDestruct()
{
	m_subscriptionGuard.unsubscribeAll();
	LordSafeDelete(movementInput);
}

void EntityPlayerMP::reconstruct(const Vector3i& spawnPos)
{
	EntityPlayerMP::doDestruct();
	EntityPlayer::doDestruct();
	EntityLivingBase::doDestruct();
	Entity::doDestruct();

	Entity::doConstruct();
	EntityLivingBase::doConstruct();
	EntityPlayer::doConstruct();
	EntityPlayerMP::doConstruct(spawnPos);
}

void EntityPlayerMP::addMoveSpeedPotionEffect(int seconds)
{
	addPotionEffect(LordNew PotionEffect(Potion::moveSpeed->getId(), seconds * 20));
}

void EntityPlayerMP::addJumpPotionEffect(int seconds)
{
	addPotionEffect(LordNew PotionEffect(Potion::jump->getId(), seconds * 20));
}

void EntityPlayerMP::addNightVisionPotionEffect(int seconds)
{
	addPotionEffect(LordNew PotionEffect(Potion::nightVision->getId(), seconds * 20));
}

void EntityPlayerMP::addPoisonPotionEffect(int seconds)
{
	addPotionEffect(LordNew PotionEffect(Potion::poison->getId(), seconds * 20));
}

void EntityPlayerMP::addEffect(int id, int seconds, int amplifier)
{
	if (id <= 0 || id > 23)
	{
		LordLogError("unsupported potion id=%d", id);
		return;
	}

	auto effect = LordNew PotionEffect(id, seconds * 20, amplifier);
	addPotionEffect(effect);
}

void EntityPlayerMP::removeEffect(int id)
{
	if (id < 0 || id > 23)
	{
		return;
	}
	removePotionEffect(id);
}

void EntityPlayerMP::clearEffects()
{
	clearActivePotions();
}

void EntityPlayerMP::setDead()
{
	EntityPlayer::setDead();
	getOffVehicle();
	takeOffAircraft();
}

void EntityPlayerMP::specialJump(float yFactor, float xFactor, float zFactor)
{
	if (!isWatchMode())
	{
		auto sender = Server::Instance()->getNetwork()->getSender();
		sender->broadCastPlayerSpecialJump(entityId, yFactor, xFactor, zFactor);
	}
}

void EntityPlayerMP::dropItemIntoChest(int itemID, int itemDamage, int itemCount)
{
    //
}

void EntityPlayerMP::buy(const Commodity & commodity, bool isConsumeCoin, bool isAddGoods)
{
	if (isConsumeCoin && m_wallet)
	{
		int moneyId = CoinManager::Instance()->coinIdToItemId(commodity.coinId);
		if (moneyId >= 10000)
		{
			subCurrency(commodity.price);
			ManorRequest::trade(getPlatformUserId() , -commodity.price);
		}
		else
		{
			m_wallet->consumeCoin(commodity.coinId, commodity.price);
		}
	}

	if (isAddGoods)
	{
		addItem(commodity.itemId, commodity.itemNum, commodity.itemMeta);
		ServerNetwork::Instance()->getSender()->notifyGetItem(this->getRaknetID(), commodity.itemId, commodity.itemMeta, commodity.itemNum);
	}
}

bool EntityPlayerMP::addItem(int id, int num, int damage, bool isReverse, int maxDamage)
{
	if (this->isDead)
		return false;
	if (id >= 10000)
	{
		addCurrency(num);
		ManorRequest::trade(getPlatformUserId(), num);
		return false;
	}
	Item* pItem = Item::getItemById(id);
	if (!pItem) 
	{
		auto pBlock = BlockManager::getBlockById(id);
		if (pBlock)
		{
			ItemStackPtr itemStack = LORD::make_shared<ItemStack>(pBlock, num, damage);
			if (itemStack)
			{
				 if (isReverse)
					itemStack->m_needAdjustStackIndex = false;
				 return this->getInventory()->addItemStackToInventory(itemStack, isReverse);
			}
		}
		return false;
	}
		

	if (maxDamage > 0)
	{
		pItem->setMaxDamage(maxDamage);
	}

	if (dynamic_cast<ItemAppIcon*>(pItem))
	{
		return true;
	}
	
	ItemStackPtr itemStack = LORD::make_shared<ItemStack>(pItem, num, damage);
	if (itemStack)
	{
		if (isReverse)
			itemStack->m_needAdjustStackIndex = false;
		return this->getInventory()->addItemStackToInventory(itemStack, isReverse);
	}

	return false;
}

void EntityPlayerMP::replaceItem(int id, int num, int damage, int stackIndex, int clipBullet)
{
	if (this->isDead)
		return;

	if (id >= 10000)
	{
		addCurrency(num);
		ManorRequest::trade(getPlatformUserId(), num);
		return;
	}

	if (stackIndex < 0 || stackIndex >= InventoryPlayer::HOTBAR_COUNT)
		return;

	if (inventory == nullptr)
		return;

	ItemStackPtr oldStack = inventory->getItemIdBySlot(stackIndex);
	if (oldStack)
	{
		inventory->mainInventory[stackIndex] = nullptr;
		inventory->hotbar[stackIndex] = nullptr;
	}

	Item* pItem = Item::getItemById(id);
	if (!pItem)
		return;

	ItemStackPtr itemStack = LORD::make_shared<ItemStack>(pItem, num, damage);
	if (itemStack)
	{
		itemStack->m_needAdjustStackIndex = false;
		auto setting = itemStack->getGunSetting();
		if (setting)
		{
			int MaxBulletNum = getGunBulletNum(setting);
			itemStack->setTotalBulletNum(MaxBulletNum);
			itemStack->setCurrentClipBulletNum(clipBullet > MaxBulletNum ? MaxBulletNum : clipBullet);
		}

		inventory->mainInventory[stackIndex] = itemStack;
		inventory->hotbar[stackIndex] = itemStack;
	}
}

void EntityPlayerMP::addGunItem(int id, int num, int damage, int clipBullet, bool isReverse)
{
	if (this->isDead)
		return;
	ItemStackPtr itemStack = LORD::make_shared<ItemStack>(Item::getItemById(id), num, damage);
	if (itemStack)
	{
		auto setting = itemStack->getGunSetting();
		if (setting)
		{
			int MaxBulletNum = getGunBulletNum(setting);
			itemStack->setTotalBulletNum(MaxBulletNum);
			itemStack->setCurrentClipBulletNum(clipBullet > MaxBulletNum ? MaxBulletNum : clipBullet);
			this->getInventory()->addItemStackToInventory(itemStack, isReverse);
		}
	}
}

void EntityPlayerMP::replaceArmorItemWithEnchant(int newId, int num, int damage, std::vector<std::vector<int>>& enchantments)
{
	if (inventory == nullptr)
		return;

	auto armorItem = dynamic_cast<ItemArmor*>(Item::itemsList[newId]);
	if (!armorItem)
	{
		return;
	}
	int armorIndex = armorItem->armorType;

	if (armorIndex < 0 || armorIndex >= InventoryPlayer::ARMOR_INVENTORY_COUNT)
		return;

	inventory->armorInventory[armorIndex] = nullptr;

	if (!this->isDead)
	{
		auto itemStack = LORD::make_shared<ItemStack>(Item::getItemById(newId), num, damage);
		for (auto enchantment : enchantments)
		{
			itemStack->addEnchantmentbyId(enchantment[0], enchantment[1]);
		}
		inventory->armorInventory[armorIndex] = itemStack;

		if (PlayerPrefs::Instance()->getBoolPrefs("IsUseCustomArmor"))
		{
			CustomArmorSettingInfo customArmorSettingInfo;
			if (CustomArmorSetting::Instance()->getArmorSettingInfo(itemStack->itemID, customArmorSettingInfo))
			{
				int curMaxHp = getBaseHealth();
				curMaxHp = curMaxHp + customArmorSettingInfo.hp;
				changeMaxHealthSingle(curMaxHp);
			}
		}

		Server::Instance()->getNetwork()->getSender()->sendEquipArmorsToTrackingPlayers(entityId, { itemStack });
	}
}

void EntityPlayerMP::addItemToEnderChest(int slot, int id, int num, int damage, std::vector<std::vector<int>> enchantments)
{
	if (this->isDead)
		return;
	Item* pItem = Item::getItemById(id);
	if (!pItem)
		return;
	ItemStackPtr itemStack = LORD::make_shared<ItemStack>(pItem, num, damage);
	if (itemStack)
	{
		for (auto enchantment : enchantments)
		{
			itemStack->addEnchantmentbyId(enchantment[0], enchantment[1]);
		}
		this->getInventoryEnderChest()->setInventorySlotContents(slot, itemStack);
	}
}

void EntityPlayerMP::addGunItemToEnderChest(int slot, int id, int num, int damage, int clipBullet)
{
	if (this->isDead)
		return;
	ItemStackPtr itemStack = LORD::make_shared<ItemStack>(Item::getItemById(id), num, damage);
	if (itemStack)
	{
		auto setting = itemStack->getGunSetting();
		if (setting)
		{
			int MaxBulletNum = getGunBulletNum(setting);
			itemStack->setTotalBulletNum(MaxBulletNum);
			itemStack->setCurrentClipBulletNum(clipBullet > MaxBulletNum ? MaxBulletNum : clipBullet);
			this->getInventoryEnderChest()->setInventorySlotContents(slot, itemStack);
		}
	}
}

void EntityPlayerMP::addItemPotion(int index, int num)
{
	if (!this->isDead)
	{
		auto itemStack = LORD::make_shared<ItemStack>(PotionManager::getItemPotionByIndex(index), num, 0);
		this->inventory->addItemStackToInventory(itemStack);
	}
}

void EntityPlayerMP::addEchantmentItem(int id, int num, int damage, std::vector<std::vector<int>> enchantments)
{
	if (!this->isDead)
	{
		auto itemStack = LORD::make_shared<ItemStack>(Item::getItemById(id), num, damage);
		for (auto enchantment : enchantments)
		{
			itemStack->addEnchantmentbyId(enchantment[0], enchantment[1]);
		}
		this->inventory->addItemStackToInventory(itemStack);
	}
}

void EntityPlayerMP::initPlayerInventory()
{
	int idx = 0;
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BlockManager::torchWood, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BlockManager::dirt, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BlockManager::dirt, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BlockManager::stained_glass_pane, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BlockManager::stainedGlass, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_LADDER, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_LADDER, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::fishingRod, 1, 0);

	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::snowball, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::grenade, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::tpScroll_01, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::createBridge_01, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::shovelRenovation, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::shovelToy, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::rabbitCooked, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::bow, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::fishCooked, 1, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(Item::sign, 16, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemSplashHeal, 1, 16384);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(PotionManager::itemSplashHeal, 1, 16384);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_TORCH_WOOD, 64, 0);
	inventory->mainInventory[idx++] = LORD::make_shared<ItemStack>(BLOCK_ID_GLASS, 64, 0);


	if (idx > 36)
	{
		throw std::out_of_range("initializing mainInventory caused out of range error");
	}

	for (size_t i = 0; i < InventoryPlayer::HOTBAR_COUNT; ++i)
	{
		inventory->hotbar[i] = inventory->mainInventory[i];
	}
}

void EntityPlayerMP::monitorPropertyChanged()
{
	using namespace std::placeholders;
	m_subscriptionGuard.add(data_Health.subscribe(std::bind(&EntityPlayerMP::onPlayerHealthChanged, this, _1, _2)));
	m_subscriptionGuard.add(m_air.subscribe(std::bind(&EntityPlayerMP::onPlayerAirChanged, this, _1, _2)));
	m_subscriptionGuard.add(foodStats->foodLevelListenable().subscribe(std::bind(&EntityPlayerMP::onPlayerFoodLevelChanged, this, _1, _2)));
	m_subscriptionGuard.add(foodStats->saturationLevelListenable().subscribe(std::bind(&EntityPlayerMP::onPlayerFoodSaturationLevelChanged, this, _1, _2)));
	m_subscriptionGuard.add(foodStats->waterlevelListenable().subscribe(std::bind(&EntityPlayerMP::onPlayerWaterLevelChange, this, _1, _2)));
}

bool EntityPlayerMP::onPlayerFoodSaturationLevelChanged(float old_saturationLevel, float new_saturationLevel)
{
	if (!Math::IsNearlyZero(old_saturationLevel - new_saturationLevel)) {
		m_propertyChanged = true;
	}

	return true;
}

bool EntityPlayerMP::onPlayerFoodLevelChanged(int old_foodLevel, int new_foodLevel)
{
	if (old_foodLevel != new_foodLevel) {
		m_propertyChanged = true;
		SCRIPT_EVENT::PlayerFoodLevelChangeEvent::invoke(this->getRaknetID(), new_foodLevel, old_foodLevel);
	}
	return true;
}

bool EntityPlayerMP::onPlayerWaterLevelChange(int old_waterLevel, int new_waterLevel)
{
	if (old_waterLevel != new_waterLevel) {
		m_propertyChanged = true;
	}
	return true;
}

bool EntityPlayerMP::onPlayerAirChanged(int old_air, int new_air)
{
	if (Math::Abs(old_air - new_air) >= 30) {
		m_propertyChanged = true;
	}
	return true;
}

bool EntityPlayerMP::onPlayerHealthChanged(float old_health, float new_health)
{
	if (!Math::IsNearlyZero(old_health - new_health)) {
		m_propertyChanged = true;
	}

	SCRIPT_EVENT::PlayerHealthChangedEvent::invoke(this, old_health, new_health);

	return true;
}

void EntityPlayerMP::monitorInventoryChanged()
{
	m_subscriptionGuard.add(inventory->onInventoryContentChanged([this] {
		m_inventoryChanged = true;
	}));
	
	m_subscriptionGuard.add(inventory->onItemInHandChanged([this] {
		m_itemInHandChanged = true;
		// attack logic read this
		this->attributeMap->removeAttributeModifiers(m_attackModifiers);
		m_attackModifiers.clear();


		if (inventory->getCurrentItem())
		{
			if (PlayerPrefs::Instance()->getBoolPrefs("IsUseCustomArmor"))
			{
				// custom attack damage
				CustomArmorSettingInfo customArmorSettingInfo;
				if (CustomArmorSetting::Instance()->getArmorSettingInfo(inventory->getCurrentItem()->itemID, customArmorSettingInfo))
				{
					Item* pItem = Item::getItemById(inventory->getCurrentItem()->itemID);
					if (pItem)
					{
						AttributeModifier * modifierAttr = pItem->getModifierAttr();
						if (modifierAttr)
						{
							if (modifierAttr->getAmount() == 0)
							{
								modifierAttr->setAmount(customArmorSettingInfo.attack);
							}
							
							m_attackModifiers.insert(std::make_pair(SharedMonsterAttributes::ATTACK_DAMAGE->getName(), modifierAttr));
						}

					}
					
				}
			}
			else
			{
				inventory->getCurrentItem()->getAttributeModifiers(m_attackModifiers);
			}

			this->attributeMap->applyAttributeModifiers(m_attackModifiers);
		}
	}));
}

void EntityPlayerMP::knockBack(Entity* pEntity, float strength, float xRatio, float zRatio)
{
	if (rand->nextFloat() < getEntityAttribute(SharedMonsterAttributes::KNOCKBACK_RESISTANCE)->getAttributeValue() || data_Health <= 0.0f)
		return;

	isAirBorne = true;
	float lensqr = Math::Sqrt(xRatio * xRatio + zRatio * zRatio);

	FloatProxy floatProxy;
	floatProxy.value = 0.0;

	SCRIPT_EVENT::KnockBackExtraStrengthEvent::invoke(this->hashCode(), pEntity->hashCode(), &floatProxy);

	motion /= 2.0f;
	motion.x -= xRatio / lensqr * strength * 2; // +floatProxy.value;
	motion.y = 0;
	motion.z -= zRatio / lensqr * strength * 2; // +floatProxy.value;

	if (Math::Abs(motion.x) > Math::Abs(motion.z))
	{
		if (motion.x > 0)
		{
			motion.x += floatProxy.value;
		}
		else
		{
			motion.x -= floatProxy.value;
		}
	}
	else
	{
		if (motion.z > 0)
		{
			motion.z += floatProxy.value;
		}
		else
		{
			motion.z -= floatProxy.value;
		}
	}

	auto startPos = position;
	auto endPos = position + motion;
	RayTraceResult trace = world->rayTraceBlocks(startPos, endPos, false, true);
	if (trace.result)
	{
		motion = trace.hitPointPos - position;
		motion.x += xRatio * 0.1f * strength * 2;
		motion.z += zRatio * 0.1f * strength * 2;
	}

	if (this->onGround && strength != 0.0f)
	{
		motion.y = 0.8f;
	}

	motion.y = Math::Min(motion.y, 0.8f);

	if (strength != 0.0f)
	{
		moveEntity(motion);
		//position += motion;
		motion = Vector3::ZERO;
		auto sender = Server::Instance()->getNetwork()->getSender();
		sender->broadCastPlayerMovement(entityId, true);
	}
}

void EntityPlayerMP::readEntityFromNBT(NBTTagCompound* pNbtCompound)
{
	EntityPlayer::readEntityFromNBT(pNbtCompound);

	if (pNbtCompound->hasKey("playerGameType"))
	{

	}
}

void EntityPlayerMP::writeEntityToNBT(NBTTagCompound* pNbtCompound)
{
	EntityPlayer::writeEntityToNBT(pNbtCompound);
	// pNbtCompound.setInteger("playerGameType", this.theItemInWorldManager.getGameType().getID());
}

void EntityPlayerMP::addExperienceLevel(int exp)
{
	EntityPlayer::addExperienceLevel(exp);
}

void EntityPlayerMP::addSelfToInternalCraftingInventory()
{
	openContainer->addCraftingToCrafters(this);
}

void EntityPlayerMP::onUpdate()
{
	try
	{
		EntityPlayer::onUpdate();
	}
	catch (const Exception &e)
	{
		LordLogError(e.getMessage().c_str());
	}

	auto sender = Server::Instance()->getNetwork()->getSender();
	// sync player inventory if changed
	if (m_inventoryChanged)
	{
		sender->sendPlayerInventory(m_raknetID, inventory, m_inventoryForceUpdate);
		m_inventoryChanged = false;
		m_inventoryForceUpdate = false;
		//LordLogInfo("sendPlayerInventory: player name: %s", getCommandSenderName().c_str());
	}
	// EntityPlayerMP will not call EntityPlayer::onUpdate() periodically, so update the wallet here to ensure the wallet is updated in every tick
	if (m_wallet)
	{
		m_wallet->onUpdate();
	}

	// sync player property if changed
	if (m_propertyChanged)
	{
		sender->sendEntityAttribute(this, this);
		sender->sendEntityAttributeToTrackingPlayers(this);
		m_propertyChanged = false;
	}

	if (m_itemInHandChanged)
	{
		if (inventory->getCurrentItem())
		{
			SCRIPT_EVENT::PlayerChangeItemInHandEvent::invoke(this->getRaknetID(), inventory->getCurrentItem()->itemID, inventory->getCurrentItem()->itemDamage);
		}
		else
		{
			SCRIPT_EVENT::PlayerChangeItemInHandEvent::invoke(this->getRaknetID(), 0, 0);
		}

		sender->sendChangeItemInHandToTrackingPlayers(entityId, inventory->getCurrentItem());
		m_itemInHandChanged = false;
	}

	if (m_lastTickIsFlying != capabilities.isFlying)
	{
		sender->sendSetFlyingToTrackingPlayers(this);
		m_lastTickIsFlying = capabilities.isFlying;
	}

	// theItemInWorldManager.updateBlockRemoving();
	--initialInvulnerability;
	openContainer->detectAndSendChanges();

	if (!world->m_isClient && !openContainer->canInteractWith(this))
	{
		closeScreen();
		openContainer = inventoryContainer;
	}

	if (m_isClothesChange)
	{
		sender->sendUserAttrFromEntityToTrackingPlayers(this);
		m_isClothesChange = false;
		m_isClothesChanged = true;
	}

	if (m_isPulled)
	{
		auto sender = Server::Instance()->getNetwork()->getSender();
		sender->broadCastPlayerMovementWithMotion(entityId, true);
		m_isPulled = false;
	}

	if (m_remainingFlyTicksByPulled > 0)
	{
		if (!SCRIPT_EVENT::FlyingByPulledCollisionEvent::invoke(entityId))
		{
			//ÂäµØ
		}

		m_remainingFlyTicksByPulled--;
		m_remainingFlyTicksByPulled = m_remainingFlyTicksByPulled > 1 ? m_remainingFlyTicksByPulled : 1;
		if (m_remainingFlyTicksByPulled == 1 && onGround)
		{
			m_remainingFlyTicksByPulled = 0;
			m_flyingByPulled = false;
		}
	}

	if (m_isCraftingChanged)
	{
		Server::Instance()->getNetwork()->getSender()->sendCraftItemUpdate(m_raknetID);
		m_isCraftingChanged = false;
	}

	updateMoveEventInvoke();
	sendUpdateBlocks();

	checkForceMovement();

	if (m_isUseElevator)
	{
		if (m_elevatorSpeed > 0 && position.y >= m_elevatorArriveHeight || m_elevatorSpeed < 0 && position.y <= m_elevatorArriveHeight)
		{
			if (!m_isElevatorArrive)
			{
				m_isElevatorArrive = true;
				SCRIPT_EVENT::ElevatorArriveEvent::invoke(getRaknetID(), m_elevatorArriveFloor, m_elevatorId);
			}
			else
			{
				m_isElevatorArrive = false;
				setElevator(false, 0.f, 0.f, 0, 0);
			}
		}
	}
}

void EntityPlayerMP::triggerEntityUpdate()
{
	try
	{
		EntityPlayer::onUpdate();
	}
	catch (const Exception &e)
	{
		LordLogError(e.getMessage().c_str());
	}
}

void EntityPlayerMP::jump()
{
	EntityPlayer::jump();
}

void EntityPlayerMP::changeCurrentItem(int slot)
{
	if (slot >= 0 && slot < InventoryPlayer::HOTBAR_COUNT)
	{
		inventory->currentItemIndex = slot;
		auto sender = Server::Instance()->getNetwork()->getSender();
		sender->sendPlayerChangeItem(m_raknetID, slot);
	}
}

void EntityPlayerMP::moveEntity(const Vector3 & vel)
{
	EntityPlayer::moveEntity(vel);
	
}

void BLOCKMAN::EntityPlayerMP::moveEntityWithHeading(float strafe, float forward)
{
	if (m_isForceTeleport)
	{
		fallDistance = 0.0f;
		motion = Vector3::ZERO;
	}
	else
	{
		EntityPlayer::moveEntityWithHeading(strafe, forward);
	}
}

void EntityPlayerMP::updateEntityActionState()
{
	if (isOnVehicle())
	{
		applyVehicleMovementInput();
	}

	if (needUpdateActionState())
	{
		EntityPlayer::updateEntityActionState();
	}
}

void EntityPlayerMP::onDeath(DamageSource* pSource)
{
	
	// mcServer.getConfigurationManager().sendChatMsg(func_110142_aN().func_94546_b());

	// todo.
	// if (!world->getGameRules().getGameRuleBooleanValue("keepInventory"))
	//{
	//	inventory->dropAllItems();
	//}

	/*Collection var2 = worldObj.getScoreboard().func_96520_a(ScoreObjectiveCriteria.field_96642_c);
	Iterator var3 = var2.iterator();	

	while (var3.hasNext())
	{
		ScoreObjective var4 = (ScoreObjective)var3.next();
		Score var5 = getWorldScoreboard().func_96529_a(getEntityName(), var4);
		var5.func_96648_a();
	}*/

	getOffVehicle();
	EntityLivingBase* pLiving = getAttackingEntity();

	if (pLiving)
	{
		if (SCRIPT_EVENT::PlayerDieEvent::invoke(this, true, dynamic_cast<EntityPlayerMP*>(pLiving)))
		{
			EntityPlayer::onDeath(pSource);
			pLiving->addToPlayerScore(this, scoreValue);
			this->isDead = true;
		}
	}
	else
	{
		if (SCRIPT_EVENT::PlayerDieEvent::invoke(this, false, reinterpret_cast<EntityPlayerMP*>(NULL)))
		{
			EntityPlayer::onDeath(pSource);
			this->isDead = true;
		}
	}

	if (this->isDead)
	{
		if (SCRIPT_EVENT::PlayerDieTipEvent::invoke(this, reinterpret_cast<EntityPlayerMP*>(NULL)))
		{
			Server::Instance()->getNetwork()->getSender()->sendSystemsChat(LanguageKey::SYSTEM_MESSAGE_PLAYER_DEATH, getEntityName(), 0);
		}
		Server::Instance()->getNetwork()->getSender()->sendEntityAttribute(this, this);
		Server::Instance()->getNetwork()->getSender()->sendEntityAttributeToTrackingPlayers(this);

		setWatchMode(true);
	}
}

bool EntityPlayerMP::attackEntityFrom(DamageSource* pSource, float amount)
{
	//if (isOnVehicle() || isEntityInvulnerable())
	if (pSource == NULL || isEntityInvulnerable())
		return false;

	bool allowPVP = true;//mcServer.isDedicatedServer() && mcServer.isPVPEnabled() && "fall".equals(pSource.damageType);

	if (!allowPVP && initialInvulnerability > 0 && pSource != DamageSource::outOfWorld)
		return false;

	EntityDamageSource* pEntityDS = dynamic_cast<EntityDamageSource*>(pSource);
	EntityPlayerMP *attackFrom = nullptr;

	if (pEntityDS)
	{
		Entity* pEntity = pSource->getEntity();
		if (pEntity == NULL)
			return false;

		if (pEntity->isClass(ENTITY_CLASS_PLAYER))
		{
			attackFrom = dynamic_cast<EntityPlayerMP*>(pEntity);
			if (!canAttackPlayer((EntityPlayer*)pEntity))
			{
				return false;
			}
		}

		if (pEntity->isClass(ENTITY_CLASS_ARROW))
		{
			EntityArrow* pArrow = (EntityArrow*)pEntity;
			Entity* pShootingEntity = pArrow->getShootingEntity();
			if (pShootingEntity == NULL || (pShootingEntity->isClass(ENTITY_CLASS_PLAYER) && !canAttackPlayer((EntityPlayer*)pShootingEntity)))
			{
				return false;
			}
		}

		if (pEntity->isClass(ENTITY_CLASS_BULLET))
		{
			EntityBullet* pBullet = (EntityBullet*)pEntity;
			Entity* pShootingEntity = pBullet->getShootingEntity();
			if (pShootingEntity == NULL || (pShootingEntity->isClass(ENTITY_CLASS_PLAYER) && !canAttackPlayer((EntityPlayer*)pShootingEntity)))
			{
				return false;
			}
		}
	}
	FloatProxy floatProxy;
	floatProxy.value = amount;

	if (!SCRIPT_EVENT::PlayerAttackedEvent::invoke(this, attackFrom, pSource->getDamageType().c_str(), &floatProxy))
		return false;
	
	float oldHealth = getHealth();
	bool isHurt = EntityPlayer::attackEntityFrom(pSource, floatProxy.value);
	if (isHurt)
		SCRIPT_EVENT::PlayerHurtEvent::invoke(this, attackFrom, pSource->getDamageType().c_str(), oldHealth - getHealth());

	return isHurt;
}

bool EntityPlayerMP::canAttackPlayer(EntityPlayer* pPlayer)
{
	return /*!mcServer.isPVPEnabled() ? false :*/ EntityPlayer::canAttackPlayer(pPlayer);
}

void EntityPlayerMP::travelToDimension(int dimension)
{
	// todo.
}

void EntityPlayerMP::sendTileEntityToPlayer(TileEntity* pTileEntity)
{
	if (pTileEntity)
	{
		// todo.
		/*Packet var2 = pTileEntity->getDescriptionPacket();

		if (var2 != null)
		{
			playerNetServerHandler.sendPacketToPlayer(var2);
		}*/
	}
}

int EntityPlayerMP::onEnchantmentModifier(Entity* target, int enchantmentModifier)
{

	FloatProxy floatProxy;
	floatProxy.value = 0;
	auto targetPlayer = dynamic_cast<EntityPlayerMP*>(target);
	if (!targetPlayer)
	{
		return enchantmentModifier;
	}

	if (!SCRIPT_EVENT::PlayerAttackedEvent::invoke(targetPlayer, this, "inFire", &floatProxy))
	{
		return 0;
	}
	return enchantmentModifier;
}

void EntityPlayerMP::placeBlock(Vector3i selectedBlockPos, Vector3 hitPos, int face)
{
	if (isWatchMode()) {
		return;
	}

	ItemStackPtr currentItem = inventory->getCurrentItem();
	if (!currentItem) {
		LordLogWarning("Failed to place block, current item stack is null");
		return;
	}

	// step 1. calc toPlaceBlockPos and meta
	Vector3i toPlaceBlockPos;
	toPlaceBlockPos.x = selectedBlockPos.x;
	toPlaceBlockPos.y = selectedBlockPos.y;
	toPlaceBlockPos.z = selectedBlockPos.z;

	vector<Vector3i>::type positions = currentItem->getItem()->getPlacePositions(Server::Instance()->getWorld(), toPlaceBlockPos, face);

	int meta = currentItem->itemDamage;
	auto itemOfToPlaceBlock = dynamic_cast<ItemBlock*>(currentItem->getItem());
	if (itemOfToPlaceBlock)
	{
		Block* toPlaceBlock = BlockManager::sBlocks[itemOfToPlaceBlock->getBlockID()];
		meta = toPlaceBlock->onBlockPlaced(Server::Instance()->getWorld(), toPlaceBlockPos, face, hitPos, meta);
	}

	// step 2. check whether player can place the block, if can not, revert the inventory and blocks
	int oldBlockId = Server::Instance()->getWorld()->getBlockId(toPlaceBlockPos);
	Block* oldBlock = oldBlockId > 0 ? BlockManager::sBlocks[oldBlockId] : NULL;
	bool canReplaceBlockOfNullCollisionBox = false;
	if (oldBlock 
		&& !SCRIPT_EVENT::PlayerPlaceBlockBoxJudgeEvent::invoke(getRaknetID(), oldBlockId)
		&& oldBlock->getCollisionBoundingBox(Server::Instance()->getWorld(), toPlaceBlockPos).isNull())
	{
		canReplaceBlockOfNullCollisionBox = true;
	}

	if ((oldBlockId != 0 && !BlockManager::isSingleSlabBlock(oldBlockId) && !canReplaceBlockOfNullCollisionBox) ||
		!SCRIPT_EVENT::PlayerPlaceBlockEvent::invoke(getRaknetID(), currentItem->getItem()->itemID, meta, toPlaceBlockPos))
	{
		inventory->setInventoryDirtyIndex(inventory->getInventoryIndexOfCurrentItem());
		Server::Instance()->getNetwork()->getSender()->sendRevertBlocks(getRaknetID(), positions);
		Server::Instance()->getNetwork()->getSender()->sendPlayerInventory(getRaknetID(), inventory);
		m_inventoryChanged = false;
		return;
	}

	// step 3. change the block and decrease item num
	if (itemOfToPlaceBlock)
	{
		setPlacingBlock(true);
	}
	if (!currentItem->tryPlaceItemIntoWorld(this, Server::Instance()->getWorld(), BlockPos(selectedBlockPos.x, selectedBlockPos.y, selectedBlockPos.z), face, hitPos))
	{
		LordLogWarning("tryPlaceItemIntoWorld failed,pos=(%d,%d,%d), face=%d", selectedBlockPos.x, selectedBlockPos.y, selectedBlockPos.z, face);
		// TODO: send back failure
	}
	if (currentItem->stackSize == 0)
	{
		inventory->setCurrentItem(nullptr);
	}
	setPlacingBlock(false);

	// step 4. notify client that block changed
	Server::Instance()->getNetwork()->getSender()->sendPlaceBlocks(getRaknetID());

}

bool EntityPlayerMP::canSyncEntity()
{
	return m_isClientReady;//!(mSynBlockContext ? mSynBlockContext->checkSync() : false) && m_isClientReady;
}

bool EntityPlayerMP::canPickupItem(Entity* pEntity, int size)
{
	EntityPlayer::canPickupItem(pEntity, size);
	auto item = dynamic_cast<EntityItem*>(pEntity);
	if (!SCRIPT_EVENT::PlayerPickupItemEvent::invoke(this->getRaknetID(), item->getEntityItem()->itemID, size, pEntity->entityId))
		return false;
	else
		return true;
}

void EntityPlayerMP::setCurrency(i64 currency)
{
	if (m_wallet && currency != -1)
	{
		m_wallet->setCurrency(currency);
		ServerNetwork::Instance()->getSender()->sendPlayerUpdateCurrency(getRaknetID(), m_wallet->getCurrency());
		SCRIPT_EVENT::PlayerCurrencyChangeEvent::invoke(this->getRaknetID(), m_wallet->getCurrency());
	}
}

void EntityPlayerMP::addCurrency(i32 currency)
{
	if (m_wallet && currency != 0)
	{
		m_wallet->addCurrency(currency);
		ServerNetwork::Instance()->getSender()->sendPlayerUpdateCurrency(getRaknetID(), m_wallet->getCurrency());
		SCRIPT_EVENT::PlayerCurrencyChangeEvent::invoke(this->getRaknetID(), m_wallet->getCurrency());
		ServerNetwork::Instance()->getSender()->notifyGetMoney(this->getRaknetID(), currency);
	}
}

void EntityPlayerMP::subCurrency(i32 currency)
{
	if (m_wallet && currency != 0)
	{
		m_wallet->subCurrency(currency);
		ServerNetwork::Instance()->getSender()->sendPlayerUpdateCurrency(getRaknetID(), m_wallet->getCurrency());
		SCRIPT_EVENT::PlayerCurrencyChangeEvent::invoke(this->getRaknetID(), m_wallet->getCurrency());
	}
}

i64 EntityPlayerMP::getCurrency()
{
	if (m_wallet)
	{
		return m_wallet->getCurrency();
	}
	return 0;
}

void EntityPlayerMP::setChangePlayerActor(bool isCanChangeActor, int changeActorCount, int needMoneyCount)
{
	if (this->isDead)
		return;

	if (changeActorCount == 0 || needMoneyCount == 0 || isCanChangeActor)
		SCRIPT_EVENT::PlayerBuyActorResultEvent::invoke(this->getRaknetID(), true);
	else 
		Server::Instance()->getNetwork()->getSender()->syncChangePlayerActorInfo(m_raknetID, changeActorCount, needMoneyCount);

}

void EntityPlayerMP::attackActorNpc(Entity * entity, PLAYER_ATTACK_TYPE damageType)
{
	if (isWatchMode()) {
		return;
	}

	if (entity)
	{
		if (damageType == PLAYER_ATTACK_DAMAGE)
		{
			SCRIPT_EVENT::PlayerAttackActorNpcEvent::invoke(this->getRaknetID(), entity->entityId);
			return;
		}
		if (damageType == PLAYER_ATTACK_GUN)
		{
			SCRIPT_EVENT::PlayerShotActorNpcEvent::invoke(this->getRaknetID(), entity->entityId);
			return;
		}
	}
}

void EntityPlayerMP::setSpeedAdditionLevel(int level)
{
	float addition = getSpeedAddition();
	EntityPlayer::setSpeedAdditionLevel(level);
	if (addition != getSpeedAddition())
		ServerNetwork::Instance()->getSender()->syncEntitySpeedAdditionLevel(entityId, level);
}

void EntityPlayerMP::updateExp(int level, float exp, int maxExp)
{
	ServerNetwork::Instance()->getSender()->sendPlayerExpInfo(getRaknetID(), level, exp, maxExp);
}
	
void EntityPlayerMP::upgradeResource(int resourceId)
{
	SCRIPT_EVENT::PlayerUpgradeResourceEvent::invoke(this->getRaknetID(), resourceId);
}


void EntityPlayerMP::setUnlockedCommodities(std::vector<std::vector<int>> unlockedCommodities)
{
	m_unlockedCommodities.clear();
	m_unlockedCommodities = unlockedCommodities;
	for (auto iter : m_unlockedCommodities)
	{
		ServerNetwork::Instance()->getSender()->sendInitUnlockedCommodity(getRaknetID(), iter);
	}
}

void EntityPlayerMP::playSkillEffect(String name, float duration, int range, Vector3 color)
{
	ServerNetwork::Instance()->getSender()->sendEntitySkillEffect(position, name, duration, range, range, color, 1);
}

void EntityPlayerMP::sendSkillEffect(int effectId)
{
	SkillEffect* pSkill = SkillSetting::getSkillEffect(effectId);
	if (!pSkill)
		return;
	if (pSkill->effectType == 0)
	{
		ServerNetwork::Instance()->getSender()->sendEntitySkillEffect(position, pSkill->name, pSkill->duration,
			pSkill->width, pSkill->height, pSkill->color, pSkill->density);
	}
	else
	{
		ServerWorld *serverWorld = dynamic_cast<ServerWorld*>(world);
		serverWorld->addSimpleEffect(pSkill->name, position, rotationYaw, pSkill->duration, 0, pSkill->density);
	}
}

void BLOCKMAN::EntityPlayerMP::playAnimate()
{
	auto sender = Server::Instance()->getNetwork()->getSender();
	NULL_RETURN_VOID(sender);
	sender->sendEntityAnimateToTackingPlayers(NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_ATTACK, entityId, true);
}

void EntityPlayerMP::setOccupation(int occupation)
{
	m_occupation = occupation;
	ServerNetwork::Instance()->getSender()->sendEntityPlayerOccupation(0, entityId, occupation);
}

void EntityPlayerMP::resetClothes(ui64 userId)
{
	UserAttrInfo attrInfo;
	if (Server::Instance()->getEnableRoom())
	{
		bool succ = Server::Instance()->getRoomManager()->getUserAttrInfo(userId, attrInfo);
		if (succ)
		{
			this->m_faceID = attrInfo.faceId;
			this->m_hairID = attrInfo.hairId;
			this->m_topsID = attrInfo.topsId;
			this->m_pantsID = attrInfo.pantsId;
			this->m_shoesID = attrInfo.shoesId;
			this->m_glassesId = attrInfo.glassesId;
			this->m_scarfId = attrInfo.scarfId;
			this->m_wingId = attrInfo.wingId;
			this->m_hatId = attrInfo.hatId;
			this->m_decorate_hatId = attrInfo.decoratehatId;
			this->m_armId = attrInfo.armId;
			this->m_tailId = attrInfo.tailId;
			this->m_crownId = attrInfo.crownId;
			this->m_bagId = attrInfo.bagId;
			this->m_extra_wingId = 0;
			this->m_decorate_hatId = 0;
			this->m_armId = 0;
			this->m_defaultIdle = attrInfo.defaultIdle;
			this->m_suitsId = attrInfo.suitsId;
			this->m_privilegeId = 0;
			this->m_actionLockedInfoMap.clear();
			for (int i = 0; i < attrInfo.unlockedSelectableActions.size(); ++i)
			{
				this->m_actionLockedInfoMap[attrInfo.unlockedSelectableActions[i]] = false;
			}

			ServerNetwork::Instance()->getSender()->sendUserAttrToTrackingPlayers(entityId, attrInfo, this);
			m_isClothesChanged = false;
		}
		else
		{
			LordLogError("reset clothes get user attr info failure");
		}

	}
}

bool EntityPlayerMP::tryBuyVehicle(int vehicleId)
{
	if (CarSetting::m_sbIsCarFree)
		return false;

	if (hasOwnVehicle(vehicleId))
		return false;

	const CarSetting* pSetting = CarSetting::getCarSetting(vehicleId);
	if (!pSetting || pSetting->price <= 0)
		return false;

	i64 money = getCurrency();
	if (money < pSetting->price)
		return false;

	subCurrency(pSetting->price);
	addOwnVehicle(vehicleId);
	syncOwnVehicle();

	SCRIPT_EVENT::PlayerBuyVehicleSuccessEvent::invoke(getRaknetID(), vehicleId);

	return true;
}

void EntityPlayerMP::syncOwnVehicle()
{
	ServerNetwork::Instance()->getSender()->sendPlayerOwnVehicle(getRaknetID(), m_ownVehicle);
}

void EntityPlayerMP::addBackpackCapacity(int capacity)
{
	if (capacity == 0)
		return;
	EntityPlayer::addBackpackCapacity(capacity);
	if (m_backpack)
		ServerNetwork::Instance()->getSender()->sendUpdateBackpack(getRaknetID(), m_backpack->getCapacity(), m_backpack->getMaxCapacity());
}

void EntityPlayerMP::subBackpackCapacity(int capacity)
{
	if (capacity == 0)
		return;
	EntityPlayer::subBackpackCapacity(capacity);
	if (m_backpack)
		ServerNetwork::Instance()->getSender()->sendUpdateBackpack(getRaknetID(), m_backpack->getCapacity(), m_backpack->getMaxCapacity());
}

void EntityPlayerMP::resetBackpack(int capacity, int maxCapacity)
{
	EntityPlayer::resetBackpack(capacity, maxCapacity);
	if (m_backpack)
		ServerNetwork::Instance()->getSender()->sendUpdateBackpack(getRaknetID(), m_backpack->getCapacity(), m_backpack->getMaxCapacity());
}

void EntityPlayerMP::SetKnockBackCoefficient(float knockBackCoefficient)
{
	this->m_knockBackCoefficient = knockBackCoefficient;
}

void EntityPlayerMP::onItemPickup(Entity* pEntity, int size)
{
	EntityPlayer::onItemPickup(pEntity, size);
	openContainer->detectAndSendChanges();
	auto item = dynamic_cast<EntityItem*>(pEntity);
	if (item)
	{
		SCRIPT_EVENT::PlayerPickupItemEndEvent::invoke(this->getRaknetID(), item->getEntityItem()->itemID, size, item->getEntityItem()->itemDamage, pEntity->entityId);
		return;
	}

	auto arrow = dynamic_cast<EntityArrow*>(pEntity);
	if (arrow)
	{
		SCRIPT_EVENT::PlayerPickupItemEndEvent::invoke(this->getRaknetID(), arrow->getArrowID(), size, 0, pEntity->entityId);
		return;
	}
}

SLEEP_RESULT EntityPlayerMP::sleepInBedAt(const BlockPos& pos)
{
	SLEEP_RESULT ret = EntityPlayer::sleepInBedAt(pos);

	if (ret == SLEEP_RESULT_OK)
	{
		/*Packet17Sleep var5 = new Packet17Sleep(this, 0, par1, par2, par3);
		getServerForPlayer().getEntityTracker().sendPacketToAllPlayersTrackingEntity(this, var5);
		playerNetServerHandler.setPlayerLocation(posX, posY, posZ, rotationYaw, rotationPitch);
		playerNetServerHandler.sendPacketToPlayer(var5);*/
	}

	return ret;
}

void EntityPlayerMP::wakeUpPlayer(bool immediately, bool update, bool setSpawn)
{
	if (isPlayerSleeping())
	{
		// todo.
		// getServerForPlayer().getEntityTracker().sendPacketToAllAssociatedPlayers(this, new Packet18Animation(this, 3));
	}

	EntityPlayer::wakeUpPlayer(immediately, update, setSpawn);

	// todo.
	/*if (playerNetServerHandler != null)
	{
		playerNetServerHandler.setPlayerLocation(posX, posY, posZ, rotationYaw, rotationPitch);
	}*/
}

void EntityPlayerMP::sendSlotContents(Container* pContainer, int slot, ItemStackPtr pStack)
{

	if (!(dynamic_cast<SlotCrafting*>(pContainer->getSlot(slot))))
	{
		if (!playerInventoryBeingManipulated)
		{
			// todo.
			// playerNetServerHandler.sendPacketToPlayer(new Packet103SetSlot(pContainer.windowId, slot, pStack));
		}
	}
}

void EntityPlayerMP::sendContainerAndContentsToPlayer(Container* pContainer, ItemStackArr& stacks)
{
	// playerNetServerHandler.sendPacketToPlayer(new Packet104WindowItems(pContainer.windowId, stacks));
	// playerNetServerHandler.sendPacketToPlayer(new Packet103SetSlot(-1, -1, inventory.getItemStack()));
}

void EntityPlayerMP::sendProgressBarUpdate(Container* pContainer, int toUpdate, int val)
{
	// playerNetServerHandler.sendPacketToPlayer(new Packet105UpdateProgressbar(pContainer.windowId, toUpdate, val));
}

void EntityPlayerMP::sendContainerToPlayer(Container* pContainer)
{
	ItemStackArr stacks;
	pContainer->getInventory(stacks);
	sendContainerAndContentsToPlayer(pContainer, stacks);
}

void EntityPlayerMP::closeScreen()
{
	// todo.
	// playerNetServerHandler.sendPacketToPlayer(new Packet101CloseWindow(openContainer.windowId));
	closeContainer();
}

void EntityPlayerMP::updateHeldItem()
{
	if (!playerInventoryBeingManipulated)
	{
		// todo.
		// playerNetServerHandler.sendPacketToPlayer(new Packet103SetSlot(-1, -1, inventory.getItemStack()));
	}
}

void EntityPlayerMP::closeContainer()
{
	// no need to operator container in server side... just tell all the clients that the chest is closed by some body.
	// openContainer->onContainerClosed(this);
	// openContainer = inventoryContainer;
}

void EntityPlayerMP::setAllowFlying(bool allowFlying)
{
	EntityPlayer::setAllowFlying(allowFlying);
	Server::Instance()->getNetwork()->getSender()->sendSetAllowFlying(getRaknetID(), allowFlying);
}

void EntityPlayerMP::setWatchMode(bool isWatchMode)
{
	EntityPlayer::setWatchMode(isWatchMode);
	Server::Instance()->getNetwork()->getSender()->sendSetWatchMode(getRaknetID(), entityId, isWatchMode);
	Server::Instance()->getNetwork()->getSender()->sendSetWatchModeToTrackingPlayers(entityId, isWatchMode);
}

void EntityPlayerMP::addChatMessage(const String& msg)
{
	// todo.
	// playerNetServerHandler.sendPacketToPlayer(new Packet3Chat(ChatMessageComponent.func_111077_e(msg)));
}

void EntityPlayerMP::onItemUseFinish()
{
	// todo.
	// playerNetServerHandler.sendPacketToPlayer(new Packet38EntityStatus(entityId, (byte)9));
	EntityPlayer::onItemUseFinish();
}

bool EntityPlayerMP::useItem(NETWORK_DEFINE::C2SPacketUseItemUseType useType, const Vector3i& pos, int param0)
{
	bool bResult = false;
	ItemStackPtr currentItem = inventory->getCurrentItem();
	if (!currentItem) {
		LordLogError("EntityPlayerMP::useItem but current item in hand is nullptr");
		return bResult;
	}

	int size = currentItem->stackSize;
	ItemStackPtr pStackAfterUsed;
	ItemBucket* bucket = nullptr;

	switch (useType)
	{
	case NETWORK_DEFINE::C2SPacketUseItemUseType::ITEM_USE_TYPE_EASTABLE:
		pStackAfterUsed = currentItem->onFoodEaten(world, this);
		break;

	case NETWORK_DEFINE::C2SPacketUseItemUseType::ITEM_USE_TYPE_BUCKET:
		bucket = dynamic_cast<ItemBucket*>(currentItem->getItem());
		if (!bucket) {
			return bResult;
		}
		pStackAfterUsed = bucket->doIt(currentItem, world, this, pos, param0);
		break;
	case NETWORK_DEFINE::C2SPacketUseItemUseType::ITEM_USE_TYPE_COMMON:
		pStackAfterUsed = currentItem->useItemRightClick(world, this);
		break;

	default:
		return bResult;
	}
	
	if (pStackAfterUsed != itemInUse || (pStackAfterUsed && pStackAfterUsed->stackSize != size))
	{
		inventory->setInventorySlotContents(inventory->getInventoryIndexOfCurrentItem(), pStackAfterUsed);

		if (pStackAfterUsed->stackSize == 0)
		{
			inventory->setInventorySlotContents(inventory->getInventoryIndexOfCurrentItem(), nullptr);
		}

		bResult = true;
	}

	if (currentItem->itemID == ITEM_ID_FIRE_BALL)
	{
		bResult = true;
	}

	if (bResult)
	{
		SCRIPT_EVENT::PlayerUseCommonItemEvent::invoke(this->getRaknetID(), currentItem->itemID);
	}

	return bResult;
}

void EntityPlayerMP::updatePotionMetadata()
{
	EntityLivingBase::updatePotionMetadata();
}

void EntityPlayerMP::setItemInUse(ItemStackPtr pStack, int duration)
{
	EntityPlayer::setItemInUse(pStack, duration);

	if (pStack && pStack->getItem() && pStack->getItem()->getItemUseAction(pStack) == ACTION_TYPE_EAT)
	{
		// todo.
		// getServerForPlayer().getEntityTracker().sendPacketToAllAssociatedPlayers(this, new Packet18Animation(this, 5));
	}
}

void EntityPlayerMP::clonePlayer(EntityPlayer* pPlayer, bool all)
{
	EntityPlayer::clonePlayer(pPlayer, all);
	if (all)
	{
		m_inventoryChanged = true;
		auto entityPlayerMP = dynamic_cast<EntityPlayerMP*>(pPlayer);
		if (entityPlayerMP)
		{
			if (pPlayer->m_wallet->getCurrency() >= 0)
				setCurrency(pPlayer->m_wallet->getCurrency());
			if (!pPlayer->m_actorName.empty())
				world->changePlayerActor(this, pPlayer->m_actorName, pPlayer->m_actorBodyId);
			m_isClientReady = entityPlayerMP->m_isClientReady;
			changeMaxHealth(pPlayer->getMaxHealth());
			setEntityHealth(pPlayer->getHealth());
			if (m_bagMaxCapacity != 1)
				setBagInfo(entityPlayerMP->m_bagCapacity, m_bagMaxCapacity);
			setScale(entityPlayerMP->m_scale);
			auto entityMap = world->getLoadedEntityMap();
			for (auto entity : *entityMap)
			{
				auto merchant = dynamic_cast<EntityMerchant*>(entity.second);
				if (merchant)
				{
					CommodityMapping mapping = CommodityManager::Instance()->getMapping(merchant->entityId, entityPlayerMP->getRaknetID());
					CommodityManager::Instance()->addMapping(merchant->entityId, getRaknetID(), mapping.index);
				}
			}
		}
	}
}

void EntityPlayerMP::onNewPotionEffect(PotionEffect* pEff)
{
	EntityPlayer::onNewPotionEffect(pEff);
	Server::Instance()->getNetwork()->getSender()->sendAddEntityEffectToTrackingPlayers(entityId, pEff);
}

void EntityPlayerMP::onChangedPotionEffect(PotionEffect* pEff, bool apply)
{
	EntityPlayer::onChangedPotionEffect(pEff, apply);
	Server::Instance()->getNetwork()->getSender()->sendAddEntityEffectToTrackingPlayers(entityId, pEff);
}

void EntityPlayerMP::onFinishedPotionEffect(PotionEffect* pEff)
{
	EntityPlayer::onFinishedPotionEffect(pEff);
	Server::Instance()->getNetwork()->getSender()->sendRemoveEntityEffectToTrackingPlayers(entityId, pEff->getPotionID());
	SCRIPT_EVENT::PlayerPotionEffectFinishedEvent::invoke(this->getRaknetID(), pEff->getPotionID());
}

bool EntityPlayerMP::setToLastPosition(float x, float y, float z)
{
	if (x != position.x || z != position.z)
	{
		float yaw = rotationYaw;
		float pitch = rotationPitch;
		auto posX = static_cast<int>(Math::Floor(position.x * 32.F));
		auto posY = static_cast<int>(Math::Floor(position.y * 32.F));
		auto posZ = static_cast<int>(Math::Floor(position.z * 32.F));
		Server::Instance()->getNetwork()->getSender()->sendEntityTeleport(getRaknetID(), entityId, posX, posY, posZ, yaw, pitch);
		return true;
	}

	return false;
}

void EntityPlayerMP::changeMaxHealth(float health, bool unChangeCurrent/* = false*/)
{
	float lastBaseHealth = getBaseHealth();
	EntityPlayer::changeMaxHealth(health, unChangeCurrent);
	float curBaseHealth = getBaseHealth();
	if (lastBaseHealth != curBaseHealth)
	{
		Server::Instance()->getNetwork()->getSender()->broadCastChangePlayerMaxHealth(entityId, curBaseHealth, unChangeCurrent);
	}
}

void EntityPlayerMP::changeMaxHealthSingle(float maxHealth)
{
	float lastBaseHealth = getBaseHealth();
	EntityPlayer::changeMaxHealthSingle(maxHealth);
	float curBaseHealth = getBaseHealth();
	if (lastBaseHealth != curBaseHealth)
	{
		Server::Instance()->getNetwork()->getSender()->broadCastChangePlayerMaxHealth(entityId, curBaseHealth, true);
	}
}

bool EntityPlayerMP::equipArmor(int id, int damage)
{
	if (this->isDead)
		return false;
	int slot = inventory->getFirstEmptyStackInInventory();
	if (slot < 0)
		return false;

	ItemStackPtr itemStack = LORD::make_shared<ItemStack>(Item::getItemById(id), 1, damage);
	inventory->setInventorySlotContents(slot, itemStack);
	if (inventory->equipArmor(itemStack))
	{
		Server::Instance()->getNetwork()->getSender()->sendEquipArmors(this, entityId, { itemStack });
		Server::Instance()->getNetwork()->getSender()->sendEquipArmorsToTrackingPlayers(entityId, { itemStack });
		SCRIPT_EVENT::PlayerArmorChangeEvent::invoke(this->getRaknetID());
		return true;
	}
	return false;
}

void EntityPlayerMP::setPositionAndUpdate(float x, float y, float z)
{
	auto peer = Server::Instance()->getNetwork()->findPlayerByRakssid(getRaknetID());
	if (!peer)
		return;

	peer->setPlayerLocation({x, y, z}, rotationYaw, rotationPitch);
	auto sender = Server::Instance()->getNetwork()->getSender();
	if (world->getPlayerEntity(entityId) == NULL) // is loging in!!!!!
	{
		float yaw = rotationYaw;
		float pitch = rotationPitch;
		auto posX = static_cast<int>(Math::Floor(x * 32.F));
		auto posY = static_cast<int>(Math::Floor(y * 32.F));
		auto posZ = static_cast<int>(Math::Floor(z * 32.F));
		sender->sendEntityTeleport(getRaknetID(), entityId, posX, posY, posZ, yaw, pitch);
	}
	else
	{
		sender->broadCastPlayerMovement(entityId, true);
		sender->broadCastPlayerRotation(entityId, true);
	}
	
}

void EntityPlayerMP::onCriticalHit(Entity* pEntity)
{
	// todo.
	// getServerForPlayer().getEntityTracker().sendPacketToAllAssociatedPlayers(this, new Packet18Animation(pEntity, 6));
}

void EntityPlayerMP::onEnchantmentCritical(Entity* pEntity)
{
	// todo.
	// getServerForPlayer().getEntityTracker().sendPacketToAllAssociatedPlayers(this, new Packet18Animation(pEntity, 7));
}

void EntityPlayerMP::sendPlayerAbilities()
{
	// todo.
	/*if (playerNetServerHandler != null)
	{
		playerNetServerHandler.sendPacketToPlayer(new Packet202PlayerAbilities(capabilities));
	}*/
}

void EntityPlayerMP::setGameType(GAME_TYPE type)
{
	// todo.
	// theItemInWorldManager.setGameType(type);
	// playerNetServerHandler.sendPacketToPlayer(new Packet70GameEvent(3, type.getID()));
	if (type == GAME_TYPE_CREATIVE)
	{
		capabilities.allowFlying = true;
		capabilities.isCreativeMode = true;
		capabilities.disableDamage = true;
		capabilities.isWatchMode = false;
	}
	else
	{
		capabilities.allowFlying = false;
		capabilities.isCreativeMode = false;
		capabilities.disableDamage = false;
		capabilities.isFlying = false;
		capabilities.isWatchMode = false;
	}

	capabilities.allowEdit = type != GAME_TYPE_ADVENTURE;
}

void EntityPlayerMP::notifyClientReloadBullet()
{
	Server::Instance()->getNetwork()->getSender()->sendReloadBullet(m_raknetID);
}

void EntityPlayerMP::sendGetBulletSound()
{
	Server::Instance()->getNetwork()->getSender()->sendPlaySoundByType(m_raknetID, (int)ST_FemalePlayerBeHit);
}

bool EntityPlayerMP::takeOnAircraft(EntityAircraft * pAircraft)
{
	bool bRet = EntityPlayer::takeOnAircraft(pAircraft);
	if (bRet)
	{
		setPositionAndUpdate(position.x, position.y, position.z);
		Server::Instance()->getNetwork()->getSender()->broadcastTakeAircraft(entityId , true, pAircraft->entityId);
		//LordLogInfo("+++++EntityPlayerMP id %d takeOnAircraft pos(%f, %f, %f)", entityId, position.x, position.y, position.z);
	}
	return bRet;
}

bool EntityPlayerMP::takeOffAircraft()
{
	int aircraftId = m_aircraftEntityId;
	bool bRet = EntityPlayer::takeOffAircraft();
	if (bRet)
	{
		setPositionAndUpdate(position.x, position.y, position.z);
		Server::Instance()->getNetwork()->getSender()->broadcastTakeAircraft(entityId, false, aircraftId);
		//LordLogInfo("-----EntityPlayerMP id %d takeOffAircraft pos(%f, %f, %f)", entityId, position.x, position.y, position.z);
	}
	return bRet;
}

bool EntityPlayerMP::getOnVehicle(EntityVehicle* vehicle, bool bForce, bool bIsDriver)
{
	if (EntityPlayer::getOnVehicle(vehicle, bForce, bIsDriver))
	{
		auto sender = Server::Instance()->getNetwork()->getSender();
		sender->broadCastSyncTakeVehicle(entityId, true, bIsDriver, vehicle->entityId);
		return true;
	}
	return false;
}

bool EntityPlayerMP::getOffVehicle()
{
	int vehicleId = m_vehicleEntityId;
	if (EntityPlayer::getOffVehicle())
	{
		auto sender = Server::Instance()->getNetwork()->getSender();
		sender->broadCastSyncTakeVehicle(entityId, false, false, vehicleId);
		return true;
	}
	return false;
}

bool BLOCKMAN::EntityPlayerMP::getOnFlyingMount(EntityFlyingMount* mount)
{
	if (!EntityPlayer::getOnFlyingMount(mount))
		return false;
	auto sender = Server::Instance()->getNetwork()->getSender();
	sender->broadcastGetOnFlyingMount(entityId, mount->entityId);
	return true;
}

bool BLOCKMAN::EntityPlayerMP::getOffFlyingMount()
{
	if (!EntityPlayer::getOffFlyingMount())
		return false;
	auto sender = Server::Instance()->getNetwork()->getSender();
	sender->broadcastGetOffFlyingMount(entityId);
	return true;
}

bool BLOCKMAN::EntityPlayerMP::getOnFloatingMount(EntityFloatingMount* mount)
{
	if (!EntityPlayer::getOnFloatingMount(mount))
		return false;
	auto sender = Server::Instance()->getNetwork()->getSender();
	sender->broadcastGetOnFloatingMount(entityId, mount->entityId);
	return true;
}

bool BLOCKMAN::EntityPlayerMP::getOffFloatingMount()
{
	if (!EntityPlayer::getOffFloatingMount())
		return false;
	auto sender = Server::Instance()->getNetwork()->getSender();
	sender->broadcastGetOffFloatingMount(entityId);
	return true;
}

void BLOCKMAN::EntityPlayerMP::setOpenChest(TileEntityChest * chest)
{
	m_openChest = chest;
	m_openChest->openChest();
}

void EntityPlayerMP::closeOpenChest()
{
	if (m_openChest)
	{
		auto iter = find(m_openChest->openChestPlayers.begin(), m_openChest->openChestPlayers.end(), this->entityId);
		if (iter != m_openChest->openChestPlayers.end())
		{
			m_openChest->openChestPlayers.erase(iter);
		}
		m_openChest->closeChest();
	}
}

void BLOCKMAN::EntityPlayerMP::setOpenFurnace(TileEntityFurnace * furnace)
{
	m_openFurnace = furnace;
	m_openFurnace->openChest();
}

void BLOCKMAN::EntityPlayerMP::closeOpenFurnace()
{
	if (m_openFurnace)
	{
		m_openFurnace->closeChest();
	}
}

void EntityPlayerMP::sendChatToPlayer(ChatMessageComponent* chatComponent)
{
	// todo.
	// playerNetServerHandler.sendPacketToPlayer(new Packet3Chat(chatComponent));
}

bool EntityPlayerMP::canCommandSenderUseCommand(int par1, const String& par2Str)
{
	// todo.
	// return "seed".equals(par2Str) && !mcServer.isDedicatedServer() ? true : (!"tell".equals(par2Str) && !"help".equals(par2Str) && !"me".equals(par2Str) ? (mcServer.getConfigurationManager().areCommandsAllowed(username) ? mcServer.func_110455_j() >= par1 : false) : true);
	return true;

}

String EntityPlayerMP::getPlayerIP()
{
	// todo.
	return String("127.0.0.1");
}

void EntityPlayerMP::requestTexturePackLoad(const String& par1Str, int par2)
{
	// todo.
	/*
	String var3 = par1Str + "\u0000" + par2;
	playerNetServerHandler.sendPacketToPlayer(new Packet250CustomPayload("MC|TPack", var3.getBytes()));
	*/
}

BlockPos EntityPlayerMP::getPlayerCoordinates()
{
	BlockPos pos;
	pos.x = int(Math::Floor(position.x));
	pos.z = int(Math::Floor(position.z));
	pos.y = int(Math::Floor(position.y + 0.5f));
	return pos;
}

bool EntityPlayerMP::setCameraLocked(bool bLocked)
{
	if (isOnVehicle() && bLocked)
	{
		getOffVehicle();
	}

	if (m_isCameraLocked != bLocked)
	{
		EntityPlayer::setCameraLocked(bLocked);
		SCRIPT_EVENT::PlayerSetCameraLockedEvent::invoke(getRaknetID(), m_isCameraLocked);
		ServerNetwork::Instance()->getSender()->broadCastCameraLockToClient(bLocked, entityId);
		return true;
	}
	return false;
}

void BLOCKMAN::EntityPlayerMP::changeInvisible(ui64 rakssId, bool isInvisible)
{
	if (rakssId == 0)
		ServerNetwork::Instance()->getSender()->broadCastPlayerInvisible(entityId, isInvisible);
	else
		ServerNetwork::Instance()->getSender()->sendPlayerInvisible(rakssId, entityId, isInvisible);
}

void BLOCKMAN::EntityPlayerMP::changeHeart(int hp, int maxHp)
{
	ServerNetwork::Instance()->getSender()->sendPlayerChangeHeart(getRaknetID(), hp, maxHp);
}

void BLOCKMAN::EntityPlayerMP::showBossStripWithIcon(String name, int curProgress, int maxProgress, String iconPath)
{
	ServerNetwork::Instance()->getSender()->sendPlayerBossStripWithIcon(getRaknetID(), name, curProgress, maxProgress, iconPath);
}

//void BLOCKMAN::EntityPlayerMP::changeIsCanJump(bool isCanJump)
//{
//	ServerNetwork::Instance()->getSender()->sendPlayerChangeIsCanJump(getRaknetID(), isCanJump);
//}

void BLOCKMAN::EntityPlayerMP::setScale(float s)
{
	if (Math::floatEqual(m_scale, s))
	{
		return;
	}

	m_scale = s;

	//SCRIPT_EVENT::SyncEntityPlayerInfoEvent::invoke(getPlatformUserId(), entityId, "scale", StringUtil::ToString(m_scale));
}

void BLOCKMAN::EntityPlayerMP::changeDefense(int defense, int maxDefense)
{
	ServerNetwork::Instance()->getSender()->sendPlayerChangeDefense(getRaknetID(), defense, maxDefense);
}

void BLOCKMAN::EntityPlayerMP::changeAttack(int attack, int maxAttack)
{
	ServerNetwork::Instance()->getSender()->sendPlayerChangeAttack(getRaknetID(), attack, maxAttack);
}

void BLOCKMAN::EntityPlayerMP::changeApple(int cur, int max)
{
	ServerNetwork::Instance()->getSender()->sendPlayerChangeApple(getRaknetID(), cur, max);
}

void BLOCKMAN::EntityPlayerMP::appleFresh(int time)
{
	ServerNetwork::Instance()->getSender()->sendPlayerAppleFresh(getRaknetID(), time);
}

void BLOCKMAN::EntityPlayerMP::setPersonalShopArea(Vector3 startPos, Vector3 endPos)
{
	ServerNetwork::Instance()->getSender()->sendSetPersonalShopArea(getRaknetID(), startPos, endPos);
}

void BLOCKMAN::EntityPlayerMP::changeSwitchableProps(String data)
{
	ServerNetwork::Instance()->getSender()->sendSwitchablePropsData(getRaknetID(), data);
}

void BLOCKMAN::EntityPlayerMP::changeUpgradeProps(String data)
{
	ServerNetwork::Instance()->getSender()->sendUpgradePropsData(getRaknetID(), data);
}

void BLOCKMAN::EntityPlayerMP::changeEnchantmentProps(String data)
{
	ServerNetwork::Instance()->getSender()->sendEnchantmentPropsData(getRaknetID(), data);
}

void BLOCKMAN::EntityPlayerMP::changeSuperProps(String data)
{
	ServerNetwork::Instance()->getSender()->sendSuperPropsData(getRaknetID(), data);
}

void BLOCKMAN::EntityPlayerMP::changeCustomProps(String data)
{
	ServerNetwork::Instance()->getSender()->sendCustomPropsData(getRaknetID(), data);
}

void BLOCKMAN::EntityPlayerMP::updateCustomProps(String data)
{
	ServerNetwork::Instance()->getSender()->updateCustomProps(getRaknetID(), data);
}

void BLOCKMAN::EntityPlayerMP::attackCreatureEntity(EntityCreature* entityCreature, int attackType)
{
	if (isWatchMode()) {
		return;
	}

	if (entityCreature)
	{
		float damage = attributeMap->getAttributeInstance(SharedMonsterAttributes::ATTACK_DAMAGE)->getAttributeValue();

		if (PlayerPrefs::Instance()->getBoolPrefs("IsUseCustomArmor"))
		{
			// float baseDamage = attributeMap->getAttributeInstance(SharedMonsterAttributes::ATTACK_DAMAGE)->getBaseValue();
			// damage -= baseDamage;
			if (inventory)
			{
				damage += inventory->getTotalArmorCustomAttackValue();
			}
		}

		int itemID = 0;
		if (this->inventory && this->inventory->getCurrentItem() && this->inventory->getCurrentItem()->getItem())
		{
			itemID = this->inventory->getCurrentItem()->getItem()->itemID;
		}
		if (!SCRIPT_EVENT::PlayerAttackCreatureEvent::invoke(this->getRaknetID(), entityCreature->entityId, attackType, entityCreature->getPosition(), damage, itemID))
			return;
		entityCreature->changeCreatureAction(CreatureActionState::CREATURE_AS_BE_PLAYER_ATTACK);
		entityCreature->notifyAttackFrom(this);
	}
}

void BLOCKMAN::EntityPlayerMP::initUpdateBlocks()
{
	auto blocks = BlockChangeRecorderServer::Instance()->getChangedPositions();
	if (mSynBlockContext)
	{
		mSynBlockContext->init(position, blocks);
		updateMoveEventInvoke();
	}
}

void BLOCKMAN::EntityPlayerMP::sendUpdateBlocks()
{
	if (mSynBlockContext)
		mSynBlockContext->sync(m_raknetID);
}

void BLOCKMAN::EntityPlayerMP::addUpdateBlocks(std::vector<BlockPos>& blocks)
{
	for (const auto& position : blocks)
	{
		addSingleUpdateBlock(position, world->getBlockId(position), world->getBlockMeta(position));
	}
}

void BLOCKMAN::EntityPlayerMP::addSingleUpdateBlock(BlockPos pos, int block_id, int meta)
{
	if (mSynBlockContext)
		mSynBlockContext->add(position, pos, block_id, meta);
}

int BLOCKMAN::EntityPlayerMP::getSynblockNum()
{
	if (mSynBlockContext)
		return mSynBlockContext->getblockNum();
	return 0;
}

void BLOCKMAN::EntityPlayerMP::setExtraInventoryCount(int count)
{
	inventory->setExtraInventoryCount(count);
	Server::Instance()->getNetwork()->getSender()->sendPlayerInventory(getRaknetID(), inventory);
}

void EntityPlayerMP::updateMoveEventInvoke()
{
	if (mSynBlockContext)
		mSynBlockContext->move(position);
}

int EntityPlayerMP::getSlotByItemId(int ItemId)
{
	int slot = inventory->getItemSlotByHotbat(ItemId);
	return slot;
}

void BLOCKMAN::EntityPlayerMP::setArmItem(int itemId)
{
	Server::Instance()->getNetwork()->getSender()->sendSetArmItem(getRaknetID(), itemId);
}

void BLOCKMAN::EntityPlayerMP::addGunBulletNum(int gunId, int bulletNum)
{
	EntityPlayer::addGunBulletNum(gunId, bulletNum);
	Server::Instance()->getNetwork()->getSender()->sendAddGunBulletNum(getRaknetID(), gunId, bulletNum);
}

void BLOCKMAN::EntityPlayerMP::subGunRecoil(int gunId, float recoil)
{
	EntityPlayer::subGunRecoil(gunId, recoil);
	Server::Instance()->getNetwork()->getSender()->sendSubGunRecoil(getRaknetID(), gunId, recoil);
}

void BLOCKMAN::EntityPlayerMP::subGunFireCd(int gunId, float fireCd)
{
	EntityPlayer::subGunFireCd(gunId, fireCd);
	Server::Instance()->getNetwork()->getSender()->sendSubGunFireCd(getRaknetID(), gunId, fireCd);
}

void BLOCKMAN::EntityPlayerMP::setOnFrozen(float frozenTime)
{
	EntityPlayer::setOnFrozen(frozenTime);
	ServerNetwork::Instance()->getSender()->syncEntityFrozenTime(0, entityId, frozenTime);
}

void BLOCKMAN::EntityPlayerMP::setOnHurt(float hurtTime)
{
	EntityPlayer::setOnHurt(hurtTime);
	ServerNetwork::Instance()->getSender()->syncEntityHurtTime(0, entityId, hurtTime);
}

void BLOCKMAN::EntityPlayerMP::setOnColorful(float colorfulTime)
{
	EntityPlayer::setOnColorful(colorfulTime);
	ServerNetwork::Instance()->getSender()->syncEntityColorfulTime(0, entityId, colorfulTime);
}

void BLOCKMAN::EntityPlayerMP::setDisableMove(float disableMoveTime)
{
	EntityPlayer::setDisableMove(disableMoveTime);
	ServerNetwork::Instance()->getSender()->syncEntityDisableMoveTime(0, entityId, disableMoveTime);
}

void BLOCKMAN::EntityPlayerMP::setOnFire(int seconds)
{
	fire = seconds * 20;
}

void EntityPlayerMP::attackEntity(Entity * entity)
{
	if (isWatchMode()) {
		return;
	}

	switch (entity->getClassID())
	{
	case ENTITY_CLASS_SESSION_NPC:
		SCRIPT_EVENT::PlayerAttackSessionNpcEvent::invoke(this->getRaknetID(), entity->hashCode());
		break;
	case ENTITY_CLASS_BLOCKMAN:
	case ENTITY_CLASS_BLOCKMAN_EMPTY:
		attackTargetEntityWithCurrentItem(entity);
		break;
	case ENTITY_CLASS_BUILD_NPC:
		attackTargetEntityWithCurrentItem(entity);
		break;
	case ENTITY_CLASS_DECORATION:
		if (!SCRIPT_EVENT::PlayerAttackDecorationEvent::invoke(this->getRaknetID(), entity->hashCode()))
		{
			if (auto entityDecoration = dynamic_cast<EntityDecoration*>(entity))
			{
				entityDecoration->changeAction();
			}
		}
		break;
	}
}

void BLOCKMAN::EntityPlayerMP::enableMovement()
{
	EntityPlayer::enableMovement();
	ServerNetwork::Instance()->getSender()->sendPlayerEnableMovement(this->hashCode());
}

void BLOCKMAN::EntityPlayerMP::checkForceMovement()
{
	m_movePlayerUpdateTick++;
	if (m_movePlayerUpdateTick % 80 != 0)
		return;
	if (isPlayerMoved())
	{
		ServerNetwork::Instance()->getSender()->broadCastPlayerMovement(entityId, false);
		recordOldPos();
	}
}

bool BLOCKMAN::EntityPlayerMP::isPlayerMoved(bool precise/* = false*/)
{
	float dx = position.x - oldPos.x;
	float dy = boundingBox.vMin.y - oldMinY;
	float dz = position.z - oldPos.z;
	return dx * dx + dy * dy + dz * dz > 0.25;
}

void EntityPlayerMP::recordOldPos()
{
	oldPos = position;
	oldMinY = boundingBox.vMin.y;
}

void EntityPlayerMP::setBagInfo(int curCapacity, int maxCapacity)
{
	m_bagCapacity = curCapacity;
	m_bagMaxCapacity = maxCapacity;
	ServerNetwork::Instance()->getSender()->syncPlayerBagInfo(0, this);
}

void EntityPlayerMP::setBirdConvert(bool isConvert)
{
	m_isBirdConvert = isConvert;
	ServerNetwork::Instance()->getSender()->syncPlayerBagInfo(0, this);
}

void EntityPlayerMP::setFlyWing(bool isEquipWing, float glideMoveAddition, float glideDropResistance)
{
	m_isEquipWing = isEquipWing;
	m_glideMoveAddition = glideMoveAddition;
	m_glideDropResistance = glideDropResistance;
	ServerNetwork::Instance()->getSender()->broadCastSetPlayerGlide(this);
}

void EntityPlayerMP::setGlide(bool isOpenGlide)
{
	if (isOpenGlide && m_flyingByPulled)
	{
		m_remainingFlyTicksByPulled = 0;
		motion = Vector3::ZERO;
		m_flyingByPulled = false;
		m_isPulled = true;
		capabilities.isParachute = false;
	}
	m_isGlide = isOpenGlide;
	ServerNetwork::Instance()->getSender()->broadCastSetPlayerGlide(this);
}

void EntityPlayerMP::setElevator(bool isUse, float speed, float arriveHeight, i32 arriveFloor, i32 elevatorId)
{
	m_isUseElevator = isUse;
	m_elevatorSpeed = speed;
	m_elevatorArriveHeight = arriveHeight;
	m_elevatorArriveFloor = arriveFloor;
	m_elevatorId = elevatorId;
	if (m_isGlide)
	{
		SCRIPT_EVENT::PlayerOpenGlideEvent::invoke(getPlatformUserId(), false);
		setGlide(false);
	}
	ServerNetwork::Instance()->getSender()->sendSetPlayerElevator(getRaknetID(), this);
}

void BLOCKMAN::EntityPlayerMP::setFlying(bool isFlying)
{
	capabilities.isFlying = isFlying;
	auto packet = LORD::make_shared<S2CPacketSyncPlayerSetFlyingState>();
	packet->playerEntityId = entityId;
	packet->isFlying = isFlying;
	Server::Instance()->getNetwork()->getSender()->sendPacketToTrackingPlayers(entityId, packet, true);
}

int BLOCKMAN::EntityPlayerMP::getClothesId(String partName)
{
	if (partName == "custom_face")
		return this->m_faceID;
	if (partName == "custom_hair")
		return this->m_hairID;
	if (partName == "clothes_tops")
		return this->m_topsID;
	if (partName == "clothes_pants")
		return this->m_pantsID;
	if (partName == "custom_shoes")
		return this->m_shoesID;
	if (partName == "custom_glasses")
		return this->m_glassesId;
	if (partName == "custom_scarf")
		return this->m_scarfId;
	if (partName == "custom_wing")
		return this->m_wingId;
	if (partName == "custom_hat")
		return this->m_hatId;
	if (partName == "custom_decorate_hat")
		return this->m_decorate_hatId;
	if (partName == "custom_hand")
		return this->m_armId;
	if (partName == "custom_tail")
		return this->m_tailId;
	if (partName == "custom_crown")
		return this->m_crownId;
	if (partName == "custom_bag")
		return this->m_bagId;
	if (partName == "custom_wing_flag")
		return this->m_extra_wingId;
	if (partName == "custom_foot_halo")
		return this->m_footHaloId;
	if (partName == "custom_back_effect")
		return this->m_backEffectId;
	if (partName == "custom_head_effect")
		return this->m_headEffectId;
	if (partName == "custom_privilege")
		return this->m_privilegeId;
	return 0;
}

i64 BLOCKMAN::EntityPlayerMP::getDiamond()
{
	if (m_wallet)
		return m_wallet->getDiamondsGolds();
	return 0;
}

i64 BLOCKMAN::EntityPlayerMP::getGold()
{
	if (m_wallet)
		return m_wallet->getGolds();
	return 0;
}

void BLOCKMAN::EntityPlayerMP::changeNamePerspective(ui64 rakssId, bool isPerspective)
{
	if (rakssId != 0)
		ServerNetwork::Instance()->getSender()->sendPlayerNamePerspective(rakssId, entityId, isPerspective);
}

void BLOCKMAN::EntityPlayerMP::setHoldItemId(int id)
{
	m_holdItemId = id;
}

int BLOCKMAN::EntityPlayerMP::getBulletNum(int stackIndex)
{
	if (this->isDead)
		return 0;
	ItemStackPtr itemStack = inventory->getItemIdBySlot(stackIndex);
	if (itemStack)
	{
		return itemStack->getCurrentClipBulletNum();
	}
	return 0;
}

}
