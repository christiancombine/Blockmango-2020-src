#include "gui_commonTip.h"
#include "RootGuiLayout.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Network/ClientNetwork.h"
#include "Actor/ActorObject.h"
#include "Setting/GuideSystemSetting.h"
#include "UI/GUIImagesetManager.h"
#include "game.h"

using namespace LORD;
using namespace rapidjson;

namespace BLOCKMAN
{

	gui_commonTip::gui_commonTip() :
		gui_layout("CommonTip.json")
	{
	}


	gui_commonTip::~gui_commonTip()
	{
	}

	void gui_commonTip::onHide()
	{
		if (m_guideId > 0 && ClientNetwork::Instance()->isAlive())
		{
			GuideSystemSetting::setGuideStatus(m_guideId, STATUS_DONE);
			m_guideId = 0;
		}
	}

	void gui_commonTip::onLoad()
	{
		GUIImagesetManager::Instance()->CreateFromFile("common_tip.json");
		m_titleText = getWindow<GUIStaticText>("CommonTip-Title");
		m_messageText = getWindow<GUIStaticText>("CommonTip-Message");
		m_actorWindow = getWindow<GuiActorWindow>("CommonTip-Actor");
		getWindow("CommonTip")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_commonTip::onClick, this, std::placeholders::_1));
		m_subscriptionGuard.add(ShowCommonTipEvent::subscribe(std::bind(&gui_commonTip::showTip, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowActorCommonTipEvent::subscribe(std::bind(&gui_commonTip::open, this, std::placeholders::_1)));
	}

	void gui_commonTip::onUpdate(ui32 nTimeElapse)
	{
		i32  i = 0;
		for (String& tip : m_tips)
		{
			i++;
			if (i == m_page)
			{
				m_messageText->SetText(getString(tip));
				break;
			}
		}
	}

	bool gui_commonTip::onClick(const EventArgs)
	{
		if (m_page >= (int) m_tips.size())
		{
			RootGuiLayout::Instance()->setCommonTipVisible(false);
		}
		else
		{
			m_page += 1;
		}
		return true;
	}

	bool gui_commonTip::open(const String & tips)
	{
		m_page = 1;
		m_guideId = 0;
		parseData(tips);
		RootGuiLayout::Instance()->setCommonTipVisible(true);
		return true;
	}

	bool gui_commonTip::showTip(vector<String>::type tips, i32 guideId)
	{
		m_page = 1;
		m_tips.clear();
		for (auto& tip: tips)
		{
			String msg = getString(tip).c_str();
			if (StringUtil::Find(tip, "%s"))
			{
				msg = StringUtil::Format(msg.c_str(), GameClient::CGame::Instance()->getPlayerName().c_str());
			}
			m_tips.push_back(msg);
		}
		m_guideId = guideId;
		m_actorWindow->SetActor("npc14.actor");
		m_titleText->SetText(getString("gui_game_elf"));
		RootGuiLayout::Instance()->setCommonTipVisible(true);
		return false;
	}

	void gui_commonTip::parseData(const String & tips)
	{
		Document doc = Document();
		doc.Parse(tips.c_str());
		if (doc.HasParseError())
		{
			return;
		}

		if (doc.HasMember("title"))
		{
			m_titleText->SetText(getString(doc.FindMember("title")->value.GetString()));
		}

		if (doc.HasMember("actorName"))
		{
			String actorName = doc.FindMember("actorName")->value.GetString();
			if (!actorName.empty())
			{
				m_actorWindow->SetVisible(true);
				m_actorWindow->SetActor(actorName.c_str());
			}
			else
			{
				m_actorWindow->SetVisible(false);
			}
		}
		else
		{
			m_actorWindow->SetVisible(false);
		}

		if (doc.HasMember("tips"))
		{
			m_tips.clear();
			auto content = doc.FindMember("tips")->value.GetArray();
			for (const auto& item : content)
			{
				m_tips.push_back(item.GetString());
			}
		}
	}
}
