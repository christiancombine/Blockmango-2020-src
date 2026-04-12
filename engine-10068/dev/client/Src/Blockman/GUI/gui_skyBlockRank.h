#ifndef __GUI_SKYBLOCK_RASNK_HEADER__
#define __GUI_SKYBLOCK_RASNK_HEADER__
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIListBox.h"
#include "GUI/RootGuiLayout.h"
#include "rapidjson/document.h"

using namespace LORD;

namespace BLOCKMAN
{
	struct SKyBlockRankData
	{
		String name;
		ui64 userId;
		int score;
		int rank;

		SKyBlockRankData(String n, ui64 uId, int s, int r)
			: name(n)
			, userId(uId)
			, score(s)
			, rank(r)
		{
		}
	};

	class gui_skyBlockRank : public gui_layout
	{
	public:
		gui_skyBlockRank();
		virtual ~gui_skyBlockRank();
		void onLoad() override;
		bool onExitClick(const EventArgs & events);
		void onShow() override;
		void RefreshUI();
		void refreshSelfRank();
		void refreshContent(GUIListBox* list, std::list<SKyBlockRankData*>& data);

	private:
		GUIListBox*									m_rankListBox = nullptr;
 		GUIStaticText*								m_selfRankText = nullptr;
 		GUIStaticText*								m_selfNameText = nullptr;
 		GUIStaticText*								m_selfScoreText = nullptr;
		SKyBlockRankData*							m_selfRank = nullptr;
		std::list<SKyBlockRankData*>				m_rankList;
		SubscriptionGuard							m_subscriptionGuard;

		bool parseRankResult(const String& rankResult);
		bool receiveRankResult(const String& rankResult);
		SKyBlockRankData* getRankData(rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> &data);
		void clearData();
		bool isNeedLoad();

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}
#endif