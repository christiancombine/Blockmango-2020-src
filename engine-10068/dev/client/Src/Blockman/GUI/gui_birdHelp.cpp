#include "gui_birdHelp.h"
#include "RootGuiLayout.h"
#include "Util/UICommon.h"
#include "Setting/HelpSetting.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
using namespace LORD;

namespace BLOCKMAN
{

	gui_birdHelp::gui_birdHelp() :
		gui_layout("BirdHelp.json")
	{
	}


	gui_birdHelp::~gui_birdHelp()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_birdHelp::onShow()
	{
		refresh();
	}

	void gui_birdHelp::onHide()
	{
	}

	void gui_birdHelp::onLoad()
	{
		m_tvPage = getWindow<GUIStaticText>("BirdHelp-Page");
		m_titleText = getWindow<GUIStaticText>("BirdHelp-Title");
		m_ivImage = getWindow<GUIStaticImage>("BirdHelp-Image");
		m_messageText = getWindow<GUIStaticText>("BirdHelp-Content");
		getWindow("BirdHelp-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_birdHelp::onClick, this, std::placeholders::_1,ViewId::CLOSE));
		getWindow("BirdHelp-Up")->subscribeEvent(EventButtonClick, std::bind(&gui_birdHelp::onClick, this, std::placeholders::_1,ViewId::BTN_UP));
		getWindow("BirdHelp-Next")->subscribeEvent(EventButtonClick, std::bind(&gui_birdHelp::onClick, this, std::placeholders::_1,ViewId::BTN_NEXT));
		m_subscriptionGuard.add(ShowBirdWingTipEvent::subscribe(std::bind(&gui_birdHelp::setPageAndShow, this, std::placeholders::_1)));
	}

	bool gui_birdHelp::onClick(const EventArgs& args, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			break;
		case ViewId::BTN_UP:
			m_page--;
			refresh();
			break;
		case ViewId::BTN_NEXT:
			m_page++;
			refresh();
			break;
		}
		return true;
	}

	bool gui_birdHelp::setPageAndShow(i32 page)
	{
		m_page = page;
		RootGuiLayout::Instance()->showBirdHelp();
		return true;
	}

	bool gui_birdHelp::isNeedLoad()
	{
		return checkGameType(ClientGameType::BirdSimulator);
	}

	void gui_birdHelp::refresh()
	{
		int i = 0;
		auto helps = HelpSetting::getHelps();
		if (m_page > helps.size())
		{
			m_page = 1;
		}
		for (auto& item : helps)
		{
			i++;
			if (m_page == i)
			{
				m_ivImage->SetImage(item->icon.c_str());
				m_titleText->SetText(getString(item->name));
				m_messageText->SetText(getString(item->content));
				break;
			}
		}
		getWindow("BirdHelp-Up")->SetEnabled(m_page > 1);
		getWindow("BirdHelp-Next")->SetEnabled(m_page < helps.size());
		m_tvPage->SetText(StringUtil::Format("%d/%d", m_page, helps.size()).c_str());
	}

}
