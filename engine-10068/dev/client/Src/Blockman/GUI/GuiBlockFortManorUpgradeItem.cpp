#include "GuiBlockFortManorUpgradeItem.h"
#include "UI/GUIWindowManager.h"
#include "Setting/BlockFortItemSetting.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageManager.h"

namespace BLOCKMAN
{
	
	void GuiBlockFortManorUpgradeItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBlockFortManorUpgradeItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBlockFortManorUpgradeItem::GuiBlockFortManorUpgradeItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BlockFortUpgradeItem.json"));
		renameComponents(m_window);

		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0)->GetChildByIndex(0));
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));

		AddChildWindow(m_window);
	}

	void GuiBlockFortManorUpgradeItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}
	
	void GuiBlockFortManorUpgradeItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBlockFortManorUpgradeItem::setItem(i32 itemId)
	{
		auto item = BlockFortItemSetting::Instance()->findItem(itemId);
		if (item)
		{
			m_image->SetImage(item->icon.c_str());
			m_name->SetText(getString(item->name).c_str());
		}
	}
}