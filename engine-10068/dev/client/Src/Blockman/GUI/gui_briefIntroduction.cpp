#include "gui_briefIntroduction.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Network/ClientNetwork.h"
#include "GUI/GuiDef.h"

namespace BLOCKMAN
{
	gui_briefIntroduction::gui_briefIntroduction() :
		gui_layout("BriefIntrodunction.json")
	{
	}

	gui_briefIntroduction::~gui_briefIntroduction()
	{
		m_subscriptionGuard.unsubscribeAll();
		BriefIntroductionSetting::unloadSetting();
	}

	void gui_briefIntroduction::onLoad()
	{
		m_content = getWindow<GUILayout>("BriefIntrodunction-Content");
		m_border = getWindow<GUIStaticImage>("BriefIntrodunction-Border");

		auto Next = getWindow<GUIButton>("BriefIntrodunction-Next");
		Next->subscribeEvent(EventButtonClick, std::bind(&gui_briefIntroduction::onNextClick, this, std::placeholders::_1));
		auto Previous = getWindow<GUIButton>("BriefIntrodunction-Previous");
		Previous->subscribeEvent(EventButtonClick, std::bind(&gui_briefIntroduction::onPreviousClick, this, std::placeholders::_1));
		auto Close = getWindow<GUIButton>("BriefIntrodunction-Close");
		Close->subscribeEvent(EventButtonClick, std::bind(&gui_briefIntroduction::onCloseClick, this, std::placeholders::_1));

		BriefIntroductionSetting::loadSetting();
		m_group = BriefIntroductionSetting::getFirstGroup();
		if (m_group == nullptr)
		{
			return;
		}
		max_order = m_group->pictures.size();
		changeGroup(m_group->GroupKey);
	}

	bool gui_briefIntroduction::isNeedLoad()
	{
		return checkGameType(ClientGameType::PixelGunHall);
	}

	bool gui_briefIntroduction::changeGroup(String GroupKey)
	{
		BI_group* group = BriefIntroductionSetting::findGroupByKey(GroupKey);
		if (group == nullptr)
		{
			return false;
		}
		m_group = group;
		max_order = m_group->pictures.size();
		m_order = 0;
		changePicture();
		auto m_titleText = dynamic_cast<GUIStaticText*>(m_content->GetChildByIndex(1));
		m_titleText->SetText(LanguageManager::Instance()->getString(GroupKey).c_str());
		return true;
	}

	void gui_briefIntroduction::changePicture()
	{
		if (!m_group)
		{
			return;
		}

		auto picture = m_group->pictures[m_order];
		if (!picture)
		{
			return;
		}

		auto m_picTitle = dynamic_cast<GUIStaticText*>(m_border->GetChildByIndex(0));
		m_picTitle->SetText(LanguageManager::Instance()->getString(picture->picTitle).c_str());
		
		auto m_picture = dynamic_cast<GUIStaticImage*>(m_border->GetChildByIndex(1));
		m_picture->SetImage(picture->picText.c_str());

		auto m_picDetil = dynamic_cast<GUIStaticText*>(m_border->GetChildByIndex(2));
		m_picDetil->SetText(LanguageManager::Instance()->getString(picture->picDetil).c_str());

		auto m_orderText = dynamic_cast<GUIStaticText*>(m_content->GetChildByIndex(2));
		m_orderText->SetText(StringUtil::Format("%d/%d", m_order + 1, max_order).c_str());
	}

	bool gui_briefIntroduction::onShowWindow()
	{
		RootGuiLayout::Instance()->showBriefIntroduction();
		return true;
	}

	bool gui_briefIntroduction::onNextClick(const EventArgs & events)
	{
		m_order = (m_order + 1) % max_order;
		changePicture();
		return true;
	}

	bool gui_briefIntroduction::onPreviousClick(const EventArgs & events)
	{
		m_order = (m_order + max_order - 1) % max_order;
		changePicture();
		return true;
	}

	bool gui_briefIntroduction::onCloseClick(const EventArgs & events)
	{
		getParent()->showMainControl();
		return true;
	}


}
