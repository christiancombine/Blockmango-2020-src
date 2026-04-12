#include "UI/GUIWindowManager.h"
#include "GUI/GUISkyBlockTaskInfoItem.h"
#include "GUI/GuiDef.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Network/ClientNetwork.h"
#include "GUISkyBlockTaskRewardItem.h"
#include "Audio/SoundSystem.h"
#include "UI/GUIProgressBar.h"
#include "Util/ClientEvents.h"
#include "UI/GUILayout.h"
#include "game.h"
#include "ShellInterface.h"
#include "Item/Item.h"
#include "GameAnalytics/GameAnalyticsSetting.h"

namespace BLOCKMAN
{
	GUISkyBlockTaskInfoItem::GUISkyBlockTaskInfoItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("SkyBlockTaskInfoItem.json"));
		renameComponents(m_window);

		AddChildWindow(m_window);

		m_subscriptionGuard.add(SkyBlockAchieveMaxDareTaskEvent::subscribe(std::bind(&GUISkyBlockTaskInfoItem::onSetMaxTaskTimes, this, std::placeholders::_1)));
	}

	void GUISkyBlockTaskInfoItem::removeComponents()
	{
		if (m_window)
		{
			RemoveChildWindow(m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUISkyBlockTaskInfoItem::Destroy()
	{
		m_subscriptionGuard.unsubscribeAll();
		removeComponents();
		BaseClass::Destroy();
	}

	void GUISkyBlockTaskInfoItem::InitTask(const SkyBlockTask task)
	{
		auto status_layout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(6));
		auto button = dynamic_cast<GUIButton*>(status_layout->GetChildByIndex(3));
		button->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockTaskInfoItem::OnButtonClick, this, std::placeholders::_1));
		auto reward = m_window->GetChildByIndex(3);
		reward->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SEASON_RANK_REWARD).c_str());
		InitTaskItem(task, m_have_cube_ad);
	}

	void GUISkyBlockTaskInfoItem::InitTaskItem(const SkyBlockTask task, bool have_cube_ad)
	{
		m_data = task;

		auto icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		auto title = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(5));
		auto status_layout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(6));
		auto task_bar = dynamic_cast<GUIProgressBar*>(status_layout->GetChildByIndex(0));
		auto task_value = dynamic_cast<GUIStaticText*>(status_layout->GetChildByIndex(1));
		auto task_precen = dynamic_cast<GUIStaticText*>(status_layout->GetChildByIndex(4));
		m_have_cube_ad = have_cube_ad;

		SetRewards(task.RewardIds);
		icon->SetImage(task.Icon.c_str());

		String taskTitle = "";
		String name = "";
		Item* pItem3 = Item::getItemById(task.Remarks);
		if (pItem3)
		{
			ItemStackPtr itemStack = LORD::make_shared<ItemStack>(task.Remarks, 1, task.RemarksMeta);
			name = LanguageManager::Instance()->getItemName(pItem3->getUnlocalizedName(itemStack).c_str()).c_str();
		}

		switch (task.TaskTypeId)
		{
		case SkyBlockTaskDetailType::SKYBLOCK_COLLECT:
		case SkyBlockTaskDetailType::SKYBLOCK_PLANTE_TREE:
		case SkyBlockTaskDetailType::SKYBLOCK_CREATE:
			taskTitle = StringUtil::Format(LanguageManager::Instance()->getString(task.Title).c_str(), task.GoalNum, name.c_str()).c_str();
			break;
		case SkyBlockTaskDetailType::SKYBLOCK_STONE_MACHINE:
		case SkyBlockTaskDetailType::SKYBLOCK_EXPAND_ISLANDS:
		case SkyBlockTaskDetailType::SKYBLOCK_UNLIMITE_WATER:
		case SkyBlockTaskDetailType::SKYBLOCK_LOOK_AD:
		case SkyBlockTaskDetailType::SKYBLOCK_HAVE_PRODUCTS:
			taskTitle = LanguageManager::Instance()->getString(task.Title).c_str();
			break;
		case SkyBlockTaskDetailType::SKYBLOCK_KILL_MONSTER:
		case SkyBlockTaskDetailType::SKYBLOCK_KILL_PLAYER:
		case SkyBlockTaskDetailType::SKYBLOCK_PRODUCTS_LEVEL:
		case SkyBlockTaskDetailType::SKYBLOCK_UP_PRODUCTS:
			taskTitle = StringUtil::Format(LanguageManager::Instance()->getString(task.Title).c_str(), task.GoalNum).c_str();
			break;
		}

		title->SetText(taskTitle.c_str());



		if (task.Current >= task.GoalNum || task.Status == SKYBLOCK_TASK_STATUS_HAVE_GOT)
		{
			task_value->SetText(StringUtil::Format("%d/%d", task.GoalNum, task.GoalNum).c_str());
			task_precen->SetText(StringUtil::Format("%d%%",100).c_str());
			task_bar->SetProgress(1.f);
		}
		else
		{
			task_value->SetText(StringUtil::Format("%d/%d", task.Current, task.GoalNum).c_str());
			task_precen->SetText(StringUtil::Format("%d%%", task.Current * 100 / task.GoalNum).c_str());
			task_bar->SetProgress(task.Current * 1.0f  / task.GoalNum);
		}
		task_value->SetVisible(task.GoalNum != 0);
		SetTaskStatus();
		SetTaskQualityImg();
	}

	void GUISkyBlockTaskInfoItem::SetTaskQualityImg()
	{
		auto task_bg = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		switch (m_data.Quality)
		{
		case BLOCKMAN::SKYBLOCK_DARE_TASK_LEVEL_1:
			task_bg->SetImage("");
			break;
		case BLOCKMAN::SKYBLOCK_DARE_TASK_LEVEL_2:
			task_bg->SetImage("set:skyblock_task.json image:gree_task");
			break;
		case BLOCKMAN::SKYBLOCK_DARE_TASK_LEVEL_3:
			task_bg->SetImage("set:skyblock_task.json image:purper_task");
			break;
		case BLOCKMAN::SKYBLOCK_DARE_TASK_LEVEL_4:
			task_bg->SetImage("set:skyblock_task.json image:yellow_task");
			break;
		default:
			break;
		}
	}

	void GUISkyBlockTaskInfoItem::SetTaskStatus()
	{
		auto status_layout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(6));
		auto status_text = dynamic_cast<GUIStaticText*>(status_layout->GetChildByIndex(2));
		auto button = dynamic_cast<GUIButton*>(status_layout->GetChildByIndex(3));

		switch (m_data.Status)
		{
		case BLOCKMAN::SKYBLOCK_TASK_STATUS_LOCK:
			button->SetText(getString(LanguageKey::GUI_TASK_FINISHED));
			button->SetTouchable(false);
			break;
		case BLOCKMAN::SKYBLOCK_TASK_STATUS_HAVE_GOT:
			status_text->SetText(getString(LanguageKey::GUI_TASK_FINISHED));
			button->SetTouchable(false);
			button->SetVisible(false);
			status_text->SetVisible(true);
			break;
		case BLOCKMAN::SKYBLOCK_TASK_STATUS_UNDONE:
			button->SetVisible(false);
			status_text->SetVisible(false);
			break;
		case BLOCKMAN::SKYBLOCK_TASK_STATUS_NOT_GET:
			button->SetText(getString(LanguageKey::GUI_TASK_RECEIVE));
			button->SetTouchable(true);
			button->SetVisible(true);
			status_text->SetVisible(false);
			break;
		case BLOCKMAN::SKYBLOCK_TASK_STATUS_CAN_COMMIT:
			button->SetText(getString(LanguageKey::GUI_TASK_COMMIT));
			button->SetTouchable(true);
			button->SetVisible(true);
			status_text->SetVisible(false);
			break;
		case BLOCKMAN::SKYBLOCK_TASK_STATUS_AD:
			button->SetText(getString(LanguageKey::GUI_TASK_AD));
			button->SetTouchable(true);
			if (m_have_cube_ad)
			{
				button->SetVisible(true);
			}
			else
			{
				button->SetVisible(false);
			}
			
			status_text->SetVisible(false);
			break;
		default:
			break;
		}
	}

	void GUISkyBlockTaskInfoItem::SetRewards(String rewardIds)
	{
		list<SkyBlockTaskReward>::type rewards;		 
		if (!SkyBlockTaskSystemSetting::Instance()->findTaskRewardByIds(rewardIds, rewards))
		{
			return;
		}
		if (rewards.empty())
		{
			return ;
		}
		auto rewardLayout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(4));
		static unsigned reward_index = 0;
		int site_index = 0;
		rewardLayout->CleanupChildren();
		auto wide = 75.f;
		for (auto reward : rewards)
		{
			reward_index++;
			site_index++;
			GUIString RewardName = StringUtil::Format("Task-Reward-%d", reward_index).c_str();
			GUISkyBlockTaskRewardItem* pReward = (GUISkyBlockTaskRewardItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SKYBLOCK_TASK_REWARD_ITEM, RewardName);

			pReward->SetArea(UDim(0, wide * (site_index - 1)), UDim(0, 3.0f), UDim(0, wide), UDim(0.98f, 0));
			rewardLayout->AddChildWindow(pReward);
			pReward->InitReward(reward);
		}
	}

	void GUISkyBlockTaskInfoItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	bool GUISkyBlockTaskInfoItem::OnButtonClick(const EventArgs & events)
	{
		SoundSystem::Instance()->playEffectByType(ST_Click);

		if (m_data.TaskSort == SKYBLOCK_TASK_TYPE_DRAE && m_isMaxTaskTimes)
		{
			ShowResultTipDialogEvent::emit(ResultCode::COMMON, getString(LanguageKey::GUI_TODAY_REACH_MAX_TASK_TIMES).c_str());
		}
		else
		{
			if (m_data.Status == SKYBLOCK_TASK_STATUS_NOT_GET || m_data.Status == SKYBLOCK_TASK_STATUS_CAN_COMMIT)
			{
				m_data.Status = SKYBLOCK_TASK_STATUS_HAVE_GOT;
				m_data.HasChange = true;
				SetTaskStatus();
				SkyBlockCheckMainLineRedPointEvent::emit();
				ShowSkyBlockTaskIconEvent::emit();

				ClientNetwork::Instance()->getCommonSender()->sendSkyBlockReceiveTaskReward(m_data.Id, m_data.Status);
				return true;
			}

			if (m_data.Status == SKYBLOCK_TASK_STATUS_AD)
			{
				String taskAd = "";
				taskAd = StringUtil::Format("TaskAd#%d", m_data.Id);
				GameClient::CGame::Instance()->getShellInterface()->onWatchAd(104801, taskAd);
				if (GameClient::CGame::Instance()->isEnableIndie())
					GameAnalyticsSetting::Instance()->design(0, { "g1048task_click_lookAd_indie", StringUtil::ToString(m_data.Id) });
				else
					GameAnalyticsSetting::Instance()->design(0, { "g1048task_click_lookAd", StringUtil::ToString(m_data.Id) });
				return true;
			}
		}

		return true;
	}

	bool GUISkyBlockTaskInfoItem::onSetMaxTaskTimes(bool isMaxTaskTimes)
	{
		m_isMaxTaskTimes = isMaxTaskTimes;
		return true;
	}
}
