#include "GuiSkyBlockPrivilegeShopItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GameAnalytics/GameAnalyticsSetting.h"
#include "game.h"

using namespace LORD;
namespace BLOCKMAN
{
	void GuiSkyBlockPrivilegeShopItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	GuiSkyBlockPrivilegeShopItem::GuiSkyBlockPrivilegeShopItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("SkyBlockPrivilegeShopItem.json"));
		renameComponents(m_window);
		m_select_img = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_item_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2)->GetChildByIndex(0));
		m_pri_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(3));
		m_money_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(5));
		m_money_num = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(4)->GetChildByIndex(0));
		m_discount_panel = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(6));
		m_discount_panel->SetVisible(false);
		m_discount_text = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(6)->GetChildByIndex(1));
		m_select_img->SetVisible(false);
		m_window->subscribeEvent(EventWindowTouchUp, std::bind(&GuiSkyBlockPrivilegeShopItem::onButtonClick, this, std::placeholders::_1));
		AddChildWindow(m_window);
	}

	void GuiSkyBlockPrivilegeShopItem::Destroy()
	{
		m_subscriptionGuard.unsubscribeAll();
		removeComponents();
		BaseClass::Destroy();
	}

	vector<GUIWindow*>::type GuiSkyBlockPrivilegeShopItem::getPrivateChildren()
	{
		return { m_window };
	}

	bool GuiSkyBlockPrivilegeShopItem::onButtonClick(const EventArgs & events)
	{
		SkyBlockPrivilegeShopClickItemEvent::emit(m_goods, m_index);
		auto wallet = Blockman::Instance()->m_pPlayer->getWallet();
		if (GameClient::CGame::Instance()->isEnableIndie())
			GameAnalyticsSetting::Instance()->design(0, { "g1048click_privilege_indie",
				StringUtil::ToString(m_goods.id), StringUtil::ToString(wallet->getDiamondsGolds()), "1" });
		else
			GameAnalyticsSetting::Instance()->design(0, { "g1048click_privilege",
				StringUtil::ToString(m_goods.id), StringUtil::ToString(wallet->getDiamondsGolds()), "1" });
		return true;
	}

	void GuiSkyBlockPrivilegeShopItem::setGoods(SkyBlockPrivilegeGoods goods, int index)
	{
		m_goods = goods;
		m_index = index;
		m_pri_name->SetText(getString(m_goods.name).c_str());
		m_money_num->SetText(StringUtil::ToString(m_goods.coinNum).c_str());
		m_money_icon->SetImage(UICommon::getCurrencyIconByType(m_goods.coinId).c_str());
		m_item_icon->SetImage(m_goods.icon.c_str());

		SkyBlockGoods appGoods;
		if (SkyBlockAppShopSetting::Instance()->getItemById(appGoods, m_goods.id))
		{
			if (appGoods.disCountTips.length() > 2)
			{
				m_discount_panel->SetVisible(true);
				m_discount_text->SetText(getString(appGoods.disCountTips).c_str());
			}
			else
			{
				m_discount_panel->SetVisible(false);
			}
		}
	}
}