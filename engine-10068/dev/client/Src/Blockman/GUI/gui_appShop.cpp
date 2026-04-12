#include "gui_appShop.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIGridView.h"
#include "World/World.h"
#include "cWorld/Blockman.h"
#include "Util/LanguageKey.h"
#include "GUI/GuiAppShopItem.h"
#include "GUI/GUIAppShopAdItem.h"
#include "GUI/GuiDef.h"
#include "Util/AboutShopping.h"
#include "game.h"
#include "Setting/GameTypeSetting.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "ShellInterface.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUIRadioButton.h"

using namespace LORD;
namespace BLOCKMAN
{
	
	gui_appShop::gui_appShop() :
		gui_layout("AppShop.json")
	{
	}

	gui_appShop::~gui_appShop()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	RootGuiLayout* gui_appShop::getParent()
	{
		return dynamic_cast<RootGuiLayout*>(m_parentLayout);
	}

	void gui_appShop::onLoad()
	{
		getWindow("AppShop-TitleName")->SetText(getString(LanguageKey::GUI_STR_APP_SHOP_SHOP));
		m_tabLayout = getWindow<GUILayout>("AppShop-Content-Tabs");
		m_textTip = getWindow<GUIStaticText>("AppShop-Content-Tip");
		m_itemListLayout = getWindow<GUILayout>("AppShop-Content-ItemList");
		getWindow<GUIButton>("AppShop-BtnClose")->subscribeEvent(EventButtonClick, std::bind(&gui_appShop::onButtonClick, this, std::placeholders::_1));
		m_itemGridView = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "AppShop-Content-ItemList-GridView");
		m_itemGridView->SetArea(UDim(0, 0), UDim(0,0), UDim(1.f, 0), UDim(1.f, 0));
		m_itemGridView->InitConfig(8.f, 8.f, 4);
		m_itemListLayout->AddChildWindow(m_itemGridView);
		m_textTip->SetText(""); 
		m_subscriptionGuard.add(AppShopUpdateEvent::subscribe(std::bind(&gui_appShop::onShopUpdate, this)));
		m_subscriptionGuard.add(BuyGoodsResultEvent::subscribe(std::bind(&gui_appShop::onBuyGoodsResult, this, std::placeholders::_1)));
		m_subscriptionGuard.add(VideoAdVisibleEvent::subscribe(std::bind(&gui_appShop::onVideoAdVisible, this, std::placeholders::_1)));
		
		onShopUpdate();
	}

	void gui_appShop::onUpdate(ui32 nTimeElapse)
	{
		if (m_shopAdTime > 0)
		{
			m_shopAdTime -= nTimeElapse;
			if (m_shopAdTime < 0)
			{
				m_shopAdTime = 0;
			}
		}
		if (!isShown())
			return;
		if (m_showTipTime > 0 && m_showTipTime < 3000)
		{
			m_showTipTime = nTimeElapse + m_showTipTime;
		}
		else
		{
			m_showTipTime = 0;
			m_tipMessage = "";
		}
		m_textTip->SetText(m_tipMessage.c_str());
	}

	void gui_appShop::addTabView(GoodsGroup group, int index)
	{
		static unsigned count = 0;
		count++;
		String strTabName = StringUtil::Format("AppShop-Content-Tabs-Item-%d", count).c_str();
		String iconName = StringUtil::Format("AppShop-Content-Tabs-Item-Icon-%d", count).c_str();
		if (GUIWindowManager::Instance()->GetGUIWindow(strTabName.c_str()))
		{
			return;
		}
		GUIRadioButton* radioItem = (GUIRadioButton*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RADIO, strTabName.c_str());
		radioItem->SetArea(UDim(0, -5.f), UDim(0, index * 80.f + 15.f), UDim(1.f, 10.f), UDim(0, 80.f));
		radioItem->SetNormalImage("set:app_shop.json image:app_shop_tab_nor");
		radioItem->SetPushedImage("set:app_shop.json image:app_shop_tab_pre");
		radioItem->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
		radioItem->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "15 15 15 15");

		GUIStaticImage* iconItem = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, iconName.c_str());
		iconItem->SetVerticalAlignment(VA_CENTRE);
		iconItem->SetHorizontalAlignment(HA_CENTRE);
		iconItem->SetArea(UDim(0, -55.0f), UDim(0, 0.0f), UDim(0, 30.f), UDim(0, 30.f));

		GUIString tabName = "";
		GUIString imageRes = "";

		radioItem->SetTextColor(Color(0.921569f, 0.807843f, 0.643137f));
		radioItem->SetProperty(GWP_TEXT_BORDER, "true");
		radioItem->SetProperty(GWP_BTN_TEXT_RENDER_OFFSET, "30 0");
		radioItem->SetText(getString(group.getName()));
		radioItem->subscribeEvent(EventRadioStateChanged, std::bind(&gui_appShop::onRadioChange, this, std::placeholders::_1, group, radioItem));
		iconItem->SetImage(group.getIcon().c_str());
		radioItem->AddChildWindow(iconItem);
		m_tabLayout->AddChildWindow(radioItem);
		if (index == 0)
		{
			radioItem->SetSelected(true);
		}
	}

	bool gui_appShop::onSyncShopAdInfo(int times, int seconds)
	{
		m_shopAdTimes = times;
		m_shopAdTime = seconds * 1000;
		GUIAppShopAdItem* item = dynamic_cast<GUIAppShopAdItem*> (m_itemGridView->GetItem(0));
		if (item)
		{
			item->setAdInfo(m_shopAdTimes, m_shopAdTime);
		}
		else
		{
			m_selectTab = nullptr;
			showPage(0);
		}
		return true;
	}

	bool gui_appShop::onRadioChange(const EventArgs & events, GoodsGroup group, GUIRadioButton * radio)
	{

		if (m_selectTab == radio)
		{
			return false;
		}
		
		m_itemGridView->RemoveAllItems();
		m_itemGridView->ResetPos();
		radio->SetTextColor(Color(1.0f,1.0f,1.0f));
		if (m_selectTab)
		{
			m_selectTab->SetTextColor(Color(0.921569f, 0.807843f, 0.643137f));
		}
		static unsigned goodsCount = 0;
		vector<Goods>::type items = group.getGoods();
		float maxWidth = (m_itemGridView->GetPixelSize().x - 8.0f * 3) / 4;
		int index = 0;
		for (Goods item : items)
		{
			if (m_shopAdTimes != -1)
			{
				index++;
				if (group.getCategory() == 1 && index == 1)
				{
					goodsCount++;
					GUIString AdName = StringUtil::Format("AppShop-Content-ItemList-GridView-Item-%d", goodsCount).c_str();
					GUIAppShopAdItem* AdItem = (GUIAppShopAdItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_APP_SHOP_AD_ITEM, AdName);
					AdItem->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, Math::Min(maxWidth, 190.0f)), UDim(0, 260.f));
					AdItem->setGoods(item);
					AdItem->setAdInfo(m_shopAdTimes, m_shopAdTime);
					m_itemGridView->AddItem(AdItem);
				}
			}
			goodsCount++;
			GUIString strTabName = StringUtil::Format("AppShop-Content-ItemList-GridView-Item-%d", goodsCount).c_str();
			GuiAppShopItem* goodsItem = (GuiAppShopItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_APP_SHOP_ITEM, strTabName);
			goodsItem->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, Math::Min(maxWidth, 190.0f)), UDim(0, 260.f));
			goodsItem->setGoods(item, group.getIndex());
			m_itemGridView->AddItem(goodsItem);
			goodsItem->subscribeEvent("LackOfMoney", std::bind(&gui_appShop::onLackOfMoney, this, std::placeholders::_1));
		}
		m_selectTab = radio;
		onVideoAdVisible(m_canShowAd);
		return true;
	}

	bool gui_appShop::onButtonClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		return true;
	}

	bool gui_appShop::onLackOfMoney(const EventArgs & events)
	{
		m_showTipTime = 1;
		switch (events.handled) {
		case (int)BuyResult::LackingGood:
			m_tipMessage = getString(LanguageKey::GUI_STR_APP_SHOP_INSUFFICIENT_INVENTORY).c_str();
			break;
		case (int)BuyResult::LackOfMoney:
			m_tipMessage = getString(LanguageKey::LACK_OF_MONEY).c_str();
			break;
		case (int)BuyResult::FullInventory:
			m_tipMessage = getString(LanguageKey::GUI_STR_APP_SHOP_INVENTORY_IS_FULL).c_str();
			break;
		case (int)BuyResult::HasNoGun:
			m_tipMessage = getString(LanguageKey::HAS_NO_GUN).c_str();
			break;
		case (int)BuyResult::LackOfItem:
			m_tipMessage = getString("gui_shop_lack_of_item").c_str();
			break;
		}
		m_textTip->SetText(m_tipMessage.c_str());
		return true;
	}

	bool gui_appShop::onShopUpdate()
	{
		Shop* shop = Blockman::Instance()->getWorld()->getShop();
		if (shop)
		{
			int mun = m_tabLayout->GetChildCount();
			for (int i = 0; i < mun; i++)
			{
				m_tabLayout->RemoveChildWindow(m_tabLayout->GetChildByIndex(0));
			}

			vector<GoodsGroup>::type groups = shop->getGoods();
			for (size_t i = 0; i < groups.size(); i++)
			{
				addTabView(groups[i], i);
			}
		}
		return true;
	}

	bool gui_appShop::onBuyGoodsResult(const String& msg)
	{
		m_showTipTime = 1;
		m_tipMessage = msg;
		m_textTip->SetText(msg.c_str());
		return true;
	}

	void gui_appShop::showPage(int page)
	{
		int count = m_tabLayout->GetChildCount();
		if (page > count)
			return;
		for (int i = 0; i < count; i++)
		{
			if (page != i)
			{
				continue;
			}
			auto child = dynamic_cast<GUIRadioButton*>(m_tabLayout->GetChildByIndex(i));
			if (child)
			{
				child->SetSelected(false);
				child->SetSelected(true);
			}
			break;
		}
	}

	void gui_appShop::onGetCommonData(String key, String data)
	{
		if (key == "VideoAd")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int itemId = builder.getIntParam("itemId");
			int adType = builder.getIntParam("adType");
			bool isShow = builder.getBoolParam("isShow");
			size_t count = m_itemGridView->GetItemCount();
			for (size_t i = 0; i < count; i++)
			{
				auto item = dynamic_cast<GuiAppShopItem*>(m_itemGridView->GetItem(i));
				if (item && item->getGoodsItemId() == itemId)
				{
					item->setVideoAdInfo(adType, isShow);
				}
			}
			return;
		}

		if (key == "PixelGunShopAd")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int times = builder.getIntParam("times");
			int seconds = builder.getIntParam("seconds");
			onSyncShopAdInfo(times, seconds);
			return;
		}
		if (key == "VideoAd_HASHall")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int itemId = builder.getIntParam("itemId");
			int adType = builder.getIntParam("adType");
			bool isShow = builder.getBoolParam("isShow");
			m_isVisibleAd = isShow;
			int postion = builder.getIntParam("position");
			size_t count = m_itemGridView->GetItemCount();
			if (0 <= postion && postion < count)
			{
				auto item = dynamic_cast<GuiAppShopItem*>(m_itemGridView->GetItem(postion));
				if (item && item->getGoodsItemId() == itemId)
				{
					item->setVideoAdInfo(adType, isShow);
				}
			}
		}
	}

	void gui_appShop::onShow()
	{
		GameClient::CGame::Instance()->getShellInterface()->
			dataReport("game_shop", GameClient::CGame::Instance()->getGameType());
		if (m_isVisibleAd&&m_canShowAd)
		{
			//ClientNetwork::Instance()->getLuaCommonSender()->sendLuaCommonData("AdDisplay", "");
			GameClient::CGame::Instance()->getShellInterface()->
				dataReport("game_shop_ad_view", GameClient::CGame::Instance()->getGameType());
		}

		//ClientNetwork::Instance()->getLuaCommonSender()->sendLuaCommonData("AdDisplay", "");
	}

	bool gui_appShop::onVideoAdVisible(bool visible)
	{
		m_canShowAd = visible;
		size_t count = m_itemGridView->GetItemCount();
		for (size_t i = 0; i < count; i++)
		{
			auto item = dynamic_cast<GuiAppShopItem*>(m_itemGridView->GetItem(i));
			if (item)
			{
				item->setCanShowAd(m_canShowAd);
			}
		}
		return true;
	}


}
