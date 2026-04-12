#pragma once
#ifndef __GUI_BLOCKCITY_RANK_ITEM_HEADER__
#define __GUI_BLOCKCITY_RANK_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Game/BlockCity.h"
#include "Util/RankManager.h"
#include "GUI/GuiUrlImage.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum RankType
	{
		GlobalRank = 1,
		FriendRank = 2,
		MyRank = 3
	};

	class GuiBlockCityRankItem : public CustomGuiWindow
	{
	private:
		enum class ViewId {
			ADD_FRIEND,
			VISIT
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUILayout* m_btn = nullptr;
		GUILayout* m_ownInfo = nullptr;
		GUIButton* m_btnAddFriend = nullptr;
		GUIButton* m_btnVisit = nullptr;
		GUIStaticText	*m_rankNo = nullptr;
		GuiUrlImage	*m_pic = nullptr;
		GUIStaticText	*m_name = nullptr;
		GUIStaticText	*m_score = nullptr;
		GUIStaticText	*m_myRank = nullptr;

		UserRank m_rank;
		i32 m_addFriendTime = 0;

	private:
		void removeComponents();

	public:
		GuiBlockCityRankItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBlockCityRankItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		bool onClick(const EventArgs& args, ViewId viewId);
		void setRank(const UserRank &rank, i32 type);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
