#include "InventoryPlayer.h"
#include "Item/ItemStack.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Entity/EntityPlayer.h"
#include "Common.h"
#include "Inventory/InventorySubset.h"
#include "Inventory/InventoryFiltered.h"
#include "Inventory/InventoryCraftable.h"
#include "Inventory/InventoryHandCraftable.h"
#include "Util/CommonEvents.h"
#include "Setting/LogicSetting.h"
//#include "../../server/src/Blockman/Entity/EntityPlayerMP.h"
#include "Block/Block.h"
#include "Block/BM_Material.h"
#include "World/World.h"
#include "Setting/BulletClipSetting.h"
#include "Setting/GunSetting.h"
#include "Entity/Enchantment.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Setting/BlockSetting.h"
#include "Setting/FurnaceSetting.h"
#include "Setting/CustomArmorSetting.h"
#include "Util/PlayerPrefs.h"

namespace BLOCKMAN
{

using namespace std::placeholders;

InventoryPlayer::InventoryPlayer(EntityPlayer* pPlayer)
	: player(pPlayer)
{
	size_t index = 0;
	for (auto& ptr : mainInventory)
	{
		m_subscriptionGuard.add(ptr.subscribe(std::bind(&InventoryPlayer::onItemStackChanged, this, index++, _1, _2)));
		ptr = nullptr;
	}
	for (auto& ptr : armorInventory)
	{
		m_subscriptionGuard.add(ptr.subscribe(std::bind(&InventoryPlayer::onItemStackChanged, this, index++, _1, _2)));
		ptr = nullptr;
	}
	for (size_t i = 0; i < HOTBAR_COUNT; ++i)
	{
		hotbar[i] = nullptr;
	}
	m_subscriptionGuard.add(m_inventorySync.subscribe(std::bind(&InventoryPlayer::onInventorySync, this, _1, _2)));

	currentItemIndex = 0;
	currentItemStack = nullptr;
	itemStack = nullptr;
}

LocalEvent::Subscription InventoryPlayer::onItemInHandChanged(std::function<void()> callback)
{
	if (!m_monitoredItemInHand)
	{
		m_subscriptionGuard.add(currentItemIndex.subscribe(std::bind(&InventoryPlayer::onCurrentItemIndexChanged, this, _1, _2)));
		monitorSelectedHotbarSlot();
		m_monitoredItemInHand = true;
	}
	return m_itemInHandChangeEvent.subscribe(callback);
}

LocalEvent::Subscription InventoryPlayer::onInventoryContentChanged(std::function<void()> callback)
{
	return m_inventoryChangeEvent.subscribe(callback);
}

LocalEvent::Subscription InventoryPlayer::onInventoryContentSync(std::function<void()> callback)
{
	return m_inventorySyncEvent.subscribe(callback);
}

InventoryPlayer::~InventoryPlayer()
{
	m_subscriptionGuard.unsubscribeAll();
	m_currentItemChangeSubscription.unsubscribe();
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		mainInventory[i] = nullptr;
	}
	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		armorInventory[i] = nullptr;
	}
	for (int i = 0; i < HOTBAR_COUNT; ++i)
	{
		hotbar[i] = nullptr;
	}
	clearPrev();
	LordSafeDelete(m_mainInventoryProxy);
	LordSafeDelete(m_armorInventoryProxy);
	LordSafeDelete(m_armorFilteredInventoryProxy);
	LordSafeDelete(m_materialFilteredInventoryProxy);
	LordSafeDelete(m_fuelFilteredInventoryProxy);
	LordSafeDelete(m_inventoryAllCraftable);
	LordSafeDelete(m_inventoryHandCraftable);
}

void InventoryPlayer::clearPrev()
{
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		prevMainInventory[i] = nullptr;
	}
	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		prevArmorInventory[i] = nullptr;
	}
}

ItemStackPtr InventoryPlayer::getCurrentItem()
{
	if (currentItemIndex < 9 && currentItemIndex >= 0)
	{
		return hotbar[currentItemIndex];
	}
	else
	{
		return nullptr;
	}
}

int InventoryPlayer::getInventoryIndexOfCurrentItem()
{
	return findItemStack(getCurrentItem());
}

bool InventoryPlayer::onItemStackChanged(size_t index, ItemStackPtr old_itemStack, ItemStackPtr new_itemStack)
{
	auto& subscriptionGuard = m_itemStackSubscriptionGuards[index];
	subscriptionGuard.unsubscribeAll();
	if (new_itemStack)
	{
		subscriptionGuard.add(new_itemStack->stackSize.subscribe([this](int, int) -> bool {
			if (!player->isThrowingSnowball() && !player->isPlacingBlock()) {
				m_inventoryChangeEvent.emit();
				return true;
			}
			int index = getInventoryIndexOfCurrentItem();
			prevMainInventory[index] = mainInventory[index] == nullptr ? nullptr : mainInventory[index]->copy();
			return true;
		}));
		subscriptionGuard.add(new_itemStack->itemDamage.subscribe([this](int, int) -> bool {
			m_inventoryChangeEvent.emit();
			return true;
		}));
		subscriptionGuard.add(new_itemStack->onEnchantmentAdded([this] {
			m_inventoryChangeEvent.emit();
		}));
	}

	if (index >= MAIN_INVENTORY_COUNT && index < MAIN_INVENTORY_COUNT + ARMOR_INVENTORY_COUNT && m_armorDestroyed) {
		BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->sendDestroyArmorToTrackingPlayers(this->player->entityId, index);
		m_armorDestroyed = false;
	}

	if (!player->isThrowingSnowball() && !player->isPlacingBlock()) {
		m_inventoryChangeEvent.emit();
	}
	else
	{
		int index = getInventoryIndexOfCurrentItem();
		prevMainInventory[index] = mainInventory[index] == nullptr ? nullptr : mainInventory[index]->copy();
	}
	
	return true;
}

bool InventoryPlayer::onCurrentItemIndexChanged(int old_index, int new_index)
{
	m_currentItemChangeSubscription.unsubscribe();
	monitorSelectedHotbarSlot();
	m_itemInHandChangeEvent.emit();
	return true;
}

bool InventoryPlayer::onInventorySync(bool old_changed, bool new_changed)
{
 	if (!old_changed && new_changed) {
		m_inventorySyncEvent.emit();
		m_inventorySync = false;
	}
	return true;
}

void InventoryPlayer::monitorSelectedHotbarSlot()
{
	m_currentItemChangeSubscription = hotbar[currentItemIndex].subscribe([this](ItemStackPtr, ItemStackPtr) -> bool
	{
		m_itemInHandChangeEvent.emit();
		return true;
	});
}

int InventoryPlayer::getInventorySlotContainItem(int itemId)
{
	Item* pItem = Item::getItemById(itemId);
	if (!pItem)
	{
		auto pBlock = BlockManager::getBlockById(itemId);
		if (pBlock)
		{
			itemId = pBlock->getItemBlockId();
		}
	}

	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; ++i)
	{
		ItemStackPtr stack = mainInventory[i];
		if (mainInventory[i] != nullptr &&
			stack->itemID == itemId)
		{
			return i;
		}
	}

	return -1;
}

int InventoryPlayer::getInventorySlotContainItemAndDamage(int itemId, int subType)
{
	Item* pItem = Item::getItemById(itemId);
	if (!pItem)
	{
		auto pBlock = BlockManager::getBlockById(itemId);
		if (pBlock)
		{
			itemId = pBlock->getItemBlockId();
		}
	}

	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; ++i)
	{
		if (mainInventory[i] != nullptr &&
			mainInventory[i]->itemID == itemId &&
			mainInventory[i]->getItemDamage() == subType)
		{
			return i;
		}
	}

	return -1;
}

int InventoryPlayer::storeItemStack(ItemStackPtr pStack)
{
	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; ++i)
	{
		if (mainInventory[i] != nullptr &&
			mainInventory[i]->itemID == pStack->itemID &&
			mainInventory[i]->isStackable() &&
			mainInventory[i]->stackSize < mainInventory[i]->getMaxStackSize() &&
			mainInventory[i]->stackSize < getInventoryStackLimit() &&
			(!mainInventory[i]->getHasSubtypes() ||
				mainInventory[i]->getItemDamage() == pStack->getItemDamage()) &&
			ItemStack::areItemStackTagsEqual(mainInventory[i], pStack))
		{
			return i;
		}
	}

	return -1;
}

int InventoryPlayer::getFirstEmptyStackInInventory()
{
	int size = getCurrentInventoryCount();
	for (int i = 0; i < size; ++i)
	{
		if (mainInventory[i] == nullptr)
		{
			return i;
		}
	}
	return -1;
}

int InventoryPlayer::getFirstEmptyStackInHotbar()
{
	for (int i = 0; i < HOTBAR_COUNT; ++i)
	{
		if (hotbar[i] == nullptr)
		{
			return i;
		}
	}
	return -1;
}

int InventoryPlayer::getLastEmptyStackInInventory()
{
	int size = getCurrentInventoryCount();
	for (int i = 0; i < size; ++i)
	{
		if (mainInventory[size - i - 1] == nullptr)
		{
			return size - i - 1;
		}
	}
	return -1;
}

int InventoryPlayer::getLastEmptyStackInHotbar()
{
	for (int i = 0; i < HOTBAR_COUNT; ++i)
	{
		if (hotbar[HOTBAR_COUNT - i - 1] == nullptr)
		{
			return HOTBAR_COUNT - i - 1;
		}
	}
	return -1;
}

void InventoryPlayer::putItemToHotbar(int slot)
{
	if (slot < getCurrentInventoryCount())
	{
		int originalItemIndex = -1;
		for (int i = 0; i < HOTBAR_COUNT; ++i)
		{
			if (hotbar[i] == static_cast<ItemStackPtr>(mainInventory[slot]))
			{
				originalItemIndex = i;
			}
		}
		ItemStackPtr tmp = hotbar[currentItemIndex];
		hotbar[currentItemIndex] = mainInventory[slot];
		if (originalItemIndex != -1) {
			hotbar[originalItemIndex] = tmp;
		}
	}
}

void InventoryPlayer::putItemToHotbarByIndex(int slot, int index)
{
	if (slot < getCurrentInventoryCount() && index < HOTBAR_COUNT)
	{
		int originalItemIndex = -1;
		for (int i = 0; i < HOTBAR_COUNT; ++i)
		{
			if (hotbar[i] == static_cast<ItemStackPtr>(mainInventory[slot]))
			{
				originalItemIndex = i;
			}
		}
		ItemStackPtr tmp = hotbar[index];
		hotbar[index] = mainInventory[slot];
		if (originalItemIndex != -1) {
			hotbar[originalItemIndex] = tmp;
		}
	}
}

void InventoryPlayer::removeItemFromHotbar(int slot)
{
	if (slot >= 0 && slot < HOTBAR_COUNT)
	{
		hotbar[slot] = nullptr;
	}
}

int InventoryPlayer::getItemSlotByHotbat(int itemId)
{
	Item* pItem = Item::getItemById(itemId);
	if (!pItem)
	{
		auto pBlock = BlockManager::getBlockById(itemId);
		if (pBlock)
		{
			itemId = pBlock->getItemBlockId();
		}
	}

	int nSlot = -1;
	for (int i = 0; i < HOTBAR_COUNT; ++i)
	{
		ItemStackPtr tmp = hotbar[i];
		if (tmp != nullptr && tmp->itemID == itemId)		{
			nSlot = i;
			break;
		}
	}
	return nSlot;
}


ItemStackInfo InventoryPlayer::getHotBarInventory(int slot)
{
	ItemStackPtr stack = hotbar[slot];
	if (stack)
	{
		return stack->getItemStackInfo();
	}	

	static ItemStackInfo info = ItemStackInfo();
	return info;
}

void InventoryPlayer::setCurrentItem(ItemStackPtr itemStack)
{
	int inventoryIndex = findItemStack(hotbar[currentItemIndex]);
	hotbar[currentItemIndex] = itemStack;
	if (inventoryIndex >= 0 && inventoryIndex < getCurrentInventoryCount())
	{
		mainInventory[inventoryIndex] = itemStack;
	}
}

void InventoryPlayer::changeCurrentItem(int idx)
{
	if (idx > 0)
		idx = 1;

	if (idx < 0)
		idx = -1;

	for (currentItemIndex -= idx; currentItemIndex < 0; currentItemIndex += 9)
	{
		;
	}

	while (currentItemIndex >= 9)
	{
		currentItemIndex -= 9;
	}
}

void InventoryPlayer::changeCurrentItemToNext()
{
	int count = 9;

	do 
	{
		count--;
		if (currentItemIndex >= 8)
		{
			currentItemIndex = 0;
		}
		else
		{
			++currentItemIndex;
		}
	} 
	while (hotbar[currentItemIndex] == nullptr && count > 0);
}

int InventoryPlayer::clearInventory(int itemID, int subType)
{
	Item* pItem = Item::getItemById(itemID);
	if (!pItem)
	{
		auto pBlock = BlockManager::getBlockById(itemID);
		if (pBlock)
		{
			itemID = pBlock->getItemBlockId();
		}
	}

	int stackSize = 0;
	ItemStackPtr pStack;
	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; ++i)
	{
		pStack = mainInventory[i];

		if (pStack != nullptr &&
			(itemID <= -1 || pStack->itemID == itemID) &&
			(subType <= -1 || pStack->getItemDamage() == subType))
		{
			stackSize += pStack->stackSize;
		}
	}

	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		pStack = armorInventory[i];

		if (pStack != nullptr &&
			(itemID <= -1 || pStack->itemID == itemID) &&
			(subType <= -1 || pStack->getItemDamage() == subType))
		{
			stackSize += pStack->stackSize;
		}
	}

	for (int i = 0; i < HOTBAR_COUNT; ++i)
	{
		hotbar[i] = nullptr;
	}

	if (itemStack != nullptr)
	{
		if (itemID > -1 && itemStack->itemID != itemID)
		{
			return stackSize;
		}

		if (subType > -1 && itemStack->getItemDamage() != subType)
		{
			return stackSize;
		}

		stackSize += itemStack->stackSize;
		setItemStack(nullptr);
	}

	return stackSize;
}

void InventoryPlayer::addItemToInventory(Item* item, int subtype)
{
	if (!item)
		return;

	if (currentItemStack)
	{
		int inventoryIndex = getInventorySlotContainItemAndDamage(currentItemStack->itemID, currentItemStack->getItemDamageForDisplay());
		if (currentItemStack->isItemEnchantable() &&
			inventoryIndex >= 0 && mainInventory[inventoryIndex] == static_cast<ItemStackPtr>(hotbar[currentItemIndex]))
		{
			return;
		}
	}

	int slotIdx = getInventorySlotContainItemAndDamage(item->itemID, subtype);

	if (slotIdx >= 0)
	{
		int stackSize = mainInventory[slotIdx]->stackSize;
		mainInventory[slotIdx] = LORD::make_shared<ItemStack>(Item::itemsList[item->itemID], stackSize, subtype);
		hotbar[currentItemIndex] = mainInventory[slotIdx];
	}
	else
	{
		int hotbarEmptySlot = getFirstEmptyStackInHotbar();
		if (hotbarEmptySlot >= 0)
		{
			currentItemIndex = hotbarEmptySlot;
		}
		int count = getCurrentInventoryCount();
		for (int i = 0; i < count; ++i)
		{
			if (mainInventory[i] == static_cast<ItemStackPtr>(hotbar[currentItemIndex]))
			{
				mainInventory[i] = LORD::make_shared<ItemStack>(Item::itemsList[item->itemID], 1, subtype);
				hotbar[currentItemIndex] = mainInventory[i];
				break;
			}
		}
	}
}

int InventoryPlayer::storePartialItemStack(ItemStackPtr pStack, bool isReverse)
{
	int itemid = pStack->itemID;
	int stacksize = pStack->stackSize;
	int slot = -1;

	if (pStack->getMaxStackSize() == 1)
	{
		slot = isReverse ? getLastEmptyStackInInventory() : getFirstEmptyStackInInventory();

		if (slot < 0)
		{
			return stacksize;
		}
		else
		{
			if (mainInventory[slot] == nullptr)
			{
				mainInventory[slot] = ItemStack::copyItemStack(pStack);
				mainInventory[slot]->stackSize = 1;
				if (mainInventory[slot]->m_needPlayHotbarEffect)
				{
					mainInventoryNeedPlayEffect[slot] = true;
				}
				int hotbarSlot = -1;
				if (isReverse)
				{
					if (slot < HOTBAR_COUNT)
					{
						hotbarSlot = getLastEmptyStackInHotbar();
					}
				}
				else
				{
					hotbarSlot = getFirstEmptyStackInHotbar();
				}
				if (hotbarSlot >= 0)
				{
					hotbar[hotbarSlot] = mainInventory[slot];
				}
				return stacksize - 1;
			}

			return stacksize;
		}
	}
	else
	{
		slot = storeItemStack(pStack);

		if (slot < 0)
		{
			slot = isReverse ? getLastEmptyStackInInventory() : getFirstEmptyStackInInventory();
		}

		if (slot < 0)
		{
			return stacksize;
		}
		else
		{
			if (mainInventory[slot] == nullptr)
			{
				mainInventory[slot] = ItemStack::copyItemStack(pStack);
				mainInventory[slot]->stackSize = 0;

				int hotbarSlot = -1;
				if (isReverse)
				{
					if (slot < HOTBAR_COUNT)
					{
						hotbarSlot = getLastEmptyStackInHotbar();
					}
				}
				else
				{
					hotbarSlot = getFirstEmptyStackInHotbar();
				}
				if (hotbarSlot >= 0)
				{
					hotbar[hotbarSlot] = mainInventory[slot];
				}
			}

			int stacksize2 = stacksize;

			if (stacksize > mainInventory[slot]->getMaxStackSize() - mainInventory[slot]->stackSize)
			{
				stacksize2 = mainInventory[slot]->getMaxStackSize() - mainInventory[slot]->stackSize;
			}

			if (stacksize2 > getInventoryStackLimit() - mainInventory[slot]->stackSize)
			{
				stacksize2 = getInventoryStackLimit() - mainInventory[slot]->stackSize;
			}

			if (stacksize2 == 0)
			{
				return stacksize;
			}
			else
			{
				stacksize -= stacksize2;
				mainInventory[slot]->stackSize += stacksize2;
				mainInventory[slot]->animationsToGo = 5;
				mainInventory[slot]->m_needPlayHotbarEffect = pStack->m_needPlayHotbarEffect;
				if (mainInventory[slot]->m_needPlayHotbarEffect)
				{
					mainInventoryNeedPlayEffect[slot] = true;
				}
				return stacksize;
			}
		}
	}
}

void InventoryPlayer::decrementAnimations()
{
	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; ++i)
	{
		if (mainInventory[i] != nullptr)
		{
			mainInventory[i]->updateAnimation(player->world, player, i, hotbar[currentItemIndex] == static_cast<ItemStackPtr>(mainInventory[i]));
		}
	}
}

bool InventoryPlayer::consumeInventoryItem(int itemid, int num)
{
	int slot = getInventorySlotContainItem(itemid);

	if (slot < 0)
	{
		return false;
	}
	else
	{
		// if (--mainInventory[slot]->stackSize <= 0)
		mainInventory[slot]->stackSize -= num;
		if (mainInventory[slot]->stackSize <= 0)
		{
			for (int i = 0; i < HOTBAR_COUNT; ++i)
			{
				if (hotbar[i] == static_cast<ItemStackPtr>(mainInventory[slot]))
				{
					hotbar[i] = nullptr;
				}
			}
			mainInventory[slot] = nullptr;
		}

		return true;
	}
}

bool InventoryPlayer::hasItem(int itemid)
{
	int slot = getInventorySlotContainItem(itemid);
	return slot >= 0;
}

bool InventoryPlayer::hasItemInSlot(int slot)
{
	if (slot < 0 || slot >= HOTBAR_COUNT)
	{
		return false;
	}

	return hotbar[slot] != nullptr;
}

ItemStackPtr InventoryPlayer::getArrowItemStack()
{
	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; ++i)
	{
		ItemStackPtr stack = mainInventory[i]; 
		if (stack != nullptr) {
			if (dynamic_cast<ItemArrow*>(stack->getItem())) {
				return stack;
			}
		}
	}

	return nullptr;
}

ItemStackPtr InventoryPlayer::getBulletItemStack(int id)
{
	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; ++i)
	{
		ItemStackPtr stack = mainInventory[i];
		if (stack != nullptr)
		{
			if (stack->itemID == id)
				return stack;
		}
	}
	return nullptr;
}

bool InventoryPlayer::addItemStackToInventory(ItemStackPtr pStack, bool isReverse)
{
	bool nResult = false;
	int slot = -1;

	do 
	{
		ERROR_BREAK(pStack);
		ERROR_BREAK(pStack->stackSize != 0);
		if (pStack->isItemDamaged())
		{
			slot = isReverse ? getLastEmptyStackInInventory() : getFirstEmptyStackInInventory();

			if (slot >= 0)
			{
				mainInventory[slot] = ItemStack::copyItemStack(pStack);
				mainInventory[slot]->animationsToGo = 5;
				if (mainInventory[slot]->m_needPlayHotbarEffect)
				{
					mainInventoryNeedPlayEffect[slot] = true;
				}
				pStack->stackSize = 0;
				nResult = true;
				int hotbarSlot = -1;
				if (isReverse)
				{
					if (slot < HOTBAR_COUNT)
					{
						hotbarSlot = getLastEmptyStackInHotbar();
					}
				}
				else
				{
					hotbarSlot = getFirstEmptyStackInHotbar();
				}
				if (hotbarSlot >= 0)
				{
					hotbar[hotbarSlot] = mainInventory[slot];
				}
			}
			else if (player->capabilities.isCreativeMode)
			{
				pStack->stackSize = 0;
				nResult = true;
			}
			else
				nResult = false;
		}
		else
		{
			int stackSize;
			do
			{
				stackSize = pStack->stackSize;
				pStack->stackSize = storePartialItemStack(pStack, isReverse);
			} while (pStack->stackSize > 0 && pStack->stackSize < stackSize);

			if (pStack->stackSize == stackSize && player->capabilities.isCreativeMode)
			{
				pStack->stackSize = 0;
				nResult = true;
			}
			else
			{
				nResult = pStack->stackSize < stackSize;
			}
		}

	} while (0);

	return nResult;
}

bool InventoryPlayer::canItemStackAddedToInventory(const ItemStackPtr pStack)
{
	if (!pStack || pStack->stackSize == 0)
	{
		return false;
	}

	if (player->capabilities.isCreativeMode)
	{
		return true;
	}

	if (pStack->isItemDamaged())
	{
		return getFirstEmptyStackInInventory() >= 0;
	}

	const int maxItemStackSize = (std::min)(pStack->getMaxStackSize(), getInventoryStackLimit());
	int decreaseSize = 0;
	int inventoryCount = getCurrentInventoryCount();
	for (int i = 0; i < inventoryCount; ++i)
	{
		if (mainInventory[i] == nullptr)
		{
			decreaseSize += maxItemStackSize;
		}
		else if (mainInventory[i] != nullptr &&
			mainInventory[i]->itemID == pStack->itemID &&
			mainInventory[i]->isStackable() &&
			mainInventory[i]->stackSize < mainInventory[i]->getMaxStackSize() &&
			mainInventory[i]->stackSize < getInventoryStackLimit() &&
			(!mainInventory[i]->getHasSubtypes() ||
				mainInventory[i]->getItemDamage() == pStack->getItemDamage()) &&
			ItemStack::areItemStackTagsEqual(mainInventory[i], pStack))
		{
			decreaseSize += maxItemStackSize - mainInventory[i]->stackSize;
		}

		if (decreaseSize >= pStack->stackSize)
		{
			return true;
		}
	}
	return false;
}

ItemStackPtr InventoryPlayer::decrStackSize(int slot, int count)
{
	auto* pItemArr = mainInventory;

	if (slot >= MAIN_INVENTORY_COUNT)
	{
		pItemArr = armorInventory;
		slot -= MAIN_INVENTORY_COUNT;
		if (slot >= ARMOR_INVENTORY_COUNT)
			return nullptr;
	}

	if (pItemArr[slot] != nullptr)
	{
		ItemStackPtr pStack;

		if (pItemArr[slot]->stackSize <= count)
		{
			pStack = pItemArr[slot];
			if (pItemArr == mainInventory)
			{
				for (int i = 0; i < HOTBAR_COUNT; ++i)
				{
					if (hotbar[i] == static_cast<ItemStackPtr>(pItemArr[slot]))
					{
						hotbar[i] = nullptr;
					}
				}
			}
			pItemArr[slot] = nullptr;
			return pStack;
		}
		else
		{
			pStack = pItemArr[slot]->splitStack(count);

			if (pItemArr[slot]->stackSize == 0)
			{
				if (pItemArr == mainInventory)
				{
					for (int i = 0; i < HOTBAR_COUNT; ++i)
					{
						if (hotbar[i] == static_cast<ItemStackPtr>(pItemArr[slot]))
						{
							hotbar[i] = nullptr;
						}
					}
				}
				pItemArr[slot] = nullptr;
			}

			return pStack;
		}
	}
	return nullptr;
}

ItemStackPtr InventoryPlayer::getStackInSlotOnClosing(int slot)
{
	auto* pItemArr = mainInventory;

	if (slot >= MAIN_INVENTORY_COUNT)
	{
		pItemArr = armorInventory;
		slot -= MAIN_INVENTORY_COUNT;
		if (slot >= ARMOR_INVENTORY_COUNT)
			return nullptr;
	}

	if (pItemArr[slot] != nullptr)
	{
		ItemStackPtr pStack = pItemArr[slot];
		pItemArr[slot] = nullptr;
		return pStack;
	}

	return nullptr;
}

void InventoryPlayer::swapStacksInSlots(int slot1, int slot2)
{
	auto first = getStackInSlot(slot1);
	auto second = getStackInSlot(slot2);
	if (slot1 >= 0 && slot1 < getCurrentInventoryCount())
		mainInventory[slot1] = second;
	if (slot2 >= 0 && slot2 < getCurrentInventoryCount())
		mainInventory[slot2] = first;
}

void InventoryPlayer::setInventorySlotContents(int slot, ItemStackPtr pStack)
{
	if (slot >= 0 && slot < getCurrentInventoryCount())
	{
		ItemStackPtr old_itemStack = mainInventory[slot];
		for (auto& itemStack : hotbar)
		{
			if (itemStack == old_itemStack)
			{
				itemStack = pStack;
				break;
			}
		}
		mainInventory[slot] = pStack;
	}
}

float InventoryPlayer::getStrVsBlock(Block* pBlock)
{
	float strength = 1.0F;
	if (LogicSetting::Instance()->getGameType() == ClientGameType::SkyBlockMain || LogicSetting::Instance()->getGameType() == ClientGameType::SkyBlockMineCave
		|| LogicSetting::Instance()->getGameType() == ClientGameType::SkyBlockProduct)
	{
		strength = BlockSetting::Instance()->getBreakBlockValue(0, pBlock->getBlockID());

		if (hotbar[currentItemIndex] != nullptr)
		{
			auto item_str = hotbar[currentItemIndex]->getStrVsBlock(pBlock);
			if (item_str != 1.0F)
			{
				strength = item_str;
			}
		}
		return strength;
	}

	if (hotbar[currentItemIndex] != nullptr)
	{
		strength *= hotbar[currentItemIndex]->getStrVsBlock(pBlock);
	}

	return strength;
}

NBTTagList* InventoryPlayer::writeToNBT(NBTTagList* pNBTList)
{
	NBTTagCompound* pCompound;
	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; ++i)
	{
		if (mainInventory[i] == nullptr)
			continue;

		pCompound = LordNew NBTTagCompound();
		pCompound->setByte("Slot", (i8)i);
		mainInventory[i]->writeToNBT(pCompound);
		pNBTList->appendTag(pCompound);
	}

	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		if (armorInventory[i] == nullptr)
			continue;
		
		pCompound = LordNew NBTTagCompound();
		pCompound->setByte("Slot", (i8)(i + 100));
		armorInventory[i]->writeToNBT(pCompound);
		pNBTList->appendTag(pCompound);
	}

	return pNBTList;
}

void InventoryPlayer::readFromNBT(NBTTagList* pNBTList)
{
	int count = getCurrentInventoryCount();
	for (int i = 0; i < pNBTList->tagCount(); ++i)
	{
		NBTTagCompound* pCompound = (NBTTagCompound*)pNBTList->tagAt(i);
		int slot = pCompound->getByte("Slot") & 255;
		ItemStackPtr pStack = ItemStack::loadItemStackFromNBT(pCompound);

		if (pStack != nullptr)
		{
			if (slot >= 0 && slot < count)
			{
				LordAssert(mainInventory[slot] == nullptr);
				mainInventory[slot] = pStack;
			}

			if (slot >= 100 && slot < ARMOR_INVENTORY_COUNT + 100)
			{
				LordAssert(armorInventory[slot - 100] == nullptr);
				armorInventory[slot - 100] = pStack;
			}
		}
	}
}

ItemStackPtr InventoryPlayer::getStackInSlot(int slot)
{
	auto* pItemArr = mainInventory;

	if (slot >= MAIN_INVENTORY_COUNT)
	{
		slot -= MAIN_INVENTORY_COUNT;
		pItemArr = armorInventory;

		if (slot >= ARMOR_INVENTORY_COUNT)
			return nullptr;
	}

	return pItemArr[slot];
}

ItemStackInfo InventoryPlayer::getItemStackInfo(int slot)
{
	auto stack = getStackInSlot(slot);
	if (stack)
	{
		return stack->getItemStackInfo();
	}
	static ItemStackInfo info = ItemStackInfo();
	return info;
}

bool InventoryPlayer::canHarvestBlock(Block* pBlock)
{
	if (!SCRIPT_EVENT::BlockCanHarvestEvent::invoke(pBlock->getBlockID()))
	{
		return false;
	}

	if (pBlock->getMaterial().isToolNotRequired())
	{
		return true;
	}
	else
	{
		ItemStackPtr pStack = getCurrentItem();
		return pStack != nullptr ? pStack->canHarvestBlock(pBlock) : false;
	}
}

int InventoryPlayer::getTotalArmorValue()
{
	int nResult = 0;

	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		if (armorInventory[i] == nullptr)
			continue;

		ItemArmor* pArmor = dynamic_cast<ItemArmor*>(armorInventory[i]->getItem());
		if (pArmor)
		{
			int defence = pArmor->damageReduceAmount;

			if (PlayerPrefs::Instance()->getBoolPrefs("IsUseCustomArmor"))
			{
				CustomArmorSettingInfo customArmorSettingInfo;
				if (CustomArmorSetting::Instance()->getArmorSettingInfo(pArmor->itemID, customArmorSettingInfo))
				{
					defence = customArmorSettingInfo.defence;
				}
			}

			nResult += defence;
		}
	}

	return nResult;
}

int InventoryPlayer::getTotalArmorCustomAttackValue()
{
	int nResult = 0;

	if (PlayerPrefs::Instance()->getBoolPrefs("IsUseCustomArmor"))
	{
		for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
		{
			if (armorInventory[i] == nullptr)
				continue;

			ItemArmor* pArmor = dynamic_cast<ItemArmor*>(armorInventory[i]->getItem());
			if (pArmor)
			{
				CustomArmorSettingInfo customArmorSettingInfo;
				if (CustomArmorSetting::Instance()->getArmorSettingInfo(armorInventory[i]->itemID, customArmorSettingInfo))
				{
					nResult += customArmorSettingInfo.attack;
				}
			}
		}
	}

	return nResult;
}

void InventoryPlayer::damageArmor(float damage)
{
	damage /= 4.0F;

	if (damage < 1.0F)
	{
		damage = 1.0F;
	}

	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		if (armorInventory[i] == nullptr)
			continue;

		ItemArmor* pArmor = dynamic_cast<ItemArmor*>(armorInventory[i]->getItem());
		if (pArmor)
		{
			armorInventory[i]->damageItem((int)damage, player);

			if (armorInventory[i]->stackSize == 0)
			{
				m_armorDestroyed = true;
				armorInventory[i] = nullptr;
			}
		}
	}
}

void InventoryPlayer::damageItem(int slot, float damage)
{
	if (slot >= MAIN_INVENTORY_COUNT)
	{
		int index = slot - MAIN_INVENTORY_COUNT;
		if (index < ARMOR_INVENTORY_COUNT && index >= 0)
		{
			if (armorInventory[index] != nullptr)
			{
				armorInventory[index]->damageItem((int)damage, player);
			}
		}
	}
	else
	{
		if (slot < getCurrentInventoryCount() && slot >= 0)
		{
			if (mainInventory[slot] != nullptr)
			{
				mainInventory[slot]->damageItem((int)damage, player);
			}
		}
	}
}

bool InventoryPlayer::equipArmor(ItemStackPtr armorStack)
{

	if (!armorStack) {
		LordLogError("armorStack is nullptr, but is tried to be equiped");
		return false;
	}

	int inventoryIndex = getMainInventory()->findItemStack(armorStack);
	if (inventoryIndex < 0)
	{
		LordLogError("item does not belong to main inventory, but is tried to be equiped");
		return false;
	}
	auto armorItem = dynamic_cast<ItemArmor*>(Item::itemsList[armorStack->itemID]);
	if (!armorItem)
	{
		LordLogError("item %d is not an armor, but is tried to be equiped", armorStack->itemID);
		return false;
	}
	decrStackSize(inventoryIndex, 1);
	int armorIndex = armorItem->armorType;
	if (armorIndex == Item::ARMOR_TYPE_EXTRA_PACKAGE)
	{
		if (armorInventory[armorIndex] != nullptr)
		{
			armorIndex = Item::ARMOR_TYPE_EXTRA_PACKAGE + 1;
		}
	}
	if (armorInventory[armorIndex] != nullptr)
	{
		if (addItemStackToInventory(armorInventory[armorIndex]))
		{
			if (PlayerPrefs::Instance()->getBoolPrefs("IsUseCustomArmor"))
			{
				CustomArmorSettingInfo customArmorSettingInfo;
				if (CustomArmorSetting::Instance()->getArmorSettingInfo(armorInventory[armorIndex]->itemID, customArmorSettingInfo))
				{
					if (player)
					{
						float curMaxHp = player->getBaseHealth();
						curMaxHp = curMaxHp - customArmorSettingInfo.hp;

						player->changeMaxHealthSingle(curMaxHp);
					}
				}
			}
		}
	}
	armorInventory[armorIndex] = armorStack;

	if (PlayerPrefs::Instance()->getBoolPrefs("IsUseCustomArmor"))
	{
		CustomArmorSettingInfo customArmorSettingInfo;
		if (CustomArmorSetting::Instance()->getArmorSettingInfo(armorStack->itemID, customArmorSettingInfo))
		{
			if (player)
			{
				float curMaxHp = player->getBaseHealth();
				curMaxHp = curMaxHp + customArmorSettingInfo.hp;
				player->changeMaxHealthSingle(curMaxHp);

				float curHp = player->getHealth();
 				if (curHp > curMaxHp)
 				{
 					curHp = curMaxHp;
 				}
 				player->setEntityHealth(curHp);
			}
		}
	}

	return true;
}

bool InventoryPlayer::unequipArmor(ItemStackPtr armorStack)
{
	int armorIndex = getArmorInventory()->findItemStack(armorStack);
	if (armorIndex < 0)
	{
		LordLogError("item does not belong to armor inventory, but is tried to be unequiped");
		return false;
	}
	if (addItemStackToInventory(armorInventory[armorIndex]))
	{
		armorInventory[armorIndex] = nullptr;

		if (PlayerPrefs::Instance()->getBoolPrefs("IsUseCustomArmor"))
		{
			CustomArmorSettingInfo customArmorSettingInfo;
			if (CustomArmorSetting::Instance()->getArmorSettingInfo(armorStack->itemID, customArmorSettingInfo))
			{
				if (player)
				{
					float curMaxHp = player->getBaseHealth();
					curMaxHp = curMaxHp - customArmorSettingInfo.hp;
					
					float curHp = player->getHealth();
					if (curHp > curMaxHp) 
					{
						curHp = curMaxHp;
					}
					player->setEntityHealth(curHp);

					player->changeMaxHealthSingle(curMaxHp);
				}
			}
		}
		return true;
	}
	return false;
}

void InventoryPlayer::destroyArmor(int slotIndex)
{
	int armorIndex = slotIndex - MAIN_INVENTORY_COUNT;
	if (armorIndex < 0)
	{
		LordLogError("item does not belong to armor inventory, but is tried to be destroyed");
		return;
	}
	armorInventory[armorIndex] = nullptr;
}

void InventoryPlayer::dropAllItems()
{
	//auto pPlayerMP = dynamic_cast<EntityPlayerMP*>(player);

	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		if (mainInventory[i] != nullptr)
		{
			player->dropPlayerItemWithRandomChoice(mainInventory[i], true);

			if (SCRIPT_EVENT::ClearDropItemEvent::invoke(mainInventory[i]->itemID, mainInventory[i]->itemDamage,
				SCRIPT_EVENT::ClearDropItemType_Main, mainInventory[i]->stackSize, player ? player->hashCode() : 0))
			{
				mainInventory[i] = nullptr;
			}
		}
	}

	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		if (armorInventory[i] != nullptr)
		{
			player->dropPlayerItemWithRandomChoice(armorInventory[i], true);

			if (SCRIPT_EVENT::ClearDropItemEvent::invoke(armorInventory[i]->itemID, armorInventory[i]->itemDamage,
				SCRIPT_EVENT::ClearDropItemType_Armor, armorInventory[i]->stackSize, player ? player->hashCode() : 0))
			{
				armorInventory[i] = nullptr;
			}
		}
	}

	for (int i = 0; i < HOTBAR_COUNT; ++i)
	{
		if (hotbar[i] != nullptr)
		{
			if (SCRIPT_EVENT::ClearDropItemEvent::invoke(hotbar[i]->itemID, hotbar[i]->itemDamage,
				SCRIPT_EVENT::ClearDropItemType_Hotbar, hotbar[i]->stackSize, player ? player->hashCode() : 0))
			{
				hotbar[i] = nullptr;
			}
		}
	}
}

bool InventoryPlayer::isUseableByPlayer(EntityPlayer* pPlayer)
{
	return player->isDead ? false : pPlayer->getDistanceSqToEntity(player) <= 64.0f;
}

bool InventoryPlayer::hasItemStack(ItemStackPtr pStack)
{
	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		if (armorInventory[i] != nullptr && armorInventory[i]->isItemEqual(pStack))
		{
			return true;
		}
	}

	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; ++i)
	{
		if (mainInventory[i] != nullptr && mainInventory[i]->isItemEqual(pStack))
		{
			return true;
		}
	}

	return false;
}

void InventoryPlayer::copyInventory(InventoryPlayer& inventory)
{
	extra_inventory_count = inventory.extra_inventory_count;
	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; ++i)
	{
		mainInventory[i] = ItemStack::copyItemStack(inventory.mainInventory[i]);
	}

	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		armorInventory[i] = ItemStack::copyItemStack(inventory.armorInventory[i]);
	}

	for (int i = 0; i < HOTBAR_COUNT; ++i)
	{
		for (int j = 0; j < count; ++j)
		{
			if (inventory.hotbar[i] == static_cast<ItemStackPtr>(inventory.mainInventory[j]))
			{
				hotbar[i] = mainInventory[j];
				break;
			}
		}
	}
	currentItemIndex = inventory.currentItemIndex;
}

int InventoryPlayer::getItemSlotByStack(ItemStackPtr pStack)
{
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		if (mainInventory[i] != nullptr && mainInventory[i]->isItemEqual(pStack))
		{
			return i;
		}
	}

	return -1;
}

ItemStackPtr InventoryPlayer::getFirstSpareGun(int bulletItemId)
{
	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; ++i)
	{
		if (mainInventory[i] != nullptr)
		{
			ItemGun* pGun = dynamic_cast<ItemGun*>(mainInventory[i]->getItem());
			const GunSetting* pSetting = mainInventory[i]->getGunSetting();
			if (pGun && pSetting && pSetting->adaptedBulletId == bulletItemId && pSetting->maxBulletNum > mainInventory[i]->getTotalBulletNum())
			{
				return mainInventory[i];
			}
		}
	}

	return nullptr;
}

ItemStackPtr InventoryPlayer::getKeyById(int keyId)
{
	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; ++i)
	{
		if (mainInventory[i] != nullptr)
		{
			Item* pItem = mainInventory[i]->getItem();
			if(pItem && pItem->itemID == keyId)
				return mainInventory[i];
		}
	}

	return nullptr;
}

bool InventoryPlayer::notifyUpdateToProxy()
{
	if (m_mainInventoryProxy)
	{
		m_mainInventoryProxy->onUpdate();
	}
	if (m_armorInventoryProxy)
	{
		m_armorInventoryProxy->onUpdate();
	}
	if (m_armorFilteredInventoryProxy)
	{
		m_armorFilteredInventoryProxy->onUpdate();
	}
	if (m_materialFilteredInventoryProxy)
	{
		m_materialFilteredInventoryProxy->onUpdate();
	}
	if (m_fuelFilteredInventoryProxy)
	{
		m_fuelFilteredInventoryProxy->onUpdate();
	}
	if (m_inventoryAllCraftable)
	{
		m_inventoryAllCraftable->onUpdate();
	}
	if (m_inventoryHandCraftable)
	{
		m_inventoryHandCraftable->onUpdate();
	}
	return true;
}

void InventoryPlayer::notifyUpdateCraft(int tab, bool all_switch)
{
	if (m_inventoryAllCraftable)
	{
		m_inventoryAllCraftable->updateAvailableRecipes(tab);
		m_inventoryAllCraftable->updateCraftableItemStacks(all_switch);
	}
	if (m_inventoryHandCraftable)
	{
		m_inventoryHandCraftable->updateAvailableRecipes(tab);
		m_inventoryHandCraftable->updateCraftableItemStacks(all_switch);
	}
}

IInventory* InventoryPlayer::getMainInventory()
{
	if (!m_mainInventoryProxy)
	{
        m_mainInventoryProxy = LordNew InventorySubset(this, 0, MAIN_INVENTORY_COUNT);
	}
	return m_mainInventoryProxy;
}

IInventory* InventoryPlayer::getArmorInventory()
{
	if (!m_armorInventoryProxy)
	{
		m_armorInventoryProxy = LordNew InventorySubset(this, MAIN_INVENTORY_COUNT, ARMOR_INVENTORY_COUNT);
	}
	return m_armorInventoryProxy;
}

IInventory* InventoryPlayer::getArmorFilteredInventory()
{
	if (!m_armorFilteredInventoryProxy)
	{
		auto mainInventory = getMainInventory();
		m_armorFilteredInventoryProxy = LordNew InventoryFiltered(mainInventory, 
			[mainInventory](int slot) -> bool 
		{
			auto itemStack = mainInventory->getStackInSlot(slot);
			return itemStack ? ItemArmor::isArmor(itemStack->itemID) : false;
		});
	}
	return m_armorFilteredInventoryProxy;
}

IInventory * InventoryPlayer::getMaterialFilteredInventory()
{
	if (!m_materialFilteredInventoryProxy)
	{
		auto mainInventory = getMainInventory();
		m_materialFilteredInventoryProxy = LordNew InventoryFiltered(mainInventory,
			[mainInventory](int slot) -> bool
		{
			auto itemStack = mainInventory->getStackInSlot(slot);
			FurnaceMaterialSettingInfo furnaceMaterialSettingInfo;
			int targetId = 0;
			int targetMeta = 0;
			if (itemStack && FurnaceSetting::Instance()->getMaterialInfoFromMap(itemStack->itemID, itemStack->itemDamage, furnaceMaterialSettingInfo))
			{
				targetId = furnaceMaterialSettingInfo.furnaceMaterialId;
				targetMeta = furnaceMaterialSettingInfo.furnaceMaterialMeta;
			}
			return itemStack && itemStack->itemID == targetId && itemStack->itemDamage == targetMeta;
		});
	}
	return m_materialFilteredInventoryProxy;
}

IInventory * InventoryPlayer::getFuelFilteredInventory()
{
	if (!m_fuelFilteredInventoryProxy)
	{
		auto mainInventory = getMainInventory();
		m_fuelFilteredInventoryProxy = LordNew InventoryFiltered(mainInventory,
			[mainInventory](int slot) -> bool
		{
			auto itemStack = mainInventory->getStackInSlot(slot);
			FurnaceFuelSettingInfo furnaceFuelSettingInfo;
			int targetId = 0;
			if (itemStack && FurnaceSetting::Instance()->getFuelInfoFromMap(itemStack->itemID, furnaceFuelSettingInfo))
			{
				targetId = furnaceFuelSettingInfo.furnaceFuelId;
			}
			return itemStack ? itemStack->itemID == targetId : false;
		});
	}
	return m_fuelFilteredInventoryProxy;
}

IInventory* InventoryPlayer::getCustomClassificationInventory(int tabId, vector<int>::type itemIds)
{
	if (!m_customClassificationInventoryProxy[tabId])
	{
		auto mainInventory = getMainInventory();
		m_customClassificationInventoryProxy[tabId] = LordNew InventoryFiltered(mainInventory,
			[mainInventory, itemIds](int slot) -> bool
		{
			auto itemStack = mainInventory->getStackInSlot(slot);
			return itemStack ? find(itemIds.begin(), itemIds.end(), itemStack->itemID) != itemIds.end() : false;
		});
	}
	return m_customClassificationInventoryProxy[tabId];
}

InventoryCraftable * InventoryPlayer::getInventoryAllCraftable()
{
	if (!m_inventoryAllCraftable)
	{
		m_inventoryAllCraftable = LordNew InventoryCraftable(getMainInventory());
	}
	return m_inventoryAllCraftable;
}

InventoryCraftable * InventoryPlayer::getInventoryHandCraftable()
{
	if (!m_inventoryHandCraftable)
	{
		m_inventoryHandCraftable = LordNew InventoryHandCraftable(getMainInventory());
	}
	return m_inventoryHandCraftable;
}

void InventoryPlayer::onUpdate()
{
	if (m_inventoryChanged)
	{
		m_inventoryChanged = false;
		notifyUpdateToProxy();
	}
}

void InventoryPlayer::updateInventoryCd(float fDeltaTime)
{
	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; ++i)
	{
		if (mainInventory[i] != nullptr)
		{
			mainInventory[i]->updateCdTime(fDeltaTime);
		}
	}
}

bool InventoryPlayer::isCanAddItem(int itemId, int itemMeta, int addNum)
{
	if (itemId >= 10000)
	{
		return true;
	}

	Item* pItem = Item::getItemById(itemId);

	if (!pItem)
		return false;

	if (dynamic_cast<ItemAppIcon*>(pItem))
		return true;


	if (auto block = dynamic_cast<ItemBlock*>(pItem))
	{
		itemId = BlockManager::getItemBlockId(itemId);
	}

	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; ++i)
	{
		auto stack = mainInventory[i];
		if (stack != nullptr && stack->itemID == itemId &&
			stack->getItem()->getMetadata(stack->getItemDamage()) == itemMeta)
		{
			int lastStack = mainInventory[i]->getMaxStackSize() - mainInventory[i]->stackSize;
			addNum -= lastStack;
			if (addNum <= 0)
			{
				return true;
			}
		}
	}
	int size = getEmptyStackCount() * pItem->getItemStackLimit();
	return size >= addNum;
}

bool InventoryPlayer::SkyBlockisCanBuyPrivilege(String items, int num)
{
	int needSoltNum = 0;
	int haveSoltNum = getEmptyStackCount();
	int count = getCurrentInventoryCount();

	StringArray itemsArr = StringUtil::Split(items, "#");
	for (size_t i = 0; i < itemsArr.size(); i++)
	{
		int itemId = StringUtil::ParseInt(itemsArr[i]);
		int number = num;
		bool isNeedSolt = true;

		if (itemId >= 10000)
		{
			continue;
		}

		Item* pItem = Item::getItemById(itemId);
		if (!pItem)
			continue;

		if (dynamic_cast<ItemAppIcon*>(pItem))
			continue;
	
		for (int i = 0; i < count; ++i)
		{
			auto stack = mainInventory[i];
			if (stack != nullptr && stack->itemID == itemId &&
				stack->getItem()->getMetadata(stack->getItemDamage()) == 0)
			{
				int lastStack = stack->getMaxStackSize() - stack->stackSize;
				number -= lastStack;
				if (number <= 0)
				{
					isNeedSolt = false;
					break;
				}
			}
		}

		if (isNeedSolt)
		{
			if (number % pItem->getItemStackLimit() == 0)
				needSoltNum += number / pItem->getItemStackLimit();
			else
				needSoltNum += number / pItem->getItemStackLimit() + 1;
		}
	}
	return haveSoltNum >= needSoltNum;
}

ItemStackPtr InventoryPlayer::getItemIdBySlot(int nSlot)
{
	int nItemId = -1;
	ItemStackPtr tmp = hotbar[nSlot];
	if (tmp != nullptr) {
		return tmp;
	}
	return nullptr;
}

ItemStackPtr InventoryPlayer::getArmorItemIdBySlot(int nSlot)
{
	ItemStackPtr tmp = armorInventory[nSlot];
	if (tmp != nullptr) {
		return tmp;
	}
	return nullptr;
}

bool InventoryPlayer::isContainItem(int itemId, int itemMeta)
{
	Item* pItem = Item::getItemById(itemId);
	if (!pItem)
	{
		auto pBlock = BlockManager::getBlockById(itemId);
		if (pBlock)
		{
			itemId = pBlock->getItemBlockId();
		}
	}

	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; ++i)
	{
		if (mainInventory[i] != nullptr &&
			mainInventory[i]->itemID == itemId &&
			mainInventory[i]->getItem()->getMetadata(mainInventory[i]->getItemDamage()) == itemMeta)
		{
			return true;
		}
	}
	return false;
}
bool InventoryPlayer::isEnchantment(int slot)
{
	if (slot >= 0 && slot < getCurrentInventoryCount())
	{
		return mainInventory[slot]->isItemEnchanted();
	}

	return false;
}

int InventoryPlayer::getItemStackEnchantmentId(int slot, int enchantmentIndex)
{
	ItemStackPtr itemStack = getStackInSlot(slot);
	if (itemStack)
	{
		EnchantmentHelper::EnchantMentMap outmap;
		EnchantmentHelper::getEnchantments(itemStack, outmap);
		int index = 1;
		for (auto iter : outmap)
		{
			if (index == enchantmentIndex)
			{
				return iter.first;
			}
			index++;
		}
	}
	return 0;
}

int InventoryPlayer::getItemStackEnchantmentLv(int slot, int enchantmentIndex)
{
	ItemStackPtr itemStack = getStackInSlot(slot);
	if (itemStack)
	{
		EnchantmentHelper::EnchantMentMap outmap;
		EnchantmentHelper::getEnchantments(itemStack, outmap);
		int index = 1;
		for (auto iter : outmap)
		{
			if (index == enchantmentIndex)
			{
				return iter.second;
			}
			index++;
		}
	}
	return 0;
}

int InventoryPlayer::getItemStackEnchantmentCount(int slot)
{
	ItemStackPtr itemStack = getStackInSlot(slot);
	if (itemStack)
	{
		EnchantmentHelper::EnchantMentMap outmap;
		EnchantmentHelper::getEnchantments(itemStack, outmap);
		int index = 0;
		for (auto iter : outmap)
		{
			index++;
		}
		return index;
	}
	return 0;
}

int InventoryPlayer::getCurrentInventoryCount()
{
	int inventory_count = extra_inventory_count + LogicSetting::Instance()->getMaxInventorySize();
	return inventory_count < MAIN_INVENTORY_COUNT ? inventory_count : MAIN_INVENTORY_COUNT;
}

void InventoryPlayer::removeAllItemFromHotBar()
{
	for (int i = 0; i != HOTBAR_COUNT; ++i)
	{
		removeItemFromHotbar(i);
	}
}

int InventoryPlayer::getEmptyStackCount()
{
	int empty = 0;
	int count = getCurrentInventoryCount();
	for (int i = 0; i < count; i++)
	{
		if (mainInventory[i] == nullptr)
		{
			empty++;
		}
	}
	return empty;
}

int InventoryPlayer::getArmorSlotById(int itemId)
{
	ItemArmor* pItem = dynamic_cast<ItemArmor*>(Item::getItemById(itemId));
	if (pItem == nullptr)
	{
		return -1;
	}
	int index = pItem->getArmorType();
	index += MAIN_INVENTORY_COUNT;
	return index;
}
}
