#include "GUIWalkingDeadStoreItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "cWorld/Blockman.h"
#include "Util/UICommon.h"
#include "Network/ClientNetwork.h"


namespace BLOCKMAN
{
	GUIWalkingDeadStoreItem::GUIWalkingDeadStoreItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("WalkingDeadStoreItem.json"));
		renameComponents(m_window);
		AddChildWindow(m_window);
		SetTouchable(false);
		findViews();
	}

	void GUIWalkingDeadStoreItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIWalkingDeadStoreItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
		if (m_goods && m_goods->Change)
		{
			InitStoreItems(m_goods);
		}
	}

	void GUIWalkingDeadStoreItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIWalkingDeadStoreItem::findViews()
	{
		m_itemcontent = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(0));
		m_prop = dynamic_cast<GUILayout*>(m_itemcontent->GetChildByIndex(0));
		m_itemImagebg = dynamic_cast<GUIStaticImage*>(m_prop->GetChildByIndex(1));
		m_itemIcon = dynamic_cast<GUIStaticImage*>(m_itemImagebg->GetChildByIndex(0));
		m_itemNum = dynamic_cast<GUIStaticText*>(m_itemImagebg->GetChildByIndex(1));

		m_btnBuy = dynamic_cast<GUIButton*>(m_prop->GetChildByIndex(2));
		m_itemMoneyImage = dynamic_cast<GUIStaticImage*>(m_btnBuy->GetChildByIndex(0));
	

		m_Price_text = dynamic_cast<GUIStaticText*>(m_btnBuy->GetChildByIndex(1));
		m_itemName = dynamic_cast<GUIStaticText*>(m_prop->GetChildByIndex(0));

		m_btnBuy->subscribeEvent(EventButtonClick, std::bind(&GUIWalkingDeadStoreItem::onButtonClick, this, std::placeholders::_1));
		m_itemIcon->subscribeEvent(EventWindowTouchUp, std::bind(&GUIWalkingDeadStoreItem::onItemClick, this, std::placeholders::_1));
	}

	void GUIWalkingDeadStoreItem::InitStoreItems(WalkingDeadStoreGoods * goods)
	{
		if (goods == nullptr)
		{
			return;
		}
		m_goods_id = goods->GoodsId;
		m_goods = goods;
		m_itemIcon->SetImage(m_goods->Image.c_str());
		int price = m_goods->Price;
		int vip = WalkingDeadStoreSetting::getVipLevel();
		if (vip == 1)
		{
			price = m_goods->PriceA;
		}
		else if (vip == 2) 
		{
			price = m_goods->PriceB;
		}
		else if (vip == 3)
		{
			price = m_goods->PriceC;
		}
		m_Price_text->SetText(StringUtil::ToString(price).c_str());
		m_itemName->SetText(LanguageManager::Instance()->getString(m_goods->Name).c_str());
		
		auto m_cur_store_type = WalkingDeadStoreSetting::getWalkingDeadStoreType();

		if (m_cur_store_type == GeneralShop)
		{
			
			m_itemMoneyImage->SetImage("set:app_shop.json image:app_shop_diamonds_mods");
			m_itemNum->SetText(StringUtil::ToString(m_goods->Num).c_str());
		}
		else if (m_cur_store_type == MerchantShop)
		{
			m_itemMoneyImage->SetImage("set:prop.json image:02");
			m_itemNum->SetText(StringUtil::ToString(m_goods->Num).c_str());
		}
		else if (m_cur_store_type == TravellerShop)
		{
			m_itemMoneyImage->SetImage("set:prop.json image:03");
			m_itemNum->SetText(StringUtil::ToString(m_goods->Num).c_str());
		}
	}

	bool GUIWalkingDeadStoreItem::onItemClick(const EventArgs & events)
	{
		UpdateWalkingDeadStoreAppShopInfoEvent::emit(m_goods_id);
		return true;
	}

	
	bool GUIWalkingDeadStoreItem::onButtonClick(const EventArgs & events)
	{
		//Look in the configuration for this item
		auto goods = WalkingDeadStoreSetting::findGoodsById(m_goods_id);
		if (goods == NULL)
		{
			return false;
		}
		int price = m_goods->Price;
		int vip = WalkingDeadStoreSetting::getVipLevel();
		if (vip == 1)
		{
			price = m_goods->PriceA;
		}
		else if (vip == 2)
		{
			price = m_goods->PriceB;
		}
		else if (vip == 3)
		{
			price = m_goods->PriceC;
		}
		UICommon::checkMoney(price, goods->MoneyType, [=](bool isSuccess) {
			if (isSuccess)
			{
				ClientNetwork::Instance()->getCommonSender()->sendWalkingDeadBuyStoreGoods(m_goods_id);
			}
		});
		return true;
	}

}
