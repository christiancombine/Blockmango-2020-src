#pragma once
#ifndef __GUI_CHEST_LOTTERY_HEADER__
#define __GUI_CHEST_LOTTERY_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "Util/SubscriptionGuard.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUILayout.h"
#include "GUI/GUIChestLotteryItem.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_chestLottery : public gui_layout
	{
	private:
		SubscriptionGuard m_subscriptionGuard;

		GUILayout* m_integral = nullptr;
		GUIProgressBar* m_integral_progress = nullptr;
		GUIStaticText* m_integral_progress_text = nullptr;

		GUIChestLotteryItem* m_chest_lv1 = nullptr;
		GUIChestLotteryItem* m_chest_lv2 = nullptr;
		GUIChestLotteryItem* m_chest_lv3 = nullptr;

	public:
		gui_chestLottery();
		~gui_chestLottery();

		void onLoad() override;
		bool isNeedLoad() override;
		void onGetCommonData(String key, String data);

	private:	
		bool setIntegralProgress(int curIntegral, int needIntegral);
		void onGetShowChestLotteryData(String data);
		bool onCloseClick(const EventArgs & events);
		bool onUltimateChestClick(const EventArgs & events);
		bool onUltimateChestQuestionClick(const EventArgs & events);
		bool onProbabilityClick(const EventArgs & events);
		bool onUpdateLotteryChestData(const String& data);
		bool onUpdateLotteryChestKey(int key);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};

}
#endif