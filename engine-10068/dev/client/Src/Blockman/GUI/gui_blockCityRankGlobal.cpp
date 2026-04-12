#include "gui_blockCityRankGlobal.h"
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
	gui_blockCityRankGlobal::gui_blockCityRankGlobal()
		: gui_layout("BlockCityRankGlobal.json")
	{
	}

	gui_blockCityRankGlobal::~gui_blockCityRankGlobal()
	{
		m_rankLayoutMap.clear();
		m_radioTabMap.clear();
		m_radioCategoryMap.clear();
		m_listMap.clear();
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_blockCityRankGlobal::onLoad()
	{
		m_first = getWindow<GUILayout>("BlockCityRankGlobal-Onlist-First");
		m_second = getWindow<GUILayout>("BlockCityRankGlobal-Onlist-Second");
		m_third = getWindow<GUILayout>("BlockCityRankGlobal-Onlist-Third");
		m_constructionRanks = getWindow<GUIListBox>("BlockCityRankGlobal-Ranks");
		m_charmTotalRanks = getWindow<GUIListBox>("BlockCityRankGlobal-Charm-Rank-Total");
		m_charmWeekRanks = getWindow<GUIListBox>("BlockCityRankGlobal-Charm-Rank-Week");
		m_myRank = getWindow<GuiBlockCityRankItem>("BlockCityRankGlobal-MyRank");
		m_charmCategory = getWindow<GUILayout>("BlockCityRankGlobal-Charm-Category");
		m_charmCategory->SetVisible(false);

		m_radioTabMap[ViewId::CHARM] = getWindow<GUIRadioButton>("BlockCityRankGlobal-Labs-Charm");
		m_radioTabMap[ViewId::CONSTRUCTION] = getWindow<GUIRadioButton>("BlockCityRankGlobal-Labs-Construction");

		m_radioCategoryMap[ViewId::TOTAL] = getWindow<GUIRadioButton>("BlockCityRankGlobal-Charm-Total");
		m_radioCategoryMap[ViewId::WEEK] = getWindow<GUIRadioButton>("BlockCityRankGlobal-Charm-Week");

		m_listMap[ViewId::CONSTRUCTION] = m_constructionRanks;
		m_listMap[ViewId::TOTAL] = m_charmTotalRanks;
		m_listMap[ViewId::WEEK] = m_charmWeekRanks;

		m_subscriptionGuard.add(UpdateBlockCityGolbalRankEvent::subscribe(std::bind(&gui_blockCityRankGlobal::onUpdateBlockCityGlobalRank, this, std::placeholders::_1, "construction")));
		m_subscriptionGuard.add(UpdateBlockCityMyRankEvent::subscribe(std::bind(&gui_blockCityRankGlobal::onUpdateMyRank, this, std::placeholders::_1, "12345")));
		m_subscriptionGuard.add(UserActorChangeEvent::subscribe(std::bind(&gui_blockCityRankGlobal::userActorChange, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UpdateBlockCityGolbalCharmRankEvent::subscribe(std::bind(&gui_blockCityRankGlobal::onUpdateBlockCityGlobalRank, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(UpdateBlockCityMyCharmRankEvent::subscribe(std::bind(&gui_blockCityRankGlobal::onUpdateMyRank, this, std::placeholders::_1, std::placeholders::_2)));

		map<ViewId, GUIRadioButton*>::iterator  iter;
		for (iter = m_radioTabMap.begin(); iter != m_radioTabMap.end(); iter++)
		{
			iter->second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockCityRankGlobal::onRadioChange, this, std::placeholders::_1, iter->first));
		}

		map<ViewId, GUIRadioButton*>::iterator  iterCategory;
		for (iterCategory = m_radioCategoryMap.begin(); iterCategory != m_radioCategoryMap.end(); iterCategory++)
		{
			iterCategory->second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockCityRankGlobal::onCategoryRadioChange, this, std::placeholders::_1, iterCategory->first));
		}
	}

	void gui_blockCityRankGlobal::onShow()
	{
		if (m_constructionRanks &&(!m_constructionRanks->getContainerWindow() || m_constructionRanks->getContainerWindow()->GetChildCount() == 0))
		{
			ClientHttpRequest::loadGlobalRank("score", 0, 50);
			//ClientHttpRequest::loadAppRank("all", 0, 50);
			//ClientHttpRequest::loadAppRank("week", 0, 50);
		}
	}

	bool gui_blockCityRankGlobal::isNeedLoad()
	{
		return checkGameType({ ClientGameType::BlockCity ,ClientGameType::BlockFort });
	}

	bool gui_blockCityRankGlobal::onUpdateBlockCityGlobalRank(const vector<UserRank>::type& items, const String& type)
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

	bool gui_blockCityRankGlobal::onUpdateMyRank(UserRank &item, const String& type)
	{
		m_myRank->setRank(item, RankType::MyRank);
		return true;
	}

	bool gui_blockCityRankGlobal::userActorChange(ui64 userId)
	{
		GUILayout * view = m_rankLayoutMap[userId];
		if (view)
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
		return true;
	}

	void gui_blockCityRankGlobal::onsetRank(const vector<UserRank>::type& items, GUIListBox* view)
	{
		static i32 count = 0;
		view->ClearAllItem();
		for (auto item : items)
		{
			String iterLayout = StringUtil::Format("BlockCityRank-GolbalRank-Items-%d", count++).c_str();
			GuiBlockCityRankItem *itemView = (GuiBlockCityRankItem *)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_RANK_ITEM, iterLayout.c_str());
			itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.f), UDim(0.0f, 64.f));
			itemView->setRank(item, RankType::GlobalRank);
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

	bool gui_blockCityRankGlobal::onRadioChange(const EventArgs & events, ViewId viewId)
	{
		if (m_radioTabMap[viewId]->IsSelected())
		{
			switch (viewId)
			{
			case BLOCKMAN::gui_blockCityRankGlobal::ViewId::CHARM:
				if (m_radioCategoryMap[ViewId::TOTAL]->IsSelected())
				{
					ClientHttpRequest::loadMyAppRank("all");
					onSetPodiumInfo(m_charmTotal);
					onShowListView(ViewId::TOTAL);
				}
				else
				{
					m_radioCategoryMap[ViewId::TOTAL]->SetSelected(true);
				}
				IsShowBlockCityCharmRankEvent::emit(true);
				break;
			case BLOCKMAN::gui_blockCityRankGlobal::ViewId::CONSTRUCTION:
				ClientHttpRequest::loadMyRank("score");
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

	bool gui_blockCityRankGlobal::onCategoryRadioChange(const EventArgs & events, ViewId viewId)
	{
		if (m_radioCategoryMap[viewId]->IsSelected())
		{
			onShowListView(viewId);
			if (viewId == ViewId::TOTAL)
			{
				ClientHttpRequest::loadMyAppRank("all");
				onSetPodiumInfo(m_charmTotal);
			}
			if (viewId == ViewId::WEEK)
			{
				ClientHttpRequest::loadMyAppRank("week");
				onSetPodiumInfo(m_charmWeek);
			}
			playSoundByType(ST_Click);
		}
		return true;
	}

	void gui_blockCityRankGlobal::onShowListView(ViewId viewId)
	{
		map<ViewId, GUIListBox*>::iterator  iter;
		for (iter = m_listMap.begin(); iter != m_listMap.end(); iter++)
		{
			iter->second->SetVisible(false);
		}
		m_listMap[viewId]->SetVisible(true);
	}

	void gui_blockCityRankGlobal::onSetPodiumInfo(const vector<UserRank>::type& items)
	{
		for (auto item : items)
		{
			switch (item.rank)
			{
			case 1:
			{
				GUIWindow* tvUserName = m_first->GetChildByIndex(0)->GetChildByIndex(0);
				GUIWindow* tvUserScore = m_first->GetChildByIndex(2)->GetChildByIndex(0)->GetChildByIndex(0);
				tvUserName->SetText(item.userName.c_str());
				tvUserScore->SetText(StringUtil::ToString(item.score).c_str());
				m_rankLayoutMap[item.userId] = m_first;
			}
			break;
			case 2:
			{
				GUIWindow* tvUserName = m_second->GetChildByIndex(0)->GetChildByIndex(0);
				GUIWindow* tvUserScore = m_second->GetChildByIndex(2)->GetChildByIndex(0)->GetChildByIndex(0);
				tvUserName->SetText(item.userName.c_str());
				tvUserScore->SetText(StringUtil::ToString(item.score).c_str());
				m_rankLayoutMap[item.userId] = m_second;
			}
			break;
			case 3:
			{
				GUIWindow* tvUserName = m_third->GetChildByIndex(0)->GetChildByIndex(0);
				GUIWindow* tvUserScore = m_third->GetChildByIndex(2)->GetChildByIndex(0)->GetChildByIndex(0);
				tvUserName->SetText(item.userName.c_str());
				tvUserScore->SetText(StringUtil::ToString(item.score).c_str());
				m_rankLayoutMap[item.userId] = m_third;
			}
			break;
			}

			if (item.rank < 4)
			{
				ClientHttpRequest::loadUserInfo(item.userId);
			}
		}
	}

}
