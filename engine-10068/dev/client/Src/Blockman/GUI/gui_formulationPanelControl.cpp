#include "gui_formulationPanelControl.h"

#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "GUI/gui_playerInventoryControl.h"
#include "game.h"

#include "Item/Item.h"
#include "Item/ItemStack.h"
#include "Item/PotionManager.h"
#include "Item/Items.h"
#include "Inventory/InventoryPlayer.h"

#include "GUI/GuiDef.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "Util/StringUtil.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Setting/ClassificationSetting.h"

namespace BLOCKMAN
{
	gui_formulationPanelControl::gui_formulationPanelControl()
		: gui_layout("FormulationPanel.json")
	{
		FormulationSetting::loadSetting();
	}

	gui_formulationPanelControl::~gui_formulationPanelControl()
	{
		FormulationSetting::unloadSetting();
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_formulationPanelControl::onLoad()
	{
		m_gridView = getWindow<GUIGridView>("FormulationPanel-GridView");
		m_craftButton = getWindow<GUIButton>("FormulationPanel-CommitButton");
		m_target_icon = getWindow<GUIStaticImage>("FormulationPanel-Target-Icon");
		m_refresh_btn = getWindow<GUIButton>("FormulationPanel-Btn-Refresh");
		for (int i = 0;i < 4;i++)
		{
			m_material_map[i] = getWindow<GUIStaticImage>(StringUtil::Format("FormulationPanel-Material%d", i + 1).c_str());
		}

		m_craftButton->SetText(LanguageManager::Instance()->getString(LanguageKey::CRAFTING_PANEL_MAKE).c_str());
		m_craftButton->subscribeEvent("ButtonClick", SubscriberSlot(&gui_formulationPanelControl::onCraftButtonClick, this));
		m_refresh_btn->subscribeEvent("ButtonClick", SubscriberSlot(&gui_formulationPanelControl::onRefreshButtonClick, this));
	}

	void gui_formulationPanelControl::onShow()
	{
		float hInterval = 15;
		float vInterval = 10;
		int rowSize = 5;
		m_gridView->InitConfig(hInterval, vInterval, rowSize);
		item_size = (m_gridView->GetPixelSize().x - hInterval * (rowSize - 1)) / rowSize;
		updataGridView();
	}

	bool gui_formulationPanelControl::isNeedLoad()
	{
		return checkGameType(ClientGameType::WalkingDead);
	}

	void gui_formulationPanelControl::onUpdate(ui32 nTimeElapse)
	{
	}

	bool gui_formulationPanelControl::onItemTouchUp(const EventArgs & events,int index,int id)
	{
		selectItem(index, id);
		return true;
	}

	bool gui_formulationPanelControl::onCraftButtonClick(const EventArgs & event)
	{
		if (m_currentId)
		{
			auto setting = FormulationSetting::findFormulationById(m_currentId);
			auto pakcetSender = GameClient::CGame::Instance()->getNetwork()->getSender();
			pakcetSender->sendFormulationSubmit(setting->id , 1);
			playSoundByType(ST_Click);
		}
		return true;
	}

	bool gui_formulationPanelControl::onRefreshButtonClick(const EventArgs & event)
	{
		updataGridView();
		return true;
	}

	void gui_formulationPanelControl::updataGridView()
	{
		m_gridView->RemoveAllItems();
		m_currentIndex = -1;
		auto setting = FormulationSetting::getFormulationMap();
		vector<FormulationItem*>::type enAbleMap;
		auto iter = setting.begin();
		int i = 0;
		while (iter != setting.end())
		{
			auto item = iter->second;
			int count = checkComposable(item);
			if (count != 0)
			{
				m_gridView->AddItem(createItem(i, item, count));
				if (i == 0)
					selectItem(i, item->id);
				i++;
			}
			else
			{
				enAbleMap.push_back(item);
			}
			iter++;
		}
		for (auto it : enAbleMap)
		{
			m_gridView->AddItem(createItem(i, it, 0));
			if (i == 0)
				selectItem(i, it->id);
			i++;
		}
	}

	int gui_formulationPanelControl::checkComposable(FormulationItem* item)
	{
		int ableCount = INTPTR_MAX;
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		StringArray ids = StringUtil::Split(item->Materials, "#");

		for (String id:ids)
		{
			auto material = FormulationSetting::findMaterialById(StringUtil::ParseI32(id));
			int enoughMaterial = inventory->getItemNum(material->ItemId) / material->Count;
			ableCount = (ableCount < enoughMaterial)? ableCount:enoughMaterial;
			if (ableCount == 0)
				return 0;
		}

		return ableCount;
	}

	void gui_formulationPanelControl::selectItem(int index, int id)
	{
		if (m_currentIndex >= 0)
			m_gridView->getChildWindow(m_currentIndex)->GetChildByIndex(2)->SetVisible(false);
		m_gridView->getChildWindow(index)->GetChildByIndex(2)->SetVisible(true);

		m_currentIndex = index;
		m_currentId = id;
		auto setting = FormulationSetting::findFormulationById(m_currentId);
		m_target_icon->SetImage(setting->Icon.c_str());
		getWindow<GUIStaticText>("FormulationPanel-Detail-Name")->SetText(getString(setting->Name).c_str());
		getWindow<GUIStaticImage>("FormulationPanel-Item-Image")->SetImage(setting->Icon.c_str());
		auto settingItem = ClassificationSetting::getItem(setting->ItemId);
		if (settingItem)
		{
			getWindow<GUIStaticText>("FormulationPanel-Detail-Text")->SetText(getString(settingItem->detail).c_str());
		}
		StringArray ids = StringUtil::Split(setting->Materials, "#");
		for (int i = 0; i < 4; i++)
		{
			MaterialItem* material = nullptr;
			if (i < ids.size())
			{
				material = FormulationSetting::findMaterialById(StringUtil::ParseI32(ids[i]));
			}
			GUIStaticImage* icon = (GUIStaticImage*)m_material_map[i]->GetChildByIndex(0);
			auto text = m_material_map[i]->GetChildByIndex(1);
			if (material)
			{
				auto inventory = Blockman::Instance()->m_pPlayer->inventory;
				int haveNum = inventory->getItemNum(material->ItemId);
				if (haveNum < material->Count)
				{
					text->SetText(StringUtil::Format("▢FFFCFDE0%d/▢FFF91801%d", haveNum, material->Count).c_str());
				}
				else
				{
					text->SetText(StringUtil::Format("▢FF3BD873%d▢FFFCFDE0/%d", haveNum, material->Count).c_str());
				}
				icon->SetImage(material->Icon.c_str());
			}
			else
			{
				text->SetText("");
				icon->SetImage("");
			}
		}
	}

	GUIStaticImage * gui_formulationPanelControl::createItem(int index, FormulationItem* item, int num)
	{
		GUIStaticImage* guiItem = dynamic_cast<GUIStaticImage*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, StringUtil::Format("FormulationPanel-GridView-Item%d", index).c_str()));
		if (num == 0)
			guiItem->SetImage("set:walkingdead_formulation.json image:bg_item_not_enough");
		else
			guiItem->SetImage("set:walkingdead_formulation.json image:bg_item_enough");
		guiItem->SetArea({ 0, 0 }, { 0, 0 }, { 0, item_size }, { 0, item_size });

		guiItem->subscribeEvent(EventWindowTouchUp, std::bind(&gui_formulationPanelControl::onItemTouchUp, this, std::placeholders::_1,index, item->id));
		GUIStaticImage * guiItemIcon = dynamic_cast<GUIStaticImage*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, StringUtil::Format("FormulationPanel-GridView-Item%d-Icon", index).c_str()));
		guiItemIcon->SetImage(item->Icon.c_str());
		guiItemIcon->SetArea({ 0, 0 }, { 0, 0 }, { 0.8f, 0 }, { 0.8f, 0 });
		guiItemIcon->SetVerticalAlignment(VA_CENTRE);
		guiItemIcon->SetHorizontalAlignment(HA_CENTRE);
		guiItem->AddChildWindow(guiItemIcon);

		GUIStaticText* guiItemText = dynamic_cast<GUIStaticText*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, StringUtil::Format("FormulationPanel-GridView-Item%d-Text", index).c_str()));
		guiItemText->SetText(StringUtil::Format("▢FFFCFDE0%d",num).c_str());
		guiItemText->SetArea({ -0.1f, 0 }, { -0.05f, 0 }, { 0, 0 }, { 0, 0 });
		guiItemText->SetProperty(GWP_TEXT_SHADOW,"true");
		guiItemText->SetVerticalAlignment(VA_BOTTOM);
		guiItemText->SetHorizontalAlignment(HA_RIGHT);
		guiItemText->SetTextVertAlign(VA_BOTTOM);
		guiItemText->SetTextHorzAlign(HA_RIGHT);
		guiItemText->SetProperty(GWP_TEXT_BORDER, "true");
		guiItem->AddChildWindow(guiItemText);

		GUIStaticImage * guiItemSelect = dynamic_cast<GUIStaticImage*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, StringUtil::Format("FormulationPanel-GridView-Item%d-Select", index).c_str()));
		guiItemSelect->SetImage("set:walkingdead_formulation.json image:bg_item_selected");
		guiItemSelect->SetArea({ 0, 0 }, { 0, 0 }, { 1.0f, 0 }, { 1.0f, 0 });
		guiItemSelect->SetVisible(false);
		guiItem->AddChildWindow(guiItemSelect);

		return guiItem;
	}
}
