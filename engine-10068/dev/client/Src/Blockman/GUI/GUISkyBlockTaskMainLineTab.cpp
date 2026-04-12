#include "GUISkyBlockTaskMainLineTab.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIButton.h"
#include "Setting/SkyBlockTaskSystemSetting.h"
#include "Util/ClientEvents.h"
#include "UI/GUIStaticImage.h"
#include "Util/LanguageKey.h"

namespace BLOCKMAN
{

	GUISkyBlockTaskMainLineTab::GUISkyBlockTaskMainLineTab(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("SkyBlockTaskMainLineTab.json"));
		renameComponents(m_window);

		AddChildWindow(m_window);
	}

	void GUISkyBlockTaskMainLineTab::removeComponents()
	{
		if (m_window)
		{
			RemoveChildWindow(m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUISkyBlockTaskMainLineTab::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUISkyBlockTaskMainLineTab::InitMainLineTab(const SkyBlockTaskGroup group)
	{
		auto name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(4));
		auto button = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(5));

		m_tab_lv = group.Lv;
		switch (group.Lv)
		{
		case 0:
			name->SetText(getString(LanguageKey::GUI_TASK_LEVEL_0));
			break;
		case 1:
			name->SetText(getString(LanguageKey::GUI_TASK_LEVEL_1));
			break;
		case 2:
			name->SetText(getString(LanguageKey::GUI_TASK_LEVEL_2));
			break;
		case 3:
			name->SetText(getString(LanguageKey::GUI_TASK_LEVEL_3));
			break;
		case 4:
			name->SetText(getString(LanguageKey::GUI_TASK_LEVEL_4));
			break;
		case 5:
			name->SetText(getString(LanguageKey::GUI_TASK_LEVEL_5));
			break;
		default:
			break;
		}
		UpdateRedPoint();

		button->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockTaskMainLineTab::OnButtonClick, this, std::placeholders::_1));
	}

	void GUISkyBlockTaskMainLineTab::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	bool GUISkyBlockTaskMainLineTab::OnButtonClick(const EventArgs & events)
	{
		auto lock = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(2));
		if (lock->IsVisible())
		{
			SkyBlockTaskGroup group;
			if (!SkyBlockTaskSystemSetting::Instance()->findTaskGroupByLevel(m_tab_lv, group))
			{
				return false;
			}

			String m_tipMessage = "";
			if (group.Lv > m_lv)
				m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_MAINLINE_NOT_REACH_LEVEL).c_str(), m_lv);
			else if(group.Lv > m_progressesLv)
			{
				m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_MAINLINE_NOT_COMPLETE_CUR).c_str());
			}

			ShowSkyBlockMainLineTaskLvNotEnoughEvent::emit(m_tipMessage);
			return false;
		}
		auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
		if (IsTabSelected())
		{
			return false;
		}
		SetSelectTab(true);
		return true;
	}

	void GUISkyBlockTaskMainLineTab::SetSelectTab(bool selected)
	{
		auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
		border->SetChecked(selected);
		if (selected)
		{
			SkyBlockTaskMainLineTabChangeEvent::emit(m_tab_lv);
		}
	}

	bool GUISkyBlockTaskMainLineTab::IsTabSelected()
	{
		auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
		return border->GetChecked();
	}

	void GUISkyBlockTaskMainLineTab::UnLockTab()
	{
		auto lock = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(2));
		if (!(lock->IsVisible()))
		{
			return;
		}
		auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
		border->SetVisible(true);
		lock->SetVisible(false);
		SkyBlockTaskGroup group;;
		if (!SkyBlockTaskSystemSetting::Instance()->findTaskGroupByLevel(m_tab_lv, group))
		{
			return;
		}
		for (auto task : group.Tasks)
		{
			if (task.Status == SKYBLOCK_TASK_STATUS_LOCK)
			{
				task.Status = SKYBLOCK_TASK_STATUS_UNDONE;
			}
		}
	}

	void GUISkyBlockTaskMainLineTab::UpdateRedPoint()
	{
		SkyBlockTaskGroup group;
		if (!SkyBlockTaskSystemSetting::Instance()->findTaskGroupByLevel(m_tab_lv, group))
		{
			return;
		}
		auto red_point = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		auto all_done = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));
		for (auto task : group.Tasks)
		{
			if (task.Status == SKYBLOCK_TASK_STATUS_NOT_GET || task.Status == SKYBLOCK_TASK_STATUS_CAN_COMMIT)
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
		for (auto task : group.Tasks)
		{
			if (task.Status == SKYBLOCK_TASK_STATUS_HAVE_GOT)
			{
				task_index++;
			} 
		}
		all_done->SetVisible(task_index == group.Tasks.size());
	}

	void GUISkyBlockTaskMainLineTab::UpdateDatra(int lv, int progressesLv)
	{
		m_lv = lv;
		m_progressesLv = progressesLv;
	}

	bool GUISkyBlockTaskMainLineTab::getTabIsLock()
	{
		auto lock = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(2));
		if (lock)
		{
			return lock->IsVisible();
		}
		return false;
	}
}
