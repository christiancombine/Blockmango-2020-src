#include "gui_rewardResult.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Setting/ChestLotterySetting.h"
#include "Setting/RewardSetting.h"
#include "Network/ClientNetwork.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "GUI/GuiDef.h"
#include "Setting/TaskSystemSetting.h"
#include "ShellInterface.h"
#include "game.h"

namespace BLOCKMAN
{
	gui_rewardResult::gui_rewardResult() :
		gui_layout("ChestRewardResult.json")
	{
	}

	gui_rewardResult::~gui_rewardResult()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_rewardResult::onLoad()
	{
		GUILayout* ChestRewardResult = getWindow<GUILayout>("ChestRewardResult");
		ChestRewardResult->subscribeEvent(EventWindowTouchUp, std::bind(&gui_rewardResult::onContentClick, this, std::placeholders::_1));

		m_chest = getWindow<GuiActorWindow>("ChestRewardResult-Chest");
		m_content = getWindow<GUILayout>("ChestRewardResult-Content");

		auto Title = getWindow<GUIStaticText>("ChestRewardResult-Title");
		Title->SetText(getString(LanguageKey::GUI_CHEST_REWARD_RESULT_TITLE));

		auto Continue = getWindow<GUIButton>("ChestRewardResult-Continue");
		Continue->SetText(getString(LanguageKey::GUI_BTN_CLICK_CONTINUE));
		Continue->subscribeEvent(EventButtonClick, std::bind(&gui_rewardResult::onContinueClick, this, std::placeholders::_1));

		auto Ad = getWindow<GUIButton>("ChestRewardResult-Ad");
		Ad->SetText(LanguageManager::Instance()->getString("gui_pixel_double_reward").c_str());
		Ad->subscribeEvent(EventButtonClick, std::bind(&gui_rewardResult::onAdClick, this, std::placeholders::_1));

		m_image = getWindow<GuiEffectViewOfObtainment>("ChestRewardResult-Image");
		m_name = getWindow<GUIStaticText>("ChestRewardResult-Name");

		m_subscriptionGuard.add(ShowChestRewardResultEvent::subscribe(std::bind(&gui_rewardResult::onShowChestRewardResult, this, std::placeholders::_1)));
		m_subscriptionGuard.add(OpenLotteryChestEvent::subscribe(std::bind(&gui_rewardResult::onOpenLotteryChest, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowRewardsEvent::subscribe(std::bind(&gui_rewardResult::onShowRewards, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ReceiveTaskActiveRewardEvent::subscribe(std::bind(&gui_rewardResult::onReceiveTaskActiveReward, this, std::placeholders::_1)));
		m_subscriptionGuard.add(HideTaskActiveRewardEvent::subscribe(std::bind(&gui_rewardResult::onHideTaskActiveReward, this)));
		m_subscriptionGuard.add(VideoAdVisibleEvent::subscribe(std::bind(&gui_rewardResult::onVideoAdVisible, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowRewardsAdBtnEvent::subscribe(std::bind(&gui_rewardResult::showAdBtn, this, std::placeholders::_1)));
	}

	void gui_rewardResult::onUpdate(ui32 nTimeElapse)
	{
		if (m_wait_open_time > 0)
		{
			m_wait_open_time -= nTimeElapse;
			if (m_wait_open_time <= 0)
			{
				m_wait_open_time = 0;
				m_content->SetVisible(true);
				m_chest->SetVisible(false);
				playSoundByType(ST_GetCoin);
				hideChestActor();

				m_can_close_time = 1200;
				m_can_close = false;
			}
		}

		if (m_can_close_time > 0)
		{
			m_can_close_time -= nTimeElapse;
			if (m_can_close_time <= 0)
			{
				m_can_close = true;
			}
		}
	}

	bool gui_rewardResult::isNeedLoad()
	{
		return checkGameType(ClientGameType::PixelGunHall);
	}

	void gui_rewardResult::onGetCommonData(String key, String data)
	{
		if (key == "RewardResult")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int rewardId = builder.getIntParam("rewardId");
			onShowChestRewardResult(rewardId);
		}
		if (key == "RewardResults")
		{
			auto builder = CommonDataBuilder().fromData(data);
			String rewardIds = builder.getStringParam("rewardIds");
			onShowRewards(rewardIds);
		}
	}

	bool gui_rewardResult::onOpenLotteryChest(int chestId)
	{
		LotteryChest* chest = ChestLotterySetting::findLotteryChestById(chestId);
		if (!chest)
			return false;
		ClientNetwork::Instance()->getCommonSender()->sendPixelGunOpenLotteryChest(chestId);
		if (chest->Actor.length() <= 7)
			return false;
		m_content->SetVisible(false);
		m_chest->SetVisible(true);
		m_chest->SetActor(chest->Actor.c_str(), "idle");
		return true;
	}

	bool gui_rewardResult::onShowChestRewardResult(int id)
	{
		auto reward = RewardSetting::findRewardById(id);
		if (reward == nullptr)
		{
			getParent()->hideChestRewardResult();
			return false;
		}

		if (m_chest->IsVisible())
		{
			m_wait_open_time = 2280;
			m_chest->PlaySkill("open");
		}
		else
		{
			playSoundByType(ST_GetCoin);
			m_content->SetVisible(true);
			m_can_close_time = 1200;
			m_can_close = false;
		}

		m_image->SetVisible(true);
		m_name->SetVisible(true);
		auto Rewards = getWindow("ChestRewardResult-Rewards");
		Rewards->SetVisible(false);

		m_image->SetImage(reward->Image.c_str());
		m_name->SetText(getString(reward->Name));
		getParent()->showChestRewardResult();
		return true;
	}

	bool gui_rewardResult::onContentClick(const EventArgs & events)
	{
		if (m_wait_open_time > 0)
			return false;

		if (m_can_close == false)
		{
			return false;
		}
		getParent()->hideChestRewardResult();
		hideAdBtn();
		return true;
	}

	bool gui_rewardResult::onContinueClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		getParent()->hideChestRewardResult();
		hideAdBtn();
		return true;
	}

	bool gui_rewardResult::onShowRewards(const String & rewards)
	{
		list<Reward*>::type rewardsList = RewardSetting::findRewardByIds(rewards);

		if (rewardsList.empty())
		{
			return false;
		}

		if (m_chest->IsVisible())
		{
			m_wait_open_time = 2280;
			if (m_cur_active_id == 0)
			{
				m_chest->PlaySkill("open");
			}
		}
		else
		{
			playSoundByType(ST_GetCoin);
			m_content->SetVisible(true);
		}

		m_can_close_time = 1200;
		m_can_close = false;

		auto Rewards = getWindow("ChestRewardResult-Rewards");
		Rewards->CleanupChildren();
		Rewards->SetVisible(true);

		float Size = Rewards->GetPixelSize().x;
		float Width = 60.0f;
		float Space = 30.0f;
		float PosX = (Size - (Width + Space) * rewardsList.size() + Space) / 2;

		static unsigned int reward_index = 0;
		for (auto reward : rewardsList)
		{
			reward_index++;
			GUIString IconName = StringUtil::Format("ChestRewardResult-Rewards-Icon-%d", reward_index).c_str();
			GuiEffectViewOfObtainment* Icon = (GuiEffectViewOfObtainment*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_EFFECT_VIEW_OF_OBTAINMENT, IconName);
			Icon->SetSize({ {0.0f, Width}, {0.0f, Width} });
			Icon->SetXPosition({ 0.0f, PosX });
			Icon->SetImage(reward->Image.c_str());

			GUIString TextName = StringUtil::Format("ChestRewardResult-Rewards-Text-%d", reward_index).c_str();
			GUIStaticText* Text = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, TextName);
			Text->SetSize({ {1.0f, 0.0f}, {0.0f, 30.0f} });
			Text->SetYPosition({ 1.0f, 30.0f });
			Text->SetTextHorzAlign(HA_CENTRE);
			Text->SetTextVertAlign(VA_CENTRE);
			Text->SetTextBoader(true);
			Text->SetTextScale(1.2f);
			if (reward->CountText != "#")
			{
				Text->SetText((LanguageManager::Instance()->getString(reward->CountText)).c_str());
			}
			else
			{
				Text->SetText(StringUtil::ToString(reward->Count).c_str());
			}

			Icon->AddChildWindow(Text);
			Rewards->AddChildWindow(Icon);

			PosX += (Width + Space);
		}
		m_image->SetVisible(false);
		m_name->SetVisible(false);
		getParent()->showChestRewardResult();
		return true;
	}

	bool gui_rewardResult::showAdBtn(String reward)
	{
		if (!m_have_ad || !GameClient::CGame::Instance()->isEnableIndie())
			return false;
		m_ad_rewards = reward;
		auto Continue = getWindow<GUIButton>("ChestRewardResult-Continue");
		auto Ad = getWindow<GUIButton>("ChestRewardResult-Ad");
		Continue->SetPosition({ {-0.2f, 0.f}, {0.82f, 0.f} });
		Ad->SetPosition({ {0.2f, 0.f}, {0.82f, 0.f} });
		Ad->SetVisible(true);
		return false;
	}

	bool gui_rewardResult::onReceiveTaskActiveReward(int activeId)
	{
		if (m_cur_active_id != 0)
		{
			return false;
		}
		TaskActive* active = TaskSystemSetting::findTaskActiveById(activeId);
		if (active == nullptr)
		{
			return false;
		}
		m_cur_active_id = activeId;
		m_content->SetVisible(false);
		m_chest->SetVisible(true);
		m_chest->SetActor(active->Actor.c_str(), "idle");
		m_chest->SetCustomColor(Color::TRANS);
		m_chest->PlaySkill("chest_open_full");
		m_wait_open_time = 1500;
		onShowRewards(active->RewardIds);
		getParent()->showChestRewardResult();
		CheckDailyRedPointEvent::emit();
		ShowPixelGunTaskIconEvent::emit();
		return true;
	}

	void gui_rewardResult::hideChestActor()
	{
		ClientNetwork::Instance()->getCommonSender()->sendPixelGunReceiveTaskActiveReward(m_cur_active_id);
		m_content->SetVisible(true);
		m_chest->SetVisible(false);
		m_cur_active_id = 0;
	}

	bool gui_rewardResult::onVideoAdVisible(bool visible)
	{
		m_have_ad = visible;
		return false;
	}

	bool gui_rewardResult::onAdClick(const EventArgs & events)
	{
		GameClient::CGame::Instance()->getShellInterface()->onWatchAd((int)AdType::PIXEL_GUN_TASK, m_ad_rewards);
		m_ad_rewards = "";
		hideAdBtn();
		return false;
	}

	void gui_rewardResult::hideAdBtn()
	{
		auto Continue = getWindow<GUIButton>("ChestRewardResult-Continue");
		auto Ad = getWindow<GUIButton>("ChestRewardResult-Ad");
		Ad->SetVisible(false);
		Continue->SetPosition({ {0.f, 0.f}, {0.82f, 0.f} });
	}
}
