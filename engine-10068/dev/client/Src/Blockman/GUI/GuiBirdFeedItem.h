#pragma once
#ifndef __GUI_BIRD_FEED_ITEM_HEADER__
#define __GUI_BIRD_FEED_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "Util/SubscriptionGuard.h"
#include "Game/BirdSimulator.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBirdFeedItem : public CustomGuiWindow
	{

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		GUIStaticImage* m_image = nullptr;
		GUIStaticText* m_num = nullptr;
		GUIStaticText* m_des = nullptr;
		SubscriptionGuard m_subscriptionGuard;
		GUIStaticImage* m_guide = nullptr;
		GUIStaticImage* m_adImage = nullptr;

		BirdFood m_food;
		i32 m_selectFoodId = -1;
		bool m_isShowGuide = false;
		bool m_isFeedAd = false;

	private:
		void removeComponents();

	public:
		GuiBirdFeedItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBirdFeedItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onClick(const EventArgs& args);
		void setSelectFoodId(i32 foodId) { m_selectFoodId = foodId; }
		void setFood(BirdFood food);
		bool showFeedGuide();
		void setFoodAd(i32 expNum);
		bool onClickWatchAd(const EventArgs& args);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
