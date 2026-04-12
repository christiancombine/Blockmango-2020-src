#pragma once
#ifndef __GUI_BLOCK_CITY_RANK_FRIEND_HEADER__
#define __GUI_BLOCK_CITY_RANK_FRIEND_HEADER__

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
	class gui_blockCityRankFriend : public  gui_layout
	{
	private:
		enum class ViewId {
			CHARM,
			CONSTRUCTION,
			TOTAL,
			WEEK,
		};

	public:
		gui_blockCityRankFriend();
		virtual ~gui_blockCityRankFriend();
		void onLoad() override;
		void onShow() override;
		bool isNeedLoad() override;
	private:
		bool onUpdateBlockCityFriendRank(const vector<UserRank>::type& items, const String& type);
		bool userActorChange(ui64 userId);
		void changeActor(ui64 userId, GUILayout *view);

		bool onRadioChange(const EventArgs & events, ViewId viewId);
		bool onCategoryRadioChange(const EventArgs & events, ViewId viewId);
		void onsetRank(const vector<UserRank>::type& items, GUIListBox* view);
		void onShowListView(ViewId viewId);
		void onSetPodiumInfo(const vector<UserRank>::type& items);

	private:
		GUILayout * m_first = nullptr;
		GUILayout* m_own = nullptr;
		GUIListBox* m_constructionRanks = nullptr;
		GUIListBox* m_charmTotalRanks = nullptr;
		GUIListBox* m_charmWeekRanks = nullptr;
		GUILayout* m_charmCategory = nullptr;

		map<String, GUILayout*>::type m_rankLayoutMap;

		map<ViewId, GUIRadioButton*>::type m_radioTabMap;
		map<ViewId, GUIRadioButton*>::type m_radioCategoryMap;
		map<ViewId, GUIListBox*>::type m_listMap;
		vector<UserRank>::type m_charmTotal;
		vector<UserRank>::type m_charmWeek;
		vector<UserRank>::type m_construction;

	};
}

#endif