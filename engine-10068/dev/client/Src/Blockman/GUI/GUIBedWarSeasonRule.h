#ifndef __GUI_BED_WAR_SEASON_RULE_HEADER__
#define __GUI_BED_WAR_SEASON_RULE_HEADER__

#include "gui_layout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIBedWarSeasonRule : public gui_layout
	{
	private:

	public:
		GUIBedWarSeasonRule();
		~GUIBedWarSeasonRule();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

	private:
		bool onContentClick(const EventArgs & events);
		bool onUpdateSeasonRule(const String & data);
	};
}

#endif