#include "GUIWalkingDeadStoreGoodsPanel.h"
#include "GUI/GuiDef.h"
#include "GUIWalkingDeadStoreItem.h"
#include "Network/ClientNetwork.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{
	GUIWalkingDeadStoreGoodsPanel::GUIWalkingDeadStoreGoodsPanel() :
		gui_layout("WalkingDeadStoreGoodsPanel.json")
	{
	}

	GUIWalkingDeadStoreGoodsPanel::~GUIWalkingDeadStoreGoodsPanel()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GUIWalkingDeadStoreGoodsPanel::onLoad()
	{
		initGoodsList();
		initGoodsInfo();
	}

	void GUIWalkingDeadStoreGoodsPanel::initGoodsList()
	{
		auto GoodsList = getWindow("WalkingDeadStoreGoodsPanel-Goods-List");
		if (GoodsList == NULL)
		{
			return; 
		}
		GUIString ChildsName = StringUtil::Format("WalkingDeadStoreGoodsPanel-Goods-Container").c_str();
		m_goods_list = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, ChildsName);
		m_goods_list->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_goods_list->InitConfig(0.0f, 0.0f, 4);
		GoodsList->AddChildWindow(m_goods_list);
	}

	void GUIWalkingDeadStoreGoodsPanel::initGoodsInfo()
	{
		m_goodsInfoImage = getWindow<GUIStaticImage>("WalkingDeadStoreGoodsPanel-Goods-Info-Image");
	    m_goodsInfoName = getWindow<GUIStaticText>("WalkingDeadStoreGoodsPanel-Goods-Info-Name");
	    m_goodsInfoDesc = getWindow<GUIStaticText>("WalkingDeadStoreGoodsPanel-Goods-Info-Desc");
	    m_goodsBuy = getWindow<GUIButton>("WalkingDeadStoreGoodsPanel-Buy");
	    m_goodsPrice = getWindow<GUIStaticText>("WalkingDeadStoreGoodsPanel-Goods-Price-Text");
	    m_goodsPriceIcon = getWindow<GUIStaticImage>("WalkingDeadStoreGoodsPanel-Goods-Price-Icon");
		m_subscriptionGuard.add(UpdateWalkingDeadStoreAppShopInfoEvent::subscribe(std::bind(&GUIWalkingDeadStoreGoodsPanel::SyncGoodsInfo, this, std::placeholders::_1)));
		m_goodsBuy->subscribeEvent(EventButtonClick, std::bind(&GUIWalkingDeadStoreGoodsPanel::onButtonClick, this, std::placeholders::_1));

	}

	 bool GUIWalkingDeadStoreGoodsPanel::SyncGoodsInfo(int goodsId)
	{
		 if (m_goods_id== goodsId)
		 {
			 return false;
		 }
		m_goods_id = goodsId;
		auto goods = WalkingDeadStoreSetting::findGoodsById(goodsId);
		if (goods == NULL)
		{
			return false;
		}

		auto m_cur_store_type = WalkingDeadStoreSetting::getWalkingDeadStoreType();
		if (m_cur_store_type == TravellerShop)
		{
			m_goodsPriceIcon->SetImage("set:prop.json image:03");
		}
		else if (m_cur_store_type == MerchantShop)
		{
			m_goodsPriceIcon->SetImage("set:prop.json image:02");
		}
		else if (m_cur_store_type == GeneralShop)
		{
			m_goodsPriceIcon->SetImage("set:app_shop.json image:app_shop_diamonds_mods");
		}
		int price = goods->Price;
		int vip = WalkingDeadStoreSetting::getVipLevel();
		if (vip == 1)
		{
			price = goods->PriceA;
		}
		else if (vip == 2)
		{
			price = goods->PriceB;
		}
		else if (vip == 3)
		{
			price = goods->PriceC;
		}
		m_goodsInfoImage->SetImage(goods->Image.c_str());
		m_goodsInfoDesc->SetText(LanguageManager::Instance()->getString(goods->Desc).c_str());
		m_goodsInfoName->SetText(LanguageManager::Instance()->getString(goods->Name).c_str());
		m_goodsPrice->SetText(StringUtil::ToString(price).c_str());
		return true;
	}
	bool GUIWalkingDeadStoreGoodsPanel::onButtonClick(const EventArgs & events)
	{
		//buy goods       m_goods_id
		auto goods = WalkingDeadStoreSetting::findGoodsById(m_goods_id);
		if (goods == NULL)
		{
			return false;
		}
		int price = goods->Price;
		int vip = WalkingDeadStoreSetting::getVipLevel();
		if (vip == 1)
		{
			price = goods->PriceA;
		}
		else if (vip == 2)
		{
			price = goods->PriceB;
		}
		else if (vip == 3)
		{
			price = goods->PriceC;
		}
		UICommon::checkMoney(price, goods->MoneyType, [=](bool isSuccess) {
			if (isSuccess)
			{
				ClientNetwork::Instance()->getCommonSender()->sendWalkingDeadBuyStoreGoods(m_goods_id);
			}
		});
		return true;
	}



	void GUIWalkingDeadStoreGoodsPanel::onUpdate(ui32 nTimeElapse)
	{
		
	}

	void GUIWalkingDeadStoreGoodsPanel::updateGoodsByTabId(int tabId)
	{
		if (m_cur_tab_id == tabId)
		{
			return;
		}
		m_cur_tab_id = tabId;
		auto GoodsList = WalkingDeadStoreSetting::findGoodsListByTabId(m_cur_tab_id);
		m_goods_list->RemoveAllItems();
		m_goods_list->SetScrollOffset(0);
		float ContentSizeX = m_goods_list->GetPixelSize().x;
		float ContentSizeY = m_goods_list->GetPixelSize().y;
		static unsigned WalkingDeadChildCount = 0;
		float ChildWidth = ContentSizeX / 4;
		float ChildHeight = ContentSizeY / 2;
		for (auto goods : GoodsList)
		{
			WalkingDeadChildCount++;
			GUIString ChildName = StringUtil::Format("GUIWalkingDeadStoreGoodsPanel-Goods-%d", WalkingDeadChildCount).c_str();
			GUIWalkingDeadStoreItem*  Item = (GUIWalkingDeadStoreItem*)GUIWindowManager::Instance()->
				CreateGUIWindow(GWT_WALKING_DEAD_STORE_ITEM, ChildName);
			Item->SetWidth({ 0, ChildWidth });
			Item->SetHeight({ 0, ChildHeight });
			Item->InitStoreItems(goods);
			m_goods_list->AddItem(Item);
		}
	}

}
