#include "gui_armorPanelControl.h"

#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "game.h"
#include "Util/ClientEvents.h"
#include "Item/Items.h"
#include "Inventory/InventoryPlayer.h"
#include "GUI/GuiActorWindow.h"
#include "EntityRenders/EntityRenderManager.h"
#include "Actor/ActorManager.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIFontManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "UI/GUIImagesetManager.h"
#include "Setting/GameTypeSetting.h"
#include "GuiDef.h"
#include "Setting/SellItemSetting.h"
#include "Setting/AbandonItemSetting.h"
#include "Setting/ClassificationSetting.h"
#include "Setting/ArmorSetting.h"
#include "Setting/LogicSetting.h"
#include "GUI/GuiScrollableInventory.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "UI/GUIEditBox.h"
#include "UI/GUIRadioButton.h"
#include "Util/CommonEvents.h"
#include "Setting/CustomArmorSetting.h"
#include "UI/GUIListHorizontalBox.h"
#include "Plugins/GameClientEvents.h"
#include "Util/PlayerPrefs.h"

namespace BLOCKMAN
{

	const gui_armorPanelControl::ViewId  gui_armorPanelControl::ALL_VIEW_ID[] = {
		ViewId::ALL,
		ViewId::ARMOR,
		ViewId::MATERIALS,
		ViewId::CUSTOM,
	};

	gui_armorPanelControl::gui_armorPanelControl()
		: gui_layout("ArmorPanel.json")
	{
		ClassificationSetting::loadSetting();
		ArmorSetting::loadSetting();
	}

	gui_armorPanelControl::~gui_armorPanelControl() {
		ClassificationSetting::unloadSetting();
		ArmorSetting::unloadSetting();
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_armorPanelControl::onLoad()
	{
		m_guiScrollableInventory = getWindow<GuiScrollableInventory>("ArmorPanel-Inventory");
		if (!ClassificationSetting::haveCustomDetail())
		{
			m_guiScrollableInventory->SetArea({ 0, 5.0f }, { 0, 5.0f }, { 1.0, -10.0f }, { 1.0, -10.0f });
			getWindow<GUILayout>("ArmorPanel-Inventory-Detail")->SetVisible(false);
		}
		m_enderChest = getWindow<GUIButton>("ArmorPanel-EnderChest");
		m_enderChest->subscribeEvent(EventButtonClick, std::bind(&gui_armorPanelControl::onClickEnderChest, this, std::placeholders::_1));

		m_equipedArmorTable = getWindow<GuiSlotTable>("ArmorPanel-EquipedArmors");
		m_guiScrollableInventory->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_armorPanelControl::onItemStackTouchUp, this));
		m_equipedArmorTable->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_armorPanelControl::unequipArmor, this));
		m_actorWindow = getWindow<GuiActorWindow>("ArmorPanel-Actor");
		m_actorTitleName = getWindow<GUIStaticText>("ArmorPanel-Player-Model-Title-Name");
		m_actorInventoryName = getWindow<GUIStaticText>("ArmorPanel-Inventory-Title-Name");
		m_dressBox = getWindow<GUICheckBox>("ArmorPanel-Dress-Switch");
		m_dressBox->SetVisible(false);
		m_dressBox->subscribeEvent(EventCheckStateChanged, std::bind(&gui_armorPanelControl::onDressSwitch, this, std::placeholders::_1));
		m_dressEffect1 = getWindow<GUIStaticImage>("ArmorPanel-Switch-Red-Effect1");
		m_dressEffect2 = getWindow<GUIStaticImage>("ArmorPanel-Switch-Red-Effect2");
		m_dressEffect1->SetVisible(false);
		m_dressEffect2->SetVisible(false);
		m_dress_left = getWindow<GUIStaticText>("ArmorPanel-Switch-Left-Text");
		m_dress_right = getWindow<GUIStaticText>("ArmorPanel-Switch-Right-Text");
		m_dress_left->SetVisible(false);
		m_dress_right->SetVisible(false);
		m_textTip = getWindow<GUIStaticText>("ArmorPanel-Dress-Text");
		m_textTip->SetVisible(false);
		m_dressBg = getWindow<GUIStaticImage>("ArmorPanel-Dress-Bg");
		m_dressBg->SetVisible(false);
		m_sellSlot = getWindow<GuiItemSlot>("ArmorPanel-SellItemSlot");
		m_sellBtn = getWindow<GUIButton>("ArmorPanel-SellBtn");
		m_sellBtn->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_ARMOR_BAG_SELL_BTN_TXT).c_str());
		m_sellBtn->subscribeEvent(EventButtonClick, std::bind(&gui_armorPanelControl::onSellClick, this, std::placeholders::_1));
		m_abandonBtn = getWindow<GUIButton>("ArmorPanel-AbandonBtn");
		m_abandonBtn->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_ARMOR_BAG_ABANDON_BTN_TXT).c_str());
		m_abandonBtn->subscribeEvent(EventButtonClick, std::bind(&gui_armorPanelControl::onAbandonClick, this, std::placeholders::_1));
		m_sell_input_num = getWindow<GUIEditBox>("ArmorPanel-SellNumEdit");
		m_sell_input_num->subscribeEvent(EventWindowTextChanged, std::bind(&gui_armorPanelControl::onInputChanged, this, std::placeholders::_1));
		m_sell_input_num->SetBlinkshow(true);
		m_sell_input_num->SetText("");
		m_sell_input_show_num = getWindow<GUIStaticText>("ArmorPanel-SellNumEditNum");
		m_sell_sum_num_txt = getWindow<GUIStaticText>("ArmorPanel-SellNumTxt");
		m_sell_item_desc = getWindow<GUIStaticText>("ArmorPanel-SellItemDescTxt");
		m_sell_name = getWindow<GUIStaticText>("ArmorPanel-SellItemName");
		
		m_ic_defence = getWindow<GUIListHorizontalBox>("ArmorPanel-Defense");
		m_ic_defence->SetVisible(false);

		m_radioMap[(int)ViewId::ALL] = getWindow<GUIRadioButton>("ArmorPanel-Inventory-Radio-All");
		m_radioMap[(int)ViewId::ARMOR] = getWindow<GUIRadioButton>("ArmorPanel-Inventory-Radio-Armor");
		m_radioMap[(int)ViewId::MATERIALS] = getWindow<GUIRadioButton>("ArmorPanel-Inventory-Radio-Materials");

		m_radioMap[(int)ViewId::ALL]->SetText(LanguageManager::Instance()->getString(LanguageKey::INVENTORY_PANEL_ALL).c_str());
		m_radioMap[(int)ViewId::ARMOR]->SetText(LanguageManager::Instance()->getString(LanguageKey::INVENTORY_PANEL_EQUIPMENT).c_str());
		m_radioMap[(int)ViewId::MATERIALS]->SetText(LanguageManager::Instance()->getString(LanguageKey::INVENTORY_PANEL_MATERIALS).c_str());

        for (ViewId id :ALL_VIEW_ID)
        {
            if (id == ViewId::CUSTOM)
            {
                auto map = ClassificationSetting::getTabMap();
                int i = 0;
                auto iter = map.begin();
                for (iter;iter != map.end();)
                {
                    auto tab = iter->second;
                    auto radio = dynamic_cast<GUIRadioButton*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_RADIO, StringUtil::Format("ArmorPanel-Inventory-Radio%d", (int)ViewId::CUSTOM + tab->tabId).c_str()));
                    radio->SetArea({ 0, 199.0f + 113.0f * i }, { 0, 0 }, { 0, 104.0f }, { 0.0, 61.0f });
                    radio->SetNormalImage("set:new_gui_material.json image:armor_radio_nor");
                    radio->SetPushedImage("set:new_gui_material.json image:armor_radio_pre");
                    radio->SetVerticalAlignment(VA_BOTTOM);
                    radio->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
                    radio->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "7 9 7 9");
					radio->SetProperty(GWP_TEXT_SHADOW, "true");
                    radio->SetText(LanguageManager::Instance()->getString(tab->tabName).c_str());
                    radio->subscribeEvent(EventRadioStateChanged, std::bind(&gui_armorPanelControl::onRadioChange, this, std::placeholders::_1, id, tab->tabId));
                    getWindow<GUILayout>("ArmorPanel-Inventory-Radio")->AddChildWindow(radio);
                    m_radioMap[(int)ViewId::CUSTOM + tab->tabId] = radio;
                    i++;
                    iter++;
                }
            }
            else
            {
                m_radioMap[(int)id]->subscribeEvent(EventRadioStateChanged, std::bind(&gui_armorPanelControl::onRadioChange, this, std::placeholders::_1, id, 0));
            }

		}
		m_radioMap[(int)ViewId::ALL]->SetSelected(true);
		getWindow<GUILayout>("ArmorPanel-SellLayout")->SetVisible(false);
		getWindow<GUILayout>("ArmorPanel-AttrLayout")->SetVisible(false);

		m_add_btn = getWindow<GUIButton>("ArmorPanel-SellAddBtn");
		m_add_btn->subscribeEvent(EventButtonClick, std::bind(&gui_armorPanelControl::onButtonNumClick, this, std::placeholders::_1, true));
		m_sub_btn = getWindow<GUIButton>("ArmorPanel-SellSubBtn");
		m_sub_btn->subscribeEvent(EventButtonClick, std::bind(&gui_armorPanelControl::onButtonNumClick, this, std::placeholders::_1, false));
		m_subscriptionGuard.add(ClickPlayerInventoryOutside::subscribe(std::bind(&gui_armorPanelControl::onRemoveCurrentItem, this)));

		m_attrLayout = getWindow<GUILayout>("ArmorPanel-AttrLayout");
		m_defence_num = getWindow<GUIStaticText>("ArmorPanel-DefenceNum");
		m_attack_num = getWindow<GUIStaticText>("ArmorPanel-AttackNum");
		m_hp_num = getWindow<GUIStaticText>("ArmorPanel-HpNum");

		m_subscriptionGuard.add(RefreshSellItemEvent::subscribe(std::bind(&gui_armorPanelControl::refreshSellSelectItem, this)));

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain
			|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMineCave
			|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockProduct)
		{
			for (int id = (int)ViewId::ALL; id <= (int)ViewId::MATERIALS; ++id)
			{
				m_radioMap[(int)id]->SetVisible(false);
			}
			m_dressBox->SetVisible(true);

			m_dress_left->SetText(getString("sky_block_switch_show_armor"));
			m_dress_right->SetText(getString("sky_block_switch_show_dress"));
			m_dress_right->SetVisible(true);

			m_dressBox->SetNormalImage("set:skyblock_gift.json image:left_red-btn");
			m_dressBox->SetPushedImage("set:skyblock_gift.json image:right_red-btn");
			m_dressBox->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			m_dressBox->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "39 39 0 0");
			m_dressEffect1->SetEffectName("g1048_btn_big.effect");
			m_dressEffect1->SetVisible(true);
			m_dressEffect2->SetEffectName("g1048_btn_big.effect");
			//m_dressBox->SetArea(UDim(0.45f, 0.f), UDim(0.73f, 0.f), UDim(0.3f, 0.0f), UDim(0.08f, 0.0f));
			m_guiScrollableInventory->setStyle(GuiItemSlot::Style::SKYBLOCK2);
			m_guiScrollableInventory->SetArea(UDim(0.f, 0.f), UDim(0.f, 0.f), UDim(0.95f, 0.0f), UDim(0.95f, 0.0f));
			m_guiScrollableInventory->SetVerticalAlignment(VA_CENTRE);
			m_guiScrollableInventory->SetHorizontalAlignment(HA_CENTRE);
			m_equipedArmorTable->SetProperty(GWP_SLOT_STYLE, StringUtil::Format("%d", GuiItemSlot::SKYBLOCK2).c_str());
			m_equipedArmorTable->setTableSize(4, 1);
			m_equipedArmorTable->SetArea(UDim(-0.42f, 0.f), UDim(0.25f, 0.f), UDim(0.220238f, 0.0f), UDim(0.474006f, 0.0f));
			m_equipedArmorTable->SetProperty(GWP_TABLE_VERTICAL_INTERVAL, StringUtil::Format("%d", 20).c_str());
			getWindow<GUILayout>("ArmorPanel-Player-Model")->SetArea(UDim(0.0265625f, 0.f), UDim(0.0472222f, 0.f), UDim(0.28657f, 0.0f), UDim(0.85663f, 0.0f));
			getWindow<GUILayout>("ArmorPanel-Player-Model")->SetBackImage("");
			auto skyblockArmorPanel = getWindow<GUILayout>("ArmorPanel-Inventory-Layout");
			skyblockArmorPanel->SetArea(UDim(-0.01875f, 0.f), UDim(0.1f, 0.f), UDim(0.66902f, 0.0f), UDim(0.727699f, 0.0f));
			skyblockArmorPanel->SetBackImage("set:skyblockbag.json image:bg7");
			skyblockArmorPanel->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			skyblockArmorPanel->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "20 20 20 20");
			auto skyblockArmorPanelFoot = getWindow<GUIStaticImage>("ArmorPanel-Foot-Icon");
			skyblockArmorPanelFoot->SetImage("set:skyblockbag.json image:bg5");
			skyblockArmorPanelFoot->SetArea(UDim(0.08f, 0.f), UDim(-0.103211f, 0.f), UDim(0.69014f, 0.0f), UDim(0.7364f, 0.0f));
			skyblockArmorPanelFoot->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			skyblockArmorPanelFoot->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "65 65 45 125");
			getWindow<GuiActorWindow>("ArmorPanel-Actor")->SetArea(UDim(0.085f, 0.f), UDim(-0.5f, 0.f), UDim(1.2f, 0.0f), UDim(1.2f, 0.0f));
			getWindow<GUIStaticText>("ArmorPanel-Inventory-Title-Name")->SetVisible(false);
			getWindow<GUIStaticText>("ArmorPanel-Player-Model-Title-Name")->SetArea(UDim(0.25f, 0.f), UDim(0.1628135f, 0.f), UDim(0.69014f, 0.0f), UDim(0.0458716f, 0.0f));
			getWindow<GUILayout>("ArmorPanel-SellLayout")->SetVisible(true);
			getWindow<GUILayout>("ArmorPanel-AttrLayout")->SetVisible(true);
			getWindow<GUIStaticImage>("ArmorPanel-DefenceBg")->SetImage("set:skyblockbag.json image:state_bg");
			getWindow<GUIStaticImage>("ArmorPanel-AttackBg")->SetImage("set:skyblockbag.json image:state_bg");
			getWindow<GUIStaticImage>("ArmorPanel-HpBg")->SetImage("set:skyblockbag.json image:state_bg");
			getWindow<GUIStaticImage>("ArmorPanel-DefenceIcon")->SetImage("set:skyblockbag.json image:defence");
			getWindow<GUIStaticImage>("ArmorPanel-AttackIcon")->SetImage("set:skyblockbag.json image:attack");
			getWindow<GUIStaticImage>("ArmorPanel-HpIcon")->SetImage("set:skyblockbag.json image:heart");
			getWindow<GUIStaticImage>("ArmorPanel-SellBg")->SetImage("set:skyblockbag.json image:bg6");
			m_sell_input_num->SetProperty(GWP_EDIT_BACK_IMAGE, "set:skyblockbag.json image:jiage_bg");
			m_add_btn->SetPushedImage("set:skyblockbag.json image:sub_bg");
			m_add_btn->SetNormalImage("set:skyblockbag.json image:sub_bg");
			m_sub_btn->SetPushedImage("set:skyblockbag.json image:add_bg");
			m_sub_btn->SetNormalImage("set:skyblockbag.json image:add_bg");
			m_sellSlot->setStyle(GuiItemSlot::SKYBLOCK2);
			getWindow<GUIStaticImage>("ArmorPanel-MoneyIcon")->SetImage("set:items.json image:currency_money");
		}

		if(checkGameType(ClientGameType::SkyWarLuckyBlock))
		{
			for (int id = (int)ViewId::ALL; id <= (int)ViewId::MATERIALS; ++id)
			{
				m_radioMap[(int)id]->SetVisible(false);
			}
			m_equipedArmorTable->SetProperty(GWP_SLOT_STYLE, StringUtil::Format("%d", GuiItemSlot::SKY_WAR_LUCKY_BLOCK2).c_str());
			m_equipedArmorTable->setTableSize(4, 1);
			m_equipedArmorTable->SetSize({ {0.220238f, 0.f }, { 0.474006f, 0.f } });
			m_equipedArmorTable->SetPosition({ {-0.3f, 0.f }, { 0.25f, 0.f } });
			m_equipedArmorTable->SetProperty(GWP_TABLE_VERTICAL_INTERVAL, StringUtil::Format("%d", 20).c_str());
			m_equipedArmorTable->SetProperty(GWP_SLOT_WIDTH, StringUtil::Format("%d", 55).c_str());
			m_equipedArmorTable->SetProperty(GWP_SLOT_HEIGHT, StringUtil::Format("%d", 60).c_str());

			getWindow<GUIStaticImage>("ArmorPanel-Foot-Icon")->SetVisible(false);
			auto PlayerBg = getWindow<GUILayout>("ArmorPanel-Player-Model");
			PlayerBg->SetSize({ {0.32f, 0.f }, { 0.895f, 0.f } });
			PlayerBg->SetPosition({ { 0.0265625f, 0.f }, { 0.06f, 0.f } });
			PlayerBg->SetBackImage("set:lucky_sky_backpack.json image:player_bg");
			PlayerBg->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "60 60 50 60");

			auto InvLayout = getWindow<GUILayout>("ArmorPanel-Inventory-Layout");
			InvLayout->SetSize({ {0.64f, 0.f }, { 0.54f, 0.f } });
			InvLayout->SetPosition({ {-0.008f, 0.f }, { 0.06f, 0.f } });
			InvLayout->SetBackImage("set:lucky_sky_backpack.json image:prop_bg");
			InvLayout->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "10 10 10 10");

			m_guiScrollableInventory->setStyle(GuiItemSlot::Style::SKY_WAR_LUCKY_BLOCK1);
			m_guiScrollableInventory->SetSize({ {1.f, -11.f }, { 1.f, -11.f } });
			m_guiScrollableInventory->SetProperty(GWP_TABLE_HORIZONTAL_INTERVAL, StringUtil::Format("%d", 1).c_str());
			m_guiScrollableInventory->SetProperty(GWP_TABLE_VERTICAL_INTERVAL, StringUtil::Format("%d", 1).c_str());

			getWindow<GuiActorWindow>("ArmorPanel-Actor")->SetPosition({ {0.f, 0.f }, { 0.22f, 0.f } });
			getWindow<GuiActorWindow>("ArmorPanel-Actor")->SetActorScale(0.65f);
			getWindow<GUIStaticText>("ArmorPanel-Inventory-Title-Name")->SetVisible(false);
			getWindow<GUILayout>("ArmorPanel-SellNumLayout")->SetVisible(false);

			auto NameBg = getWindow<GUIStaticImage>("ArmorPanel-Player-Model-Title-BG");
			NameBg->SetImage("set:lucky_sky_backpack.json image:name_bg");
			NameBg->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			NameBg->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "20 20 5 5");
			NameBg->SetSize({ {0.7f, 0.f}, {0.f, 40.f} });
			NameBg->SetAlwaysOnTop(true);
			getWindow<GUIStaticText>("ArmorPanel-Player-Model-Title-Name")->SetPosition({ {0.f, 0.f }, { 0.f, 4.f } });

			getWindow<GUILayout>("ArmorPanel-SellLayout")->SetVisible(true);
			getWindow<GUILayout>("ArmorPanel-SellLayout")->SetSize({ {0.64f, 0.f }, { 0.355f, 0.f } });
			getWindow<GUILayout>("ArmorPanel-SellLayout")->SetPosition({ {-0.008f, 0.f }, { -0.04f, 0.f } });
			m_sellSlot->setStyle(GuiItemSlot::SKY_WAR_LUCKY_BLOCK3);
			m_sellSlot->SetSize({ {0.789474f, 0.f}, {0.657895f, 0.f} });
			getWindow<GUIStaticImage>("ArmorPanel-MoneyIcon")->SetVisible(false);

			auto SellBg = getWindow<GUIStaticImage>("ArmorPanel-SellBg");
			SellBg->SetImage("set:lucky_sky_backpack.json image:prop_info_bg");
			SellBg->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			SellBg->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "10 200 20 20");
			getWindow<GUILayout>("ArmorPanel-SellItemDesc")->SetSize({ {0.7785f, 0.f}, {0.820513f, 0.f} });

			auto SubBtn = getWindow<GUIButton>("ArmorPanel-SellSubBtn");
			auto PlusBtn = getWindow<GUIButton>("ArmorPanel-SellAddBtn");
			SubBtn->SetText("");
			SubBtn->SetNormalImage("set:lucky_sky_backpack.json image:sub");
			SubBtn->SetPushedImage("set:lucky_sky_backpack.json image:sub");
			SubBtn->SetSize({ {0.f, 38.f}, {0.f, 42.f} });
			SubBtn->SetPosition({ { 0.f, -19.f }, { 0.f, 0.f } });
			PlusBtn->SetText("");
			PlusBtn->SetNormalImage("set:lucky_sky_backpack.json image:plus");
			PlusBtn->SetPushedImage("set:lucky_sky_backpack.json image:plus");
			PlusBtn->SetSize({ {0.f, 38.f}, {0.f, 42.f} });
			PlusBtn->SetHorizontalAlignment(HA_RIGHT);
			PlusBtn->SetPosition({{ 0.f, 19.f }, { 0.f, 0.f } });

			m_sell_input_num->SetBackImage("set:lucky_sky_backpack.json image:num_bg");
			m_sell_input_num->SetSize({ { 0.45f, 0.f }, { 0.f, 38.f } });
			m_sell_input_num->SetPosition({ {0.f, 0.f}, {0.04f, 0.f} });

			m_sell_name->SetTextHorzAlign(HA_CENTRE);
			m_sell_name->SetPosition({ {0.f, 0.f}, {-0.00515464f, 0.f} });

			m_sellBtn->SetVisible(false);
			m_abandonBtn->SetVisible(true);
			m_abandonBtn->SetNormalImage("set:lucky_sky_backpack.json image:yellow_btn");
			m_abandonBtn->SetPushedImage("set:lucky_sky_backpack.json image:yellow_btn");
			m_abandonBtn->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			m_abandonBtn->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "12 12 12 12");

			getWindow<GUILayout>("ArmorPanel-SellItemDesc")->SetPosition({ {0.213227f, 0.f}, {0.12f, 0.f} });

			m_ic_defence->SetVisible(true);
			m_ic_defence->SetSize({ {0.f, 280.f}, {0.f, 19.f} });
			m_ic_defence->SetInterval(2.f);
			for (int i = 1; i < 11; i++)
			{
				GUIString Name = StringUtil::Format("ArmorPanel-Defense-Icon-%d", i).c_str();
				auto icon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, Name);
				icon->SetImage("set:lucky_sky_backpack.json image:armor_nor");
				icon->SetSize({ {0.f, 26.f}, {0.f, 19.f} });
				m_ic_defence->AddItem(icon, false);
			}
		}
	}

	void gui_armorPanelControl::onShow()
	{
		m_enderChest->SetVisible(LogicSetting::Instance()->isUseMovingEnderChest());
		if (GameClient::CGame::Instance()->isEnterBack())
		{
			return;
		}
		String defaultIdle = "idle";
		if (Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_pPlayer->m_defaultIdle > 0)
		{
			defaultIdle = "idle_" + StringUtil::Format("%d", Blockman::Instance()->m_pPlayer->m_defaultIdle);
		}

		String actorName = "";
		if (Blockman::Instance()->m_pPlayer->m_sex == 2)
		{
			if (!PlayerPrefs::Instance()->HasProperty("GirlActorName"))
				actorName = "girl.actor";
			else
				actorName = PlayerPrefs::Instance()->getStringPrefs("GirlActorName");
		}
		else
		{
			if (!PlayerPrefs::Instance()->HasProperty("BoyActorName"))
				actorName = "boy.actor";
			else
				actorName = PlayerPrefs::Instance()->getStringPrefs("BoyActorName");
		}

		m_actorWindow->SetActor(actorName.c_str(), defaultIdle.c_str());

		if (m_actorWindow->GetActor() == NULL)
		{
			actorName = (Blockman::Instance()->m_pPlayer->m_sex == 2) ? "girl.actor" : "boy.actor";
			m_actorWindow->SetActor(actorName.c_str(), defaultIdle.c_str());
		}

		m_actorWindow->reflreshArmor(Blockman::Instance()->m_pPlayer);
		// m_actorWindow->GetActor()->SetSkyBlockColor(Color(0.6f, 0.3f, 0.5f, 1.0f));
		m_actorWindow->GetActor()->SetCustomColor(Blockman::Instance()->m_pPlayer->m_skinColor);
		if (m_actorInventoryName != nullptr) {
			m_actorInventoryName->SetText(getString(LanguageKey::WORDART_BAG));
		}
		// m_actorWindow->SetRotateY(180);
		m_selectionIndex = -1;
		m_guiScrollableInventory->selectItemSlot(m_selectionIndex);
		getWindow<GUIStaticText>("ArmorPanel-Inventory-Detail-Text")->SetText("");
		getWindow<GUIStaticText>("ArmorPanel-Inventory-Detail-Name")->SetText("");
		getWindow<GUIStaticImage>("ArmorPanel-Inventory-Detail-Item-Image")->SetImage("");
	}

	void BLOCKMAN::gui_armorPanelControl::onAttach()
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::WalkingDead)
		{
			m_guiScrollableInventory->setStyle(GuiItemSlot::Style::WALKING_DEAD);
			m_guiScrollableInventory->SetArea({ 0.0195f , 0 }, { 0.165f, 0 }, { 0.960f, 0 }, { 0.771f, 0 });
			m_guiScrollableInventory->setRowSize(7);

			getWindow<GUILayout>("ArmorPanel-Player-Model")->SetArea({ 0.0f ,-12 }, { 0, 0 }, { 0.45f, 15 }, { 1, 5 });
			getWindow<GUILayout>("ArmorPanel-Player-Model")->SetBackImage("set:walkingdead_armor.json image:bg_left_layout_down");
			getWindow<GUILayout>("ArmorPanel-Player-Model")->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			getWindow<GUILayout>("ArmorPanel-Player-Model")->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "86 5 30 15");
			getWindow<GUIStaticImage>("ArmorPanel-Foot-Icon")->SetArea({ 0 ,0 }, { 0.68f, 0 }, { 0, 250 }, { 0, 95 });
			getWindow<GUIStaticImage>("ArmorPanel-Foot-Icon")->SetVerticalAlignment(VA_TOP);
			getWindow<GUIStaticImage>("ArmorPanel-Foot-Icon")->SetImage("set:walkingdead_armor.json image:bg_player_under");
			getWindow<GUIStaticImage>("ArmorPanel-Player-Model-Title-BG")->SetArea({ 0 ,0 }, { 0.68f, 70 }, { 0, 209 }, { 0, 41 });
			getWindow<GUIStaticImage>("ArmorPanel-Player-Model-Title-BG")->SetImage("set:walkingdead_armor.json image:bg_player_name");
			getWindow<GUIStaticImage>("ArmorPanel-Player-BG")->SetVisible(true);
			m_equipedArmorTable->SetProperty(GWP_SLOT_STYLE, StringUtil::ToString((int)(GuiItemSlot::Style::WALKING_DEAD_ARMOR)).c_str());
			m_equipedArmorTable->SetProperty(GWP_SLOT_WIDTH, "82");
			m_equipedArmorTable->SetProperty(GWP_SLOT_HEIGHT, "82");
			m_equipedArmorTable->SetArea({ 0 , 0 }, { 0, 110 }, { 1, -208 }, { 0, 310 });
			m_actorTitleName->SetFont(GUIFontManager::Instance()->GetFont("HT14"));

			getWindow<GUILayout>("ArmorPanel-Inventory-Layout")->SetArea({ 0 , 0 }, { 0, 0 }, { 0.55f, 0 }, { 0.67f, 0 });
			getWindow<GUILayout>("ArmorPanel-Inventory-Layout")->SetBackImage("set:walkingdead_armor.json image:bg_right_layout");
			getWindow<GUILayout>("ArmorPanel-Inventory-Layout")->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			getWindow<GUILayout>("ArmorPanel-Inventory-Layout")->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "40 120 120 60");

			getWindow<GUILayout>("ArmorPanel-Inventory-Radio")->SetArea({ 0.0195f , 0 }, { -0.873f, 0 }, { 0.961f, 0 }, { 0, 0 });

			getWindow<GUILayout>("ArmorPanel-Inventory-Detail")->SetArea({ 0 , 0 }, { 0.5f, 10 }, { 1, 0 }, { 0.5f, 10 });
			getWindow<GUILayout>("ArmorPanel-Inventory-Detail")->SetBackImage("set:walkingdead_armor.json image:bg_right_detail");
			getWindow<GUILayout>("ArmorPanel-Inventory-Detail")->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			getWindow<GUIStaticImage>("ArmorPanel-Inventory-Detail-Item-BackGround")->SetImage("set:walkingdead_armor.json image:bg_detail_item");
			getWindow<GUIStaticImage>("ArmorPanel-Inventory-Detail-Name-BG")->SetImage("set:walkingdead_armor.json image:bg_detail_name");
			getWindow<GUIStaticImage>("ArmorPanel-Inventory-Detail-Name-BG")->SetSize({ {0,158.625f} ,{0,24.75f} });
			getWindow("ArmorPanel-Inventory-Title-Name")->SetVisible(false);

			getWindow<GUIButton>("ArmorPanel-EnderChest")->SetPushedImage("set:walkingdead_armor.json image:btn_moving_ender_chest");
			getWindow<GUIButton>("ArmorPanel-EnderChest")->SetNormalImage("set:walkingdead_armor.json image:btn_moving_ender_chest");
			getWindow<GUIButton>("ArmorPanel-EnderChest")->SetArea({ 0 , 30 }, { 0, 25 }, { 0, 59 }, { 0, 46 });

			auto ender_text_bg = dynamic_cast<GUIStaticImage*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, "ArmorPanel-EnderChest-Text-BG"));
			ender_text_bg->SetArea({ 0 , 90 }, { 0, 42 }, { 0, 136 }, { 0, 22 });
			ender_text_bg->SetImage("set:walkingdead_armor.json image:bg_ender_chest_text");
			getWindow<GUILayout>("ArmorPanel-Player-Model")->AddChildWindow(ender_text_bg);

			auto ender_text = dynamic_cast<GUIStaticText*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, "ArmorPanel-EnderChest-Text"));
			ender_text->SetText(LanguageManager::Instance()->getString("inventory_panel_ender_chest").c_str());
			ender_text->SetFont(GUIFontManager::Instance()->GetFont("HT14"));
			ender_text->SetTextVertAlign(VA_CENTRE);
			ender_text->SetArea({ 0 , 0 }, { 0, 0 }, { 1, 0 }, { 1, 0 });
			ender_text_bg->AddChildWindow(ender_text);

			auto map = ClassificationSetting::getTabMap();
			float layoutX = getWindow("ArmorPanel-Inventory-Radio")->GetPixelSize().x;
			float intervalX = 3.0;
			int tabSize = map.size() + 1;
			float itemSizeX = (layoutX - intervalX * (tabSize + 1)) / tabSize;

			m_radioMap[(int)ViewId::ALL]->SetNormalImage("set:walkingdead_armor.json image:bg_tab_normal");
			m_radioMap[(int)ViewId::ALL]->SetPushedImage("set:walkingdead_armor.json image:bg_tab_selected");
			m_radioMap[(int)ViewId::ALL]->SetArea({ 0, intervalX }, { 0, 0 }, { 0, itemSizeX }, { 0.0, 35.0f });
			m_radioMap[(int)ViewId::ALL]->SetProperty(GWP_WINDOW_STRETCH_TYPE, "None");
			m_radioMap[(int)ViewId::ALL]->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "0 0 0 0");
			m_radioMap[(int)ViewId::ALL]->SetProperty(GWP_TEXT_SHADOW, "false");
			m_radioMap[(int)ViewId::ALL]->SetTextColor((Color(124.f / 255.f, 70.f / 255.f, 23.f / 255.f)));
			m_radioMap[(int)ViewId::ALL]->SetProperty(GWP_TEXT_SHADOW, "false");
			m_radioMap[(int)ViewId::ALL]->SetRenderOffset({ 0,2 });
			m_radioMap[(int)ViewId::ARMOR]->SetVisible(false);

			int i = 1;
			auto iter = map.begin();
			for (iter; iter != map.end();)
			{
				auto tab = iter->second;
				auto radio = m_radioMap[(int)ViewId::CUSTOM + tab->tabId];
				radio->SetNormalImage("set:walkingdead_armor.json image:bg_tab_normal");
				radio->SetPushedImage("set:walkingdead_armor.json image:bg_tab_selected");
				radio->SetArea({ 0, intervalX + (intervalX + itemSizeX) * i }, { 0, 0 }, { 0,itemSizeX }, { 0.0, 35.0f });
				radio->SetProperty(GWP_WINDOW_STRETCH_TYPE, "None");
				radio->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "0 0 0 0");
				radio->SetProperty(GWP_TEXT_SHADOW, "false");
				radio->SetTextColor((Color(198.f / 255.f, 179.f / 255.f, 143.f / 255.f)));
				radio->SetRenderOffset({ 0,2 });
				iter++;
				i++;
			}
		}
	}

	void gui_armorPanelControl::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown()) return;
	
		if (m_showTipTime > 0 && m_showTipTime < 1000)
		{
			m_showTipTime = nTimeElapse + m_showTipTime;
		}
		else
		{
			m_showTipTime = 0;
			m_dressBg->SetVisible(false);
			m_tipMessage = "";
		}
		m_textTip->SetText(m_tipMessage.c_str());

		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		m_actorTitleName->SetText(Blockman::Instance()->m_pPlayer->getEntityName().c_str());
		if (inventory == nullptr)
			return;
		auto armorInventory = inventory->getArmorInventory();
		//m_guiScrollableInventory->setInventory(inventory->getArmorFilteredInventory());
		i32 size = m_equipedArmorTable->getColumnCount() * m_equipedArmorTable->getRowCount();
		for (int i = 0; i < size; ++i)
		{
			m_equipedArmorTable->setItemStack(i / m_equipedArmorTable->getColumnCount(),
				i % m_equipedArmorTable->getColumnCount(), armorInventory->getStackInSlot(i));
			if (i >= 4)
				continue;
			m_equipedArmorTable->setItemStackArmorType(i / m_equipedArmorTable->getColumnCount(),
				i % m_equipedArmorTable->getColumnCount(), ArmorType(i));
		}
		updateData();
		updateArmorUI();
	}

	void gui_armorPanelControl::updateArmorUI()
	{
		if (GameClient::CGame::Instance()->GetGameType() != ClientGameType::SkyBlockMain
			&& GameClient::CGame::Instance()->GetGameType() != ClientGameType::SkyBlockMineCave
			&& GameClient::CGame::Instance()->GetGameType() != ClientGameType::SkyBlockProduct
			&& GameClient::CGame::Instance()->GetGameType() != ClientGameType::SkyWarLuckyBlock)
		{
			return;
		}

		EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
		if (player)
		{
			int defense = player->getTotalArmorValue();
			m_defence_num->SetText(StringUtil::Format("%d", defense).c_str());
			updateIconDefense(defense);

			int damageCurrentItem = 0;
			int baseDamage = (int)player->getAttributeMap()->getAttributeInstance(SharedMonsterAttributes::ATTACK_DAMAGE)->getBaseValue();
			CustomArmorSettingInfo customArmorSettingInfo;
			if (player->inventory 
				&& player->inventory->getCurrentItem() 
				&& CustomArmorSetting::Instance()->getArmorSettingInfo(player->inventory->getCurrentItem()->itemID, customArmorSettingInfo))
			{
				damageCurrentItem = customArmorSettingInfo.attack;
			}
			int damageArmor = player->getTotalArmorCustomAttackValue();
			m_attack_num->SetText(StringUtil::Format("%d", damageCurrentItem + damageArmor + baseDamage).c_str());

			int hp = (int)player->getBaseHealth();
			m_hp_num->SetText(StringUtil::Format("%d", hp).c_str());
		}
	}

	bool gui_armorPanelControl::equipArmor(ItemStackPtr itemStack)
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		auto index = inventory->findItemStack(itemStack);
		if (inventory->equipArmor(itemStack))
		{
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendEquipArmor(index);
			Blockman::Instance()->m_pPlayer->m_outLooksChanged = true;
			m_actorWindow->reflreshArmor(Blockman::Instance()->m_pPlayer);
		}
		return true;
	}

	bool gui_armorPanelControl::unequipArmor(const EventArgs & event)
	{
		auto touchEvent = dynamic_cast<const GuiSlotTable::ItemStackTouchEventArgs&>(event);
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		auto itemStack = inventory->getArmorInventory()->getStackInSlot(touchEvent.row * 2 + touchEvent.column);

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain
			|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMineCave
			|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockProduct
			|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyWarLuckyBlock)
		{
			itemStack = inventory->getArmorInventory()->getStackInSlot(touchEvent.row);
		}
		auto index = inventory->findItemStack(itemStack);
		if (itemStack)
		{
			auto index = inventory->findItemStack(itemStack);
			auto MainInventorySize = inventory->getCurrentInventoryCount();
			auto setting = ArmorSetting::getSettingByItemId(itemStack->itemID);
			if (setting)
			{
				for (int i = MainInventorySize - setting->extraInvCount; i < MainInventorySize; i++)
				{
					if (inventory->getMainInventory()->getStackInSlot(i))
					{
						ShowResultTipDialogEvent::emit(ResultCode::COMMON, LanguageKey::GUI_ARMOR_CAN_NOT_UNEQUIP_BAG);
						return false;
					}
				}
			}
			if (inventory->unequipArmor(itemStack))
			{
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendUnequipArmor(index);
			}
			Blockman::Instance()->m_pPlayer->m_outLooksChanged = true;
			m_actorWindow->reflreshArmor(Blockman::Instance()->m_pPlayer);
		}
		return true;
	}

	bool gui_armorPanelControl::onClickEnderChest(const EventArgs & events)
	{
		ClientNetwork::Instance()->getCommonSender()->sendOpenMovingEnderChest();
		return true;
	}

	bool gui_armorPanelControl::onItemStackTouchUp(const EventArgs & event)
	{
		auto touchIndex = dynamic_cast<const GuiScrollableInventory::ItemStackTouchEventArgs&>(event).index;
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		if (m_showViewId != ViewId::ARMOR)
		{
			int m_selectionMainIndex = m_selectionIndex;
			int touchMainIndex = touchIndex;
			if (m_showViewId != ViewId::ALL)
			{
				if (m_selectionIndex != -1)
				{
					auto m_selectionItemStack = m_guiScrollableInventory->getInventory()->getStackInSlot(m_selectionIndex);
					m_selectionMainIndex = inventory->getMainInventory()->findItemStack(m_selectionItemStack);
				}
				auto touchItemStack = m_guiScrollableInventory->getInventory()->getStackInSlot(touchIndex);
				touchMainIndex = inventory->getMainInventory()->findItemStack(touchItemStack);
			}

			ItemStackPtr currentItem = inventory->getCurrentItem();
			if (m_selectionIndex == touchIndex)
			{
				equipArmor(inventory->getMainInventory()->getStackInSlot(touchMainIndex));
				m_selectionIndex = -1;
			}
			else if (m_selectionIndex >= 0)
			{
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendSwapItemPosition(m_selectionMainIndex, touchMainIndex);
				inventory->swapStacksInSlots(m_selectionMainIndex, touchMainIndex);
				m_selectionIndex = -1;
			}
			else if (m_guiScrollableInventory->getItemStack(touchIndex) == nullptr)
			{
				inventory->removeCurrentItemFromHotbar();
			}
			else if (m_guiScrollableInventory->getItemStack(touchIndex) == currentItem)
			{
				m_selectionIndex = touchIndex;
			}
			else
			{
				inventory->putItemToHotbar(touchMainIndex);
			}

			ItemStackPtr touchStack = inventory->getMainInventory()->getStackInSlot(touchMainIndex);

			if (touchStack && touchStack->stackSize > 0 && !checkGameType(ClientGameType::SkyWarLuckyBlock))
			{
				ShowItemNameEvent::emit(touchStack);
			}
			m_guiScrollableInventory->selectItemSlot(m_selectionIndex);

			refreshSellSelectItem();

			if (ClassificationSetting::haveCustomDetail())
			{
				String detailText = "";
				if (m_selectionIndex != -1)
				{
					auto settingItem = ClassificationSetting::getItem(touchStack->getItem()->itemID);
					if (settingItem)
					{
						StringArray details = StringUtil::Split(settingItem->detail, "#");
						for (String detail : details)
						{
							detailText += LanguageManager::Instance()->getString(detail);
						}
					}
					getWindow<GUIStaticText>("ArmorPanel-Inventory-Detail-Text")->SetText(detailText.c_str());
					getWindow<GUIStaticText>("ArmorPanel-Inventory-Detail-Name")->SetText(LanguageManager::Instance()->getItemName(touchStack->getItemName()).c_str());
					if (settingItem)
					{
						getWindow<GUIStaticImage>("ArmorPanel-Inventory-Detail-Item-Image")->SetImage(GuiItemStack::getItemSpriteName(settingItem->itemId, 0, 0).c_str());
					}
				}
				else
				{
					getWindow<GUIStaticText>("ArmorPanel-Inventory-Detail-Text")->SetText(detailText.c_str());
					getWindow<GUIStaticText>("ArmorPanel-Inventory-Detail-Name")->SetText("");
					getWindow<GUIStaticImage>("ArmorPanel-Inventory-Detail-Item-Image")->SetImage("");
				}
			}
		}
		else
		{
			equipArmor(inventory->getArmorFilteredInventory()->getStackInSlot(touchIndex));
		}
		PlayerChangeItemEvent::emit(inventory->currentItemIndex, false);
		return true;
	}

	bool gui_armorPanelControl::onRadioChange(const EventArgs & events , const ViewId viewId , int tabId)
	{
		if (m_radioMap[(int)viewId + tabId]->IsSelected()) {
			m_showViewId = viewId;
			m_customTabId = tabId;
			m_selectionIndex = -1;
			m_guiScrollableInventory->selectItemSlot(m_selectionIndex);
		}
		getWindow<GUIStaticText>("ArmorPanel-Inventory-Detail-Text")->SetText("");
		getWindow<GUIStaticText>("ArmorPanel-Inventory-Detail-Name")->SetText("");
		getWindow<GUIStaticImage>("ArmorPanel-Inventory-Detail-Item-Image")->SetImage("");
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::WalkingDead)
		{
			for (auto iter : m_radioMap)
			{
				iter.second->SetTextColor((Color(198.f / 255.f, 179.f / 255.f, 143.f / 255.f)));
			}
			m_radioMap[(int)viewId + tabId]->SetTextColor((Color(124.f / 255.f, 70.f / 255.f, 23.f / 255.f)));
		}
		return true;
	}

	bool gui_armorPanelControl::onRemoveCurrentItem()
	{
		if (m_selectionIndex != -1)
		{
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendRemoveItemPosition(m_selectionIndex);
			m_selectionIndex = -1;
			m_guiScrollableInventory->selectItemSlot(m_selectionIndex);
		}
		return true;
	}

	void gui_armorPanelControl::updateData()
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		if (inventory == nullptr)
			return;
		switch (m_showViewId)
		{
		case ViewId::ALL:
			m_guiScrollableInventory->setInventory(inventory->getMainInventory());
			m_guiScrollableInventory->setShowCount(inventory->getCurrentInventoryCount());
			break;
		case ViewId::ARMOR:
			m_guiScrollableInventory->setInventory(inventory->getArmorFilteredInventory());
			break;
		case ViewId::MATERIALS:
			break;
        case ViewId::CUSTOM:
            auto tab = ClassificationSetting::findTabById(m_customTabId);
            m_guiScrollableInventory->setInventory(inventory->getCustomClassificationInventory(m_customTabId, tab->itemIds));
            break;
		}
	}

	bool gui_armorPanelControl::refreshSellSelectItem()
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		ItemStackPtr touchStack = inventory->getMainInventory()->getStackInSlot(m_selectionIndex);
		if (touchStack && touchStack->stackSize > 0)
		{
			m_sellSlot->setItemStack(touchStack);

			SellItemInfo sellItemInfo;
			AbandonItemInfo abandonInfo;
			bool canAbandon = true;
			int money = 0;
			String desc = "";
			if (m_abandonBtn->IsVisible())
			{
				if (touchStack && AbandonItemSetting::Instance()->getAbandonInfoFromMap(touchStack->itemID, touchStack->itemDamage, abandonInfo))
				{
					canAbandon = abandonInfo.canAbandon;
					desc = abandonInfo.itemDesc;
				}
				if (canAbandon)
				{
					m_cur_sell_num = touchStack->stackSize;
					m_abandonBtn->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_ARMOR_BAG_ABANDON_BTN_TXT).c_str());
				}
				else
				{
					m_cur_sell_num = 1;
					m_abandonBtn->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_ARMOR_BAG_CAN_NOT_ABANDON_BTN_TXT).c_str());
				}
			}
			else
			{
				if (touchStack && SellItemSetting::Instance()->getSellInfoFromMap(touchStack->itemID, touchStack->itemDamage, sellItemInfo))
				{
					money = sellItemInfo.itemMoney;
					desc = sellItemInfo.itemDesc;
				}

				if (money > 0)
				{
					m_cur_sell_num = touchStack->stackSize;
					m_cur_sell_money = money;
					m_sellBtn->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_ARMOR_BAG_SELL_BTN_TXT).c_str());
				}
				else
				{
					m_cur_sell_num = 1;
					m_cur_sell_money = 0;
					m_sellBtn->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_ARMOR_BAG_CAN_NOT_SELL_BTN_TXT).c_str());
				}
			}
			m_sell_item_desc->SetText(LanguageManager::Instance()->getString(desc.c_str()).c_str());
			m_sell_item_desc->SetVisible(true);
			m_sell_input_show_num->SetText(StringUtil::ToString(m_cur_sell_num).c_str());
			m_sell_input_show_num->SetVisible(true);
			m_sell_sum_num_txt->SetText(StringUtil::Format(getString(LanguageKey::GUI_ARMOR_BAG_SELL_SUM_NUM_TXT).c_str(), m_cur_sell_num * m_cur_sell_money).c_str());
			m_sell_sum_num_txt->SetVisible(true);
			if (m_abandonBtn->IsVisible() && abandonInfo.itemName.size() > 0)
			{
				m_sell_name->SetText(LanguageManager::Instance()->getString(abandonInfo.itemName.c_str()).c_str());
			}
			else
			{
				m_sell_name->SetText(LanguageManager::Instance()->getItemName(touchStack->getItemName()).c_str());
			}
			m_sell_name->SetVisible(true);

		}
		else
		{
			m_cur_sell_num = 1;
			m_cur_sell_money = 0;
			m_sellSlot->setItemStack(nullptr);
			m_sell_input_show_num->SetVisible(false);
			m_sell_sum_num_txt->SetVisible(false);
			m_sell_item_desc->SetVisible(false);
			m_sell_name->SetVisible(false);
		}
		return true;
	}

	bool gui_armorPanelControl::onInputChanged(const EventArgs & events)
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		ItemStackPtr touchStack = inventory->getMainInventory()->getStackInSlot(m_selectionIndex);
		m_sell_input_num->SetText("");

		if (touchStack == nullptr) return true;

		if (m_cur_sell_num < 1 || m_cur_sell_num > touchStack->stackSize) return true;

		std::string text = m_sell_input_num->GetText().c_str();
		m_sell_input_num->SetText("");
		m_cur_sell_num = std::atoi(text.c_str());
		m_sell_input_show_num->SetText(StringUtil::ToString(m_cur_sell_num).c_str());
		m_sell_sum_num_txt->SetText(StringUtil::Format(getString(LanguageKey::GUI_ARMOR_BAG_SELL_SUM_NUM_TXT).c_str(), m_cur_sell_num * m_cur_sell_money).c_str());
		return true;
	}

	bool gui_armorPanelControl::onButtonNumClick(const EventArgs & events, bool add)
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		ItemStackPtr touchStack = inventory->getMainInventory()->getStackInSlot(m_selectionIndex);

		if (touchStack == nullptr)
		{
			m_sell_input_show_num->SetVisible(false);
			m_sell_sum_num_txt->SetVisible(false);
			m_sell_item_desc->SetVisible(false);
			m_sell_name->SetVisible(false);
			return true;
		}

		if (add)
		{
			if (m_cur_sell_num < touchStack->stackSize)
				m_cur_sell_num++;
		}
		else
		{
			if (m_cur_sell_num > 1)
				m_cur_sell_num--;
		}
		m_sell_input_show_num->SetVisible(true);
		m_sell_sum_num_txt->SetVisible(true);

		m_sell_input_show_num->SetText(StringUtil::ToString(m_cur_sell_num).c_str());
		m_sell_sum_num_txt->SetText(StringUtil::Format(getString(LanguageKey::GUI_ARMOR_BAG_SELL_SUM_NUM_TXT).c_str(), m_cur_sell_num * m_cur_sell_money).c_str());

		return true;
	}

	bool gui_armorPanelControl::onSellClick(const EventArgs & events)
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		ItemStackPtr touchStack = inventory->getMainInventory()->getStackInSlot(m_selectionIndex);

		if (touchStack == nullptr) return true;

		if (SCRIPT_EVENT::SellItemEvent::invoke(touchStack->itemID, touchStack->itemDamage))
		{
			sellItem();
		}
		return true;
	}

	bool BLOCKMAN::gui_armorPanelControl::onAbandonClick(const EventArgs & events)
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		ItemStackPtr touchStack = inventory->getMainInventory()->getStackInSlot(m_selectionIndex);

		if (touchStack == nullptr) return true;

		if (m_cur_sell_num < 1 || m_cur_sell_num > touchStack->stackSize) return true;

		AbandonItemInfo abandonInfo;
		if (AbandonItemSetting::Instance()->getAbandonInfoFromMap(touchStack->itemID, touchStack->itemDamage, abandonInfo) && !abandonInfo.canAbandon)
			return true;
		GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendAbandonItem(m_selectionIndex, m_cur_sell_num);
		m_selectionIndex = -1;
		m_guiScrollableInventory->selectItemSlot(m_selectionIndex);
		return true;
	}

	void BLOCKMAN::gui_armorPanelControl::updateIconDefense(int value)
	{
		if (!m_ic_defence->IsVisible())
			return;

		int num = floor(value / 2);
		int rema = value % 2;
		for (int i = 1; i < 11; i++)
		{
			auto icon = dynamic_cast<GUIStaticImage*>(m_ic_defence->getContainerWindow()->GetChildByIndex(i - 1));
			if (i <= num)
			{
				icon->SetImage("set:lucky_sky_backpack.json image:armor_has");
			}
			else if(rema > 0 && i == num + 1)
			{
				icon->SetImage("set:lucky_sky_backpack.json image:armor_half");
			}
			else
			{
				icon->SetImage("set:lucky_sky_backpack.json image:armor_nor");
			}
		}
	}
	void gui_armorPanelControl::sellItem()
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		ItemStackPtr touchStack = inventory->getMainInventory()->getStackInSlot(m_selectionIndex);

		if (touchStack == nullptr) return;

		if (m_cur_sell_num < 1 || m_cur_sell_num > touchStack->stackSize) return;

		if (m_cur_sell_money <= 0) return;

		GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendSellItem(m_selectionIndex, touchStack->itemID, touchStack->itemDamage, m_cur_sell_num);
		m_selectionIndex = -1;
		m_guiScrollableInventory->selectItemSlot(m_selectionIndex);
	}

	bool gui_armorPanelControl::onDressSwitch(const EventArgs & events)
	{
		m_showTipTime = 1;
		m_textTip->SetVisible(true);
		m_dressBg->SetVisible(true);
		m_isCraftAllSwitch = !m_isCraftAllSwitch;
		if (m_isCraftAllSwitch)
		{
			m_tipMessage = LanguageManager::Instance()->getString("sky_block_switch_armor");
			LogicSetting::Instance()->setHideArmor(false);
			m_actorWindow->reflreshArmor(Blockman::Instance()->m_pPlayer);
			m_dressEffect2->SetVisible(true);
			m_dressEffect1->SetVisible(false);
			m_dress_left->SetVisible(true);
			m_dress_right->SetVisible(false);
		}
		else
		{
			m_tipMessage = LanguageManager::Instance()->getString("sky_block_switch_dress");
			LogicSetting::Instance()->setHideArmor(true);
			m_actorWindow->reflreshArmor(Blockman::Instance()->m_pPlayer);
			m_dressEffect2->SetVisible(false);
			m_dressEffect1->SetVisible(true);
			m_dress_left->SetVisible(false);
			m_dress_right->SetVisible(true);
		}
		return true;
	}
}
