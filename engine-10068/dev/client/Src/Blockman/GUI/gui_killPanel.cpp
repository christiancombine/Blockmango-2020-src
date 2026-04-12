#include "gui_killPanel.h"
#include "GUIKillMsgItem.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"
#include "Network/protocol/CommonDataBuilder.h"

namespace BLOCKMAN
{
	unsigned gui_killPanel::messageIndex = 0;

	gui_killPanel::gui_killPanel()
		: gui_layout("KillPanel.json")
		, m_totalTime(0)
		, m_isPlaying(false)
	{
	}

	gui_killPanel::~gui_killPanel()
	{
	}

	void gui_killPanel::onLoad()
	{
		m_killMsgList = getWindow<GUILayout>("KillPanel-Kill-Msg-List");
		m_comboKillIcon = getWindow<GUIStaticImage>("KillPanel-Combo-Kill-Icon");
		m_originPos = m_comboKillIcon->GetPosition();
		m_originPos.d_y.d_offset = m_originPos.d_y.d_offset;
		auto rect = m_comboKillIcon->GetUnclippedOuterRect();
		m_moveDistance = rect.top / 2;
		m_subscriptionGuard.add(ReceiveKillMsgEvent::subscribe(std::bind(&gui_killPanel::onReceiveKillMsg, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowKillsEvent::subscribe(std::bind(&gui_killPanel::showComboKill, this, std::placeholders::_1, std::placeholders::_2, 
			std::placeholders::_3, std::placeholders::_4)));
	}

	void gui_killPanel::onGetCommonData(String key, String data)
	{
		if (key == "LuckyBlockGeneralDeathTip")
		{
			auto build = CommonDataBuilder().fromData(data);
			tipTxt = build.getStringParam("tipTxt");
			auto name = build.getStringParam("name");
			auto color = build.getStringParam("color");
			addNormalMsg(tipTxt,name,color);
		}
	}

	void gui_killPanel::onUpdate(ui32 nTimeElapse)
	{
		updateChildrens(nTimeElapse);
		updateIcon(nTimeElapse);
	}

	bool gui_killPanel::onReceiveKillMsg(const String & data)
	{
		if (data.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The kill msg content  HasParseError.");
			LordDelete(doc);
			return false;
		}

		int kills = doc->HasMember("kills") ? doc->FindMember("kills")->value.GetInt() : 0;
		int head = doc->HasMember("head") ? doc->FindMember("head")->value.GetInt() : 0;
		int armsId = doc->HasMember("armsId") ? doc->FindMember("armsId")->value.GetInt() : 0;
		int poison = doc->HasMember("poison") ? doc->FindMember("poison")->value.GetInt() : 0;
		int spike = doc->HasMember("spike") ? doc->FindMember("spike")->value.GetInt() : 0;
		String killer = doc->HasMember("killer") ? doc->FindMember("killer")->value.GetString() : "";
		String dead = doc->HasMember("dead") ? doc->FindMember("dead")->value.GetString() : "";
		String color = doc->HasMember("color") ? doc->FindMember("color")->value.GetString() : "";
		String killerFame = doc->HasMember("killerFame") ? doc->FindMember("killerFame")->value.GetString() : "";
		String deadFame = doc->HasMember("deadFame") ? doc->FindMember("deadFame")->value.GetString() : "";

		addKillMsg(new KillMsg(kills, head, armsId, poison, spike, killer, dead, color, killerFame, deadFame));
		LordDelete(doc);
		return true;
	}

	bool gui_killPanel::showComboKill(const String& icon, uint soundType, i32 stayTime, i32 animateTime)
	{
		if(m_playNow.icon == "" && m_playNext.icon == "")
		{
			m_playNow.icon = icon.c_str();
			m_playNow.sound = soundType;
			m_playNow.stay = stayTime;
			m_playNow.animate = animateTime;
		}
		else if (m_playNow.icon != "")
		{
			m_playNext.icon = icon.c_str();
			m_playNext.sound = soundType;
			m_playNext.stay = stayTime;
			m_playNext.animate = animateTime;
			return true;
		}
		else if (m_playNow.icon == "" && m_playNext.icon != "")
		{
			m_playNext.icon = icon.c_str();
			m_playNext.sound = soundType;
			m_playNext.stay = stayTime;
			m_playNext.animate = animateTime;

			m_playNow = m_playNext;

			m_playNext.icon = "";
			m_playNext.sound = 0;
			m_playNext.stay = 0;
			m_playNext.animate = 0;
		}
		m_isPlaying = true;
		playSoundByType((SoundType)m_playNow.sound);
		m_comboKillIcon->SetVisible(true);
		m_comboKillIcon->SetImage(m_playNow.icon);
		m_comboKillIcon->SetOverlayImage(m_playNow.icon);
		m_comboKillIcon->SetRenderAsBlock(false);
		m_comboKillIcon->SetPosition(m_originPos);
		m_comboKillIcon->SetAlpha(1.0f);
		m_stayTime = m_playNow.stay > 0 ? m_playNow.stay : m_stayTime;
		m_animateTime = m_playNow.animate > 0 ? m_playNow.animate : m_animateTime;
		m_totalTime = 0;
		return true;
	}

	void gui_killPanel::updateIcon(ui32 nTimeElapse)
	{
		if (!m_isPlaying)
		{
			m_comboKillIcon->SetVisible(m_isPlaying);
			return;
		}
			
		m_totalTime += nTimeElapse;
		if (m_totalTime < m_stayTime)
		{
			return;
		}
		float ratio = (float)(m_totalTime - m_stayTime) / m_animateTime;
		ratio = ratio > 1.0f ? 1.0f : ratio;
		float moveY = ratio * m_moveDistance;
		LORD::UVector2 newPos = UVector2(m_originPos.d_x, UDim(m_originPos.d_y.d_scale, m_originPos.d_y.d_offset - moveY));
		m_comboKillIcon->SetPosition(newPos);
		m_comboKillIcon->SetAlpha(1.0f - ratio);
		if (ratio >= 1.0f)
		{
			m_isPlaying = false;
			m_comboKillIcon->SetVisible(false);
			m_playNow.icon = "";
			m_playNow.sound = 0;
			m_playNow.stay = 0;
			m_playNow.animate = 0;

			if (m_playNext.icon != "")
			{
				showComboKill(m_playNext.icon.c_str(), m_playNext.sound, m_playNext.stay, m_playNext.animate);
			}
		}
	}

	void gui_killPanel::updateChildrens(ui32 nTimeElapse)
	{
		if (m_msgs.size() == 0)
			return;
		bool isNeedUpdatePosition = false;
		for (auto iter = m_msgs.begin(); iter != m_msgs.end(); )
		{
			(*iter)->time = (*iter)->time > nTimeElapse ? (*iter)->time - nTimeElapse : 0;
			if ((*iter)->time == 0)
			{
				if ((*iter)->window)
				{
					m_killMsgList->RemoveChildWindow((*iter)->window);
				}
				LordDelete *iter;
				iter = m_msgs.erase(iter);
				isNeedUpdatePosition = true;
			}
			else
			{
				++iter;
			}
		}
		if (isNeedUpdatePosition)
		{
			const size_t child_count = m_killMsgList->GetChildCount();
			for (size_t i = 0; i < child_count; ++i)
			{
				GUIWindow* window = m_killMsgList->GetChildByIndex(i);
				window->SetYPosition({ 0, i * 50.0f });
			}
		}
	}

	void gui_killPanel::addKillMsg(KillMsg * msg)
	{
		m_msgs.push_back(msg);
		messageIndex++;
		GUIString itemName = StringUtil::Format("KillPanel-Item-%d", messageIndex).c_str();
		GUIKillMsgItem* pKillMsgItem = (GUIKillMsgItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_KILL_MSG_ITEM, itemName);
		pKillMsgItem->SetWidth({ 1, 0.0f });
		pKillMsgItem->SetHeight({ 0, 50.f });
		pKillMsgItem->SetYPosition({ 0, m_killMsgList->GetChildCount() * 50.0f });
		pKillMsgItem->setKillMsg(msg);
		m_killMsgList->AddChildWindow(pKillMsgItem);
		msg->window = pKillMsgItem;
	}

	void gui_killPanel::addNormalMsg(const String& msg, const String& name, const String& color)
	{
		auto pMsg = new KillMsg(0, 0, 0, 0, 0, name, "", color, "", "");
		m_msgs.push_back(pMsg);
		messageIndex++;
		GUIString itemName = StringUtil::Format("KillPanel-Item-%d", messageIndex).c_str();
		GUIKillMsgItem* pKillMsgItem = (GUIKillMsgItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_KILL_MSG_ITEM, itemName);
		pKillMsgItem->SetWidth({ 1, 0.0f });
		pKillMsgItem->SetHeight({ 0, 50.f });
		pKillMsgItem->SetYPosition({ 0, m_killMsgList->GetChildCount() * 50.0f });
		pKillMsgItem->setNormalMsg(pMsg,msg);
		m_killMsgList->AddChildWindow(pKillMsgItem);
		pMsg->window = pKillMsgItem;
	}

}