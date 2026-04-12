#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "Setting/RewardSetting.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GUITaskInfoItem.h"
#include "GUI/GuiDef.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Network/ClientNetwork.h"
#include "GUITaskRewardItem.h"
#include "Audio/SoundSystem.h"

namespace BLOCKMAN
{
	GUITaskInfoItem::GUITaskInfoItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("TaskInfoItem.json"));
		renameComponents(m_window);

		AddChildWindow(m_window);
	}

	void GUITaskInfoItem::removeComponents()
	{
		if (m_window)
		{
			RemoveChildWindow(m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUITaskInfoItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUITaskInfoItem::InitTask(Task* task)
	{
		auto status_layout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(6));
		auto button = dynamic_cast<GUIButton*>(status_layout->GetChildByIndex(2));
		button->subscribeEvent(EventButtonClick, std::bind(&GUITaskInfoItem::OnButtonClick, this, std::placeholders::_1));
		auto reward = m_window->GetChildByIndex(2);
		reward->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SEASON_RANK_REWARD).c_str());
		InitTaskItem(task);
	}

	void GUITaskInfoItem::InitTaskItem(Task* task)
	{
		m_data = task;

		auto icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		auto action = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(4));
		auto action_value = dynamic_cast<GUIStaticText*>(action->GetChildByIndex(1));
		auto title = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(5));
		auto status_layout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(6));
		auto task_value = dynamic_cast<GUIStaticText*>(status_layout->GetChildByIndex(0));
		auto button = dynamic_cast<GUIButton*>(status_layout->GetChildByIndex(2));

		SetRewards(task->RewardIds);
		icon->SetImage(task->Icon.c_str());
		action_value->SetText(StringUtil::Format("x%d", task->ActiveValue).c_str());
		action->SetVisible(task->ActiveValue > 0);
		title->SetText(LanguageManager::Instance()->getString(task->Title).c_str());
		if (task->Current >= task->GoalNum)
			task_value->SetText(StringUtil::Format("▢FF23F014%d/%d", task->GoalNum, task->GoalNum).c_str());
		else
			task_value->SetText(StringUtil::Format("▢FFFF0000%d▢FF23F014/%d", task->Current, task->GoalNum).c_str());
		task_value->SetVisible(task->GoalNum != 0);
		SetTaskStatus();
	}

	void GUITaskInfoItem::SetTaskStatus()
	{
		auto backgroud = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		auto status_layout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(6));
		auto status_text = dynamic_cast<GUIStaticText*>(status_layout->GetChildByIndex(1));
		auto button = dynamic_cast<GUIButton*>(status_layout->GetChildByIndex(2));
		
		switch (m_data->Status)
		{
		case BLOCKMAN::TASK_STATUS_LOCK:
			backgroud->SetImage("set:pixel_gun_task.json image:bg_task_finished");
			button->SetNormalImage("set:pixel_gun_task.json image:btn_finished");
			button->SetText(LanguageManager::Instance()->getString("task_finished").c_str());
			button->SetTouchable(false);
			break;
		case BLOCKMAN::TASK_STATUS_HAVE_GOT:
			backgroud->SetImage("set:pixel_gun_task.json image:bg_task_finished");
			button->SetNormalImage("set:pixel_gun_task.json image:btn_finished");
			button->SetText(LanguageManager::Instance()->getString("task_finished").c_str());
			button->SetTouchable(false);
			break;
		case BLOCKMAN::TASK_STATUS_UNDONE:
			backgroud->SetImage("set:pixel_gun_task.json image:bg_task_processing");
			if (m_data->JumpTo != SKIP_NONE)
			{
				button->SetVisible(true);
				button->SetNormalImage("set:season_rank.json image:btn_yellow");
				button->SetPushedImage("set:season_rank.json image:btn_yellow");
				button->SetText(LanguageManager::Instance()->getString("task_skip").c_str());
				button->SetTouchable(true);
			}
			else
			{
				button->SetVisible(false);
				status_text->SetText(LanguageManager::Instance()->getString("task_processing").c_str());
			}
			break;
		case BLOCKMAN::TASK_STATUS_NOT_GET:
			backgroud->SetImage("set:pixel_gun_task.json image:bg_task_receive");
			button->SetNormalImage("set:chest_lottery.json image:btn_green");
			button->SetPushedImage("set:chest_lottery.json image:btn_green");
			button->SetText(LanguageManager::Instance()->getString("task_receive").c_str());
			button->SetTouchable(true);
			button->SetVisible(true);
			break;
		default:
			break;
		}
	}

	void GUITaskInfoItem::SetRewards(String rewardIds)
	{
		list<Reward*>::type rewards = RewardSetting::findRewardByIds(rewardIds);
		if (rewards.empty())
		{
			return;
		}

		auto rewardLayout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(3));
		static unsigned reward_index = 0;
		int site_index = 0;
		rewardLayout->CleanupChildren();
		for (auto reward : rewards)
		{
			reward_index++;
			site_index++;
 			GUIString RewardName = StringUtil::Format("Task-Reward-%d", reward_index).c_str();
			GUITaskRewardItem* pReward = (GUITaskRewardItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_TASK_REWARD_ITEM, RewardName);
			
			pReward->SetSize({ {0.0f, 110.0f}, {1.0f, 0.0f} });
			pReward->SetPosition({ {0.0f, 4.0f * site_index + (site_index - 1) * 110.0f}, {0.0f, 0.0f} });
			rewardLayout->AddChildWindow(pReward);
			pReward->InitReward(reward);
		} 
	}

	void GUITaskInfoItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	bool GUITaskInfoItem::OnButtonClick(const EventArgs & events)
	{
		SoundSystem::Instance()->playEffectByType(ST_Click);
		if (m_data->Status == TASK_STATUS_NOT_GET)
		{
			m_data->Status = TASK_STATUS_HAVE_GOT;
			m_data->HasChange = true;
			SetTaskStatus();
			CheckDailyRedPointEvent::emit();
			CheckMainLineRedPointEvent::emit();
			ShowPixelGunTaskIconEvent::emit();
			ClientNetwork::Instance()->getCommonSender()->sendPixelGunReceiveTaskReward(m_data->Id);
			if (m_data->ShowAd)
				ShowRewardsAdBtnEvent::emit(StringUtil::ToString(m_data->Id));
			ShowRewardsEvent::emit(m_data->RewardIds);
			return true;
		}
		OnSkipClick();
		return true;
	}	

	void GUITaskInfoItem::OnSkipClick()
	{
		switch (m_data->JumpTo)
		{
		case BLOCKMAN::SKIP_FIGHT:
			ShowPixelGunModeSelectEvent::emit();
			break;
		case BLOCKMAN::SKIP_GUNSTORE:
			OpenGunStoreEvent::emit();
			break;
		case BLOCKMAN::SKIP_ARMOR:
			ShowArmorUpgradeEvent::emit();
			break;
		case BLOCKMAN::SKIP_LOTTERY:
			OpenChestLotteryEvent::emit();
			break;
		default:
			break;
		}
	}
}
