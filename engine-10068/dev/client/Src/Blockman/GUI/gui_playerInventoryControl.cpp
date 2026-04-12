#include "gui_playerInventoryControl.h"

#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"

#include "UI/GUIFontManager.h"
#include "GUI/GuiItemStack.h"
#include "GUI/GuiSlotTable.h"
#include "GUI/gui_mainInventoryPanelControl.h"
#include "GUI/gui_craftingPanelControl.h"
#include "GUI/gui_armorPanelControl.h"
#include "GUI/gui_formulationPanelControl.h"
#include "GUI/gui_designationPanelControl.h"

#include "Inventory/InventoryPlayer.h"
#include "Util/LanguageKey.h"
#include "game.h"
#include "Setting/GameTypeSetting.h"
#include <iterator>
#include "Util/PlayerPrefs.h"
#include "GuiDef.h"
#include "UI/GUIFontManager.h"
#include "Util/CommonEvents.h"

namespace BLOCKMAN
{
	const gui_playerInventoryControl::InventoryTabId gui_playerInventoryControl::ALL_INVENTORY_TAB_ID[] = {
		InventoryTabId::ARMOR,
		InventoryTabId::CRAFTING_TYPE1,
		InventoryTabId::CRAFTING,
		InventoryTabId::DESIGNATION,
		InventoryTabId::MAIN_INVENTORY,
	};

	gui_playerInventoryControl::gui_playerInventoryControl()
		: gui_layout("PlayerInventory.json")
	{
	}

	gui_playerInventoryControl::~gui_playerInventoryControl()
	{
		for (int i = ARMOR; i <= InventoryTabId::CRAFTING_TYPE1; ++i)
		{
			LordSafeDelete(m_panels[i]);
		}
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::WalkingDead)
		{
			LordSafeDelete(m_panels[InventoryTabId::CRAFTING]);
			LordSafeDelete(m_panels[InventoryTabId::DESIGNATION]);
		}
		LordSafeDelete(m_panels[InventoryTabId::MAIN_INVENTORY]);
		
        m_subscriptionGuard.unsubscribeAll();
	}

	void gui_playerInventoryControl::onLoad()
	{
		m_hotbar = getWindow<GuiSlotTable>("PlayerInventory-VisibleBar-SlotTable");
		m_hotbar->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_playerInventoryControl::onItemStackTouchUp, this));

		getWindow("PlayerInventory-OutSize")->subscribeEvent(EventWindowTouchUp, SubscriberSlot(&gui_playerInventoryControl::onClickOutside, this));
		getWindow("PlayerInventory-ToggleInventoryButton")->subscribeEvent(EventWindowTouchUp, SubscriberSlot(&gui_playerInventoryControl::onCloseInventory, this));
		getWindow("PlayerInventory-CloseButton")->subscribeEvent(EventWindowTouchUp,SubscriberSlot(&gui_playerInventoryControl::onCloseInventory, this));
		getWindow<GUIStaticText>("PlayerInventory-Title")->SetText(getString(LanguageKey::WORDART_ROLE));

		m_tabs[InventoryTabId::ARMOR] = getWindow<GUIRadioButton>("PlayerInventory-ArmorTab");
		m_tabs[InventoryTabId::CRAFTING] = getWindow<GUIRadioButton>("PlayerInventory-CraftingTab");
		m_tabs[InventoryTabId::CRAFTING]->SetVisible(false);
		m_tabs[InventoryTabId::MAIN_INVENTORY] = getWindow<GUIRadioButton>("PlayerInventory-MainInventoryTab");
		m_tabs[InventoryTabId::DESIGNATION] = getWindow<GUIRadioButton>("PlayerInventory-DesignationTab");

		for (int i = CRAFTING_TYPE1; i <= InventoryTabId::CRAFTING_TYPE5; ++i)
		{
			String btn_name = StringUtil::Format("PlayerInventory-CraftingTab%d", i).c_str();
			m_tabs[i] = getWindow<GUIRadioButton>(btn_name.c_str());
		}

		bool isShowCrafting = PlayerPrefs::Instance()->getBoolPrefs("IsShowCrafting");
		if (isShowCrafting)
		{
			for (int i = CRAFTING_TYPE1; i <= InventoryTabId::CRAFTING_TYPE5; ++i)
			{
				m_tabs[i]->SetVisible(true);
			}
		}
		else
		{
			for (int i = CRAFTING_TYPE1; i <= InventoryTabId::CRAFTING_TYPE5; ++i)
			{
				m_tabs[i]->SetVisible(false);
			}
		}

		m_radioNameMap[InventoryTabId::ARMOR] = getWindow<GUIStaticText>("PlayerInventory-ArmorTab-Name");
		m_radioNameMap[InventoryTabId::CRAFTING] = getWindow<GUIStaticText>("PlayerInventory-CraftingTab-Name");
        m_radioNameMap[InventoryTabId::DESIGNATION] = getWindow<GUIStaticText>("PlayerInventory-DesignationTab-Name");

		for (int i = CRAFTING_TYPE1; i <= InventoryTabId::CRAFTING_TYPE5; ++i)
		{
			String tab_name = StringUtil::Format("PlayerInventory-CraftingTab-Name%d", i).c_str();
			m_radioNameMap[i] = getWindow<GUIStaticText>(tab_name.c_str());
		}

		m_radioNameMap[InventoryTabId::ARMOR]->SetText(getString(LanguageKey::WORDART_ROLE));
		m_radioNameMap[InventoryTabId::CRAFTING]->SetText(getString(LanguageKey::WORDART_COMPOUND));
		m_radioNameMap[InventoryTabId::DESIGNATION]->SetText(getString(LanguageKey::DESIGNATION));

		for (int i = CRAFTING_TYPE1; i <= InventoryTabId::CRAFTING_TYPE5; ++i)
		{
			String nameMap = StringUtil::Format("gui_compound%d", i).c_str();
			m_radioNameMap[i]->SetText(getString(nameMap.c_str()).c_str());
		}

		m_panels[InventoryTabId::ARMOR] = m_panels[InventoryTabId::ARMOR] ? m_panels[InventoryTabId::ARMOR] : LordNew gui_armorPanelControl;
		m_panels[InventoryTabId::MAIN_INVENTORY] = m_panels[InventoryTabId::MAIN_INVENTORY] ? m_panels[InventoryTabId::MAIN_INVENTORY] : LordNew gui_mainInventoryPanelControl;

		for (int i = CRAFTING_TYPE1; i <= InventoryTabId::CRAFTING_TYPE5; ++i)
		{
			m_panels[i] = m_panels[InventoryTabId::CRAFTING_TYPE1] ? m_panels[InventoryTabId::CRAFTING_TYPE1] : LordNew gui_craftingPanelControl;
		}

		auto pInventoryPanel = getWindow<GUILayout>("PlayerInventory-InventoryPanel");

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::WalkingDead)
		{
            m_panels[InventoryTabId::CRAFTING] = m_panels[InventoryTabId::CRAFTING] ? m_panels[InventoryTabId::CRAFTING] : LordNew gui_formulationPanelControl;
			m_panels[InventoryTabId::DESIGNATION] = m_panels[InventoryTabId::DESIGNATION] ? m_panels[InventoryTabId::DESIGNATION] : LordNew gui_designationPanelControl;
        }
		else
        {
            m_panels[InventoryTabId::CRAFTING] = m_panels[InventoryTabId::CRAFTING_TYPE1] ? m_panels[InventoryTabId::CRAFTING_TYPE1] : LordNew gui_craftingPanelControl;
        }

		m_subscriptionGuard.add(Blockman::Instance()->m_pPlayer->inventory->onInventoryContentSync([this] {
			m_isInventoryChanged = true;
		}));

		m_subscriptionGuard.add(CraftAllSwitchEvent::subscribe(std::bind(&gui_playerInventoryControl::onCraftAllSwitchChanged, this, std::placeholders::_1)));

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain
			|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMineCave
			|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockProduct)
		{
			getWindow<GUIStaticImage>("PlayerInventory-ItemBarBg")->SetArea(UDim(0, 0), UDim(0, 0.f), UDim(0.0f, 673.0f), UDim(0.0f, 77.0f));
			getWindow<GUIStaticImage>("PlayerInventory-ItemBarBg")->SetImage("set:skyblock_main.json image:skyblock_backpack_bg");
			getWindow<GUIStaticImage>("PlayerInventory-ToggleInventoryButton")->SetArea(UDim(0.9f, 0.f), UDim(0, -2.f), UDim(0.0f, 63.0f), UDim(0.0f, 65.0f));
			m_hotbar->SetArea(UDim(0.f, 0.f), UDim(0.f, 0.f), UDim(0.9f, 0.f), UDim(1.0f, 0.f));
			m_hotbar->SetProperty(GWP_SLOT_WIDTH, "63");
			m_hotbar->SetProperty(GWP_SLOT_HEIGHT, "65");
			m_hotbar->SetProperty(GWP_SLOT_STYLE, StringUtil::Format("%d", GuiItemSlot::SKYBLOCK).c_str());

			getWindow<GUILayout>("PlayerInventory-Inventory")->SetArea(UDim(0.f, 0.f), UDim(0.f, 0.f), UDim(0.774219f, 0.0f), UDim(0.8875f, 0.0f));
			getWindow<GUILayout>("PlayerInventory-InventoryPanel")->SetVisible(false);
			pInventoryPanel = getWindow<GUILayout>("SkyBlockPlayerInventory-InventoryPane");
			pInventoryPanel->SetVisible(true);
			getWindow<GUIStaticImage>("PlayerInventory-Page-Title-Bg")->SetArea(UDim(0.3f, 0.f), UDim(0.05f, 0.f), UDim(0.387683f, 0.0f), UDim(0.132042f, 0.0f));
			getWindow<GUIStaticImage>("PlayerInventory-Page-Title-Bg")->SetImage("set:skyblockbag.json image:title");
			getWindow<GUIStaticImage>("PlayerInventory-CloseButton")->SetArea(UDim(1.55469f, 0.f), UDim(0.f, 0.f), UDim(0.20573f, 0.0f), UDim(0.940476f, 0.0f));
			getWindow<GUIStaticImage>("PlayerInventory-CloseButton")->SetImage("set:craftableImg.json image:close");
			getWindow<GUIStaticText>("PlayerInventory-Title")->SetArea(UDim(0.f, 0.f), UDim(0.04f, 0.f), UDim(0.290676f, 0.0f), UDim(0.098592f, 0.0f));

			int count = 0;
			for (int i = ARMOR; i <= InventoryTabId::CRAFTING_TYPE5; ++i)
			{
				m_tabs[i]->SetArea(UDim(-0.0902805f, 0.f), UDim(0.192488f + count * 0.109546f, 0.f), UDim(0.102926f, 0.0f), UDim(0.101721f, 0.0f));
				m_tabs[i]->SetNormalImage("set:skyblockbag.json image:tab_normal");
				m_tabs[i]->SetPushedImage("set:skyblockbag.json image:tab_select");
				m_radioNameMap[i]->SetFont(GUIFontManager::Instance()->GetFont("HT16"));
				count++;
			}
		}

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::WalkingDead)
		{
			m_tabs[InventoryTabId::CRAFTING]->SetVisible(true);
			m_tabs[InventoryTabId::DESIGNATION]->SetVisible(true);
			auto pInventory = getWindow<GUILayout>("PlayerInventory-Inventory");
			pInventory->SetBackImage("set:walkingdead_mainInventory.json image:bg_main_frame");
			pInventory->SetArea({ 0.f, 0.0f }, { 0.0f, 65.0f }, { 0.75f, 0.0f }, { 1.0f, -150.0f });
			pInventory->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			pInventory->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "150 170 160 100");
			auto pInventoryBg = getWindow<GUIStaticImage>("PlayerInventory-InventoryBg");
			pInventoryBg->SetImage("set:walkingdead_mainInventory.json image:bg_main_background");
			pInventoryBg->SetVisible(true);
			pInventoryBg->SetArea({ -0.005f, 25.0f }, { -0.008f, 25.0f }, { 1.01f, -50.0f }, { 1.013f, -50.0f });
			pInventoryPanel->SetArea({ 0.0f, 25.0f }, { 0.0f, 25.0f }, { 1.0f, -50.0f }, { 1.0f, -50.0f });
			pInventoryPanel->SetBackImage("");
			int index = 0;
			for (auto tabId : ALL_INVENTORY_TAB_ID)
			{
				float positionY = 85.0f * index;
				switch (tabId)
				{
				case InventoryTabId::ARMOR:
				case InventoryTabId::CRAFTING:
				case InventoryTabId::DESIGNATION:
					if (m_tabs[tabId])
					{
						m_tabs[tabId]->SetArea({ 0, 25 - 154 }, { 0.16f, positionY }, { 0, 166 }, { 0, 63 });
						m_tabs[tabId]->SetNormalImage("set:walkingdead_mainInventory.json image:bg_main_tab_normal");
						m_tabs[tabId]->SetPushedImage("set:walkingdead_mainInventory.json image:bg_main_tab_selected");
						m_tabs[tabId]->SetProperty(GWP_WINDOW_STRETCH_TYPE, "None");
						m_tabs[tabId]->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "0 0 0 0");
						auto gui_text = dynamic_cast<GUIStaticText*>(m_tabs[tabId]->GetChildByIndex(0));
						gui_text->SetPosition({ {0.05f, 0 }, { -0.05f, 0 } });
						gui_text->SetFont(GUIFontManager::Instance()->GetFont("HT16"));
						gui_text->SetTextColor((Color(198.f / 255.f, 179.f / 255.f, 143.f / 255.f)));
						gui_text->SetProperty(GWP_TEXT_BORDER, "false");
						index++;
					}
					break;
				default:
					break;
				}
			}

			auto pTitle = getWindow<GUIStaticImage>("PlayerInventory-Page-Title-Bg");
			pTitle->SetArea({ 0.0f, 0.0f }, { 0.0f, -15.0f }, { 0.3f, 0.0f }, { 0.0f, 50.0f });
			pTitle->SetImage("set:walkingdead_mainInventory.json image:bg_title");
			pTitle->SetHorizontalAlignment(HA_CENTRE);

			auto pTitleText = getWindow<GUIStaticText>("PlayerInventory-Title");
			pTitleText->SetArea({ 0.0f, 0.0f }, { 0.0f, -15.0f }, { 0.3f, 0.0f }, { 0.0f, 50.0f });
			pTitleText->SetFont(GUIFontManager::Instance()->GetFont("HT18"));
			pTitleText->SetTextColor((Color(119.f / 255.f, 98.f / 255.f, 42.f / 255.f)));
			pTitleText->SetProperty(GWP_TEXT_BORDER, "false");

			auto close = getWindow<GUIStaticImage>("PlayerInventory-CloseButton");
			close->SetArea({ 2.16f, -40.0f }, { 0.0f, 0.0f }, { 0.0f, 50.0f }, { 0.0f, 50.0f });
			close->SetImage("set:walkingdead_mainInventory.json image:btn_close");
		}

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyWarLuckyBlock)
		{
			getWindow<GUIStaticImage>("PlayerInventory-ItemBarBg")->SetArea(UDim(0, 0), UDim(0, 0.f), UDim(0.0f, 673.0f), UDim(0.0f, 77.0f));

			getWindow<GUILayout>("PlayerInventory-Inventory")->SetArea(UDim(0.f, 0.f), UDim(0.1f, 0.f), UDim(0.7f, 0.0f), UDim(0.8f, 0.0f));
			getWindow<GUILayout>("PlayerInventory-InventoryPanel")->SetArea(UDim(0.f, 0.f), UDim(0.101721f, 0.f), UDim(1.f, 0.0f), UDim(0.873239f, 0.0f));
			getWindow<GUILayout>("PlayerInventory-InventoryPanel")->SetBackImage("set:lucky_sky_backpack.json image:bg");
			getWindow<GUILayout>("PlayerInventory-InventoryPanel")->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "70 70 70 70");
			getWindow<GUIStaticImage>("PlayerInventory-Page-Title-Bg")->SetArea(UDim(0.05f, 0.f), UDim(0.01f, 0.f), UDim(0.25f, 0.0f), UDim(0.14f, 0.0f));
			getWindow<GUIStaticImage>("PlayerInventory-Page-Title-Bg")->SetImage("set:lucky_sky_backpack.json image:title");
			getWindow<GUIStaticImage>("PlayerInventory-CloseButton")->SetArea(UDim(3.25, 0.f), UDim(0.1f, 0.f), UDim(0.541935f, 0.0f), UDim(0.824051f, 0.0f));
			getWindow<GUIStaticImage>("PlayerInventory-CloseButton")->SetImage("set:lucky_sky_backpack.json image:clost_btn");
			getWindow<GUIStaticText>("PlayerInventory-Title")->SetHorizontalAlignment(HA_LEFT);
			getWindow<GUIStaticText>("PlayerInventory-Title")->SetTextColor(Color(0.662745f, 0.458824f, 0.1647f));
			getWindow<GUIStaticText>("PlayerInventory-Title")->SetTextBoader(Color(0.662745f, 0.458824f, 0.1647f));
			getWindow<GUIStaticText>("PlayerInventory-Title")->SetArea(UDim(0.06f, 0.f), UDim(0.01f, 0.f), UDim(0.25f, 0.0f), UDim(0.12f, 0.0f));

			int count = 0;
			int i = ARMOR;
			GUIString Name = StringUtil::Format("TabLine-%d", i).c_str();
			auto tabLine = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, Name);
			tabLine->SetImage("set:lucky_sky_backpack.json image:tab_line");
			getWindow<GUILayout>("PlayerInventory-Inventory")->AddChildWindow(tabLine);
			tabLine->SetVisible(true);
			tabLine->SetPosition({ {0.005f, 0.f}, {0.16f + count * 0.109546f, 0.f} });
			tabLine->SetSize({ {0.f, 18.f}, {0.15f, 0.f} });
			tabLine->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			tabLine->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "5 5 5 5");
			m_tabs[i]->SetArea(UDim(-0.155f, 0.f), UDim(0.15f + count * 0.109546f, 0.f), UDim(0.17f, 0.0f), UDim(0.15f, 0.0f));
			m_tabs[i]->SetNormalImage("set:lucky_sky_backpack.json image:tab_nor");
			m_tabs[i]->SetPushedImage("set:lucky_sky_backpack.json image:tab_sele");
			m_tabs[i]->SetAlwaysOnTop(true);
			m_radioNameMap[i]->SetFont(GUIFontManager::Instance()->GetFont("HT16"));
			m_radioNameMap[i]->SetTextColor(Color(0.447f, 0.3098f, 0.149f));

			tabLine->SetVisible(false);
			m_tabs[i]->SetArea(UDim(0.f, 0.f), UDim(0.f, 0.f), UDim(0.f, 0.0f), UDim(0.f, 0.0f));
			m_radioNameMap[i]->SetVisible(false);
		}

		for (int i = ARMOR; i <= InventoryTabId::MAIN_INVENTORY; ++i)
		{
			NULL_CONTINUE(m_panels[i]);
			m_panels[i]->attachTo(this, pInventoryPanel);

			if (m_tabs[i])
			{
				m_tabs[i]->subscribeEvent(EventRadioStateChanged, std::bind(&gui_playerInventoryControl::onRadioStateChanged, this, std::placeholders::_1, i));
				m_tabs[i]->subscribeEvent(EventWindowTouchUp, std::bind(&gui_playerInventoryControl::onClick, this, std::placeholders::_1));
			}
		}
	}

	void gui_playerInventoryControl::onShow()
	{
		playSoundByType(ST_open_inventory);
		if (m_isInCraftingTable)
		{
			if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::WalkingDead)
			{
				m_selectedTab = InventoryTabId::CRAFTING;
			}
			else
			{
				m_selectedTab = InventoryTabId::CRAFTING_TYPE1;
			}

			m_tabs[m_selectedTab]->SetVisible(true);
			m_tabs[m_selectedTab]->SetSelected(true);
			for (int i = InventoryTabId::CRAFTING_TYPE2; i <= InventoryTabId::CRAFTING_TYPE5; ++i)
			{
				m_tabs[i]->SetVisible(true);
			}
		}
		else
		{
			m_selectedTab = InventoryTabId::ARMOR;
			m_tabs[m_selectedTab]->SetVisible(true);
			m_tabs[m_selectedTab]->SetSelected(true);
			dynamic_cast<gui_armorPanelControl*>(m_panels[InventoryTabId::ARMOR])->onShow();
			for (int i = InventoryTabId::CRAFTING_TYPE2; i <= InventoryTabId::CRAFTING_TYPE5; ++i)
			{
				m_tabs[i]->SetVisible(false);
			}
		}

		showTab(m_selectedTab);
	}

	void gui_playerInventoryControl::onHide()
	{
		playSoundByType(ST_close_inventory);
		m_isInCraftingTable = false;
	}

	void gui_playerInventoryControl::onUpdate(ui32 nTimeElapse)
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		for (int i = 0; i < 9; ++i)
		{
			m_hotbar->setItemStack(0, i, inventory->hotbar[i]);
		}
		m_hotbar->selectItemSlot(inventory->currentItemIndex);
		if (getParent()->isPlayerInventoryShown() &&
			(m_selectedTab == CRAFTING_TYPE1
				|| m_selectedTab == CRAFTING_TYPE2
				|| m_selectedTab == CRAFTING_TYPE3
				|| m_selectedTab == CRAFTING_TYPE4
				|| m_selectedTab == CRAFTING_TYPE5) && m_isInventoryChanged) {
			Blockman::Instance()->m_pPlayer->inventory->notifyUpdateToProxy();
			notifyUpdateCraft();
			m_isInventoryChanged = false;
		}
	}

	bool gui_playerInventoryControl::onClickOutside(const EventArgs & events)
	{
		ClickPlayerInventoryOutside::emit();
		return true;
	}

	bool gui_playerInventoryControl::onCloseInventory(const EventArgs& events)
	{
		getParent()->showMainControl();
		getParent()->checkGunUI();
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_playerInventoryControl::onClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_playerInventoryControl::onRadioStateChanged(const EventArgs & events, int tabId)
	{
		auto wEventArgs = dynamic_cast<const WindowEventArgs&>(events);
		GUIRadioButton* rButton = dynamic_cast<GUIRadioButton*>(wEventArgs.window);
		if (rButton && rButton->IsSelected()) {
			m_selectedTab = tabId;

			showTab(m_selectedTab);
			if (m_selectedTab == CRAFTING_TYPE1
				|| m_selectedTab == CRAFTING_TYPE2
				|| m_selectedTab == CRAFTING_TYPE3
				|| m_selectedTab == CRAFTING_TYPE4
				|| m_selectedTab == CRAFTING_TYPE5) {
				Blockman::Instance()->m_pPlayer->inventory->notifyUpdateToProxy();
				notifyUpdateCraft();
				dynamic_cast<gui_craftingPanelControl*>(m_panels[m_selectedTab])->recipeResetPos();
			}
		}
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::WalkingDead)
		{
			int index = 0;
			for (auto tabId : ALL_INVENTORY_TAB_ID)
			{
				float positionY = 85.0f * index;
				switch (tabId)
				{
				case InventoryTabId::ARMOR:
				case InventoryTabId::CRAFTING:
				case InventoryTabId::DESIGNATION:
					if (m_tabs[tabId])
					{
						auto gui_text = dynamic_cast<GUIStaticText*>(m_tabs[tabId]->GetChildByIndex(0));
						gui_text->SetTextColor((Color(198.f / 255.f, 179.f / 255.f, 143.f / 255.f)));
						index++;
					}
					break;
				default:
					break;
				}
			}
			auto gui_text = dynamic_cast<GUIStaticText*>(rButton->GetChildByIndex(0));
			gui_text->SetTextColor((Color(124.f / 255.f, 70.f / 255.f, 23.f / 255.f)));
		}
		return true;
	}

	bool gui_playerInventoryControl::onCraftAllSwitchChanged(bool all_switch)
	{
		m_isCraftAllSwitch = all_switch;
		notifyUpdateCraft();
		return true;
	}

	void gui_playerInventoryControl::notifyUpdateCraft()
	{
		Blockman::Instance()->m_pPlayer->inventory->notifyUpdateCraft(m_selectedTab, m_isCraftAllSwitch);
	}

	void gui_playerInventoryControl::armorPanelSellItem()
	{
		if (dynamic_cast<gui_armorPanelControl*>(m_panels[InventoryTabId::ARMOR]))
			dynamic_cast<gui_armorPanelControl*>(m_panels[InventoryTabId::ARMOR])->sellItem();
	}
	
	void gui_playerInventoryControl::setInCraftingTable(bool isInCraftingTable)
	{
		m_isInCraftingTable = isInCraftingTable;
		if (m_isInCraftingTable) {
			Blockman::Instance()->m_pPlayer->inventory->notifyUpdateToProxy();
			notifyUpdateCraft();
		}
	}


	void gui_playerInventoryControl::showTab(int tab)
	{
        for (auto tabId : ALL_INVENTORY_TAB_ID)
        {
            if (m_panels[tabId] && m_panels[tabId]->isShown() && tabId != tab)
            {
                m_panels[tabId]->hide();
            }
        }
		if (m_panels[tab])
		{
			m_panels[tab]->show();
		}
        m_tabs[tab]->SetSelected(true);
		switch (tab)
		{
		case InventoryTabId::CRAFTING_TYPE1:
		case InventoryTabId::CRAFTING_TYPE2:
		case InventoryTabId::CRAFTING_TYPE3:
		case InventoryTabId::CRAFTING_TYPE4:
		case InventoryTabId::CRAFTING_TYPE5:
			getWindow<GUIStaticText>("PlayerInventory-Title")->SetText(getString(LanguageKey::WORDART_COMPOUND));
			break;
		case InventoryTabId::ARMOR:
			getWindow<GUIStaticText>("PlayerInventory-Title")->SetText(getString(LanguageKey::WORDART_ROLE));
			break;
        case InventoryTabId::CRAFTING:
            getWindow<GUIStaticText>("PlayerInventory-Title")->SetText(getString(LanguageKey::WORDART_COMPOUND));
            break;
        case InventoryTabId::DESIGNATION:
            getWindow<GUIStaticText>("PlayerInventory-Title")->SetText(getString(LanguageKey::DESIGNATION));
            break;
		default:
			break;
		}
	}

	bool gui_playerInventoryControl::onItemStackTouchUp(const EventArgs & events)
	{
		auto touchEvent = dynamic_cast<const GuiSlotTable::ItemStackTouchEventArgs&>(events);
		LordAssert(touchEvent.row == 0);
		Blockman::Instance()->m_pPlayer->inventory->currentItemIndex = touchEvent.column;
		ItemStackPtr touchStack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (touchStack && touchStack->stackSize > 0)
		{
			ShowItemNameEvent::emit(touchStack);
		}
		PlayerChangeItemEvent::emit(Blockman::Instance()->m_pPlayer->inventory->currentItemIndex, false);
		return false;
	}
}