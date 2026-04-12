#ifndef __GUI_PIXEL_GUN_WEEK_ACTIVE_HEADER__
#define __GUI_PIXEL_GUN_WEEK_ACTIVE_HEADER__

#include "gui_layout.h"
#include "UI/GUIStaticText.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIPixelGunWeekActive : public gui_layout
	{
	private:
		int m_last_time = 604800000;
		GUIStaticText* m_countdown = nullptr;

	public:
		GUIPixelGunWeekActive();
		~GUIPixelGunWeekActive();

		bool isForceUpdate() { return true; }
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

	private:
		void initChests();
		bool setWeekActiveValue(int value);
		bool setWeekActiveTime(int seconds);
	};
}

#endif