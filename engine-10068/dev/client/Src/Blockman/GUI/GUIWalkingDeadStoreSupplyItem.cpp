#include "GUIWalkingDeadStoreSupplyItem.h"
#include "UI/GUIWindowManager.h"
#include "Network/ClientNetwork.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{
	GUIWalkingDeadStoreSupplyItem::GUIWalkingDeadStoreSupplyItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("WalkingDeadSupplyItem.json"));
		renameComponents(m_window);
		initView();
		AddChildWindow(m_window);
		SetTouchable(false);
	}


	void GUIWalkingDeadStoreSupplyItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIWalkingDeadStoreSupplyItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
		if (m_data && m_data->Change)
		{
			InitSupplyItems(m_data);
		}
	}

	void GUIWalkingDeadStoreSupplyItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIWalkingDeadStoreSupplyItem::initView()
	{
		m_itemContent = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(0));
		m_itemName = dynamic_cast<GUIStaticText*>(m_itemContent->GetChildByIndex(1));
		m_itemImage = dynamic_cast<GUIStaticImage*>(m_itemContent->GetChildByIndex(3));
		m_itemQuestion = dynamic_cast<GUIButton*>(m_itemContent->GetChildByIndex(0));
		m_itemDesc = dynamic_cast<GUIStaticText*>(m_itemContent->GetChildByIndex(2));

		m_itemBuy = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(2));
		m_itemBuy_text = dynamic_cast<GUIStaticText*>(m_itemBuy->GetChildByIndex(0));
		m_itemBuy->SetVisible(false);

		m_itemReceive = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(1));
		m_itemReceive_text = dynamic_cast<GUIStaticText*>(m_itemReceive->GetChildByIndex(0));
		m_itemReceive->SetVisible(false);

		//m_itemQuestion->SetVisible(true);
		m_itemBuy_text->SetText(getString("walking.dead.supply.buy"));
		m_itemReceive_text->SetText(getString("walking.dead.supply.receive"));

		m_itemPrice = dynamic_cast<GUIStaticText*>(m_itemImage->GetChildByIndex(0)->GetChildByIndex(1));
		auto m_itemO_Price = m_itemImage->GetChildByIndex(0)->GetChildByIndex(2);
		m_itemO_Price->SetVisible(false);

		m_itemQuestion->subscribeEvent(EventButtonClick, std::bind(&GUIWalkingDeadStoreSupplyItem::OnQuestionButtonClick, this, std::placeholders::_1));
		m_itemBuy->subscribeEvent(EventButtonClick, std::bind(&GUIWalkingDeadStoreSupplyItem::OnBuyButtonClick, this, std::placeholders::_1));
		m_itemReceive->subscribeEvent(EventButtonClick, std::bind(&GUIWalkingDeadStoreSupplyItem::OnReceiveButtonClick, this, std::placeholders::_1));
	}

	void GUIWalkingDeadStoreSupplyItem::InitSupplyItems(WalkingDeadSupply * chest)
	{
		if (!chest)
			return;
		m_data = chest;
		//m_itemQuestion->SetVisible(true);
		if (m_data->Status == CanBuy)
		{
			m_itemReceive->SetVisible(false);
			m_itemBuy->SetVisible(true);
			m_itemBuy->SetEnabled(true);
			m_itemBuy_text->SetText(getString("walking.dead.supply.buy"));
		}
		else if (m_data->Status == CanReceive )
		{
			m_itemReceive->SetVisible(true);
			m_itemReceive->SetEnabled(true);
			m_itemReceive_text->SetText(getString("walking.dead.supply.receive"));
			m_itemBuy->SetVisible(false);
		}
		else if (m_data->Status == UnReceive)
		{
			m_itemReceive->SetVisible(false);
			m_itemBuy->SetVisible(true);
			m_itemBuy->SetEnabled(false);
			m_itemBuy_text->SetText(getString("walking.dead.supply.received"));
			m_itemBuy_text->SetTextColor((Color(176.f / 255.f, 176.f / 255.f, 176.f / 255.f)));
			
		}
		else
		{
			m_itemReceive->SetVisible(false);
			m_itemBuy->SetVisible(true);
			m_itemBuy->SetEnabled(false);
			m_itemBuy_text->SetText(getString("walking.dead.supply.no.buy"));
			m_itemBuy_text->SetTextColor((Color(176.f / 255.f, 176.f / 255.f, 176.f / 255.f)));
		}
		m_itemImage->SetImage(m_data->Image.c_str());
		m_itemName->SetText(getString(m_data->Name));
		m_itemDesc->SetText(getString(m_data->Describe));
		int price = m_data->Price;
		int vip = WalkingDeadStoreSetting::getVipLevel();
		if (vip == 1)
		{
			price = m_data->PriceA;
		}
		else if (vip == 2)
		{
			price = m_data->PriceB;
		}
		else if (vip == 3)
		{
			price = m_data->PriceC;
		}
		m_itemPrice->SetText(StringUtil::ToString(price).c_str());

	}

	bool GUIWalkingDeadStoreSupplyItem::OnBuyButtonClick(const EventArgs & events)
	{
		if (m_data == nullptr)
		{
			return false;
		}
		int price = m_data->Price;
		int vip = WalkingDeadStoreSetting::getVipLevel();
		if (vip == 1)
		{
			price = m_data->PriceA;
		}
		else if (vip == 2)
		{
			price = m_data->PriceB;
		}
		else if (vip == 3)
		{
			price = m_data->PriceC;
		}
		UICommon::checkMoney(price, m_data->MoneyType, [=](bool isSuccess) {
			if (isSuccess)
			{
				ClientNetwork::Instance()->getCommonSender()->sendWalkingDeadBuyStoreSupply(m_data->Id);
			}
		});
		return true;
	}

	bool GUIWalkingDeadStoreSupplyItem::OnQuestionButtonClick(const EventArgs & events)
	{
		WalkingDeadshowSupplyPopupEvent::emit(m_data->Id);
		return true;
	}

	bool GUIWalkingDeadStoreSupplyItem::OnReceiveButtonClick(const EventArgs & events)
	{
		if (m_data == nullptr)
		{
			return false;
		}
		ClientNetwork::Instance()->getCommonSender()->sendWalkingDeadBuyStoreSupply(m_data->Id);
		return true;
	}

}



