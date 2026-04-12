#pragma once
#ifndef __GUI_BLOCKCITY_BUY_FLY_ITEM_HEADER__
#define __GUI_BLOCKCITY_BUY_FLY_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Game/BlockCity.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBlockCityBuyFlyItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		GUIStaticImage* m_frameUp = nullptr;
		GUIStaticImage* m_frameDown = nullptr;
		GUIStaticText* m_flyText = nullptr;
		GUIStaticImage* m_flyImage = nullptr;
		GUIButton* m_btn = nullptr;
		GUIButton* m_adBtn = nullptr;
		GUIStaticText* m_buyText = nullptr;
		GUIStaticImage* m_buyImage = nullptr;
		SubscriptionGuard m_subscriptionGuard;

	private:
		void removeComponents();

	public:
		GuiBlockCityBuyFlyItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBlockCityBuyFlyItem() { m_subscriptionGuard.unsubscribeAll(); }

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void setItem(BlockCityFlyingInfo item);
		bool onClick(const EventArgs& args);
		bool onWatchAd(const EventArgs& args);

	private:
		BlockCityFlyingInfo m_fly;

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
