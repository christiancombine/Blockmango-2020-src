#include "gui_blockCityRankFriend.h"
#include "GUI/GuiDef.h"
#include "Util/UICommon.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Util/ClientEvents.h"
#include "Network/ClientHttpRequest.h"
#include "GUI/GuiBlockCityRankItem.h"
#include "Util/FriendManager.h"
#include "GUI/GuiActorWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIListBox.h"
#include "UI/GUIRadioButton.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_blockCityRankFriend::gui_blockCityRankFriend()
		: gui_layout("BlockCityRankFriend.json")
	{
	}

	gui_blockCityRankFriend::~gui_blockCityRankFriend()
	{
		m_rankLayoutMap.clear();
		m_radioTabMap.clear();
		m_radioCategoryMap.clear();
		m_listMap.clear();
	}

	void gui_blockCityRankFriend::onLoad()
	{
		setText("BlockCityRankFriend-Labs-Charm-Text", "gui_blockcity_rank_charm");
		setText("BlockCityRankFriend-Labs-Construction-Text", "gui_blockcity_rank_construction");
		setText("BlockCityRankFriend-Charm-Total", "gui_rank_vice_title_hist_name");
		setText("BlockCityRankFriend-Charm-Week", "gui_rank_vice_title_right_name");

		m_first = getWindow<GUILayout>("BlockCityRankFriend-Onlist-First");
		m_own = getWindow<GUILayout>("BlockCityRankFriend-Onlist-Own");
		m_constructionRanks = getWindow<GUIListBox>("BlockCityRankFriend-Ranks");
		m_charmTotalRanks = getWindow<GUIListBox>("BlockCityRankFriend-Charm-Rank-Total");
		m_charmWeekRanks = getWindow<GUIListBox>("BlockCityRankFriend-Charm-Rank-Week");
		m_charmCategory = getWindow<GUILayout>("BlockCityRankFriend-Charm-Category");
		m_charmCategory->SetVisible(false);

		m_radioTabMap[ViewId::CHARM] = getWindow<GUIRadioButton>("BlockCityRankFriend-Labs-Charm");
		m_radioTabMap[ViewId::CONSTRUCTION] = getWindow<GUIRadioButton>("BlockCityRankFriend-Labs-Construction");

		m_radioCategoryMap[ViewId::TOTAL] = getWindow<GUIRadioButton>("BlockCityRankFriend-Charm-Total");
		m_radioCategoryMap[ViewId::WEEK] = getWindow<GUIRadioButton>("BlockCityRankFriend-Charm-Week");

		m_listMap[ViewId::CONSTRUCTION] = m_constructionRanks;
		m_listMap[ViewId::TOTAL] = m_charmTotalRanks;
		m_listMap[ViewId::WEEK] = m_charmWeekRanks;

		m_subscriptionGuard.add(UpdateBlockCityFriendRankEvent::subscribe(std::bind(&gui_blockCityRankFriend::onUpdateBlockCityFriendRank, this, std::placeholders::_1, "construction")));
		m_subscriptionGuard.add(UserActorChangeEvent::subscribe(std::bind(&gui_blockCityRankFriend::userActorChange, this, std::placeholders::_1)));
		//m_subscriptionGuard.add(UpdateBlockCityFriendCharmRankEvent::subscribe(std::bind(&gui_blockCityRankFriend::onUpdateBlockCityFriendRank, this, std::placeholders::_1, std::placeholders::_2)));

		map<ViewId, GUIRadioButton*>::iterator  iter;
		for (iter = m_radioTabMap.begin(); iter != m_radioTabMap.end(); iter++)
		{
			iter->second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockCityRankFriend::onRadioChange, this, std::placeholders::_1, iter->first));
		}

		map<ViewId, GUIRadioButton*>::iterator  iterCategory;
		for (iterCategory = m_radioCategoryMap.begin(); iterCategory != m_radioCategoryMap.end(); iterCategory++)
		{
			iterCategory->second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockCityRankFriend::onCategoryRadioChange, this, std::placeholders::_1, iterCategory->first));
		}
	}

	void gui_blockCityRankFriend::onShow()
	{
		if (m_constructionRanks && (!m_constructionRanks->getContainerWindow() || m_constructionRanks->getContainerWindow()->GetChildCount() == 0))
		{
			ClientHttpRequest::loadFriend();
		}
	}

	bool gui_blockCityRankFriend::isNeedLoad()
	{
		return checkGameType({ ClientGameType::BlockCity ,ClientGameType::BlockFort });
	}

	bool gui_blockCityRankFriend::onUpdateBlockCityFriendRank(const vector<UserRank>::type& items, const String& type)
	{
		if (type == "construction")
		{
			onsetRank(items, m_listMap[ViewId::CONSTRUCTION]);
			m_radioTabMap[ViewId::CONSTRUCTION]->SetSelected(true);
		}
		else if (type == "all")
		{
			onsetRank(items, m_listMap[ViewId::TOTAL]);
			m_radioTabMap[ViewId::CHARM]->SetSelected(true);
		}
		else if (type == "week")
		{
			onsetRank(items, m_listMap[ViewId::WEEK]);
		}
		return true;
	}

	bool gui_blockCityRankFriend::userActorChange(ui64 userId)
	{
		String key = StringUtil::ToString((i64)userId) + "first";
		GUILayout * view = m_rankLayoutMap[key];
		if (view)
		{
			changeActor(userId, view);
		}

		key = StringUtil::ToString((i64)userId) + "own";
		view = m_rankLayoutMap[key];
		if (view)
		{
			changeActor(userId, view);
		}

		return true;
	}

	void gui_blockCityRankFriend::changeActor(ui64 userId, GUILayout *view)
	{
		UserActor *userActor = FriendManager::findUserActor(userId);
		if (userActor)
		{
			GuiActorWindow* actorWindow = dynamic_cast<GuiActorWindow*>(view->GetChildByIndex(1));
			actorWindow->SetActor(userActor->sex == 1 ? "boy.actor" : "girl.actor", "idle");
			for (const auto& item : userActor->items)
			{
				if (item.resName.length() > 0 && item.resId.length() > 0 && item.resId != "0")
				{
					actorWindow->UseBodyPart(item.resName, item.resId);
				}
			}
			actorWindow->SetCustomColor(userActor->actorColor);
		}
	}

	void gui_blockCityRankFriend::onsetRank(const vector<UserRank>::type& items, GUIListBox* view)
	{
		static i32 count = 0;
		view->ClearAllItem();
		for (auto item : items)
		{
			String iterLayout = StringUtil::Format("BlockCityRank-FriendRank-Items-%d", count++).c_str();
			GuiBlockCityRankItem *itemView = (GuiBlockCityRankItem *)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_RANK_ITEM, iterLayout.c_str());
			itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.f), UDim(0.0f, 64.f));
			itemView->setRank(item, RankType::FriendRank);
			view->AddItem(itemView, false);
		}
		if (view == m_constructionRanks)
		{
			m_construction = items;
		}
		else if (view == m_charmTotalRanks)
		{
			m_charmTotal = items;
		}
		else
		{
			m_charmWeek = items;
		}
	}

	bool gui_blockCityRankFriend::onRadioChange(const EventArgs & events, ViewId viewId)
	{
		if (m_radioTabMap[viewId]->IsSelected())
		{
			switch (viewId)
			{
			case BLOCKMAN::gui_blockCityRankFriend::ViewId::CHARM:
				if (m_radioCategoryMap[ViewId::TOTAL]->IsSelected())
				{
					onShowListView(ViewId::TOTAL);
					onSetPodiumInfo(m_charmTotal);
				}
				else
				{
					m_radioCategoryMap[ViewId::TOTAL]->SetSelected(true);
				}
				IsShowBlockCityCharmRankEvent::emit(true);
				break;
			case BLOCKMAN::gui_blockCityRankFriend::ViewId::CONSTRUCTION:
				onSetPodiumInfo(m_construction);
				onShowListView(ViewId::CONSTRUCTION);
				IsShowBlockCityCharmRankEvent::emit(false);
				break;
			}
			m_charmCategory->SetVisible(viewId == ViewId::CHARM);
			playSoundByType(ST_Click);
		}
		return true;
	}

	bool gui_blockCityRankFriend::onCategoryRadioChange(const EventArgs & events, ViewId viewId)
	{
		if (m_radioCategoryMap[viewId]->IsSelected())
		{
			switch (viewId)
			{
			case BLOCKMAN::gui_blockCityRankFriend::ViewId::TOTAL:
				onSetPodiumInfo(m_charmTotal);
				break;
			case BLOCKMAN::gui_blockCityRankFriend::ViewId::WEEK:
				onSetPodiumInfo(m_charmWeek);
				break;
			default:
				break;
			}
			onShowListView(viewId);
			playSoundByType(ST_Click);
		}
		return true;
	}

	void gui_blockCityRankFriend::onShowListView(ViewId viewId)
	{
		map<ViewId, GUIListBox*>::iterator  iter;
		for (iter = m_listMap.begin(); iter != m_listMap.end(); iter++)
		{
			iter->second->SetVisible(false);
		}
		m_listMap[viewId]->SetVisible(true);
	}

	void gui_blockCityRankFriend::onSetPodiumInfo(const vector<UserRank>::type& items)
	{
		for (auto item : items)
		{
			if (item.rank == 1)
			{
				GUIWindow* tvUserName = m_first->GetChildByIndex(0)->GetChildByIndex(0);
				GUIWindow* tvUserScore = m_first->GetChildByIndex(2)->GetChildByIndex(0)->GetChildByIndex(0);
				tvUserName->SetText(item.userName.c_str());
				tvUserScore->SetText(StringUtil::ToString(item.score).c_str());
				String key = StringUtil::ToString((i64)item.userId) + "first";
				m_rankLayoutMap[key] = m_first;
				ClientHttpRequest::loadUserInfo(item.userId);
			}

			if (item.userId == Blockman::Instance()->m_pPlayer->getPlatformUserId())
			{
				GUIWindow* tvUserName = m_own->GetChildByIndex(0)->GetChildByIndex(0);
				GUIWindow* tvUserScore = m_own->GetChildByIndex(2)->GetChildByIndex(0)->GetChildByIndex(0);
				GUIWindow* tvRank = m_own->GetChildByIndex(0)->GetChildByIndex(1)->GetChildByIndex(0);
				tvUserName->SetText(item.userName.c_str());
				tvUserScore->SetText(StringUtil::ToString(item.score).c_str());
				tvRank->SetText(StringUtil::ToString(item.rank).c_str());
				String key = StringUtil::ToString((i64)item.userId) + "own";
				m_rankLayoutMap[key] = m_own;
				ClientHttpRequest::loadUserInfo(item.userId);
			}
		}
	}
}
