#include "gui_craftingPanelControl.h"

#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "GUI/gui_playerInventoryControl.h"
#include "game.h"

#include "Item/Recipes.h"
#include "Inventory/InventoryPlayer.h"
#include "Inventory/InventoryCraftable.h"

#include "UI/GUIButton.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "Util/StringUtil.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "GuiDef.h"

namespace BLOCKMAN
{
	gui_craftingPanelControl::gui_craftingPanelControl()
		: gui_layout("CraftingPanel.json")
	{
	}

	void gui_craftingPanelControl::onLoad()
	{
		m_recipes = getWindow<GuiScrollableInventory>("CraftingPanel-Recipes");
		m_craftTable = getWindow<GuiSlotTable>("CraftingPanel-Materials");
		// m_outputName = getWindow<GUIStaticText>("CraftingPanel-OutputName");
		m_craftButton = getWindow<GUIButton>("CraftingPanel-CraftButton");
		m_craftButtonText = getWindow<GUIStaticText>("CraftingPanel-Result-Text");
		m_craftButtonIcon = getWindow<GuiItemStack>("CraftingPanel-Result-Icon");
		m_craftAllButton = getWindow<GUICheckBox>("CraftingPanel-RecipesBox");
		m_switchTxt = getWindow<GUIStaticText>("CraftingPanel-SwitchTxt");

		m_craftButton->SetText(LanguageManager::Instance()->getString(LanguageKey::CRAFTING_PANEL_MAKE).c_str());
		m_recipes->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_craftingPanelControl::onItemStackTouchUp, this));
		m_craftButton->subscribeEvent("ButtonClick", SubscriberSlot(&gui_craftingPanelControl::onCraftButtonClick, this));
		m_craftAllButton->subscribeEvent(EventCheckStateChanged, std::bind(&gui_craftingPanelControl::onCheckStateChanged, this, std::placeholders::_1));
 
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain
			|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMineCave
			|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockProduct)
		{
			getWindow<GUIStaticImage>("CraftingPanel-Resutl-Icon-Bg")->SetImage("set:skyblockbag.json image:slot_normal");
			m_recipes->setStyle(GuiItemSlot::Style::SKYBLOCK2);
			m_recipes->SetProperty(GWP_TABLE_HORIZONTAL_INTERVAL, StringUtil::Format("%d", 4).c_str());
			m_recipes->SetProperty(GWP_TABLE_VERTICAL_INTERVAL, StringUtil::Format("%d", 4).c_str());
			m_recipes->SetWidth(UDim(0.92, 0.0));
			m_craftTable->SetProperty(GWP_SLOT_STYLE, StringUtil::Format("%d", GuiItemSlot::SKYBLOCK2).c_str());
		}

		if (m_isCraftAllSwitch)
		{
			m_switchTxt->SetText(LanguageManager::Instance()->getString(LanguageKey::CRAFTING_SWITCH_ON_TXT).c_str());
		}
		else
		{
			m_switchTxt->SetText(LanguageManager::Instance()->getString(LanguageKey::CRAFTING_SWITCH_OFF_TXT).c_str());
		}
	}

	void gui_craftingPanelControl::onHide()
	{
		m_selectedRecipe = nullptr;
	}

	void gui_craftingPanelControl::onShow()
	{
		CraftAllSwitchEvent::emit(m_isCraftAllSwitch);
	}

	void gui_craftingPanelControl::onUpdate(ui32 nTimeElapse)
	{
		updateCraftTableSize(nTimeElapse);
		updateCraftTableContent();
		updateRecipes(nTimeElapse);
		updateCraftButton();
		// updateOutputName();
	}

	bool gui_craftingPanelControl::onItemStackTouchUp(const EventArgs & event)
	{
		auto& touchEvent = dynamic_cast<const GuiScrollableInventory::ItemStackTouchEventArgs&>(event);
		InventoryCraftable* inventory;
		if (m_craftTableSize == 2)
		{
			inventory = Blockman::Instance()->m_pPlayer->inventory->getInventoryHandCraftable();
		}
		else
		{
			inventory = Blockman::Instance()->m_pPlayer->inventory->getInventoryAllCraftable();
		}
		m_selectedRecipe = inventory->getRecipe(touchEvent.index);

		ItemStackPtr touchStack = inventory->getStackInSlot(touchEvent.index);
		if (touchStack && touchStack->stackSize > 0)
		{
			ShowItemNameEvent::emit(touchStack);
		}
		return true;
	}

	bool gui_craftingPanelControl::onCraftButtonClick(const EventArgs & event)
	{
		if (m_selectedRecipe)
		{
			if (m_craftTableSize == 2)
			{
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendCraftWithHand(m_selectedRecipe->getId());
			}
			else
			{
				auto pakcetSender = GameClient::CGame::Instance()->getNetwork()->getSender();
				auto craftingTablePosition = Blockman::Instance()->m_pPlayer->getActivatedBlock();
				pakcetSender->sendCraftWithCraftingTable(m_selectedRecipe->getId(), craftingTablePosition);
			}
			//playSonud("random.click");
			playSoundByType(ST_Click);
		}
		return true;
	}

	bool gui_craftingPanelControl::onCheckStateChanged(const EventArgs & event)
	{
		m_isCraftAllSwitch = !m_isCraftAllSwitch;
		if (m_isCraftAllSwitch)
		{
			m_switchTxt->SetText(LanguageManager::Instance()->getString(LanguageKey::CRAFTING_SWITCH_ON_TXT).c_str());
		}
		else
		{
			m_switchTxt->SetText(LanguageManager::Instance()->getString(LanguageKey::CRAFTING_SWITCH_OFF_TXT).c_str());
		}
		CraftAllSwitchEvent::emit(m_isCraftAllSwitch);
		m_selectedRecipe = nullptr;
		if (m_recipes) m_recipes->resetPos();
		return true;
	}

	void gui_craftingPanelControl::recipeResetPos()
	{
		if (m_recipes) m_recipes->resetPos();
	}

	inline gui_playerInventoryControl * gui_craftingPanelControl::getParent()
	{
		return dynamic_cast<gui_playerInventoryControl*>(m_parentLayout);
	}

	void gui_craftingPanelControl::updateCraftTableSize(ui32 nTimeElapse)
	{
		m_craftTableSize = getParent()->isInCraftingTable() ? 3 : 2;
		m_craftTable->setTableSize(m_craftTableSize, m_craftTableSize);
		m_craftTable->Update(nTimeElapse);
	}

	void gui_craftingPanelControl::updateCraftTableContent()
	{
		if (m_selectedRecipe)
		{
			auto materials = m_selectedRecipe->getRecipeItems();
			for (size_t i = 0, counter = 0; i < m_craftTableSize; ++i)
			{
				for (size_t j = 0; j < m_craftTableSize; ++j)
				{
					if (counter < materials.size() 
						&& i < m_selectedRecipe->getRecipeHeight()
						&& j < m_selectedRecipe->getRecipeWidth())
					{
						m_craftTable->setIngredient(i, j, materials[counter]);
						m_craftTable->setGrayedOut(i, j, !m_selectedRecipe->materialsAvailable(counter));
						++counter;
					}
					else
					{
						m_craftTable->setItemStack(i, j, nullptr);
					}
				}
			}
		}
		else
		{
			for (size_t i = 0; i < m_craftTableSize; ++i)
			{
				for (size_t j = 0; j < m_craftTableSize; ++j)
				{
					m_craftTable->setItemStack(i, j, nullptr);
				}
			}
		}
	}

	void gui_craftingPanelControl::updateRecipes(ui32 nTimeElapse)
	{
		InventoryCraftable* inventory;
		if (m_craftTableSize == 2)
		{
			inventory = Blockman::Instance()->m_pPlayer->inventory->getInventoryHandCraftable();
		}
		else
		{
			inventory = Blockman::Instance()->m_pPlayer->inventory->getInventoryAllCraftable();
		}
		m_recipes->setInventory(inventory);
		m_recipes->Update(nTimeElapse);
		auto size = inventory->getSizeInventory();
		for (int i = 0; i < size; ++i)
		{
			// m_recipes->setGrayedOut(i, !inventory->getStackInSlot(i)->stackSize);
			if (inventory->getStackInSlot(i)->stackSize == 0)
			{
				if (inventory->isAllNotAvailable(i))
				{
					m_recipes->setGrayedOut(i, true, 0);
				}
				else
				{
					m_recipes->setGrayedOut(i, true, 1);
				}
			}
			else
			{
				m_recipes->setGrayedOut(i, false);
			}
		}
	}

	void gui_craftingPanelControl::updateCraftButton()
	{
		if (m_selectedRecipe)
		{
			auto craftingOutput = m_selectedRecipe->getRecipeOutput();
			m_craftButtonText->SetText((StringUtil::ToString(craftingOutput->stackSize)).c_str());
			m_craftButtonIcon->setItemStack(craftingOutput);
			if (m_selectedRecipe->getLimit() > 0)
			{
				m_craftButton->SetEnabled(true);
				m_craftButtonIcon->setGrayedOut(false);
			}
			else
			{
				m_craftButton->SetEnabled(false);
				m_craftButtonIcon->setGrayedOut(true);
			}
		}
		else
		{
			m_craftButtonText->SetText("");
			m_craftButtonIcon->setItemStack(nullptr);
			m_craftButton->SetEnabled(false);
		}
	}

// 	void gui_craftingPanelControl::updateOutputName()
// 	{
// 		if (m_selectedRecipe)
// 		{
// 			auto craftingOutput = m_selectedRecipe->getRecipeOutput();
// 			String  itemName = LanguageManager::Instance()->getItemName(craftingOutput->getItemName()).c_str();
// 			m_outputName->SetText(itemName.c_str());
// 		}
// 		else
// 		{
// 			m_outputName->SetText("");
// 		}
// 	}
}
