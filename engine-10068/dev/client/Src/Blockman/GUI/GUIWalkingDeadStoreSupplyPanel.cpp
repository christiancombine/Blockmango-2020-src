#include "GUIWalkingDeadStoreSupplyPanel.h"
#include "GUI/GuiDef.h"
#include "GUIWalkingDeadStoreSupplyItem.h"

namespace BLOCKMAN
{
	GUIWalkingDeadStoreSupplyPanel::GUIWalkingDeadStoreSupplyPanel() :
		gui_layout("WalkingDeadStoreSupplyPanel.json")
	{
	}

	GUIWalkingDeadStoreSupplyPanel::~GUIWalkingDeadStoreSupplyPanel()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GUIWalkingDeadStoreSupplyPanel::onLoad()
	{
		initGoodsList();
	}

	void GUIWalkingDeadStoreSupplyPanel::initGoodsList()
	{
		auto WalkingDeadStoreSupplyPanel = getWindow("WalkingDeadStoreSupplyPanel-Content");
		if (WalkingDeadStoreSupplyPanel == NULL)
		{
			return;
		}
		GUIString ChildsName = StringUtil::Format("WalkingDeadStoreSupplyPanel-Supply-List").c_str();
		m_supply_list = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, ChildsName);
		m_supply_list->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_supply_list->InitConfig(4.f, 4.f, 5);
		WalkingDeadStoreSupplyPanel->AddChildWindow(m_supply_list);
	}

	void GUIWalkingDeadStoreSupplyPanel::onUpdate(ui32 nTimeElapse)
	{

	}

	void GUIWalkingDeadStoreSupplyPanel::updateSupplyByTabId()
	{
		auto Supply = WalkingDeadStoreSetting::findAllSupplys();
		m_supply_list->RemoveAllItems();
		float ContentSize = m_supply_list->GetPixelSize().x;
		float ContentSizey = m_supply_list->GetPixelSize().y;
		static unsigned WalkingDeadChildCount = 0;
		float ChildWidth = (ContentSize - 4.0f * 4) / 4;
		for (auto chest : Supply)
		{
			WalkingDeadChildCount++;
			GUIString ChildName = StringUtil::Format("GUIWalkingDeadStoreSupplyPanel-chests-%d", WalkingDeadChildCount).c_str();
			auto Item = (GUIWalkingDeadStoreSupplyItem*)GUIWindowManager::Instance()->
				CreateGUIWindow(GWT_WALKING_DEAD_STORE_SUPPLY_ITEM, ChildName);
			Item->SetWidth({ 0, ChildWidth });
			Item->SetHeight({ 0, ContentSizey });
			Item->InitSupplyItems(chest.second);
			m_supply_list->AddItem(Item);
		}


	}

}
