#include "gui_checkin.h"
#include "RootGuiLayout.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Util/UICommon.h"
#include "Network/ClientNetwork.h"
#include "GUI/GuiDef.h"
#include "UI/GUIImagesetManager.h"
#include "Util/RedDotManager.h"

using namespace LORD;
using namespace rapidjson;

namespace BLOCKMAN
{

	void gui_checkin::initView()
	{
		if (checkGameType(ClientGameType::BirdSimulator))
		{
			m_fileName = "BirdCheckin";
			m_jsonName = "BirdCheckin.json";
			GUIImagesetManager::Instance()->CreateFromFile("bird_checkin.json");
		}

		if (checkGameType(ClientGameType::BlockCity))
		{
			m_fileName = "BlockCityCheckin";
			m_jsonName = "BlockCityCheckin.json";
			GUIImagesetManager::Instance()->CreateFromFile("blockcity_checkin.json");
		}
	}

	String gui_checkin::getViewName(const String& name)
	{
		return StringUtil::Format("%s-%s", m_fileName.c_str(), name.c_str());
	}

	gui_checkin::gui_checkin() :
		gui_layout("BirdCheckin.json")
	{
		
	}

	gui_checkin::~gui_checkin()
	{

	}

	void gui_checkin::onShow()
	{
		ClientNetwork::Instance()->getSender()->sendCheckin(0);
	}

	void gui_checkin::onHide()
	{
	}

	void gui_checkin::onStart()
	{
		initView();
	}

	void gui_checkin::onLoad()
	{
		setText(getViewName("Title"), "gui_bird_checkin_title");
		setText(getViewName("Checkin"), "gui_bird_checkin_receive");
		m_gvItems = getWindow<GUIGridView>(getViewName("Items").c_str());
		m_btnReceive = getWindow<GUIButton>(getViewName("Checkin").c_str());
		m_tvTimeLeft = getWindow<GUIStaticText>(getViewName("TimeLeft").c_str());
		m_gvItems->InitConfig(21.f, 6.f, 4);
		m_gvItems->SetMoveAble(false);
		getWindow(getViewName("Checkin").c_str())->subscribeEvent(EventButtonClick, std::bind(&gui_checkin::onClick, this, std::placeholders::_1, ViewId::SURE));
		getWindow(getViewName("Close").c_str())->subscribeEvent(EventButtonClick, std::bind(&gui_checkin::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		m_subscriptionGuard.add(BirdCheckinDataUpdateEvent::subscribe(std::bind(&gui_checkin::dataUpdate, this, std::placeholders::_1)));
		if (checkGameType(ClientGameType::BlockCity))
		{
			m_gvItems->InitConfig(21.f, 14.f, 4);
		}
	}

	void gui_checkin::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{

			if (m_gvViewXSize != m_gvItems->GetPixelSize().x && checkGameType(ClientGameType::BlockCity))
			{
				m_gvItems->InitConfig(25.f, 14.f, 4);
			}

			if (m_gvViewXSize != m_gvItems->GetPixelSize().x && checkGameType(ClientGameType::BirdSimulator))
			{
				m_gvItems->InitConfig(21.f, 6.f, 4);
			}

			m_timeLeft -= nTimeElapse;
			if (m_timeLeft < 0)
			{
				ClientNetwork::Instance()->getSender()->sendCheckin(0);
			}
			m_timeLeft = m_timeLeft > 0 ? m_timeLeft : 0;
			m_tvTimeLeft->SetText(StringUtil::Format(getString("gui_bird_checkin_time_left").c_str(), UICommon::timeFormat(m_timeLeft).c_str()).c_str());
		}
	}

	bool gui_checkin::onClick(const EventArgs, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::SURE:
			ClientNetwork::Instance()->getSender()->sendCheckin(1);
			break;
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			if (checkGameType(ClientGameType::BlockCity))
			{
				UpdateIsHaveViewShowEvent::emit(false);
			}
			break;
		}
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_checkin::dataUpdate(const String & content)
	{
		parseData(content);
		return true;
	}

	bool gui_checkin::isNeedLoad()
	{
		return checkGameType({ ClientGameType::BirdSimulator, ClientGameType::BlockCity });
	}

	void gui_checkin::parseData(const String & content)
	{
		Document doc = Document();
		doc.Parse(content.c_str());
		if (doc.HasParseError())
		{
			return;
		}
		vector<CheckinItem>::type items = vector<CheckinItem>::type();
		if (doc.HasMember("timeLeft"))
		{
			m_timeLeft = doc.FindMember("timeLeft")->value.GetInt();
		}

		if (doc.HasMember("needGuide"))
		{
			m_isNeedGuide = doc.FindMember("needGuide")->value.GetBool();
		}

		if (doc.HasMember("data"))
		{
			auto data = doc.FindMember("data")->value.GetArray();
			for (const auto& item : data)
			{
				CheckinItem checkinItem = CheckinItem();
				if (item.HasMember("id"))
				{
					checkinItem.id = item.FindMember("id")->value.GetInt();
				}
				if (item.HasMember("num"))
				{
					checkinItem.num = item.FindMember("num")->value.GetInt();
				}
				if (item.HasMember("state"))
				{
					checkinItem.state = item.FindMember("state")->value.GetInt();
				}
				if (item.HasMember("icon"))
				{
					checkinItem.icon = item.FindMember("icon")->value.GetString();
				}
				if (item.HasMember("name"))
				{
					checkinItem.name = item.FindMember("name")->value.GetString();
				}
				items.push_back(checkinItem);
			}
		}
		initItem(items);
	}

	void gui_checkin::initItem(const vector<CheckinItem>::type & items)
	{
		static int count = 0;
		bool isCanReceive = false;


		if (items.size() != m_gvItems->GetItemCount())
		{
			m_gvItems->RemoveAllItems();
			for (const auto& item : items)
			{
				String iterLayout = getViewName(StringUtil::Format("ItemView-Item-%d", count++));
				GuiCheckinItem* itemView = (GuiCheckinItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_CHECKIN_ITEM, iterLayout.c_str());
				if (checkGameType(ClientGameType::BirdSimulator ))
				{
					itemView->init("BirdCheckinItem.json");
					m_gvViewXSize = m_gvItems->GetPixelSize().x;
					float high = (m_gvViewXSize - 21 * 3) / 4;
					itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, item.id == 7 ? (high * 2 + 21) : high), UDim(0.0f, 230.f));
					itemView->setBirdCheckin(item);
				}

				if (checkGameType(ClientGameType::BlockCity))
				{
					itemView->init("BlockCityCheckinItem.json");
					m_gvViewXSize = m_gvItems->GetPixelSize().x;
					float high = (m_gvViewXSize - 25 * 3) / 4;
					itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, item.id == 7 ? (high * 2 + 25) : high), UDim(0.0f, 213.f));
					itemView->setBlockCityCheckin(item);
				}
				
				m_gvItems->AddItem(itemView);
				if (item.state == 2)
				{
					isCanReceive = true;
				}
			}
		}
		else
		{
			for (size_t i = 0; i < m_gvItems->GetItemCount(); i++)
			{
				auto view = dynamic_cast<GuiCheckinItem*>(m_gvItems->GetItem(i));
				if (checkGameType(ClientGameType::BirdSimulator))
				{
					m_gvViewXSize = m_gvItems->GetPixelSize().x;
					float high = (m_gvViewXSize - 21 * 3) / 4;
					view->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, items[i].id == 7 ? (high * 2 + 21) : high), UDim(0.0f, 230.f));
					view->setBirdCheckin(items[i]);
				}

				if (checkGameType(ClientGameType::BlockCity))
				{
					m_gvViewXSize = m_gvItems->GetPixelSize().x;
					float high = (m_gvViewXSize - 25 * 3) / 4;
					view->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, items[i].id == 7 ? (high * 2 + 25) : high), UDim(0.0f, 213.f));
					view->setBlockCityCheckin(items[i]);
				}

				if (items[i].state == 2)
				{
					isCanReceive = true;
				}
			}
		}
		
		m_btnReceive->SetEnabled(isCanReceive);
		setText(m_btnReceive, isCanReceive ? "gui_bird_checkin_receive" : "gui_bird_checkin_received");
		if (m_gvItems->GetContainer())
		{
			m_gvItems->GetContainer()->SetTouchable(false);
		}

		if (isCanReceive && !m_isNeedGuide && !isShown())
		{
			RootGuiLayout::Instance()->showCheckin();
		}

		RedDotManager::canCheckin = isCanReceive;
	}

}
