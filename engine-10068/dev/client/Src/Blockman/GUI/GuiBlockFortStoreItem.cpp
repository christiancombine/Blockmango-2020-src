#include "GuiBlockFortStoreItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Setting/BlockFortItemSetting.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"

namespace BLOCKMAN
{
	
	void GuiBlockFortStoreItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBlockFortStoreItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBlockFortStoreItem::GuiBlockFortStoreItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BlockFortStoreItem.json"));
		renameComponents(m_window);

		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_ivLimit = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(1));
		m_ivLock = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_ivLimit->SetText(getString("buy_limit"));
		AddChildWindow(m_window);
	}

	void GuiBlockFortStoreItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
		if (m_item.id > 0 && BlockFortItemSetting::Instance())
		{
			if (auto itemInfo = BlockFortItemSetting::Instance()->findItem(m_item.id))
			{
				i32 level = Blockman::Instance()->m_pPlayer->m_blockFort->getManorManager()->curManor.level;
				m_ivLock->SetVisible(itemInfo->level > level);
			}
		}
	}

	
	void GuiBlockFortStoreItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBlockFortStoreItem::setItem(const BlockFortStoreItem & item)
	{
		m_item = item;
		m_ivLimit->SetVisible(m_item.limit > 0);
		if (auto itemInfo = BlockFortItemSetting::Instance()->findItem(m_item.id))
		{
			i32 level = Blockman::Instance()->m_pPlayer->m_blockFort->getManorManager()->curManor.level;
			m_icon->SetImage(itemInfo->icon.c_str());
			m_ivLock->SetVisible(itemInfo->level > level);
		}
	}

	void GuiBlockFortStoreItem::setSelectId(i32 id)
	{
		m_selectId = id;
		m_window->SetBackImage(m_item.id == m_selectId ? "set:blockfort_store.json image:item_pre" : "set:blockfort_store.json image:item_nor");
	}
}