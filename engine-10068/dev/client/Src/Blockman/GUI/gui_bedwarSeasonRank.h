#ifndef __GUI_BED_WAR_SEASON_RANK_HEADER__
#define __GUI_BED_WAR_SEASON_RANK_HEADER__

#include "gui_layout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIBedWarSeasonRule;
	class RootGuiLayout;
	class gui_bedwarSeasonRank : public gui_layout
	{
	private:
		GUIBedWarSeasonRule* m_season_rule = nullptr;
	public:
		gui_bedwarSeasonRank();
		~gui_bedwarSeasonRank();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;
		bool isNeedLoad() override;
		void onGetCommonData(String key, String data);

	private:
		bool onCloseClick(const EventArgs & events);
		bool onQuestionClick(const EventArgs & events);
		bool onArmoryClick(const EventArgs & events);
		bool onBattleClick(const EventArgs & events);
		bool onSeasonInit(int honorId, int rank, int honor, int endTime);
		bool onUpdateSeasonRankData(int type, const String & data);

	private:
		RootGuiLayout * getParent();
	};
}

#endif
