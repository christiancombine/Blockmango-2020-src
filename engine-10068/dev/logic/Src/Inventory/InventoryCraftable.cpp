#include "InventoryCraftable.h"
#include <climits>
#include <algorithm>
#include "Inventory/CraftingManager.h"
#include "Item/ItemStack.h"
#include <Item/Recipes.h>
#include "Setting/RecipeTabSetting.h"
#include "Item/Recipes.h"

namespace BLOCKMAN
{
// 	bool availableRecipesCompare(const std::shared_ptr<CraftingRecipe>& v1, const std::shared_ptr<CraftingRecipe>& v2)
// 	{
// 		if (v1->m_craftingLimit != v2->m_craftingLimit)
// 		{
// 			return v1->m_craftingLimit > v2->m_craftingLimit;
// 		}
// 		else
// 		{
// 			if (v1->isAllNotAvailable() && !v2->isAllNotAvailable())
// 			{
// 				return false;
// 			}
// 			if (!v1->isAllNotAvailable() && v2->isAllNotAvailable())
// 			{
// 				return true;
// 			}
// 			
// 			return false;
// 		}
// 	}

	bool availableRecipesAllCompare(const std::shared_ptr<CraftingRecipe>& v1, const std::shared_ptr<CraftingRecipe>& v2)
	{
		return v1->getRecipeAllWeight() < v2->getRecipeAllWeight();
	}

	bool availableRecipesTypeCompare(const std::shared_ptr<CraftingRecipe>& v1, const std::shared_ptr<CraftingRecipe>& v2)
	{
		return v1->getRecipeTypeWeight() < v2->getRecipeTypeWeight();
	}

	set<ShapedRecipes*>::type InventoryCraftable::calculateAvailableShapedRecipes(int tab)
	{
		set<ItemStackPtr>::type availableMaterials;
		set<ShapedRecipes*>::type availableRecipes;
		auto rlist = CraftingManager::Instance()->getRecipeList();
		for (auto recipe : CraftingManager::Instance()->getRecipeList())
		{
			auto shapedRecipe = dynamic_cast<ShapedRecipes*>(recipe);

			if (!shapedRecipe || !shapedRecipe->matchTab(tab))
			{
				continue;
			}
			if (availableRecipes.find(shapedRecipe) == availableRecipes.end())
			{
				availableMaterials.insert(shapedRecipe->getRecipeOutput());
				availableRecipes.insert(shapedRecipe);
			}
		}
		return availableRecipes;
	}

	void InventoryCraftable::updateAvailableRecipes(int tab)
	{
		auto availableShapedRecipes = calculateAvailableShapedRecipes(tab - 1);
		m_availableRecipes.clear();
 		vector<std::shared_ptr<CraftingRecipe>>::type new_m_availablerecipes;
		for (auto shapedRecipe : availableShapedRecipes)
		{
			new_m_availablerecipes.push_back(LORD::make_shared<CraftingRecipe>(shapedRecipe));
		}
		m_availableRecipes = std::move(new_m_availablerecipes);
	}

	void InventoryCraftable::updateCraftableItemStacks(bool all_switch)
	{
		m_craftableItems.clear();
		for (auto recipe : m_availableRecipes)
		{
			std::shared_ptr<ItemStack> craftable(recipe->getRecipeOutput()->copy());
			int craftingLimit = INT_MAX;
			map<IngredientPtr, int>::type requiredItemNumbers;
			for (auto ingredient : recipe->getRecipeItems())
			{
				if (ingredient)
				{
					++requiredItemNumbers[ingredient];
				}
			}
			int counter = 0;
			for (auto pair : requiredItemNumbers)
			{
				int availableMaterialNum = 0;
				auto size = m_inventory->getSizeInventory();
				for (int i = 0; i < size; ++i)
				{
					auto itemInInventory = m_inventory->getStackInSlot(i);
					if (pair.first->matches(itemInInventory))
					{
						availableMaterialNum += itemInInventory->stackSize;
					}
				}
				using std::min;
				craftingLimit = min(craftingLimit, availableMaterialNum / pair.second);

				auto requiredIngredients = recipe->getRecipeItems();
				for (size_t i = 0; i < requiredIngredients.size(); ++i)
				{
					if (requiredIngredients[i] && requiredIngredients[i] == pair.first)
					{
						recipe->m_materialsAvailable[i] = availableMaterialNum-- > 0;
					}
				}
			}
			craftable->stackSize = craftingLimit * craftable->stackSize;
			recipe->m_craftingLimit = craftingLimit;
		}

		if (!all_switch)
		{
			auto iter = m_availableRecipes.begin();

			while (iter != m_availableRecipes.end())
			{
				if ((*iter)->isAllNotAvailable())
				{
					iter = m_availableRecipes.erase(iter);
				}
				else
				{
					iter++;
				}
			}
		}

		// std::sort(m_availableRecipes.begin(), m_availableRecipes.end(), availableRecipesCompare);

		if (all_switch)
		{
			std::sort(m_availableRecipes.begin(), m_availableRecipes.end(), availableRecipesAllCompare);
		}
		else
		{
			std::sort(m_availableRecipes.begin(), m_availableRecipes.end(), availableRecipesTypeCompare);
		}

		for (auto recipe : m_availableRecipes)
		{
			std::shared_ptr<ItemStack> craftable(recipe->getRecipeOutput()->copy());
			craftable->stackSize = recipe->m_craftingLimit * craftable->stackSize;
			m_craftableItems.push_back(craftable);
		}
	}

	InventoryCraftable::InventoryCraftable(IInventory * inventory)
		: InventoryProxy(inventory)
	{
		onUpdate();
	}

	void InventoryCraftable::onUpdate()
	{
		updateAvailableRecipes();
		updateCraftableItemStacks();
	}
	bool InventoryCraftable::isAllNotAvailable(int slot)
	{
		if (slot < 0 || slot >= (int)m_availableRecipes.size()) return false;

		return m_availableRecipes[slot]->isAllNotAvailable();
	}
}
