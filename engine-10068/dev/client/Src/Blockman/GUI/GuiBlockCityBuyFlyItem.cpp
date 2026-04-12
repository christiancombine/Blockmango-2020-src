#include "GuiBlockCityBuyFlyItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Network/ClientNetwork.h"
#include "GUI/RootGuiLayout.h"
#include "game.h"
#include "ShellInterface.h"

namespace BLOCKMAN
{
	
	void GuiBlockCityBuyFlyItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBlockCityBuyFlyItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBlockCityBuyFlyItem::GuiBlockCityBuyFlyItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BlockCityBuyFlyItem.json"));
		renameComponents(m_window);

		m_frameUp = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_frameDown = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_flyText = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(5)->GetChildByIndex(1));
		m_flyImage = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(4)->GetChildByIndex(0));
		m_adBtn = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		m_btn = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(2));
		m_buyText = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2)->GetChildByIndex(1));
		m_buyImage = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2)->GetChildByIndex(0));

		m_btn->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCityBuyFlyItem::onClick, this, std::placeholders::_1));
		m_adBtn->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCityBuyFlyItem::onWatchAd, this, std::placeholders::_1));
		AddChildWindow(m_window);
	}

	void GuiBlockCityBuyFlyItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	
	void GuiBlockCityBuyFlyItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBlockCityBuyFlyItem::setItem(BlockCityFlyingInfo item)
	{
		m_fly = item;
		m_flyText->SetText(getString(item.flyMsg).c_str());
		m_buyImage->SetImage(UICommon::getCurrencyIconByType(item.moneyType).c_str());
		m_buyText->SetText(StringUtil::ToString(item.price).c_str());
		m_window->SetBackImage(item.day == 0 ? "set:blockcity_ad.json image:buy_fly_back" : "set:blockcity_buy_fly.json image:item_frame");
		m_frameUp->SetImage(item.day == 0 ? "set:blockcity_ad.json image:buy_fly_back_up" : "set:blockcity_buy_fly.json image:item_frame_up");
		m_frameDown->SetImage(item.day == 0 ? "set:blockcity_ad.json image:buy_fly_back_down" : "set:blockcity_buy_fly.json image:item_frame_down");
		switch (item.day)
		{
		case 0:
			m_flyImage->SetImage("set:blockcity_buy_fly.json image:one");
			m_flyText->SetText(getString("fly_msg_ad").c_str());
			m_adBtn->SetVisible(true);
			m_btn->SetVisible(false);
			break;
		case 1:
			m_flyImage->SetImage("set:blockcity_buy_fly.json image:one");
			m_adBtn->SetVisible(false);
			m_btn->SetVisible(true);
			break;
		case 3:
			m_flyImage->SetImage("set:blockcity_buy_fly.json image:three");
			m_adBtn->SetVisible(false);
			m_btn->SetVisible(true);
			break;
		case 7:
			m_flyImage->SetImage("set:blockcity_buy_fly.json image:seven");
			m_adBtn->SetVisible(false);
			m_btn->SetVisible(true);
			break;
		}
	}

	bool GuiBlockCityBuyFlyItem::onClick(const EventArgs& args)
	{
		UICommon::checkMoney(m_fly.price, m_fly.moneyType, [=](bool isSuccess) {
			if (isSuccess)
			{
				ClientNetwork::Instance()->getCommonSender()->sendBlockCityOperation(3, m_fly.id);
			}
		});
		RootGuiLayout::Instance()->hideWindowByLayoutId(RootGuiLayout::LayoutId::BLOCK_CITY_BUY_FLY);
		return true;
	}

	bool GuiBlockCityBuyFlyItem::onWatchAd(const EventArgs& args)
	{
		GameClient::CGame::Instance()->getShellInterface()->onWatchAd((int)AdType::BLOCK_CITY_FLYING);
		GameClient::CGame::Instance()->getShellInterface()->
			dataReport("realm_fly_ad_click", GameClient::CGame::Instance()->getGameType());
		GameClient::CGame::Instance()->getShellInterface()->
			dataReport("click_reward_ad", GameClient::CGame::Instance()->getGameType());
		RootGuiLayout::Instance()->hideWindowByLayoutId(RootGuiLayout::LayoutId::BLOCK_CITY_BUY_FLY);
		return true;
	}

}