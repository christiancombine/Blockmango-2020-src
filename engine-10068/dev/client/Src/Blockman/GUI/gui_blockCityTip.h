#pragma once
#ifndef __GUI_BLOCKCITY_TIP_HEADER__
#define __GUI_BLOCKCITY_TIP_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIListHorizontalBox.h"
#include "UI/GUICheckBox.h"
#include "Util/TipType.h"
#include "Inventory/Wallet.h"
#include "Game/BlockCity.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_blockCityTip : public  gui_layout
	{
	private:
		enum class ViewId {
			CLOSE,
			LEFT,
			RIGHT,
			BUY_ONE,
			BUY_MANY,
			BUY_CANCEL,
			WATCH_AD,
		};
	public:
		gui_blockCityTip();
		virtual ~gui_blockCityTip();
		void onLoad() override;
		bool isNeedLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClick(const EventArgs & events, ViewId viewId);
		bool showTip(const String & msg);
		bool showCommonTip(const vector<BlockCityPayment>::type& pays, i32 tipType, const String & msg);
		bool showTigerLotteryTip(i32 entityId);
		bool showBlockCityLockItems();
		bool setDeleteFriendId(i64 userId);
		bool onVideoAdVisible(bool visible);
		void onGetCommonData(String key, String data) override;

	private:
		void btnSure();
		void onWatchAd();

	private:
		BlockCityTipType m_tipType = BlockCityTipType::COMMON;

		GUIStaticText* m_tvTitle = nullptr;
		GUIStaticText* m_tvPanelMsg = nullptr;
		GUIButton* m_btnLeft = nullptr;
		GUIButton* m_btnRight = nullptr;
		GUIStaticImage* m_leftIcon = nullptr;
		GUIStaticText* m_tvLeftValue = nullptr;

		GUILayout* m_buyLayout = nullptr;
		GUIStaticText* m_tvBuyOneName = nullptr;
		GUIStaticImage* m_buyOneIcon = nullptr;
		GUIStaticText* m_tvBuyOneValue = nullptr;
		GUIStaticText* m_tvBuyManyName = nullptr;
		GUIStaticImage* m_buyManyIcon = nullptr;
		GUIStaticText* m_tvBuyManyValue = nullptr;

		GUILayout* m_timeLayout = nullptr;
		GUIStaticText* m_tvTime = nullptr;
		GUIStaticImage* m_timeMoneyIcon = nullptr;
		GUIStaticText* m_tvTimeMoneyValue = nullptr;

		GUIListHorizontalBox* m_list = nullptr;

		GUIButton* m_watchAdBtn = nullptr;

		i32 m_price = 0;
		i32 m_leftTime = 0;
		i32 m_cubePrice = 0;
		i32 m_storeBuyType = 0;
		i32 m_storTabId = 0;
		i32 m_storItemId = 0;
		i32 m_index = -1;
		CurrencyType m_currencyType = CurrencyType::PLATFORM_MONEY;

		i32 m_tigerLotteryEntityId = 0;
		i64 m_deleteFriendId = 0;

		bool m_canUseGold = false;
		bool m_isAdReady = false;
		bool m_canShowTiger1Ad = false;
		bool m_canShowTiger2Ad = false;
	};
}

#endif