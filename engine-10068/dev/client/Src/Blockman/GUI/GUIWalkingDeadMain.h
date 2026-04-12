#ifndef __GUI_WALKINGDEAD_MAIN_HEADER__
#define __GUI_WALKINGDEAD_MAIN_HEADER__

#include "gui_layout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIWalkingDeadMain : public gui_layout
	{
	public:
		GUIWalkingDeadMain();
		~GUIWalkingDeadMain();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

	private:
		void updateVipTime();
		void updateDailySupplyTime();
		void updateEnderBoxUI();
		bool onButtonClick(const EventArgs & events);

	private:
		GUIStaticImage* m_vip_time = nullptr;
		GUIStaticImage* m_red_point = nullptr;
		GUIButton* m_refresh_box_button = nullptr;
		GUIButton* m_end_box_button = nullptr;
		GUIStaticText* m_vip_time_text = nullptr;
		GUIStaticText* m_vip_time_text_bg = nullptr;
		GUIStaticText* m_daily_supply_text = nullptr;
		
	};
}

#endif