#include "ItemCrafter.h"
#include "Item/Recipes.h"
#include "Inventory/InventoryPlayer.h"
#include "Item/ItemStack.h"
#include "Util/CommonEvents.h"
#include "Entity/EntityPlayer.h"
#include "Inventory/CraftingManager.h"
#include "Script/Event/LogicScriptEvents.h"

namespace BLOCKMAN
{
	bool ItemCrafter::tryConsumeItemsFromInventory(ShapedRecipes * recipe, IInventory * inventory)
	{
		auto size = InventoryPlayer::MAIN_INVENTORY_COUNT;
		if (auto inv = dynamic_cast<InventoryPlayer*>(inventory))
		{
			size = inv->getCurrentInventoryCount();
		}
		vector<int>::type usage(size);
		for (auto ingredient : recipe->getRecipeItems())
		{
			if (!ingredient)
			{
				continue;
			}
			bool found = false;
			for (int i = 0; i < size; ++i)
			{
				auto targetStack = inventory->getStackInSlot(i);
				if (ingredient->matches(targetStack) && targetStack->stackSize > usage[i])
				{
					++usage[i];
					found = true;
					break;
				}
			}
			if (!found)
			{
				return false;
			}
		}
		for (int i = 0; i < size; ++i)
		{
			inventory->decrStackSize(i, usage[i]);
		}
		return true;
	}

	bool ItemCrafter::craftWithinInventory(int entityId, ShapedRecipes * recipe, InventoryPlayer * inventory)
	{
		if (!inventory->canItemStackAddedToInventory(recipe->getRecipeOutput()))
		{
			return false;
		}
		if (!tryConsumeItemsFromInventory(recipe, inventory))
		{
			return false;
		}
		auto itemStack = recipe->getRecipeOutput()->copy();
		int old_size = itemStack->stackSize;
		LordAssert(inventory->addItemStackToInventory(itemStack));
		LordAssert(old_size != itemStack->stackSize);
		SCRIPT_EVENT::CraftItemSucEvent::invoke(entityId, itemStack->itemID, itemStack->itemDamage, old_size);
		return true;
	}

	bool ItemCrafter::craftItem(EntityPlayer* player, int recipeId)
	{
		auto recipe = dynamic_cast<ShapedRecipes*>(CraftingManager::Instance()->getRecipe(recipeId));
		if (recipe)
		{
			if (player)
			{
				return craftWithinInventory(player->hashCode(), recipe, player->inventory);
			}
			else
			{
				LordLogError("r ItemCrafter::craftItem player is null,please check");
				return false;
			}
		}
		else
		{
			LordLogError("recipe with id %d is not a ShapedRecipe", recipeId);
			return false;
		}
	}
}