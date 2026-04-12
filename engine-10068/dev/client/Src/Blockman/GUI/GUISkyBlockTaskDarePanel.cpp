#include "GUISkyBlockTaskDarePanel.h"
#include "Util/ClientEvents.h"
#include "UI/GUILayout.h"
#include "Setting/SkyBlockTaskSystemSetting.h"
#include "GUI/GuiDef.h"
#include "GUISkyBlockTaskPanel.h"
#include "Network/ClientNetwork.h"
#include "Util/LanguageKey.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{
	GUISkyBlockTaskDarePanel::GUISkyBlockTaskDarePanel() :
		gui_layout("SkyBlockTaskDarePanel.json")
	{

	}

	GUISkyBlockTaskDarePanel::~GUISkyBlockTaskDarePanel()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GUISkyBlockTaskDarePanel::onLoad()
	{

		m_subscriptionGuard.add(SkyBlockUpdateDareTaskDataEvent::subscribe(std::bind(&GUISkyBlockTaskDarePanel::onUpdateDareTaskData, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));


		m_subscriptionGuard.add(UpdateSkyBlockDareTaskInfoEvent::subscribe(std::bind(&GUISkyBlockTaskDarePanel::onUpdateDareTaskInfo, this, std::placeholders::_1)));

		m_nextTimeText = getWindow<GUIStaticText>("SkyBlockTaskDarePanel-Next-Time");
		m_nextTime = getWindow<GUIStaticText>("SkyBlockTaskDarePanel-Time-Text");


		m_addTimes = getWindow<GUIButton>("SkyBlockTaskDarePanel-Add-Times");

		m_addTimes->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockTaskDarePanel::OnAddTimesButtonClick, this, std::placeholders::_1));

		m_timesText = getWindow<GUIStaticText>("SkyBlockTaskDarePanel-Cur-Times");
		m_timesNum = getWindow<GUIStaticText>("SkyBlockTaskDarePanel-Last-Times");

		m_refresh_coinImg = getWindow<GUIStaticImage>("SkyBlockTaskDarePanel-Money-Img");
		m_refresh_coinNum = getWindow<GUIStaticText>("SkyBlockTaskDarePanel-Money-Num");
		m_refresh_free_text = getWindow<GUIStaticText>("SkyBlockTaskDarePanel-Money-Num-Free");
		m_refresh_task = getWindow<GUIButton>("SkyBlockTaskDarePanel-Refresh-Task");
		m_refresh_free_task = getWindow<GUIButton>("SkyBlockTaskDarePanel-Refresh-Free-Task");
		m_refresh_task->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockTaskDarePanel::OnRefreshButtonClick, this, std::placeholders::_1));
		m_refresh_free_task->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockTaskDarePanel::OnRefreshButtonClick, this, std::placeholders::_1));
		m_refresh_task->SetVisible(false);
		m_refresh_free_task->SetVisible(false);



		GUILayout* Panels = getWindow<GUILayout>("SkyBlockTaskDarePanel-TaskPanels");
		if (Panels == NULL)
			return;

		static unsigned mainline_tab_index = 0;
		auto ChildWidth = 180.f;

		GUIString PanelName = StringUtil::Format("SkyBlockTaskDarePanel-Panels-%d", mainline_tab_index).c_str();
		GUISkyBlockTaskPanel* pTaskPanel = (GUISkyBlockTaskPanel*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SKYBLOCK_TASK_PANEL, PanelName);
		pTaskPanel->SetSize({ {1.0f, -12.0f}, {1.0f, -12.0f} });
		pTaskPanel->SetPosition({ {0.0f, 6.0f}, {0.0f, 6.0f} });
		Panels->AddChildWindow(pTaskPanel);
		pTaskPanel->SetVisible(true);
		m_panels = pTaskPanel;
	}

	bool GUISkyBlockTaskDarePanel::onUpdateDareTaskData(int lv, int progressesLevel, const String & data)
	{
		SkyBlockTaskGroup result;
		if (!SkyBlockTaskSystemSetting::Instance()->parseUpdateDareTaskData(data, result))
		{
			return false;
		}


		if (m_panels)
		{
			m_panels->ShowDareTasks(result, m_have_cube_ad);
		}
		ShowSkyBlockTaskIconEvent::emit();

		for (auto task : result.Tasks)
		{
			if (task.Status == SKYBLOCK_TASK_STATUS_NOT_GET || task.Status == SKYBLOCK_TASK_STATUS_CAN_COMMIT)
			{
				SkyBlockCheckDareRedPointEvent::emit(true);
				return true;
			}
		}

		SkyBlockCheckDareRedPointEvent::emit(false);
		return true;
	}

	bool GUISkyBlockTaskDarePanel::onUpdateDareTaskInfo(const String & data)
	{
		parseDaretaskInfo(data);

		m_nextTimeText->SetText(getString(LanguageKey::GUI_SKYBLOCK_DARE_TASK_NEXT_TIME));
		m_timesText->SetText(getString(LanguageKey::GUI_SKYBLOCK_HAVE_DARE_TASK_TIME));

		m_timesNum->SetText(StringUtil::Format("%d/%d", m_cur_tiems, m_max_tiems).c_str());
		m_refresh_coinImg->SetImage(UICommon::getCurrencyIconByType(m_re_coinId).c_str());
		m_refresh_coinNum->SetText(StringUtil::ToString(m_re_coinNum).c_str());


		m_refresh_free_text->SetText(StringUtil::Format(getString(LanguageKey::GUI_REFRESH_FREE_DARE_TASK).c_str(), m_cur_free_times, m_max_free_times).c_str());
		
		if (m_cur_free_times > 0)
		{
			m_refresh_task->SetVisible(false);
			m_refresh_free_task->SetVisible(true);
		}
		else
		{
			m_refresh_task->SetVisible(true);
			m_refresh_free_task->SetVisible(false);
		}
		

		SkyBlockAchieveMaxDareTaskEvent::emit(m_cur_tiems <= 0);
		return true;
	}

	bool GUISkyBlockTaskDarePanel::OnAddTimesButtonClick(const EventArgs & events)
	{
		ClientNetwork::Instance()->getCommonSender()->sendSkyBlockDareTaskOperate(ADD_TIMES, m_have_cube_ad);
		return true;
	}

	bool GUISkyBlockTaskDarePanel::OnRefreshButtonClick(const EventArgs & events)
	{
		ClientNetwork::Instance()->getCommonSender()->sendSkyBlockDareTaskOperate(REFRESH_TASK, m_have_cube_ad);
		return true;
	}

	bool GUISkyBlockTaskDarePanel::parseDaretaskInfo(const String & info)
	{
		if (info.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(info.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("curTimes") || !doc->HasMember("maxTimes") || !doc->HasMember("addCoinId") || !doc->HasMember("refreshCoinId") || !doc->HasMember("refreshCoinNum")
			|| !doc->HasMember("addCoinNum") || !doc->HasMember("nextTime") || !doc->HasMember("curFreeTimes") || !doc->HasMember("maxFreeTimes"))
		{
			LordLogError("GUISkyBlockTaskDarePanel::parseDaretaskInfo The game result content missed some field");
			LordDelete(doc);
			return false;
		}

		m_cur_tiems = doc->FindMember("curTimes")->value.GetInt();
		m_max_tiems = doc->FindMember("maxTimes")->value.GetInt();
		m_add_coinId = doc->FindMember("addCoinId")->value.GetInt();
		m_add_coinNum = doc->FindMember("addCoinNum")->value.GetInt();
		m_re_coinId = doc->FindMember("refreshCoinId")->value.GetInt();
		m_re_coinNum = doc->FindMember("refreshCoinNum")->value.GetInt();
		m_next_time = doc->FindMember("nextTime")->value.GetInt();
		m_cur_free_times = doc->FindMember("curFreeTimes")->value.GetInt();
		m_max_free_times = doc->FindMember("maxFreeTimes")->value.GetInt();
		m_next_time = m_next_time * 1000;
		LordDelete(doc);
		return true;
	}

	void GUISkyBlockTaskDarePanel::onUpdate(ui32 nTimeElapse)
	{
		if (m_next_time > 0)
		{
			m_next_time -= nTimeElapse;
			int seconds = m_next_time / 1000;
			int second = seconds % 60;
			int minute = (seconds % 3600) / 60;
			int hour = seconds / 3600;
			m_nextTime->SetText(StringUtil::Format("%s:%s:%s",
				hour < 10 ? StringUtil::Format("0%d", hour).c_str() : StringUtil::ToString(hour).c_str(),
				minute < 10 ? StringUtil::Format("0%d", minute).c_str() : StringUtil::ToString(minute).c_str(),
				second < 10 ? StringUtil::Format("0%d", second).c_str() : StringUtil::ToString(second).c_str()).c_str());
		}
	}

	void GUISkyBlockTaskDarePanel::updateAdStatus(bool have_cube_ad)
	{
		m_have_cube_ad = have_cube_ad;
	}
}

 