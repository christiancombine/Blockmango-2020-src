#pragma once
#ifndef __GUI_BLOCKFORT_TIP_HEADER__
#define __GUI_BLOCKFORT_TIP_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "Util/TipType.h"
#include "Inventory/Wallet.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_blockFortTip : public  gui_layout
	{
	private:
		enum class ViewId {
			CLOSE,
			SURE,
		};
	public:
		gui_blockFortTip();
		virtual ~gui_blockFortTip();
		void onLoad() override;
		bool isNeedLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClick(const EventArgs & events, ViewId viewId);
		bool showCommonTip(const String & msg);
		bool showFunctionTip(const String & msg, i32 tipType);
		bool showLockOfMoneyTip(i32 difference, i32 currencyType);
		bool setDeleteFriendId(i64 userId);

	private:
		void btnSure();

	private:
		GUIStaticText* m_tvTitle = nullptr;
		GUIStaticText* m_tvMsg = nullptr;
		GUILayout* m_commonButtonsLayout = nullptr;
		GUIButton* m_leftBtn = nullptr;
		GUIButton* m_rightBtn = nullptr;

		BlockFortTipType m_tipType = BlockFortTipType::COMMON;
		CurrencyType m_currencyType = CurrencyType::PLATFORM_MONEY;
		i32 m_price = 0;
		i64 m_deleteFriendId = 0;
	};
}

#endif