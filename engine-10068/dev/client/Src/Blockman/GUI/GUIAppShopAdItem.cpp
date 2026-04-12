#include "GUIAppShopAdItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Network/ClientNetwork.h"
#include "GUI/GuiItemStack.h"
#include "game.h"
#include "ShellInterface.h"
#include "Enums/Enums.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"

using namespace LORD;
namespace BLOCKMAN
{
	void GUIAppShopAdItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	GUIAppShopAdItem::GUIAppShopAdItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("AppShopAdItem.json"));
		renameComponents(m_window);
		m_itemName = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_itemIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_showAd = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		m_adIcon = dynamic_cast<GUIStaticImage*>(m_showAd->GetChildByIndex(0));
		m_itemNum = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(4));
		m_showAd->subscribeEvent(EventButtonClick, std::bind(&GUIAppShopAdItem::onButtonClick, this, std::placeholders::_1));
		AddChildWindow(m_window);
	}

	void GUIAppShopAdItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
		if (m_vedioAdTime > 0)
		{
			m_vedioAdTime -= nTimeElapse;
			if (m_vedioAdTime < 0)
			{
				m_vedioAdTime = 0;
				m_showAd->SetText("");
				m_adIcon->SetVisible(true);
			}
			else
			{
				int seconds = m_vedioAdTime / 1000;
				int second = seconds % 60;
				int minute = (seconds % 3600) / 60;
				int hour = seconds / 3600;
				m_showAd->SetText(StringUtil::Format("%s:%s:%s",
					hour < 10 ? StringUtil::Format("0%d", hour).c_str() : StringUtil::ToString(hour).c_str(),
					minute < 10 ? StringUtil::Format("0%d", minute).c_str() : StringUtil::ToString(minute).c_str(),
					second < 10 ? StringUtil::Format("0%d", second).c_str() : StringUtil::ToString(second).c_str()).c_str());
			}
		}
	}

	void GUIAppShopAdItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	bool GUIAppShopAdItem::onButtonClick(const EventArgs & events)
	{
		if (m_vedioAdTimes > 0 && m_vedioAdTime <= 0)
		{
			GameClient::CGame::Instance()->getShellInterface()->onWatchAd((int)AdType::SHOP);
		}
		return true;
	}

	void GUIAppShopAdItem::setGoods(Goods goods)
	{
		String name = LanguageManager::Instance()->getString(goods.desc);
		m_itemName->SetText(name.c_str());
		m_itemNum->SetText(StringUtil::ToString(goods.itemNum).c_str());
		m_showAd->SetText("");
		m_adIcon->SetVisible(true);
		m_window->GetChildByIndex(1)->GetChildByIndex(0)->SetText(LanguageManager::Instance()->getString("gui_ranch_build_price_free").c_str());
		if (m_itemIcon)
			GuiItemStack::showItemIcon(m_itemIcon, goods.itemId, goods.itemMeta, goods.icon);
	}

	void GUIAppShopAdItem::setAdInfo(int times, int time)
	{
		m_vedioAdTimes = times;
		m_vedioAdTime = time;
		if (times <= 0)
		{
			m_adIcon->SetVisible(false);
			m_showAd->SetEnabled(false);
			m_showAd->SetText(LanguageManager::Instance()->getString("gui.ad.used.up").c_str());
		}
		else
		{
			m_showAd->SetEnabled(true);
			if (time > 0)
			{
				m_adIcon->SetVisible(false);
			}
			else
			{
				m_adIcon->SetVisible(true);
				m_showAd->SetText("");
			}
		}
	}
}