#include "gui_bedwarSeasonRank.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "GUI/GUIBedWarSeasonRankPanel.h"
#include "GUI/GUIBedWarSeasonRewardItem.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Setting/SeasonSetting.h"
#include "Setting/RewardSetting.h"
#include "Network/ClientHttpRequest.h"
#include "Network/ClientNetwork.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "GUI/RootGuiLayout.h"
#include "GUI/GUIBedWarSeasonRule.h"
#include "Plugins/GameClientEvents.h"

namespace BLOCKMAN
{
	gui_bedwarSeasonRank::gui_bedwarSeasonRank() :
		gui_layout("BedWarSeasonRank.json")
	{
		m_season_rule = m_season_rule ? m_season_rule : LordNew GUIBedWarSeasonRule;
	}

	gui_bedwarSeasonRank::~gui_bedwarSeasonRank()
	{
		LordSafeDelete(m_season_rule);
		m_subscriptionGuard.unsubscribeAll();
		SeasonSetting::unloadSetting();
	}

	RootGuiLayout* gui_bedwarSeasonRank::getParent()
	{
		return dynamic_cast<RootGuiLayout*>(m_parentLayout);
	}

	void gui_bedwarSeasonRank::onLoad()
	{
		SeasonSetting::loadSetting();

		GUILayout* SeasonRank = getWindow<GUILayout>("BedWarSeasonRank");
		m_season_rule->attachTo(this, SeasonRank);
		m_season_rule->hide();

		getWindow("BedWarSeasonRank-Season-Countdown-Text")->SetText("");
		getWindow("BedWarSeasonRank-My-Reward-Text")->SetText(getString(LanguageKey::GUI_BED_WAR_SEASON_RANK_REWARD));
		getWindow("BedWarSeasonRank-Armory-Text")->SetText(getString(LanguageKey::GUI_BED_WAR_STORE_NAME));
		getWindow("BedWarSeasonRank-Battle-Text")->SetText(getString(LanguageKey::GUI_BED_WAR_SEASON_RANK_BATTLE));
		getWindow("BedWarSeasonRank-Title")->SetText(getString(LanguageKey::GUI_BED_WAR_SEASON_RANK_TITLE));

		auto BtnClose = getWindow<GUIButton>("BedWarSeasonRank-Close");
		BtnClose->subscribeEvent(EventButtonClick, std::bind(&gui_bedwarSeasonRank::onCloseClick, this, std::placeholders::_1));
		auto BtnQuesition = getWindow<GUIButton>("BedWarSeasonRank-Question");
		BtnQuesition->subscribeEvent(EventButtonClick, std::bind(&gui_bedwarSeasonRank::onQuestionClick, this, std::placeholders::_1));
		auto BtnArmory = getWindow<GUIButton>("BedWarSeasonRank-Armory");
		BtnArmory->subscribeEvent(EventButtonClick, std::bind(&gui_bedwarSeasonRank::onArmoryClick, this, std::placeholders::_1));
		auto BtnBattle = getWindow<GUIButton>("BedWarSeasonRank-Battle");
		BtnBattle->subscribeEvent(EventButtonClick, std::bind(&gui_bedwarSeasonRank::onBattleClick, this, std::placeholders::_1));

		auto LeftRank = getWindow<GUIBedWarSeasonRankPanel>("BedWarSeasonRank-Left-Rank-Panel");
	    auto RightRank = getWindow<GUIBedWarSeasonRankPanel>("BedWarSeasonRank-Right-Rank-Panel");
		onSeasonInit(SeasonHonorId::SEASON_NONE, 0, 0, 0);

		m_subscriptionGuard.add(UpdateBedWarCurrentSeasonInfoEvent::subscribe(std::bind(&gui_bedwarSeasonRank::onSeasonInit, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(UpdateBedWarSeasonRankDataEvent::subscribe(std::bind(&gui_bedwarSeasonRank::onUpdateSeasonRankData, this, std::placeholders::_1, std::placeholders::_2)));

	}

	void gui_bedwarSeasonRank::onUpdate(ui32 nTimeElapse)
	{

	}

	void gui_bedwarSeasonRank::onShow()
	{
		auto LeftRank = getWindow<GUIBedWarSeasonRankPanel>("BedWarSeasonRank-Left-Rank-Panel");
		list<SeasonRankItem>::type LeftRanks = SeasonSetting::findSeasonRankItemByType(TYPE_LEFT);
		LeftRank->SetSeasonRankData(LeftRanks);
		auto RightRank = getWindow<GUIBedWarSeasonRankPanel>("BedWarSeasonRank-Right-Rank-Panel");
		list<SeasonRankItem>::type RightRanks = SeasonSetting::findSeasonRankItemByType(TYPE_RIGHT);
		RightRank->SetSeasonRankData(RightRanks);
	}

	bool gui_bedwarSeasonRank::isNeedLoad()
	{
		return checkGameType(ClientGameType::BED_WAR_HALL);
	}

	void gui_bedwarSeasonRank::onGetCommonData(String key, String data)
	{
		if (key == "ShowSeasonRank")
		{
			OpenBedWarSeasonRankEvent::emit();
			return;
		}
		if (key == "CurrentSeasonInfo")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int honorId = builder.getIntParam("honorId");
			int rank = builder.getIntParam("rank");
			int honor = builder.getIntParam("honor");
			int endTime = builder.getIntParam("endTime");
			UpdateBedWarCurrentSeasonInfoEvent::emit(honorId, rank, honor, endTime);
			return;
		}
	}

	bool gui_bedwarSeasonRank::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		return true;
	}

	bool gui_bedwarSeasonRank::onQuestionClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		m_season_rule->show();
		return true;
	}

	bool gui_bedwarSeasonRank::onArmoryClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		OpenBedWarRuneEvent::emit();
		ClientNetwork::Instance()->getCommonSender()->sendBedWarOpenRune();
		return true;
	}

	bool gui_bedwarSeasonRank::onBattleClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		getParent()->showMainControl();
		SCRIPT_EVENT::CallPluginFunctionEvent::invoke("StartGame", "g1008");
		return true;
	}

	bool gui_bedwarSeasonRank::onSeasonInit(int honorId, int rank, int honor, int endTime)
	{
		auto SeasonName = getWindow("BedWarSeasonRank-Season-Name");
		auto Countdown = getWindow("BedWarSeasonRank-Season-Countdown-Text");
		auto MyRewardInfo = getWindow("BedWarSeasonRank-My-Reward-Info");
		auto MyRank = getWindow("BedWarSeasonRank-My-Rank-Text");
		auto MyName = getWindow("BedWarSeasonRank-My-Rank-Name");
		auto MyHonor = getWindow("BedWarSeasonRank-My-Rank-Honor-Text");
		auto MyReward = getWindow<GUIStaticImage>("BedWarSeasonRank-My-Rank-Reward");
		auto MyRewardCount = getWindow("BedWarSeasonRank-My-Rank-Reward-Count");
		auto LeftRank = getWindow<GUIBedWarSeasonRankPanel>("BedWarSeasonRank-Left-Rank-Panel");
		auto RightRank = getWindow<GUIBedWarSeasonRankPanel>("BedWarSeasonRank-Right-Rank-Panel");
		Season* season = SeasonSetting::findSeasonByHonorId((SeasonHonorId)honorId);
		if (season)
		{
			SeasonHonorId NextHonorId = SeasonHonorId::SEASON_NONE;
			if (season->HonorId == SeasonHonorId::KING)
			{
				NextHonorId = season->HonorId;
				LeftRank->InitUI(season->HonorId);
				RightRank->InitUI(season->HonorId);
			}
			else
			{
				NextHonorId = (SeasonHonorId)(honorId + 1);
				LeftRank->InitUI(NextHonorId);
				RightRank->InitUI(season->HonorId);
			}
			LeftRank->SetSeasonHonorId(season->HonorId);
			RightRank->SetSeasonHonorId(season->HonorId);
			LeftRank->SetSeasonRankType(SeasonRankType::TYPE_LEFT);
			RightRank->SetSeasonRankType(SeasonRankType::TYPE_RIGHT);
			SeasonName->SetText(getString(season->Name).c_str());
			Countdown->SetText(StringUtil::Format(getString(LanguageKey::GUI_SEASON_RANK_COUNTDOWN).c_str(), endTime).c_str());
			MyRank->SetText(rank > 0 ? StringUtil::ToString(rank).c_str() : "-");
			MyHonor->SetText(StringUtil::ToString(honor).c_str());

			MyRewardInfo->CleanupChildren();
			list<int>::type rewardIds;
			Season* next_season = SeasonSetting::findSeasonByHonorId(NextHonorId);
			if (next_season != nullptr)
			{
				rewardIds = honor >= next_season->Score ? season->UpRewardIds : season->NormalRewardIds;
				MyReward->SetVisible(honor >= next_season->Score);
				if (honor >= next_season->Score)
				{
					auto rank_reward = SeasonSetting::findSeasonRankRewardBySeasonIdAndRank(season->Id, rank);
					if (rank_reward)
					{
						auto reward = RewardSetting::findRewardById(rank_reward->RewardId);
						if (reward)
						{
							MyReward->SetVisible(true);
							MyReward->SetImage(reward->Image.c_str());
							MyRewardCount->SetText(StringUtil::Format("x%d", reward->Count).c_str());
						}
						else 
						{
							MyReward->SetVisible(false);
						}
					}
					else
					{
						MyReward->SetVisible(false);
					}
				}
			}
			else
			{
				rewardIds = season->NormalRewardIds;
			}

			static unsigned MyRewardIndex = 0;
			float ItemWidth = 75.0f;
			float ItemHeight = 40.0f;
			float PositionX = 0;
			for (auto rewardId : rewardIds)
			{
				auto reward = RewardSetting::findRewardById(rewardId);
				if (reward != nullptr)
				{
					MyRewardIndex++;
					GUIString ChildName = StringUtil::Format("BedWarSeasonRank-My-Reward-Item-%d", MyRewardIndex).c_str();
					GUIBedWarSeasonRewardItem* RewardItem = (GUIBedWarSeasonRewardItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BEDWAR_SEASON_REWARD_ITEM, ChildName);
					RewardItem->SetArea({ 0.0f, PositionX }, { 0.0f, 1.0f }, { 0.0f, ItemWidth }, { 0.0f, ItemHeight });
					RewardItem->SetSeasonReward(reward, false);
					MyRewardInfo->AddChildWindow(RewardItem);
					PositionX += ItemWidth;
				}
			}
		}
		else
		{
			LeftRank->InitUI(SeasonHonorId::SEASON_NONE);
			RightRank->InitUI(SeasonHonorId::SEASON_NONE);
			SeasonName->SetText("");
			Countdown->SetText("");
			MyRank->SetText("");
			MyHonor->SetText("");
			MyReward->SetVisible(false);
		}
		EntityPlayerSPMP* pPlayer = Blockman::Instance()->m_pPlayer;
		MyName->SetText(pPlayer->getCommandSenderName().c_str());
		return true;
	}

	bool gui_bedwarSeasonRank::onUpdateSeasonRankData(int type, const String & data)
	{
		SeasonSetting::parseSeasonRankData(type, data);
		if (isShown())
		{
			onShow();
		}
		return true;
	}

}
