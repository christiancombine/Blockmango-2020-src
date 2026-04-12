#include "GuiSkyBlockPrivilegeRewardItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Audio/SoundSystem.h"
#include "Plugins/GameClientEvents.h"

using namespace LORD;
namespace BLOCKMAN
{
	void GuiSkyBlockPrivilegeRewardItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	GuiSkyBlockPrivilegeRewardItem::GuiSkyBlockPrivilegeRewardItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("SkyblockPrivilegeRewardItem.json"));
		renameComponents(m_window);

		m_item_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_select_img = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_have_img = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_have_img->SetVisible(false);
		m_select_img->SetVisible(false);
		m_item_icon->subscribeEvent(EventWindowTouchUp, std::bind(&GuiSkyBlockPrivilegeRewardItem::onButtonClick, this, std::placeholders::_1));
		AddChildWindow(m_window);
	}

	void GuiSkyBlockPrivilegeRewardItem::setGoods(SkyBlockPrivilegeReward goods, int index, int fromPanel)
	{
		m_goods = goods;
		m_index = index;
		m_fromPanel = fromPanel;
		m_item_icon->SetImage(m_goods.icon.c_str());
		m_have_img->SetVisible(false);
		if (m_fromPanel == SKYBLOCK_PRI_REWARD_SUPREME)
		{
			if (index == 0)
				m_select_img->SetVisible(true);

			if (SkyBlockPrivilegeShopSetting::Instance()->isHavePriImg(m_goods.id))
				m_have_img->SetVisible(true);
		}
	}

	bool GuiSkyBlockPrivilegeRewardItem::onButtonClick(const EventArgs & events)
	{
		if (m_fromPanel == SKYBLOCK_PRI_REWARD_NORNAL)
		{
			SkyBlockPrivilegeRewardClickItemEvent::emit(m_goods, m_index);
			return true;
		}

		if (m_fromPanel == SKYBLOCK_RECIVE_NEW_PRI)
		{
			SkyBlockReciveNewPriClickItemEvent::emit(m_goods, m_index);
			return true;
		}

		if (m_fromPanel == SKYBLOCK_PRI_REWARD_SUPREME)
		{

			SkyBlockSupremeClickItemEvent::emit(m_index);
			SCRIPT_EVENT::SkyBlockShowPriDetailEvent::invoke(m_goods.name, m_goods.detail, m_goods.icon);
		};

		return true;
	}

}