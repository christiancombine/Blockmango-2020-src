#include "IInventory.h"
#include "Item/ItemStack.h"

namespace BLOCKMAN
{
	void IInventory::swapStacksInSlots(int slot1, int slot2)
	{
		auto temp = getStackInSlot(slot1);
		setInventorySlotContents(slot1, getStackInSlot(slot2));
		setInventorySlotContents(slot2, temp);
	}

	int IInventory::findItemStack(ItemStackPtr itemStack)
	{
		int size = getSizeInventory();
		for (int i = 0; i < size; ++i)
		{
			if (getStackInSlot(i) == itemStack)
			{
				return i;
			}
		}
		return -1;
	}

	ItemStackPtr IInventory::findCanSaveItemStackById(int itemId, int itemMeta)
	{
		int size = getSizeInventory();
		for (int i = 0; i < size; ++i)
		{
			if (getStackInSlot(i) && getStackInSlot(i)->itemID == itemId && getStackInSlot(i)->itemDamage == itemMeta && getStackInSlot(i)->stackSize < getStackInSlot(i)->getMaxStackSize())
			{
				return getStackInSlot(i);
			}
		}
		return nullptr;
	}

	ItemStackInfo IInventory::getItemStackInfo(int slot)
	{
		static ItemStackInfo info = ItemStackInfo();
		return info;
	}

	int IInventory::getFirstEmptySlot()
	{
		int size = getSizeInventory();
		for (int i = 0; i < size; ++i)
		{
			if (getStackInSlot(i) == nullptr)
			{
				return i;
			}
		}
		return -1;
	}

    void IInventory::initByItem(int slot, int itemId, int itemNum, int itemMeta)
    {
        //
    }

	int IInventory::getItemNum(int id, int meta)
	{
		int rt = 0;
		for (int i = 0; i < getSizeInventory(); i++)
		{

			if (meta >= 0)
			{
				if (getStackInSlot(i) && getStackInSlot(i)->itemID == id && getStackInSlot(i)->itemDamage == meta)
					rt += getStackInSlot(i)->stackSize;
			}
			else
			{
				if (getStackInSlot(i) && getStackInSlot(i)->itemID == id)
					rt += getStackInSlot(i)->stackSize;
			}
		}

		return rt;
	}

	void IInventory::clear()
	{
		for (int i = 0; i < getSizeInventory(); i++)
		{
			decrStackSize(i, getStackInSlot(i) ? getStackInSlot(i)->stackSize : 0);
		}
	}

	void IInventory::removeItem(int id, int num, int meta)
	{
		int numTemp = num;
		std::function<int(int, bool)> removeFunc = [=](int numTemp, bool checkEchantment) {
			for (int i = 0; i < getSizeInventory(); i++)
			{
				auto stack = getStackInSlot(i);
				bool isHaveItem = false;

				if (meta >= 0)
				{
					isHaveItem = (stack && stack->itemID == id && stack->subItemID == meta);
					if (checkEchantment)
						isHaveItem = isHaveItem && !stack->isItemEnchanted();
				}
				else
				{
					isHaveItem = (stack && stack->itemID == id);
					if (checkEchantment)
						isHaveItem = isHaveItem && !stack->isItemEnchanted();
				}

				if (isHaveItem)
				{
					if (stack->stackSize >= numTemp)
					{
						this->decrStackSize(i, numTemp);
						numTemp = 0;
						break;
					}
					else
					{
						numTemp -= stack->stackSize;
						this->decrStackSize(i, stack->stackSize);
					}
				}
			}
			return numTemp;
		};

		numTemp = removeFunc(numTemp, true);
		if (numTemp > 0)
			removeFunc(numTemp, false);
	}
}
