#pragma once
#ifndef __GUI_BIRD_TIP_HEADER__
#define __GUI_BIRD_TIP_HEADER__

#include "gui_layout.h"
#include "Util/TipType.h"
#include "Inventory/Wallet.h"

using namespace LORD;
namespace LORD {
	class GUIWindow;
	class GUILayout;
	class GUIStaticText;
	class GUIButton;
	class GUIStaticImage;
}
namespace BLOCKMAN
{
	class gui_birdTip : public  gui_layout
	{
	private:
		enum class ViewId {
			CLOSE,
			CANCEL,
			SURE,
			WATCH_AD,
		};
	public:
		gui_birdTip();
		virtual ~gui_birdTip();
		void onLoad() override;
		bool isNeedLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClick(const EventArgs & events, ViewId viewId);
		bool showCommonTip(const String & msg, BirdTipType tipType);
		bool showLockOfMoneyTip(i32 difference, i32 currencyType);
		bool showExpenditureTip(i32 price, i32 currencyType, BirdTipType tipType);
		bool showOpenChestTip(i32 entityId, i32 currencyType, i32 timeLeft, const String& awardId);
		bool showDeleteTip(i64 birdId, const String& birdName);
		void onGetCommonData(String key, String data) override;
		bool onVideoAdVisible(bool visible);
		bool setMoneyAdData(i32 operateType, i32 param1, i32 param2);

	private:
		void btnSure();
		bool showGotoShop();
		bool showGotoApp();
		void changeTitleAndMessage();
		bool disableRecharge();
		void showAd(int adType, String msg, String btnText);

		void onWatchAd();

	private:
		BirdTipType m_tipType = BirdTipType::COMMON;
		bool m_enableRecharge = true;

		GUIStaticText* m_tvTitle = nullptr;
		GUIStaticText* m_tvPrice = nullptr;
		GUIStaticText* m_tvPanelMsg = nullptr;
		GUIStaticText* m_tvMoneyTipValue = nullptr;

		GUIStaticText* m_tvTime = nullptr;
		GUIButton* m_btnTimePrice = nullptr;

		GUIButton* m_btnSure = nullptr;
		GUILayout* m_panelLayout = nullptr;
		GUILayout* m_moneyTipLayout = nullptr;
		GUILayout* m_timeTipLayout = nullptr;

		GUIStaticText* m_tvMoneyTipAd = nullptr;
		GUIStaticText* m_tvAdBtnText = nullptr;

		i64 m_birdId = 0;
		i32 m_price = 0;
		i32 m_leftTime = 0;
		i32 m_entityId = 0;
		CurrencyType m_currencyType = CurrencyType::PLATFORM_MONEY;

		i32 m_lackOfPrice = 0;
		int m_adType = 0;
		i32 m_adReplaceMoney = 0;
		i32 m_adReplaceEggTicket = 0;
		i32 m_moneyAdOperateType = 0;
		i32 m_moneyAdParam1 = 0;
		i32 m_moneyAdParam2 = 0;
		bool m_isAdReady = false;
		bool m_canShowMoneyAd = false;
		bool m_canShowEggTicketAd = false;
		bool m_canShowChestCDAd = false;
		bool m_canShowVipChestAd = false;
	};
}

#endif