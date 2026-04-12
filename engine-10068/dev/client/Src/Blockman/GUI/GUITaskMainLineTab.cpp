#include "GUITaskMainLineTab.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIButton.h"
#include "Setting/TaskSystemSetting.h"
#include "Util/ClientEvents.h"
#include "UI/GUIStaticImage.h"
#include "Audio/SoundSystem.h"

namespace BLOCKMAN
{
	
	GUITaskMainLineTab::GUITaskMainLineTab(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("TaskMainLineTab.json"));
		renameComponents(m_window);

		AddChildWindow(m_window);
	}

	void GUITaskMainLineTab::removeComponents()
	{
		if (m_window)
		{
			RemoveChildWindow(m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUITaskMainLineTab::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUITaskMainLineTab::InitMainLineTab(TaskGroup* group)
	{
		auto name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(4));
		auto button = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(5));
		
		m_tab_id = group->Id;
		name->SetText(StringUtil::ToString(group->Lv).c_str());
		UpdateRedPoint();
		
		button->subscribeEvent(EventButtonClick, std::bind(&GUITaskMainLineTab::OnButtonClick, this, std::placeholders::_1));
	}

	void GUITaskMainLineTab::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	bool GUITaskMainLineTab::OnButtonClick(const EventArgs & events)
	{
		auto lock = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(2));
		if (lock->IsVisible())
		{
			TaskGroup* group = TaskSystemSetting::findTaskGroupByGroupId(m_tab_id);
			if (!group)
			{
				return false;
			}
			ShowMainLineTaskLvNotEnoughEvent::emit(group->Lv);
			return false;
		}
		SoundSystem::Instance()->playEffectByType(ST_Click);
		auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
		if (IsTabSelected())
		{
			return false;
		}
		SetSelectTab(true);
		return true;
	}

	void GUITaskMainLineTab::SetSelectTab(bool selected)
	{
		auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
		border->SetChecked(selected);
		if (selected)
		{
			TaskMainLineTabChangeEvent::emit(m_tab_id);
		}
	}

	bool GUITaskMainLineTab::IsTabSelected()
	{
		auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
		return border->GetChecked();
	}

	void GUITaskMainLineTab::UnLockTab()
	{
		auto lock = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(2));
		if (!(lock->IsVisible()))
		{
			return;
		}
		auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
		border->SetVisible(true);
		lock->SetVisible(false);
		TaskGroup* group = TaskSystemSetting::findTaskGroupByGroupId(m_tab_id);
		if (!group)
		{
			return;
		}
		for (auto task : group->Tasks)
		{
			if (task->Status == TASK_STATUS_LOCK)
			{
				task->Status = TASK_STATUS_HAVE_GOT;
			}
		}
	}

	void GUITaskMainLineTab::UpdateRedPoint()
	{
		TaskGroup* group = TaskSystemSetting::findTaskGroupByGroupId(m_tab_id);
		if (!group)
		{
			return;
		}
		auto red_point = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		auto all_done = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));
		for (auto task : group->Tasks)
		{
			if (task->Status == TASK_STATUS_NOT_GET)
			{
				red_point->SetVisible(true);
				m_has_new = true;
				return;
			}
		}
		red_point->SetVisible(false);
		m_has_new = false;

		if (all_done->IsVisible())
		{
			return;
		}
		int task_index = 0;
		for (auto task : group->Tasks)
		{
			if (task->Status == TASK_STATUS_HAVE_GOT)
			{
				task_index++;
			}
		}
		all_done->SetVisible(task_index == group->Tasks.size());
	}
}
