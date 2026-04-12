#ifndef __GUI_PIXEL_GUN_DAY_ACTIVE_HEADER__
#define __GUI_PIXEL_GUN_DAY_ACTIVE_HEADER__

#include "gui_layout.h"
#include "UI/GUIStaticText.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIPixelGunDayActive : public gui_layout
	{
	private:
		int m_last_time = 86400000;
		GUIStaticText* m_countdown = nullptr;

	public:
		GUIPixelGunDayActive();
		~GUIPixelGunDayActive();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isForceUpdate() { return true; }

	private:
		void initChests();
		bool setDayActiveValue(int value);
		bool setDayActiveTime(int seconds);
	};
}

#endif