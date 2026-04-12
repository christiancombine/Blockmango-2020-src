#include "GUIGunStorePanel.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "GUI/GUIStoreGunItem.h"
#include "GUI/GUIStorePropItem.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"
#include "GUIGunStoreModuleItem.h"

namespace BLOCKMAN
{
	GUIGunStorePanel::GUIGunStorePanel() :
		gui_layout("GunStorePanel.json")
	{
	}

	GUIGunStorePanel::~GUIGunStorePanel()
	{
	}

	void GUIGunStorePanel::InitPanels(float contentSize)
	{
		this->ContentSize = contentSize;
		auto GunStorePanel = getWindow("GunStorePanel");
		if (GunStorePanel == NULL)
			return;
		list<StoreTab*>::type tabs = GunStoreSetting::getStoreTabs();
		if (tabs.empty())
			return;
		static unsigned index = 0;
		for (auto tab : tabs)
		{
			index++;
			GUIString ChildsName = StringUtil::Format("GunStorePanel-Childs-%d", index).c_str();
			GUIGridView* pChildGV = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, ChildsName);
			pChildGV->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
			pChildGV->InitConfig(4.f, 4.f, 5);
			GunStorePanel->AddChildWindow(pChildGV);
			m_panel_map[tab] = pChildGV;
		}
	}

	void GUIGunStorePanel::InitPanelChilds()
	{

	}

	void GUIGunStorePanel::ResetPanelChilds(int type)
	{
		for (auto panel : m_panel_map)
		{
			StoreTab* tab = panel.first;
			GUIGridView* childs = panel.second;
			if (tab->Type == type)
			{
				switch (tab->TabType)
				{
				case BLOCKMAN::STORE_TAB_GUN:
					ResetGunChilds(tab, childs);
					break;
				case BLOCKMAN::STORE_TAB_PROP:
					ResetPropChilds(tab, childs);
					break;
				default:
					break;
				}
			}
		}
	}

	void GUIGunStorePanel::ShowPanelByType(int type)
	{
		for (auto panel : m_panel_map)
		{
			StoreTab* tab = panel.first;
			GUIGridView* childs = panel.second;
			childs->SetVisible(tab->Type == type);
			if (tab->Type == type)
			{
				switch (tab->TabType)
				{
				case BLOCKMAN::STORE_TAB_GUN:
					InitGunChilds(tab, childs);
					break;
				case BLOCKMAN::STORE_TAB_PROP:
					InitPropChilds(tab, childs);
					break;
				default:
					break;
				}
			}
		}
	}

	void GUIGunStorePanel::ShowPropSubtypePanle(int propId)
	{
		StoreProp* prop = GunStoreSetting::findStorePropById(propId);
		if (!prop || prop->Subtype == PROP_SUBTYPE_NONE)
			return;
		switch (prop->Subtype)
		{
		case PROP_SUBTYPE_BLOCK_MODULE:
			ShowBlockModulePanle(propId);
			break;
		default:
			break;
		}
	}

	void GUIGunStorePanel::ShowBlockModulePanle(int propId)
	{
		list<StoreBlockModule*>::type modules = GunStoreSetting::findStoreBlockModule();
		if (modules.empty())
		{
			return;
		}

		GUIGridView* BlockPanel = nullptr;

		for (auto panel : m_panel_map)
		{
			StoreTab* tab = panel.first;
			if (tab->Type == 7)
			{
				BlockPanel = panel.second;
			}
		}

		if (m_module_panel)
		{
			BlockPanel->RemoveItem(m_module_panel);

			for (int i = 1; i <= 9; i++)
			{
				GUIString PanelName = StringUtil::Format("GUIGunStorePanel-Module-Blank-Panel-%d", i).c_str();
				auto blankWindow = getWindow(PanelName);
				BlockPanel->RemoveItem(blankWindow);
			}
		}

		int index = FindPropIndexById(propId, BlockPanel);
		int line = (index + 1) / 5 + ((index + 1) % 5 > 0 ? 1 : 0);
		auto propItem = BlockPanel->GetContainer()->GetChildByIndex(index);
		
		int propCount = BlockPanel->GetContainer()->GetChildCount();
		int propLine = propCount / 5 + (propCount % 5 > 0 ? 1 : 0);
		int count = modules.size();
		int mouduleLine = count / 5 + (count % 5 > 0 ? 1 : 0);

		auto height = propItem->GetPixelSize().y;
		auto width = propItem->GetPixelSize().x; 

		if (line == propLine && propCount % 5 != 0)
		{
			for (int i = 1; i <= 5 - propCount % 5; i++)
			{
				GUIString PanelName = StringUtil::Format("GUIGunStorePanel-Module-Placeholder-Panel-%d", i).c_str();
				GUILayout* PlaceholderPanel = (GUILayout*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LAYOUT, PanelName);
				PlaceholderPanel->SetWidth({ 0.f, width });
				PlaceholderPanel->SetHeight({ 0.f, height });
				PlaceholderPanel->SetTouchable(false);
				BlockPanel->AddItem(PlaceholderPanel);
			}
		}

		int lastIndex = line * 5;
		list<GUIWindow*>::type windowList;

		for (int last = propCount - 1; last >= lastIndex; last--)
		{
			auto Item = BlockPanel->GetContainer()->GetChildByIndex(last);
			BlockPanel->RemoveItem(Item);
			windowList.push_front(Item);
		}

		if (!m_module_panel)
		{
			GUIString PanelName = StringUtil::Format("GUIGunStorePanel-Module-Panel").c_str();
			GUIGunStoreModulePanel* ModulePanel = (GUIGunStoreModulePanel*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GUN_STORE_MODULE_PANEL, PanelName);
			float panelHeight = mouduleLine * height + 4 * (mouduleLine  - 1) + 30;
			float panelWidth = BlockPanel->GetPixelSize().x - 2;
			ModulePanel->SetWidth({ 0.f, panelWidth });
			ModulePanel->SetHeight({ 0.f, panelHeight});
			ModulePanel->InitModules(modules);
			BlockPanel->AddItem(ModulePanel);
			m_module_panel = ModulePanel;
		}
		else
		{
			BlockPanel->AddItem(m_module_panel);
		}

		for (int i = 1; i <= 9; i++)
		{
			GUIString PanelName = StringUtil::Format("GUIGunStorePanel-Module-Blank-Panel-%d", i).c_str();
			GUILayout* BlankPanel = (GUILayout*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LAYOUT, PanelName);
			float panelHeight = mouduleLine * height + 4 * (mouduleLine - 1) + 30;
			float blankHeight = i < 5 ? panelHeight : 5;
			BlankPanel->SetWidth({ 0.f, 0.f });
			BlankPanel->SetHeight({ 0.f, blankHeight });
			BlankPanel->SetTouchable(false);
			BlockPanel->AddItem(BlankPanel);
		}

		for (auto win : windowList)
		{
			BlockPanel->AddItem(win);
		}
	}

	void GUIGunStorePanel::InitGunChilds(StoreTab * tab, GUIGridView * childs)
	{
		if (childs->GetItemCount() != 0)
			return;
		list<StoreGun*>::type guns = GunStoreSetting::findStoreGunByTabType(tab->Type);
		childs->RemoveAllItems();
		static unsigned GunChildCount = 0;
		float ChildWidth = (ContentSize - 4.0f * 4) / 5;
		bool IsFirst = true;
		for (auto gun : guns)
		{
			if (IsFirst)
			{
				gun->IsSelected = true;
				if (tab->Type == 1)
				{
					GunStoreGunItemClickEvent::emit(gun->Id);
				}
			}
			GunChildCount++;
			GUIString ChildName = StringUtil::Format("GUIGunStorePanel-Gun-Child-%d", GunChildCount).c_str();
			GUIStoreGunItem* GunItem = (GUIStoreGunItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STORE_GUN_ITEM, ChildName);
			GunItem->SetWidth({ 0, ChildWidth });
			GunItem->SetHeight({ 0, ChildWidth * 0.8f });
			GunItem->InitStoreGun(gun);
			childs->AddItem(GunItem);
			IsFirst = false;
		}
	}

	void GUIGunStorePanel::InitPropChilds(StoreTab * tab, GUIGridView * childs)
	{
		if (childs->GetItemCount() != 0)
			return;
		list<StoreProp*>::type props = GunStoreSetting::findStorePropByTabType(tab->Type);
		childs->RemoveAllItems();
		static unsigned PropChildCount = 0;
		float ChildWidth = (ContentSize - 4.0f * 4) / 5;
		bool IsFirst = true;
		int selectProp = 0;
		int index = 0;
		for (auto prop : props)
		{
			if (IsFirst)
			{
				prop->IsSelected = true;
				if (tab->Type == 1)
				{
					GunStorePropItemClickEvent::emit(prop->Id);
				}
				else if (tab->Type == 7)
				{
					selectProp = prop->Id;
				}
			}
			index++;
			PropChildCount++;
			GUIString ChildName = StringUtil::Format("GUIGunStorePanel-Prop-Child-%d", PropChildCount).c_str();
			GUIStorePropItem* PropItem = (GUIStorePropItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STORE_PROP_ITEM, ChildName);
			PropItem->SetWidth({ 0, ChildWidth });
			PropItem->SetHeight({ 0, ChildWidth * 0.8f });
			PropItem->InitStoreProp(prop);
			childs->AddItem(PropItem);
			IsFirst = false;
		}
		if (selectProp != 0)
		{
			ShowPropSubtypePanle(selectProp);
		}
	}

	void GUIGunStorePanel::ResetGunChilds(StoreTab * tab, GUIGridView * childs)
	{
		list<StoreGun*>::type guns = GunStoreSetting::findStoreGunByTabType(tab->Type);
		size_t count = childs->GetItemCount();
		auto iter = guns.begin();
		for (size_t i = 0; i < count; i++)
		{
			if (iter == guns.end())
				break;
			GUIStoreGunItem* GunItem = (GUIStoreGunItem*)childs->GetItem(i);
			if (GunItem)
			{
				GunItem->InitStoreGun(*iter);
				if (i == 0 && m_isFirst)
				{
					m_isFirst = false;
					GunStoreGunItemClickEvent::emit((*iter)->Id);
				}
			}
			iter++;
		}
	}

	void GUIGunStorePanel::ResetPropChilds(StoreTab * tab, GUIGridView * childs)
	{
		list<StoreProp*>::type props = GunStoreSetting::findStorePropByTabType(tab->Type);
		size_t count = childs->GetItemCount();
		auto iter = props.begin();
		for (size_t i = 0; i < count; i++)
		{
			if (iter == props.end())
				break;
			GUIStorePropItem* PropItem = (GUIStorePropItem*)childs->GetItem(i);
			bool isNullWindow = PropItem->IsNullWindow();
			if (!isNullWindow)
			{
				PropItem->InitStoreProp(*iter);
				iter++;
			}
		}
	}

	void GUIGunStorePanel::ResetModuleChild()
	{
		list<StoreBlockModule*>::type modules = GunStoreSetting::findStoreBlockModule();
		if (!m_module_panel)
			return;
		size_t count = m_module_panel->GetPanelItemCount();
		auto iter = modules.begin();
		for (size_t i = 0; i < count; i++)
		{
			if (iter == modules.end())
				break;
			GUIGunStoreModuleItem* ModuleItem = (GUIGunStoreModuleItem*)m_module_panel->GetPanelItem(i);
			if (ModuleItem)
			{
				ModuleItem->InitModuleItem(*iter);
				iter++;
			}
		}
	}

	int GUIGunStorePanel::FindPropIndexById(int propId, GUIGridView* propPanel)
	{
		int count = propPanel->GetContainer()->GetChildCount();
		for (int index = 0; index < count; index++)
		{
			GUIStorePropItem* Item = (GUIStorePropItem*)propPanel->GetItem(index);
			if (Item)
			{
				if (Item->GetPropId() == propId)
				{
					return index;
				}
			}
		}
		return 0;
	}

}
