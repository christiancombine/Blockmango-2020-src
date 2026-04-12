#include "GuiBirdFeedItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientNetwork.h"
#include "Inventory/Wallet.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Util/RedDotManager.h"
#include "GUI/GuiBirdPackAndFuseItem.h"

namespace BLOCKMAN
{
	
	void GuiBirdFeedItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBirdFeedItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBirdFeedItem::GuiBirdFeedItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BirdFeedItem.json"));
		renameComponents(m_window);

		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0)->GetChildByIndex(0));
		m_num = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0)->GetChildByIndex(1));
		m_des = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_adImage = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0)->GetChildByIndex(2));

		m_image->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdFeedItem::onClick, this, std::placeholders::_1));
		m_window->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdFeedItem::onClickWatchAd, this, std::placeholders::_1));
		AddChildWindow(m_window);
	}

	void GuiBirdFeedItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
		if (m_isFeedAd)
		{
			dynamic_cast<GUILayout*>(m_window->GetChildByIndex(0))->SetBackImage("");
		}
		else
		{
			dynamic_cast<GUILayout*>(m_window->GetChildByIndex(0))->SetBackImage(m_food.id == m_selectFoodId ? "set:bird_pack.json image:item_pre" : "set:bird_pack.json image:item");
		}
		if (m_food.id == 1041001)
		{
			m_guide->SetVisible(m_isShowGuide);
		}
	}

	
	void GuiBirdFeedItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	bool GuiBirdFeedItem::onClick(const EventArgs & args)
	{
		if (m_isFeedAd)
		{
			return false;
		}
		BirdFoodItemClickEvent::emit(m_food.id, m_food.num);
		if (m_isShowGuide)
		{
			m_isShowGuide = false;
			ClickBirdFeedCookieGuideEvent::emit();
		}
		return true;
	}

	void GuiBirdFeedItem::setFood(BirdFood food)
	{
		m_food = food;
		m_isFeedAd = false;
		m_window->SetBackImage("");
		m_window->SetBackgroundColor(Color(0.839216f, 0.686275f, 0.501961f));
		m_image->SetImage(m_food.icon.c_str());
		m_adImage->SetVisible(false);
		m_num->SetText(StringUtil::Format("X%s", UICommon::numberToString(m_food.num).c_str()).c_str());
		m_des->SetText(LanguageManager::Instance()->getString(m_food.desc).c_str());

		if (m_food.id == 1041001)
		{
			m_guide = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, "BirdFeed-Guide");
			m_guide->SetArea(UDim(0.35f, 0), UDim(-0.12f, 0.0f), UDim(0.0f, 40.f), UDim(0.0f, 40.f));
			m_guide->SetEffectName("guide.effect");
			m_guide->SetImage("");
			m_image->AddChildWindow(m_guide);
		}
	}

 	bool GuiBirdFeedItem::showFeedGuide()
 	{
 		if (m_food.id == 1041001)
 		{
 			m_isShowGuide = true;
 		}
		return true;
 	}

	void GuiBirdFeedItem::setFoodAd(i32 expNum)
	{
		m_isFeedAd = true;
		m_window->SetBackImage("set:bird_ad.json image:feed_cd");
		m_window->SetBackgroundColor(Color());
		m_adImage->SetVisible(true);
		m_image->SetImage("");
		m_num->SetText("");
		m_des->SetText(StringUtil::Format(getString("gui_bird_ad_add_bird_exp").c_str(), expNum).c_str());
	}

	bool GuiBirdFeedItem::onClickWatchAd(const EventArgs& args)
	{
		if (m_isFeedAd)
		{
			ClickBirdFeedCdEvent::emit();
		}
		return true;
	}

}