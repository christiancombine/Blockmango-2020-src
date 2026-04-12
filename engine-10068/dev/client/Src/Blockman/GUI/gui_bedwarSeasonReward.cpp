#include "gui_bedwarSeasonReward.h"
#include "GUI/GUIBedWarSeasonHonorItem.h"
#include "GUI/GUIBedWarSeasonRewardItem.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Setting/SeasonSetting.h"
#include "Setting/RewardSetting.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "GUI/RootGuiLayout.h"

namespace BLOCKMAN
{
	gui_bedwarSeasonReward::gui_bedwarSeasonReward() :
		gui_layout("BedWarSeasonReward.json")
	{
	}

	gui_bedwarSeasonReward::~gui_bedwarSeasonReward()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	RootGuiLayout* gui_bedwarSeasonReward::getParent()
	{
		return dynamic_cast<RootGuiLayout*>(m_parentLayout);
	}

	void gui_bedwarSeasonReward::onLoad()
	{
		getWindow("BedWarSeasonReward-Title")->SetText(getString(LanguageKey::GUI_SEASON_REWARD_TITLE));
		getWindow("BedWarSeasonReward-Rank-Text")->SetText("");
		getWindow("BedWarSeasonReward-Bottom-Hint")->SetText(getString(LanguageKey::GUI_SEASON_REWARD_BOTTOM_HINT));

		auto BtnClose = getWindow("BedWarSeasonReward-Close");
		BtnClose->subscribeEvent(EventButtonClick, std::bind(&gui_bedwarSeasonReward::onCloseClick, this, std::placeholders::_1));

		m_subscriptionGuard.add(UpdateBedWarCurrentSeasonInfoEvent::subscribe(std::bind(&gui_bedwarSeasonReward::onUpdateCurrentSeason, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
	}

	void gui_bedwarSeasonReward::onUpdate(ui32 nTimeElapse)
	{

	}

	bool gui_bedwarSeasonReward::isNeedLoad()
	{
		return checkGameType(ClientGameType::BED_WAR_HALL);
	}

	void gui_bedwarSeasonReward::onGetCommonData(String key, String data)
	{
		if (key == "LastSeasonInfo")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int honorId = builder.getIntParam("honorId");
			int rank = builder.getIntParam("rank");
			int honor = builder.getIntParam("honor");
			onUpdateLastSeason(honorId, rank, honor);
			getParent()->addOrderShowLayout(RootGuiLayout::LayoutId::BEDWAR_SEASON_REWARD);
		}
	}

	bool gui_bedwarSeasonReward::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->hideWindowByLayoutId(RootGuiLayout::LayoutId::BEDWAR_SEASON_REWARD);
		return true;
	}

	bool gui_bedwarSeasonReward::onUpdateLastSeason(int honorId, int rank, int honor)
	{
		auto CurrentHonor = getWindow<GUIBedWarSeasonHonorItem>("BedWarSeasonReward-Left-Honor");
		CurrentHonor->SetHonorInfo(BedWarHonorItemType::BED_WAR_LAST_SEASON, honorId, rank, honor);

		auto Rank = getWindow("BedWarSeasonReward-Rank-Text");
		Rank->SetText(StringUtil::Format(getString(LanguageKey::GUI_SEASON_REWARD_RANK).c_str(), rank).c_str());

		auto RewardInfo = getWindow("BedWarSeasonReward-Rewards");
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

		static unsigned RewardIndex = 0;
		float ItemWidth = 100.0f;
		float ItemHeight = 45.0f;
		float PositionX = 0;

		SeasonRankReward* rank_reward = SeasonSetting::findSeasonRankRewardBySeasonIdAndRank(seasonId, rank);
		if (rank_reward && isUp)
		{
			RewardIndex++;
			GUIString ChildName = StringUtil::Format("BedWarSeasonReward-Reward-Item-%d", RewardIndex).c_str();
			GUIBedWarSeasonRewardItem* RewardItem = (GUIBedWarSeasonRewardItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BEDWAR_SEASON_REWARD_ITEM, ChildName);
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
				GUIString ChildName = StringUtil::Format("BedWarSeasonReward-Reward-Item-%d", RewardIndex).c_str();
				GUIBedWarSeasonRewardItem* RewardItem = (GUIBedWarSeasonRewardItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BEDWAR_SEASON_REWARD_ITEM, ChildName);
				RewardItem->SetArea({ 0.0f, PositionX }, { 0.0f, 1.0f }, { 0.0f, ItemWidth }, { 0.0f, ItemHeight });
				RewardItem->SetSeasonReward(reward, true);
				RewardInfo->AddChildWindow(RewardItem);
				PositionX += ItemWidth;
			}
		}

		return true;
	}

	bool gui_bedwarSeasonReward::onUpdateCurrentSeason(int honorId, int rank, int honor, int endTime)
	{
		m_cur_honor_id = honorId;
		auto CurrentHonor = getWindow<GUIBedWarSeasonHonorItem>("BedWarSeasonReward-Right-Honor");
		CurrentHonor->SetHonorInfo(BedWarHonorItemType::BED_WAR_CURRENT_SEASON, honorId, rank, honor);
		return true;
	}

}
