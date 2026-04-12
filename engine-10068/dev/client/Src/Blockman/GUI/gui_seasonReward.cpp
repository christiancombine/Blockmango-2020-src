#include "gui_seasonReward.h"
#include "GUI/GUISeasonHonorItem.h"
#include "GUI/GUISeasonRewardItem.h"
#include "GUI/GuiActorWindow.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Setting/SeasonSetting.h"
#include "Setting/RewardSetting.h"
#include "Network/protocol/CommonDataBuilder.h"

namespace BLOCKMAN
{
	gui_seasonReward::gui_seasonReward() :
		gui_layout("SeasonReward.json")
	{
	}

	gui_seasonReward::~gui_seasonReward()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_seasonReward::onLoad()
	{
		getWindow("SeasonReward-Title")->SetText(getString(LanguageKey::GUI_SEASON_REWARD_TITLE));
		getWindow("SeasonReward-Rank-Text")->SetText("");
		getWindow("SeasonReward-Bottom-Hint")->SetText(getString(LanguageKey::GUI_SEASON_REWARD_BOTTOM_HINT));

		auto BtnClose = getWindow("SeasonReward-Close");
		BtnClose->subscribeEvent(EventButtonClick, std::bind(&gui_seasonReward::onCloseClick, this, std::placeholders::_1));

		m_subscriptionGuard.add(UpdateCurrentSeasonInfoEvent::subscribe(std::bind(&gui_seasonReward::onUpdateCurrentSeason, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
	}

	void gui_seasonReward::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown())
		{
			return;
		}
		if (m_show_actor_time > 0)
		{
			m_show_actor_time -= nTimeElapse;
			if (m_show_actor_time <= 0)
			{
				m_show_actor_time = 0;
				getWindow("SeasonReward-Actor")->SetVisible(false);
				getWindow("SeasonReward-Border")->SetVisible(true);
			}
		}
	}

	bool gui_seasonReward::isNeedLoad()
	{
		return checkGameType(ClientGameType::PixelGunHall);
	}

	void gui_seasonReward::onGetCommonData(String key, String data)
	{
		if (key == "LastSeasonInfo")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int honorId = builder.getIntParam("honorId");
			int rank = builder.getIntParam("rank");
			int honor = builder.getIntParam("honor");
			onUpdateLastSeason(honorId, rank, honor);
			getParent()->addOrderShowLayout(RootGuiLayout::LayoutId::SEASON_REWARD);
		}
	}

	bool gui_seasonReward::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->hideWindowByLayoutId(RootGuiLayout::LayoutId::SEASON_REWARD);
		return true;
	}

	bool gui_seasonReward::onUpdateLastSeason(int honorId, int rank, int honor)
	{
		getWindow("SeasonReward-Border")->SetVisible(false);

		auto CurrentHonor = getWindow<GUISeasonHonorItem>("SeasonReward-Left-Honor");
		CurrentHonor->SetHonorInfo(HonorItemType::LAST_SEASON, honorId, rank, honor);

		auto Rank = getWindow("SeasonReward-Rank-Text");
		Rank->SetText(StringUtil::Format(getString(LanguageKey::GUI_SEASON_REWARD_RANK).c_str(), rank).c_str());
		
		auto RewardInfo = getWindow("SeasonReward-Rewards");
		RewardInfo->CleanupChildren();
		list<int>::type rewardIds;
		Season* season = SeasonSetting::findSeasonByHonorId((SeasonHonorId)honorId);
		Season* next_season = SeasonSetting::findSeasonByHonorId((SeasonHonorId)m_cur_honor_id);
		
		int seasonId = 0;
		bool isUp = false;
		if (season != nullptr)
		{
			seasonId = season->Id;
			if (next_season == nullptr)
			{
				rewardIds = season->NormalRewardIds;
			}
			else
			{
				isUp = m_cur_honor_id > season->HonorId;
				rewardIds = isUp ? season->UpRewardIds : season->NormalRewardIds;
			}
		}
		
		playSoundByType(ST_PIXEL_DUAN_WEI);
		auto Actor = getWindow<GuiActorWindow>("SeasonReward-Actor");
		String ActorName = StringUtil::Format("duanwei_%d.actor", m_cur_honor_id + 1);
		Actor->SetActor(ActorName.c_str(), "idle");
		m_show_actor_time = 2500;
			
		static unsigned RewardIndex = 0;
		float ItemWidth = 135.0f;
		float ItemHeight = 45.0f;
		float PositionX = 0;
		
		SeasonRankReward* rank_reward = SeasonSetting::findSeasonRankRewardBySeasonIdAndRank(seasonId, rank);
		if (rank_reward && isUp)
		{
			RewardIndex++;
			GUIString ChildName = StringUtil::Format("SeasonReward-Reward-Item-%d", RewardIndex).c_str();
			GUISeasonRewardItem* RewardItem = (GUISeasonRewardItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SEASON_REWARD_ITEM, ChildName);
			RewardItem->SetArea({ 0.0f, PositionX }, { 0.0f, 1.0f }, { 0.0f, ItemWidth }, { 0.0f, ItemHeight });
			RewardItem->SetSeasonRankReward(rank_reward, true);
			RewardInfo->AddChildWindow(RewardItem);
			PositionX += ItemWidth;
		}

		for (auto rewardId : rewardIds)
		{
			auto reward = RewardSetting::findRewardById(rewardId);
			if (reward != nullptr)
			{
				RewardIndex++;
				GUIString ChildName = StringUtil::Format("SeasonReward-Reward-Item-%d", RewardIndex).c_str();
				GUISeasonRewardItem* RewardItem = (GUISeasonRewardItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SEASON_REWARD_ITEM, ChildName);
				RewardItem->SetArea({ 0.0f, PositionX }, { 0.0f, 1.0f }, { 0.0f, ItemWidth }, { 0.0f, ItemHeight });
				RewardItem->SetSeasonReward(reward, true);
				RewardInfo->AddChildWindow(RewardItem);
				PositionX += ItemWidth;
			}
		}

		return true;
	}

	bool gui_seasonReward::onUpdateCurrentSeason(int honorId, int rank, int honor, int endTime)
	{
		m_cur_honor_id = honorId;
		auto CurrentHonor = getWindow<GUISeasonHonorItem>("SeasonReward-Right-Honor");
		CurrentHonor->SetHonorInfo(HonorItemType::CURRENT_SEASON, honorId, rank, honor);
		return true;
	}

}
