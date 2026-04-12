#include "ItemStack.h"

#include "Entity/Enchantment.h"
#include "Entity/EntityLivingBase.h"
#include "Entity/EntityPlayer.h"
#include "Block/Block.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Nbt/NBT.h"
#include "Inventory/InventoryPlayer.h"
#include "Setting/LogicSetting.h"
#include "World/World.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Network/CommonPacketSender.h"
#include "Common.h"
#include "Util/CommonEvents.h"

namespace BLOCKMAN
{

ItemStack::ItemStack()
	: itemID(0)
	, animationsToGo(0)
	, stackSize(0)
	, itemDamage(0)
	, subItemID(0)
	, stackTagCompound(NULL)
	, m_currentClipBulletNum(0)
	, m_totalBulletNum(0)
	, m_cd(0.0f)
{}

ItemStack::ItemStack(Block* pBlock, int size, int meta)
	: itemID(pBlock->getItemBlockId())
	, animationsToGo(0)
	, stackSize(size)
	, itemDamage(meta)
	, subItemID(meta)
	, stackTagCompound(NULL)
	, m_currentClipBulletNum(0)
	, m_totalBulletNum(0)
	, m_cd(0.0f)
{}

ItemStack::ItemStack(Item* item, int size, int meta)
	: animationsToGo(0)
	, stackSize(size)
	, itemDamage(meta)
	, subItemID(meta)
	, stackTagCompound(NULL)
	, m_currentClipBulletNum(0)
	, m_totalBulletNum(0)
	, m_cd(0.0f)
{
	if (!item)
	{
		LordLogError("Attention!!! Intend to Init ItemStack by nullptr item! make its id as 256!!");
		itemID = 256;
	}
	else 
	{
		itemID = item->itemID;
	}
}

ItemStack::ItemStack(int id, int size, int meta)
	: itemID(id)
	, animationsToGo(0)
	, stackSize(size)
	, itemDamage(meta)
	, subItemID(meta)
	, stackTagCompound(NULL)
	, m_currentClipBulletNum(0)
	, m_totalBulletNum(0)
	, m_cd(0.0f)
{
}

ItemStack::ItemStack(int id, int subId, int size, int damage)
	: itemID(id)
	, subItemID(subId)
	, animationsToGo(0)
	, stackSize(size)
	, itemDamage(damage)
	, stackTagCompound(NULL)
	, m_currentClipBulletNum(0)
	, m_totalBulletNum(0)
	, m_cd(0.0f)
{}

//ItemStack::ItemStack(const ItemStack& rhs)
//	: itemID(rhs.itemID)
//	, animationsToGo(rhs.animationsToGo)
//	, stackSize(rhs.stackSize)
//	, itemDamage(rhs.itemDamage)
//	, stackTagCompound(NULL)
//{}

ItemStackPtr ItemStack::loadItemStackFromNBT(NBTTagCompound* pNbtCompound)
{
	ItemStackPtr pStack = LORD::make_shared<ItemStack>();
	pStack->readFromNBT(pNbtCompound);
	return pStack->getItem() != NULL ? pStack : NULL;
}

ItemStackPtr ItemStack::splitStack(int count)
{
	ItemStackPtr pStack = LORD::make_shared<ItemStack>(itemID, count, itemDamage);

	if (stackTagCompound)
	{
		pStack->stackTagCompound = (NBTTagCompound*)stackTagCompound->copy();
	}

	stackSize -= count;
	return pStack;
}

Item* ItemStack::getItem()
{
	return Item::getItemById(itemID);
}

const BulletClipSetting * ItemStack::getBulletClipSetting()
{
	const GunSetting* pGunSetting = getGunSetting();
	if(!pGunSetting)
		return nullptr;

	ItemBulletClip* pBulletClip = dynamic_cast<ItemBulletClip*>(Item::itemsList[pGunSetting->adaptedBulletId]);
	if (!pBulletClip)
		return nullptr;

	return pBulletClip->getBulletClipSetting();
}

const GunSetting * ItemStack::getGunSetting()
{
	Item* pItem = Item::getItemById(itemID);
	ItemGun* pGun = dynamic_cast<ItemGun*>(pItem);
	if (!pGun)
		return nullptr;

	return pGun->getGunSetting();
}

bool ItemStack::tryPlaceItemIntoWorld(EntityPlayer* pPlayer, World* pWorld, const BlockPos& selectedBlockPos, int face, const Vector3& hit)
{
	if (pPlayer->isOnFlyingMount() || pPlayer->isOnFloatingMount())
		return false;

	Item* pItem = getItem();
	if (!pItem)
		return false;

	int size = stackSize;

	bool success = pItem->onItemUse(shared_from_this(), pPlayer, pWorld, selectedBlockPos, face, hit);

	if (success)
	{
		if (!pWorld->m_isClient && !SCRIPT_EVENT::PlacingConsumeBlockEvent::invoke(itemID, itemDamage, pPlayer->entityId, selectedBlockPos))
		{
			pPlayer->inventory->setInventoryDirtyIndex(pPlayer->inventory->getInventoryIndexOfCurrentItem());
			stackSize = size;
			GameCommon::Instance()->getCommonPacketSender()->sendPlayerInventory(pPlayer->hashCode());
		}
	}

	return success;
}

float ItemStack::getStrVsBlock(Block* pBlock)
{
	Item* pItem = getItem();
	if (!pItem)
		return 5.0F;
	return pItem->getStrVsBlock(shared_from_this(), pBlock);
}

ItemStackPtr ItemStack::useItemRightClick(World* pWorld, EntityPlayer* pPlayer)
{
	if (pPlayer->isOnFlyingMount() || pPlayer->isOnFloatingMount())
		return NULL;
	Item* pItem = getItem();
	if (!pItem)
		return NULL;
	return pItem->onItemRightClick(shared_from_this(), pWorld, pPlayer);
}

ItemStackPtr ItemStack::onFoodEaten(World* pWorld, EntityPlayer* pPlayer)
{
	if (pPlayer->isOnFlyingMount() || pPlayer->isOnFloatingMount())
		return NULL;
	Item* pItem = getItem();
	if (!pItem)
		return NULL;
	return pItem->onEaten(shared_from_this(), pWorld, pPlayer);
}

NBTTagCompound* ItemStack::writeToNBT(NBTTagCompound* pCompound)
{
	pCompound->setShort("id", (i16)itemID);
	pCompound->setByte("Count", (i8)stackSize);
	pCompound->setShort("Damage", (i16)itemDamage);

	if (stackTagCompound != NULL)
	{
		pCompound->setTag("tag", stackTagCompound);
	}

	return pCompound;
}

void ItemStack::readFromNBT(NBTTagCompound* pCompound)
{
	itemID = pCompound->getShort("id");
	stackSize = pCompound->getByte("Count");
	itemDamage = pCompound->getBool("Damage");

	if (itemDamage < 0)
		itemDamage = 0;

	if (pCompound->hasKey("tag"))
		stackTagCompound = pCompound->getCompoundTag("tag");
}

int ItemStack::getMaxStackSize()
{
	Item* pItem = getItem();
	if(pItem)
		return pItem->getItemStackLimit();
	return 0;
}

bool ItemStack::isStackable()
{
	return getMaxStackSize() > 1 && (!isItemStackDamageable() || !isItemDamaged());
}

bool ItemStack::isItemStackDamageable()
{
	Item* pItem = getItem();
	if (!pItem)
		return false;

	return pItem->getMaxDamage() > 0;
}

bool ItemStack::getHasSubtypes()
{
	Item* pItem = getItem();
	if (!pItem)
		return false;

	return pItem->getHasSubtypes();
}

bool ItemStack::isItemDamaged()
{
	return isItemStackDamageable() && itemDamage > 0;
}

void ItemStack::setItemDamage(int par1)
{
	itemDamage = par1;

	if (itemDamage < 0)
	{
		itemDamage = 0;
	}
}

int ItemStack::getMaxDamage()
{
	Item* pItem = getItem();
	if (!pItem)
		return 0;

	return pItem->getMaxDamage();
}

bool ItemStack::attemptDamageItem(int damage, Random* pRandom, EntityLivingBase* pLiving)
{
	if (!isItemStackDamageable())
	{
		return false;
	}
	else
	{
		if (damage > 0)
		{
			int var3 = EnchantmentHelper::getEnchantmentLevel(Enchantment::unbreaking->effectId, shared_from_this());
			int var4 = 0;

			for (int i = 0; var3 > 0 && i < damage; ++i)
			{
				if (EnchantmentDurability::negateDamage(shared_from_this(), var3, pRandom))
				{
					++var4;
				}
			}

			damage -= var4;

			if (damage <= 0)
			{
				return false;
			}
		}
		bool need_durable = true;

		if (pLiving && pLiving->world && !pLiving->world->m_isClient && !SCRIPT_EVENT::ToolDurableEvent::invoke(itemID, itemDamage))
		{
			need_durable = false;
		}

		if (pLiving && pLiving->world && pLiving->world->m_isClient && !LogicSetting::Instance()->getToolDurable())
		{
			need_durable = false;
		}

		if (need_durable)
		{
			itemDamage += damage;
		}

		int max = getMaxDamage();
		
		return itemDamage >= getMaxDamage();
	}
}

void ItemStack::damageItem(int damage, EntityLivingBase* pLiving)
{
	NULL_RETURN_VOID(pLiving);

	EntityPlayer* pplayer = dynamic_cast<EntityPlayer*>(pLiving);
	if (!pplayer || !pplayer->capabilities.isCreativeMode)
	{
		if (!LogicSetting::Instance()->isCanDamageItem())
		{
			return;
		}

		if (isItemStackDamageable())
		{
			if (attemptDamageItem(damage, pLiving->getRNG(), pLiving))
			{
				pLiving->renderBrokenItemStack(shared_from_this());
				--stackSize;

				if (pplayer)
				{
					ItemBow* pBow = dynamic_cast<ItemBow*>(getItem());
					if (stackSize == 0 && pBow)
					{
						pplayer->destroyCurrentEquippedItem();
					}
				}

				if (stackSize < 0)
				{
					stackSize = 0;
				}

				itemDamage = 0;
			}
		}
	}
}

void ItemStack::hitEntity(EntityLivingBase* pLiving, EntityPlayer* pPlayer)
{
	Item* pItem = getItem();
	if (!pItem)
		return;

	pItem->hitEntity(shared_from_this(), pLiving, pPlayer);
}

void ItemStack::onBlockDestroyed(World* pWorld, int blockID, const BlockPos& pos, EntityPlayer* pPlayer)
{
	Item* pItem = getItem();
	if (!pItem)
		return;

	pItem->onBlockDestroyed(shared_from_this(), pWorld, blockID, pos, pPlayer);
}

bool ItemStack::canHarvestBlock(Block* pBlock)
{
	Item* pItem = getItem();
	if (!pItem)
		return false;

	return pItem->canHarvestBlock(pBlock);
}

bool ItemStack::interactWithEntity(EntityPlayer* pPlayer, EntityLivingBase* pLiving)
{
	Item* pItem = getItem();
	if (!pItem)
		return false;

	return pItem->interactWithEntity(shared_from_this(), pPlayer, pLiving);
}

ItemStackPtr ItemStack::copy()
{
	ItemStackPtr pStack = LORD::make_shared<ItemStack>(itemID, stackSize, itemDamage);

	if (pStack)
	{
		pStack->setCurrentClipBulletNum(m_currentClipBulletNum);
		pStack->m_needAdjustStackIndex = m_needAdjustStackIndex;
		pStack->m_needPlayHotbarEffect = m_needPlayHotbarEffect;
		pStack->m_needResetStack = m_needResetStack;
	}
	
	if (pStack && pStack->getGunSetting()){
		pStack->setTotalBulletNum(m_totalBulletNum);
		if (!LogicSetting::Instance()->getGunIsNeedBulletStatus())
		{
			pStack->setCurrentClipBulletNum(m_currentClipBulletNum);
		}
	}
	if (stackTagCompound != NULL)
	{
		pStack->stackTagCompound = (NBTTagCompound*)stackTagCompound->copy();
	}

	return pStack;
}

bool ItemStack::areItemStackTagsEqual(ItemStackPtr pStack1, ItemStackPtr pStack2)
{
	return pStack1 == NULL && pStack2 == NULL ? true :
		(pStack1 != NULL && pStack2 != NULL ? 
			(pStack1->stackTagCompound == NULL && pStack2->stackTagCompound != NULL ? false : 
				pStack1->stackTagCompound == NULL || pStack1->stackTagCompound->equals(pStack2->stackTagCompound)) : false);
}

bool ItemStack::areItemStacksEqual(ItemStackPtr pStack1, ItemStackPtr pStack2)
{
	return pStack1 == NULL && pStack2 == NULL ? true : (pStack1 != NULL && pStack2 != NULL ? pStack1->isItemStackEqual(pStack2) : false);
}

bool ItemStack::reloadBulletClip(int canAddBulletNum)
{
	const BulletClipSetting* pBulletSetting = getBulletClipSetting();
	if (!pBulletSetting)
		return false;

	if (m_currentClipBulletNum >= m_totalBulletNum || canAddBulletNum <= 0) {
		return false;
	}

	m_currentClipBulletNum += canAddBulletNum;
	return true;
}

bool ItemStack::consumeBullet()
{
	if (m_currentClipBulletNum < 1)
		return false;

	m_currentClipBulletNum--;
	return true;
}

bool ItemStack::isItemStackEqual(ItemStackPtr pStack)
{
	return stackSize != pStack->stackSize ? false :
		(itemID != pStack->itemID ? false : 
			(itemDamage != pStack->itemDamage ? false :
				(stackTagCompound == NULL && pStack->stackTagCompound != NULL ? false :
					stackTagCompound == NULL || stackTagCompound->equals(pStack->stackTagCompound))));
}

bool ItemStack::isItemEqual(ItemStackPtr pStack)
{
	return itemID == pStack->itemID && itemDamage == pStack->itemDamage;
}

String ItemStack::getItemName()
{
	Item* pItem = getItem();
	if (!pItem)
		return "";

	return pItem->getUnlocalizedName(shared_from_this());
}

ItemStackPtr ItemStack::copyItemStack(ItemStackPtr pStack)
{
	return pStack == NULL ? NULL : pStack->copy();
}

String ItemStack::toString()
{
	Item* pItem = getItem();
	if (!pItem)
		return "";

	return StringUtil::ToString(stackSize) + "x" + pItem->getUnlocalizedName()
		+ "@" + StringUtil::ToString(itemDamage);
}

void ItemStack::updateAnimation(World* pWorld, Entity* pEntity, int slot, bool isCurrentItem)
{
	Item* pItem = getItem();
	if (!pItem)
		return;

	if (animationsToGo > 0)
	{
		--animationsToGo;
	}

	pItem->onUpdate(shared_from_this(), pWorld, pEntity, slot, isCurrentItem);
}

void ItemStack::onCrafting(World* pWorld, EntityPlayer* pPlayer, int amount)
{
	Item* pItem = getItem();
	if (pItem)
		pItem->onCreated(shared_from_this(), pWorld, pPlayer);
}

int ItemStack::getMaxItemUseDuration()
{
	Item* pItem = getItem();
	if (pItem)
		return pItem->getMaxItemUseDuration(shared_from_this());
	return 0;
}

ACTION_TYPE ItemStack::getItemUseAction()
{
	Item* pItem = getItem();
	if (pItem)
		return pItem->getItemUseAction(shared_from_this());
	return ACTION_TYPE_NONE;
}

void ItemStack::onPlayerStoppedUsing(World* pWorld, EntityPlayer* pPlayer, int timeLeft)
{
	Item* pItem = getItem();
	if (pItem)
		pItem->onPlayerStoppedUsing(shared_from_this(), pWorld, pPlayer, timeLeft);
}

NBTTagList* ItemStack::getEnchantmentTagList()
{
	return stackTagCompound == NULL ? NULL : (NBTTagList*)stackTagCompound->getTag("ench");
}

String ItemStack::getDisplayName()
{
	Item* pItem = getItem();
	if (!pItem)
		return "";

	String strResult = pItem->getItemDisplayName(shared_from_this());

	if (stackTagCompound != NULL && stackTagCompound->hasKey("display"))
	{
		NBTTagCompound* pCompound = stackTagCompound->getCompoundTag("display");

		if (pCompound->hasKey("Name"))
		{
			strResult = pCompound->getString("Name");
		}
	}

	return strResult;
}

void ItemStack::setItemName(const String& name)
{
	if (stackTagCompound == NULL)
	{
		stackTagCompound = LordNew NBTTagCompound("tag");
	}

	if (!stackTagCompound->hasKey("display"))
	{
		stackTagCompound->setCompoundTag("display", LordNew NBTTagCompound());
	}

	stackTagCompound->getCompoundTag("display")->setString("Name", name);
}

void ItemStack::clearCustomName()
{
	if (!stackTagCompound)
		return;
	
	if (stackTagCompound->hasKey("display"))
	{
		NBTTagCompound* pCompound = stackTagCompound->getCompoundTag("display");
		pCompound->removeTag("Name");

		if (pCompound->hasNoTags())
		{
			stackTagCompound->removeTag("display");

			if (stackTagCompound->hasNoTags())
			{
				LordDelete(stackTagCompound);
				setTagCompound(NULL);
			}
		}
	}
}

bool ItemStack::hasDisplayName()
{
	return stackTagCompound == NULL ? false : 
		(!stackTagCompound->hasKey("display") ? false :
			stackTagCompound->getCompoundTag("display")->hasKey("Name"));
}

StringArray ItemStack::getTooltip(EntityPlayer* pPlayer, bool advanced)
{
	StringArray result;
	return result;
}

bool ItemStack::hasEffect()
{
	Item* pItem = getItem();
	if (!pItem)
		return false;
	return pItem->hasEffect(shared_from_this());
}

RARITY_TYPE ItemStack::getRarity()
{
	Item* pItem = getItem();
	if (!pItem)
		return RARITY_TYPE_COMMON;
	return pItem->getRarity(shared_from_this());
}

bool ItemStack::isItemEnchantable()
{
	Item* pItem = getItem();
	if (!pItem)
		return false;
	return !pItem->isItemTool(shared_from_this()) ? false : !isItemEnchanted();
}

void ItemStack::addEnchantmentbyId(int id, int level)
{
	Enchantment *data = Enchantment::getEnchantmentById(id);
	if(data != nullptr)
		this->addEnchantment(data, level);
}

void ItemStack::addEnchantment(Enchantment* enchantment, int level)
{
	if (stackTagCompound == NULL)
	{
		setTagCompound(LordNew NBTTagCompound());
	}

	if (!stackTagCompound->hasKey("ench"))
	{
		stackTagCompound->setTag("ench", LordNew NBTTagList("ench"));
	}

	NBTTagList* pNbtEnch = (NBTTagList*)stackTagCompound->getTag("ench");
	NBTTagCompound* pCompoundEnch = LordNew NBTTagCompound();
	pCompoundEnch->setShort("id", (short)enchantment->effectId);
	pCompoundEnch->setShort("lvl", (short)((i8)level));
	pNbtEnch->appendTag(pCompoundEnch);
	m_enchantmentAddedEvent.emit();
}

bool ItemStack::isItemEnchanted()
{
	return stackTagCompound != NULL && stackTagCompound->hasKey("ench");
}

void ItemStack::setTagInfo(const String& name, NBTBase* pNbt)
{
	if (stackTagCompound == NULL)
	{
		setTagCompound(LordNew NBTTagCompound());
	}

	stackTagCompound->setTag(name, pNbt);
}

bool ItemStack::canEditBlocks()
{
	Item* pItem = getItem();
	if (!pItem)
		return false;
	return pItem->canItemEditBlocks();
}

int ItemStack::getRepairCost()
{
	return hasTagCompound() &&
		stackTagCompound->hasKey("RepairCost") ? stackTagCompound->getInteger("RepairCost") : 0;
}

void ItemStack::setRepairCost(int cost)
{
	if (!hasTagCompound())
	{
		stackTagCompound = LordNew NBTTagCompound("tag");
	}

	stackTagCompound->setInteger("RepairCost", cost);
}

void ItemStack::getAttributeModifiers(AttributeModifierMap& outmap)
{
	if (hasTagCompound() && stackTagCompound->hasKey("AttributeModifiers"))
	{
		NBTTagList* nbtAttributes = stackTagCompound->getTagList("AttributeModifiers");

		for (int i = 0; i < nbtAttributes->tagCount(); ++i)
		{
			NBTTagCompound* pCompound = (NBTTagCompound*)nbtAttributes->tagAt(i);
			AttributeModifier* modifier = SharedMonsterAttributes::readAttributeModifierFromNBT(pCompound);

			if (modifier->getID().getLeastSignificantBits() != 0L &&
				modifier->getID().getMostSignificantBits() != 0L)
			{
				outmap.insert(std::make_pair(pCompound->getString("AttributeName"), modifier));
			}
		}
	}
	else
	{
		Item* pItem = getItem();
		if(pItem)
			pItem->getAttributeModifiers(outmap);
	}
}

ItemStackInfo ItemStack::getItemStackInfo()
{
	m_itemStackInfo.id = this->itemID;
	m_itemStackInfo.meta = this->itemDamage;
	m_itemStackInfo.num = this->stackSize;
	m_itemStackInfo.bullets = this->m_currentClipBulletNum;
	return m_itemStackInfo;
}

void ItemStack::updateCdTime(float fDeltaTime)
{
	if (m_cd > 0.0f)
	{
		m_cd -= fDeltaTime;
	}
}

bool ItemStack::isGunItem()
{
	return GunSetting::isGunItem(itemID);
}

bool ItemStack::isThrowItemSkill()
{
	return SkillItemSetting::isThrowSkillItem(itemID);
}

bool ItemStack::isLongTouch()
{
	auto item = getItem();
	if (item && item->canLongTouchUse())
	{
		return true;
	}
	return SkillItemSetting::isLongTouchItem(itemID);
}

bool ItemStack::isItemSkill()
{
	return SkillItemSetting::isSkillItem(itemID);
}

bool ItemStack::isAutoCastSkill() 
{
	return SkillItemSetting::isAutoCastSkill(itemID);
}

bool ItemStack::isShowCrossHair() 
{
	return SkillItemSetting::isShowCrossHair(itemID);
}

bool ItemStack::isCanFallNoUse()
{
	return SkillItemSetting::isCanFallNoUse(itemID);
}

int ItemStack::getCanFallNoUseTip()
{
	return SkillItemSetting::getCanFallNoUseTip(itemID);
}

bool ItemStack::isFirstRender()
{
	auto item = getItem();
	if (item)
	{
		return item->isFirstRender();
	}
	return false;
}

bool ItemStack::isThirdRender()
{
	auto item = getItem();
	if (item)
	{
		return item->isThirdRender();
	}
	return false;
}


}
