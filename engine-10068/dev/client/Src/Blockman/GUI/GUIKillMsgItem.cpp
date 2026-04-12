#include "GUIKillMsgItem.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "GUI/GuiItemStack.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"

namespace BLOCKMAN
{
	GUIKillMsgItem::GUIKillMsgItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("KillMsgItem.json"));
		renameComponents(m_window);
		m_message = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_armsHint = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_headHint = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_killHint = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));
		m_poisonHint = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(4));
		m_spikeHint = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(5));

		AddChildWindow(m_window);
		SetTouchable(false);
	}

	void GUIKillMsgItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIKillMsgItem::setKillMsg(KillMsg* msg)
	{
		float right = 0;
		m_killHint->SetVisible(msg->kills > 0);
		m_killHint->SetXPosition({ 0, right });
		if (msg->kills > 0)
		{
			int kills = msg->kills > 5 ? 5 : msg->kills;
			m_killHint->SetImage(StringUtil::Format("set:gun.json image:Kill_%d", kills).c_str());
		}
		right -= msg->kills > 0 ? 50 : 0;
		m_headHint->SetVisible(msg->head > 0);
		m_headHint->SetXPosition({ 0, right });
		right -= msg->head > 0 ? 50 : 0;

		m_poisonHint->SetVisible(msg->poison > 0);
		m_poisonHint->SetXPosition({ 0, right });
		right -= msg->poison > 0 ? 50 : 0;

		m_spikeHint->SetVisible(msg->spike > 0);
		m_spikeHint->SetXPosition({ 0, right });
		right -= msg->spike > 0 ? 50 : 0;

		m_armsHint->SetVisible(msg->armsId > 0);
		m_armsHint->SetXPosition({ 0, right });
		if (msg->armsId > 0)
		{
			GuiItemStack::showItemIcon(m_armsHint, msg->armsId, 1);
		}
		right -= msg->armsId > 0 ? 50 : 0;
		right -= 5;
		m_message->SetXPosition({ 0, right });
		String content = LanguageManager::Instance()->getString(LanguageKey::GUI_KILL_MSG_ITEM_MESSAGE);
		if (msg->deadFame != "" && msg->killerFame != "")
		{
			String  str1 = msg->killer + "(";
			str1 = str1 + LanguageManager::Instance()->getString(msg->killerFame);
			str1 = str1 + ")";

			String  str2 = msg->dead + "(";
			str2 = str2 + LanguageManager::Instance()->getString(msg->deadFame);
			str2 = str2 + ")";

			content = StringUtil::Format(content.c_str(), msg->poison > 0 ? getString("pixel_posion_text").c_str() : str1.c_str(), str2.c_str());

		}
		else
		{
			content = StringUtil::Format(content.c_str(), msg->poison > 0 ? getString("pixel_posion_text").c_str() : msg->killer.c_str(), msg->dead.c_str());
		}
		content = msg->color + content;
		m_message->SetText(content.c_str());
		if (msg->kills == 0 && msg->head == 0 && msg->poison == 0 && msg->spike == 0 && msg->armsId == 0)
		{
			m_message->SetTextHorzAlign(HA_CENTRE);
		}
	}
	void GUIKillMsgItem::setNormalMsg(KillMsg* msg, const String& text)
	{
		String content = StringUtil::Format(getString(text).c_str(), msg->killer.c_str(), msg->dead.c_str());
		content = msg->color + content;
		m_message->SetXPosition({ 0, -50 });
		m_message->SetText(content.c_str());
	}

	void GUIKillMsgItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUIKillMsgItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

}
