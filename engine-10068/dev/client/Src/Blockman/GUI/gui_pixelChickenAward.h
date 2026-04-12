#pragma once
#ifndef __GUI_PIXEL_CHICKEN_AWARD_HEADER__
#define __GUI_PIXEL_CHICKEN_AWARD_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_pixelChickenAward : public gui_layout
	{
		enum ButtonAction
		{
			EXIT = 1,
			WATCH = 2,
			NEXT = 3,
			AD = 4
		};

	private:
		GUIStaticImage* m_honor = nullptr;
		GUIStaticText* m_rank = nullptr;
		GUIStaticText* m_name = nullptr;
		GUIStaticText* m_rank_total = nullptr;
		GUIStaticText* m_kill = nullptr;
		GUIStaticText* m_player_title = nullptr;
		GUIButton* m_exit = nullptr;
		GUIButton* m_watch = nullptr;
		GUIButton* m_next = nullptr;
		GUIButton* m_ad = nullptr;

		GUIStaticText* m_gold_num = nullptr;
		GUIStaticText* m_key_num = nullptr;
		GUIStaticText* m_honor_num = nullptr;
		GUIStaticText* m_exp_num = nullptr;
		GUIStaticText* m_login_num = nullptr;

		GUIStaticImage* m_effect = nullptr;

		bool m_have_ad = false;
		int m_show_effect_time = 0;
		bool m_has_show = false;

		SubscriptionGuard m_subscriptionGuard;
	public:
		gui_pixelChickenAward();
		virtual ~gui_pixelChickenAward();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isNeedLoad() override;
		bool showChickenAward(const String& data);
		bool onButtonClick(const EventArgs & events, ButtonAction action);
		void onGetCommonData(String key, String data);

		bool onVideoAdVisible(bool visible);
	private:
		void showResultCup(bool isWin);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}
#endif 
