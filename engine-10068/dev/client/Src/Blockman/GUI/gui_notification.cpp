#include "gui_notification.h"
#include "UI/GUIFont.h"
#include "Util/Random.h"
#include "Util/ClientEvents.h"
#include "rapidjson/document.h"
#include "Setting/GameTypeSetting.h"
#include "game.h"
#include "UI/GUIButton.h"
#include "UI/GUIFontManager.h"

namespace BLOCKMAN
{
	void Notification::onUpdate(ui32 nTimeElapse)
	{
		if (m_current_wait_time == 0 && m_current_roll_time == 0)
		{
			return;
		}
		m_content->SetVisible(true);
		if (m_current_wait_time > 0)
		{
			m_current_wait_time -= nTimeElapse;
			if (m_current_wait_time <= 0)
			{
				m_current_wait_time = 0;
				bool isVip = false;
				bool isShowBtn = false;
				i64 targetId = 0;
				String notification = getNotification(isVip, isShowBtn);
				if (!notification.empty())
				{
					m_current_roll_time = 1000;
					for (auto text : m_texts)
					{
						if (text->GetYPosition().d_offset > m_height / 2)
						{
							text->SetText(notification.c_str());
						}
					}

					for (auto vipImage : m_vipImage)
					{
						vipImage->SetVisible(isVip);
					}
					for (auto visitBtn : m_visitBtn)
					{
						m_effectLayout->SetVisible(isShowBtn);
						visitBtn->SetVisible(isShowBtn);
					}
				}
				else
				{
					m_content->SetVisible(false);
				}
			}
		}
		if (m_current_roll_time > 0)
		{
			m_current_roll_time -= nTimeElapse;
			if (m_current_roll_time <= 0)
			{
				m_current_roll_time = 0;
				if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain) 
				{
					m_current_wait_time = WAIT_LONG_TIME;
				}
				else 
				{
					m_current_wait_time = WAIT_TIME;
				}			
				for (auto text : m_texts)
				{
					if (text->GetYPosition().d_offset < -m_height / 2)
					{
						text->SetYPosition({ 0.0f, m_height });
					}
					else
					{
						text->SetYPosition({ 0.0f, 0.5f });
					}
				}
			}
			float move = nTimeElapse * m_height / 1000.0f;
			for (auto text : m_texts)
			{
				text->SetYPosition({ 0.0f, text->GetYPosition().d_offset - move });
			}
		}
	}

	bool Notification::onReceiveNotification(int type, const String & data)
	{
		rapidjson::Document doc = rapidjson::Document();
		doc.Parse(data.c_str());
		if (doc.HasParseError())
		{
			LordLogError("The otification data content HasParseError.");
			return false;
		}
		String message = "";
		StringArray params;
		i64 targetId = 0;
		bool top = false;
		bool isVip = false;
		bool isShowBtn = false;
		if (doc.HasMember("message"))
		{
			message = LanguageManager::Instance()->getString(doc.FindMember("message")->value.GetString()).c_str();
		}
		if (doc.HasMember("params"))
		{
			params = StringUtil::Split(doc.FindMember("params")->value.GetString(), "#");
		}
		if (doc.HasMember("top"))
		{
			top = doc.FindMember("top")->value.GetBool();
		}
		if (doc.HasMember("isVip"))
		{
			isVip = doc.FindMember("isVip")->value.GetBool();
		}
		if (doc.HasMember("isShowBtn"))
		{
			isShowBtn = doc.FindMember("isShowBtn")->value.GetBool();
		}
		if (doc.HasMember("targetId"))
		{
			targetId = doc.FindMember("targetId")->value.GetInt64();
		}
		int num = 0;
		for (String param : params)
		{
			num++;
			String key = "%" + StringUtil::ToString(num) + "s";
			message = StringUtil::Replace(message, key, LanguageManager::Instance()->getString(param).c_str());
		}
		addNotification(type, message, top, isVip, isShowBtn, targetId);
		return true;
	}

	void Notification::splitNotification(String notification)
	{
		m_current_notification.clear();
		GUIStaticText* view = m_texts.front();
		GUIFont* font = view->GetFont();
		TextStringList textList;
		vector<float>::type highList;
		font->SplitStringToMultiLine(view->GetPixelSize().x, Color::WHITE, notification.c_str(), textList, highList);
		for (auto text : textList)
		{
			m_current_notification.push_back(text.c_str());
		}
	}

	void Notification::addNotification(int type, String notification, bool top, bool isVip, bool isShowBtn, i64 targetId)
	{
		if (type == TYPE_DEFAULT)
		{
			m_default_notifications.push_back({ notification , top , isVip, isShowBtn, targetId });
		}
		else
		{
			m_custom_notifications.push_back({ notification , top , isVip, isShowBtn, targetId });
		}
		if (m_current_wait_time == 0 && m_current_roll_time == 0)
		{
			String notification = getNotification(isVip, isShowBtn);
			if (!notification.empty())
			{
				if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain)
				{
					m_current_wait_time = WAIT_LONG_TIME;
				}
				else
				{
					m_current_wait_time = WAIT_TIME;
				}
				for (auto text : m_texts)
				{
					if (text->GetYPosition().d_offset < m_height / 2)
					{
						text->SetText(notification.c_str());
					}
				}
				for (auto vipImage : m_vipImage)
				{
					vipImage->SetVisible(isVip);
				}
				for (auto visitBtn : m_visitBtn)
				{
					visitBtn->SetVisible(isShowBtn);
					m_effectLayout->SetVisible(isShowBtn);
				}
			}
		}
	}

	String Notification::getNotification(bool &isVip, bool &isShowBtn)
	{
		if (m_current_show_type == TYPE_DEFAULT && !m_custom_notifications.empty())
		{
			auto notification = m_custom_notifications.front();
			splitNotification(notification.message);
			m_rootWindow->SetAlwaysOnTop(notification.top);
			m_custom_notifications.pop_front();
			m_current_show_type = TYPE_CUSTOM;
			isVip = notification.isVip;
			isShowBtn = notification.isShowBtn;
			m_currentTargetId = notification.targetId;
		}
		if (!m_current_notification.empty())
		{
			String text = m_current_notification.front();
			m_current_notification.pop_front();
			return text;
		}
		if (!m_custom_notifications.empty())
		{
			auto notification = m_custom_notifications.front();
			splitNotification(notification.message);
			m_rootWindow->SetAlwaysOnTop(notification.top);
			m_custom_notifications.pop_front();
			m_current_show_type = TYPE_CUSTOM;
			isVip = notification.isVip;
			isShowBtn = notification.isShowBtn;
			m_currentTargetId = notification.targetId;
			return getNotification(isVip, isShowBtn);
		}
		if (!m_default_notifications.empty())
		{
			Random random = Random();
			int index = random.nextIntRange(0, m_default_notifications.size() - 1);
			auto notification = m_default_notifications[index];
			splitNotification(notification.message);
			m_rootWindow->SetAlwaysOnTop(notification.top);
			m_current_show_type = TYPE_DEFAULT;
			return getNotification(isVip, isShowBtn);
		}
		return "";
	}

	gui_notification::gui_notification()
		: gui_layout("Notification.json")
	{
		m_notification = new Notification();
	}

	gui_notification::~gui_notification()
	{
		LordSafeDelete(m_notification);
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_notification::onLoad()
	{
		getWindow<GUILayout>("Notification-Content")->SetVisible(false);
		getWindow<GUILayout>("Notification-PixelGun-Notification")->SetVisible(false);
		m_notification->m_effectLayout = getWindow<GUILayout>("Notification-Content-Effect");

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::PixelGunHall &&
			GameClient::CGame::Instance()->isEnableIndie())
		{
			m_notification->m_content = getWindow<GUILayout>("Notification-PixelGun-Notification");
			m_notification->m_texts.push_back(getWindow<GUIStaticText>("Notification-PixelGun-Notification-Content-Text-1"));
			m_notification->m_texts.push_back(getWindow<GUIStaticText>("Notification-PixelGun-Notification-Content-Text-2"));
		}
		else
		{
			if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockProduct)
			{
				getWindow<GUILayout>("Notification-Content")->SetArea({ 0.f, 0.f }, { 0.f, 75.f }, { 0.46f, 0.f }, { 0.f, 26.f });
				m_notification->m_content = getWindow<GUILayout>("Notification-Content");
			}
			else if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain)
			{
				getWindow<GUILayout>("Notification-Content")->SetArea({ 0.f, 0.f }, { 0.f, 55.f }, { 0.4f, 0.f }, { 0.f, 36.f });
				m_notification->m_vipImage.push_back(getWindow<GUIStaticImage>("Notification-Content-VIP-Img-1"));
				m_notification->m_vipImage.push_back(getWindow<GUIStaticImage>("Notification-Content-VIP-Img-2"));
				m_notification->m_visitBtn.push_back(getWindow<GUIButton>("Notification-Content-Visit-Btn-1"));
				m_notification->m_visitBtn.push_back(getWindow<GUIButton>("Notification-Content-Visit-Btn-2"));
				m_notification->m_content = getWindow<GUILayout>("Notification-Content");
				getWindow<GUIStaticText>("Notification-Content-Text-1")->SetFont(GUIFontManager::Instance()->GetFont("HT16"));
				getWindow<GUIStaticText>("Notification-Content-Text-1")->SetTextHorzAlign(HA_LEFT);
				getWindow<GUIStaticText>("Notification-Content-Text-1")->SetArea({ 0.f, 64.f }, { 0.f, 0.f }, { 1.f, 0.f }, { 1.f, 0.f });
				getWindow<GUIStaticText>("Notification-Content-Text-2")->SetFont(GUIFontManager::Instance()->GetFont("HT16"));
				getWindow<GUIStaticText>("Notification-Content-Text-2")->SetTextHorzAlign(HA_LEFT);
				getWindow<GUIStaticText>("Notification-Content-Text-2")->SetArea({ 0.f, 64.f }, { 0.f, 37.f }, { 1.f, 0.f }, { 1.f, 0.f });
				for (auto btn : m_notification->m_visitBtn)
				{
					btn->subscribeEvent(EventButtonClick, std::bind(&gui_notification::onSkyBlockVisitEvent, this));
				}
			}
			else
			{
				m_notification->m_content = getWindow<GUILayout>("Notification-Content");
			}
			m_notification->m_texts.push_back(getWindow<GUIStaticText>("Notification-Content-Text-1"));
			m_notification->m_texts.push_back(getWindow<GUIStaticText>("Notification-Content-Text-2"));
		}
		m_notification->m_rootWindow = m_rootWindow;
		m_notification->m_height = m_notification->m_content->GetHeight().d_offset;
		m_subscriptionGuard.add(ReceiveNotificationEvent::subscribe(std::bind(&gui_notification::onReceiveNotification, this, std::placeholders::_1, std::placeholders::_2)));	
	}

	void gui_notification::onUpdate(ui32 nTimeElapse)
	{
		m_notification->onUpdate(nTimeElapse);
	}

	bool gui_notification::onReceiveNotification(int type, const String & data)
	{
		return m_notification->onReceiveNotification(type, data);
	}

	bool gui_notification::onSkyBlockVisitEvent()
	{
		if (m_notification && m_notification->m_currentTargetId > 0)
			ClientNetwork::Instance()->getSender()->sendCallOnManor(m_notification->m_currentTargetId, 3, false, 0);
		return true;
	}
}