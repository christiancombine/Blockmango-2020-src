#include "gui_designationPanelControl.h"
#include "UI/GUIRadioButton.h"
#include "GUIDesignationItem.h"
#include "GuiDef.h"
#include "Util/ClientEvents.h"
#include "Network/protocol/CommonDataBuilder.h"

namespace BLOCKMAN
{
	gui_designationPanelControl::gui_designationPanelControl()
		: gui_layout("DesignationPanel.json")
	{
		DesignationSetting::loadSetting();
	}
	gui_designationPanelControl::~gui_designationPanelControl()
	{
		m_subscriptionGuard.unsubscribeAll();
		DesignationSetting::unloadSetting();

	}
	void gui_designationPanelControl::onLoad()
	{
		m_leftLayout = getWindow<GUILayout>("DesignationPanel-Left");
		m_rightLayout = getWindow<GUILayout>("DesignationPanel-Right");
		updataDesignationType();

		m_subscriptionGuard.add(UpdateDesignationDetailEvent::subscribe(std::bind(&gui_designationPanelControl::onUpdateDesignation, this, std::placeholders::_1, std::placeholders::_2)));
	}

	void gui_designationPanelControl::onGetCommonData(String key, String data)
	{
		if (key == "DesignationDetail")
		{
			auto builder = CommonDataBuilder().fromData(data);
			String designationData = builder.getStringParam("designationData");
			String designationDataType = builder.getStringParam("designationDataType");
			UpdateDesignationDetailEvent::emit(designationData, designationDataType);
		}
	}

	void gui_designationPanelControl::onHide()
	{
	}

	bool gui_designationPanelControl::isNeedLoad()
	{
		return checkGameType(ClientGameType::WalkingDead);
	}

	void gui_designationPanelControl::onUpdate(ui32 nTimeElapse)
	{
	}

	void gui_designationPanelControl::updataDesignationType()
	{
		auto map = DesignationSetting::getDesignationTypeMap();
		auto iter = map.begin();
		int i = 0;
		for (iter; iter != map.end();)
		{
			auto tab = iter->second;
			auto radio = dynamic_cast<GUIRadioButton*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_RADIO, StringUtil::Format("DesignationPanel-Left-Radio%d", i).c_str()));
			radio->SetArea({ 0, 10.0f }, { 0, 10.0f + 60.0f*i }, { 1, -20.0f }, { 0, 50 });
			radio->SetNormalImage("set:walking_dead_designation.json image:buttonNotSelect");
			radio->SetPushedImage("set:walking_dead_designation.json image:buttonOnSelect");
			radio->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			radio->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "7 9 7 9");
			radio->SetText(LanguageManager::Instance()->getString(tab->Name).c_str());
			radio->subscribeEvent(EventRadioStateChanged, std::bind(&gui_designationPanelControl::onRadioChange, this, std::placeholders::_1, tab->Id));
			radio->SetSelected(false);
			m_leftLayout->AddChildWindow(radio);
			m_radioMap[tab->Id] = radio;
			iter++;
			i++;
		}
		iter = map.begin();
		if (iter != map.end())
		{
			m_radioMap[iter->second->Id]->SetSelected(true);
			m_currentId = iter->second->Id;
		}

	}

	bool gui_designationPanelControl::onRadioChange(const EventArgs & events, int tabId)
	{
		if (m_radioMap[tabId]->IsSelected()) {
			m_currentId = tabId;
			updateDesignation();
			setLeftTextColor();
		}
		return true;
	}
	void gui_designationPanelControl::updateDesignation()
	{
		m_rightLayout->CleanupChildren();
		tools.clear();
		useDesignationItem = nullptr;
		sortDesignation();
		int i = 0;
		if (useDesignationItem != nullptr)
		{
			auto rightItem = dynamic_cast<GUIDesignationItem*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_DESIGNATION_ITEM, StringUtil::Format("DesignationPanel-Right-Layout%d", i).c_str()));
			rightItem->SetArea({ 0, 10.0f }, { 0, 10.0f + 70.0f*i }, { 1, -20.0f }, { 0, 60.0f });
			rightItem->InitDesignationItem(useDesignationItem);
			m_rightLayout->AddChildWindow(rightItem);
			i++;
		}
		for (auto iter = tools.begin(); iter != tools.end(); iter++)
		{
			auto rightItem = dynamic_cast<GUIDesignationItem*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_DESIGNATION_ITEM, StringUtil::Format("DesignationPanel-Right-Layout%d", i).c_str()));
			rightItem->SetArea({ 0, 10.0f }, { 0, 10.0f + 70.0f*i }, { 1, -20.0f }, { 0, 60.0f });
			rightItem->InitDesignationItem(*iter);
			m_rightLayout->AddChildWindow(rightItem);
			i++;
		}

	}

	void gui_designationPanelControl::sortDesignation()
	{
		auto  designationType = DesignationSetting::findDesignationTypeById(m_currentId);
		if (designationType == nullptr)
		{
			return;
		}
		auto map = DesignationSetting::getDesignationMap();
		for (auto iter = map.begin(); iter != map.end(); iter++)
		{
			auto item = iter->second;
			if (item->Type == designationType->Type)
			{
				if (item->Status == USE)
				{
					useDesignationItem = item;
				}
				else
				{
					tools.push_back(item);
				}
			}
		}
		sort(tools.begin(), tools.end(), ToolTypeSort());
	}

	bool gui_designationPanelControl::onUpdateDesignation(const String & designationData, const String & designationTypeData)
	{
		DesignationSetting::parseUpdateDesignationData(designationData);
		DesignationSetting::parseUpdateDesignationTypeData(designationTypeData);
		updateDesignation();
		return true;
	}

	void gui_designationPanelControl::setLeftTextColor()
	{
		for (auto iter = m_radioMap.begin(); iter != m_radioMap.end(); iter++)
		{
			if (iter->first == m_currentId)
			{
				iter->second->SetTextColor(Color(235.0f / 255.0f, 200.0f / 255.0f, 178.0f / 255.0f));
			}
			else
			{
				iter->second->SetTextColor(Color(219.0f / 255.0f, 204.0f / 255.0f, 176.0f / 255.0f));
			}
		}
	}
}
