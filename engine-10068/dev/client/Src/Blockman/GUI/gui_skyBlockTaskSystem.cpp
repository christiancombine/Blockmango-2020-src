#include "gui_skyBlockTaskSystem.h"
#include "GUI/GuiDef.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageKey.h"
#include "Setting/SkyBlockTaskSystemSetting.h"
#include "Network/ClientNetwork.h"

namespace BLOCKMAN
{
	gui_skyBlockTaskSystem::gui_skyBlockTaskSystem() :
		gui_layout("SkyBlockTaskSystem.json")
	{

		m_task_mainline_panel = m_task_mainline_panel ? m_task_mainline_panel : LordNew GUISkyBlockTaskMainLinePanel;
		m_task_dare_panel = m_task_dare_panel ? m_task_dare_panel : LordNew GUISkyBlockTaskDarePanel;

		LordNew SkyBlockTaskSystemSetting();
		
	}

	gui_skyBlockTaskSystem::~gui_skyBlockTaskSystem()
	{
		m_subscriptionGuard.unsubscribeAll();

		SkyBlockTaskSystemSetting::Instance()->unloadSetting();
		LordDelete SkyBlockTaskSystemSetting::Instance();
		LordSafeDelete(m_task_mainline_panel);
		LordSafeDelete(m_task_dare_panel);
	}

	void gui_skyBlockTaskSystem::onLoad()
	{
		SkyBlockTaskSystemSetting::Instance()->loadSetting();

		auto Close = getWindow<GUIButton>("SkyBlockTaskSystem-Close-Button");
		Close->subscribeEvent(EventButtonClick, std::bind(&gui_skyBlockTaskSystem::onCloseClick, this, std::placeholders::_1));

		m_title = getWindow<GUIStaticText>("SkyBlockTaskSystem-Title-Text");
		m_title->SetText(getString(LanguageKey::GUI_SKYBLOCK_MAIN_TASK_TITLE));

		initTabsAndPanels();
	}

	bool gui_skyBlockTaskSystem::isNeedLoad()
	{
		return checkGameType(ClientGameType::SkyBlockMain) || checkGameType(ClientGameType::SkyBlockMineCave) || checkGameType(ClientGameType::SkyBlockProduct);
	}

	void gui_skyBlockTaskSystem::initTabsAndPanels()
	{
		GUILayout* TaskPanels = getWindow<GUILayout>("SkyBlockTaskSystem-Panels");
		if (TaskPanels == NULL)
			return;

		m_task_tab_mainline = getWindow<GUICheckBox>("SkyBlockTaskSystem-Tab-MainLine");
		m_task_mainline_panel->attachTo(this, TaskPanels);
		m_task_dare_panel->attachTo(this, TaskPanels);
		m_task_dare_panel->hide();
		

		m_task_tab_mainline->SetText(getString(LanguageKey::GUI_SKYBLOCK_MAINLINE_TAB));
		m_task_tab_mainline->subscribeEvent(EventCheckStateChanged, std::bind(&gui_skyBlockTaskSystem::onMainLineTabSelected, this, std::placeholders::_1));

		m_task_tab_challenge = getWindow<GUICheckBox>("SkyBlockTaskSystem-Tab-Challenge");
		m_task_tab_challenge->SetText(getString(LanguageKey::GUI_SKYBLOCK_CHALLENGE_TAB));
		m_task_tab_challenge->subscribeEvent(EventCheckStateChanged, std::bind(&gui_skyBlockTaskSystem::onChallengeTabSelected, this, std::placeholders::_1));

		m_subscriptionGuard.add(SkyBlockUpdateMainLineRedPoiontEvent::subscribe(std::bind(&gui_skyBlockTaskSystem::updateMainLineRedPoint, this, std::placeholders::_1)));
		m_subscriptionGuard.add(SkyBlockCheckDareRedPointEvent::subscribe(std::bind(&gui_skyBlockTaskSystem::updateDareTaskRedPoint, this, std::placeholders::_1)));
		m_subscriptionGuard.add(unLockDareTaskEvent::subscribe(std::bind(&gui_skyBlockTaskSystem::onUnLockDareTaskEvent, this, std::placeholders::_1)));
		m_subscriptionGuard.add(VideoAdVisibleEvent::subscribe(std::bind(&gui_skyBlockTaskSystem::onCubeAdVisible, this, std::placeholders::_1)));

		m_task_tab_mainline->SetChecked(true);
	}

	bool gui_skyBlockTaskSystem::onMainLineTabSelected(const EventArgs & events)
	{
		bool checked = m_task_tab_mainline->GetChecked();
		m_task_tab_mainline->SetTouchable(!checked);
		m_task_tab_challenge->SetTouchable(checked);
		m_task_tab_challenge->SetChecked(!checked);
		m_title->SetText(getString(LanguageKey::GUI_SKYBLOCK_MAIN_TASK_TITLE));
		if (checked)
		{
			m_task_mainline_panel->show();
			m_task_dare_panel->hide();
		}
		return true;
	}

	bool gui_skyBlockTaskSystem::onUnLockDareTaskEvent(bool isUnLock)
	{
		m_isUnLockDare = isUnLock;
		return true;
	}

	bool gui_skyBlockTaskSystem::onCubeAdVisible(bool visible)
	{
		LordLogInfo("gui_skyBlockTaskSystem::onCubeAdVisible %d", visible ? 1 : 0);
		m_have_cube_ad = visible;
		return true;
	}


	bool gui_skyBlockTaskSystem::onChallengeTabSelected(const EventArgs & events)
	{
		if (!m_isUnLockDare)
		{
			m_task_tab_challenge->SetChecked(false);
			ShowResultTipDialogEvent::emit(ResultCode::COMMON, "gui_skyblock_unlock_dare");
			return false;
		}

		bool checked = m_task_tab_challenge->GetChecked();
		m_task_tab_challenge->SetTouchable(!checked);
		m_task_tab_mainline->SetTouchable(checked);
		m_task_tab_mainline->SetChecked(!checked);

		m_title->SetText(getString(LanguageKey::GUI_SKYBLOCK_DARE_TASK_TITLE));
		if (checked)
		{
			ClientNetwork::Instance()->getCommonSender()->sendSkyBlockDareTaskOperate(m_task_dare_panel->DareTaskOperateType::UP_DATA, m_have_cube_ad);
			m_task_dare_panel->show();
			m_task_dare_panel->updateAdStatus(m_have_cube_ad);
			m_task_mainline_panel->hide();
		}
		return true;
	}

	bool gui_skyBlockTaskSystem::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		return true;
	}

	bool gui_skyBlockTaskSystem::updateMainLineRedPoint(bool show)
	{
		auto RedPoint = getWindow<GUIStaticImage>("SkyBlockTaskSystem-Tab-MainLine-RedPoint");
		if (RedPoint == NULL)
		{
			return false;
		}
		RedPoint->SetVisible(show);
		return true;
	}

	bool gui_skyBlockTaskSystem::updateDareTaskRedPoint(bool show)
	{
		auto RedPoint = getWindow<GUIStaticImage>("SkyBlockTaskSystem-Tab-DareTask-RedPoint");
		if (RedPoint == NULL)
		{
			return false;
		}
		RedPoint->SetVisible(show);
		return true;
	}
	void gui_skyBlockTaskSystem::showMainTab()
	{
		m_task_mainline_panel->showMainTabByLevel();
	}
}