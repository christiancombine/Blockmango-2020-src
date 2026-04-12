#pragma once
#ifndef __GUI_BLOCK_CITY_RANK_GLOBAL_HEADER__
#define __GUI_BLOCK_CITY_RANK_GLOBAL_HEADER__

#include "gui_layout.h"
#include "Util/RankManager.h"

using namespace LORD;
namespace LORD {
	class GUILayout;
	class GUIListBox;
	class GUIRadioButton;
}
namespace BLOCKMAN
{
	class UserRank;
	class GuiBlockCityRankItem;
	class gui_blockCityRankGlobal : public  gui_layout
	{
	private:
		enum class ViewId {
			CHARM,
			CONSTRUCTION,
			TOTAL,
			WEEK,
		};

	public:
		gui_blockCityRankGlobal();
		virtual ~gui_blockCityRankGlobal();
		void onLoad() override;
		void onShow() override;
		bool isNeedLoad() override;
	private:
		bool onUpdateBlockCityGlobalRank(const vector<UserRank>::type& items, const String& type);
		bool onUpdateMyRank(UserRank &myRank, const String& type);
		bool userActorChange(ui64 userId);

		bool onRadioChange(const EventArgs & events, ViewId viewId);
		bool onCategoryRadioChange(const EventArgs & events, ViewId viewId);
		void onsetRank(const vector<UserRank>::type& items, GUIListBox* view);
		void onShowListView(ViewId viewId);
		void onSetPodiumInfo(const vector<UserRank>::type& items);

	private:
		GUILayout* m_first = nullptr;
		GUILayout* m_second = nullptr;
		GUILayout* m_third = nullptr;
		GUIListBox* m_constructionRanks = nullptr;
		GUIListBox* m_charmTotalRanks = nullptr;
		GUIListBox* m_charmWeekRanks = nullptr;
		GUILayout* m_charmCategory = nullptr;
		GuiBlockCityRankItem* m_myRank = nullptr;

		map<i64, GUILayout*>::type m_rankLayoutMap;

		map<ViewId, GUIRadioButton*>::type m_radioTabMap;
		map<ViewId, GUIRadioButton*>::type m_radioCategoryMap;
		map<ViewId, GUIListBox*>::type m_listMap;
		vector<UserRank>::type m_charmTotal;
		vector<UserRank>::type m_charmWeek;
		vector<UserRank>::type m_construction;
	};
}

#endif