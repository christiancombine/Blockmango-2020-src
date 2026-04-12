#include "GUIGunStoreModulePanel.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "GUI/GUIStoreGunItem.h"
#include "GUI/GUIStorePropItem.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"
#include "GUIGunStoreModuleItem.h"

namespace BLOCKMAN
{
	GUIGunStoreModulePanel::GUIGunStoreModulePanel(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("GunStoreModulePanel.json"));
		renameComponents(m_window);
		AddChildWindow(m_window);
		initView();
	}

	void GUIGunStoreModulePanel::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIGunStoreModulePanel::initView()
	{
		GUIString ChildsName = StringUtil::Format("GunStoreModulePanelList").c_str();
		GUIGridView* pChildGV = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, ChildsName);
		pChildGV->SetArea(UDim(0, 4), UDim(0, 14), UDim(1.f, 0), UDim(1.f, 0));
		pChildGV->InitConfig(3.f, 4.f, 5);
		m_window->AddChildWindow(pChildGV);
		m_list = pChildGV;
		m_list->SetMoveAble(false);
		m_list->SetTouchable(false);

	}

	void GUIGunStoreModulePanel::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x }, { 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUIGunStoreModulePanel::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIGunStoreModulePanel::InitModules(list<StoreBlockModule*>::type modules)
	{
		if (modules.empty())
		{
			return;
		}
		
		int count = modules.size();
		int line = count / 5 + (count % 5 > 0 ? 1 : 0);

		auto height = m_list->GetPixelSize().y;
		auto width = m_list->GetPixelSize().x;
		float ChildWidth = (width - 2.0f - 3.5f * 4) / 5;

		static unsigned ModuleChildCount = 0;
		bool IsFirst = true;
		for (auto module : modules)
		{
			if (IsFirst)
			{
				module->IsSelected = true;
				GunStoreGunItemClickEvent::emit(module->Id);
			}
			ModuleChildCount++;
			GUIString ChildName = StringUtil::Format("GUIGunStorePanel-Module-Child-%d", ModuleChildCount).c_str();
			GUIGunStoreModuleItem* ModuleItem = (GUIGunStoreModuleItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GUN_STORE_MODULE_ITEM, ChildName);
			ModuleItem->SetWidth({ 0, ChildWidth - 1 });
			ModuleItem->SetHeight({ 0, ChildWidth * 0.8f});
			ModuleItem->InitModuleItem(module);
			ModuleItem->SetTouchable(false);
			m_list->AddItem(ModuleItem);
			IsFirst = false;
		}
	}
}
