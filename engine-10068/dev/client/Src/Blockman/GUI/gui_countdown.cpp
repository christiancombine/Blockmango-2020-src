#include "gui_countdown.h"
#include "game.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Setting/GameTypeSetting.h"
#include "UI/GUILayout.h"

namespace BLOCKMAN
{

	gui_countdown::gui_countdown() : gui_layout("Countdown.json")
	{
	}

	gui_countdown::~gui_countdown()
	{
	}

	void gui_countdown::onLoad()
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BED_WAR)
		{
			getWindow<GUIStaticText>("Countdown-Last-Time")->SetVisible(false);
			getWindow<GUIStaticText>("Countdown-Respawn")->SetVisible(false);
			getWindow<GUILayout>("Countdown")->SetProperty(GWP_WINDOW_BACKGROUND_COLOR, "0 0 0 0");
			m_lastTime = getWindow<GUIStaticText>("Countdown-BedWar-Last-Time");
			m_tip = getWindow<GUIStaticText>("Countdown-BedWar-Respawn");
			m_tip->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_LACK_ENCHANTMENT_ITEM).c_str());
		}
		else
		{
			getWindow<GUIStaticText>("Countdown-BedWar-Last-Time")->SetVisible(false);
			getWindow<GUIStaticText>("Countdown-BedWar-Respawn")->SetVisible(false);
			if ((checkGameType(ClientGameType::SkyBlockMain) || checkGameType(ClientGameType::SkyBlockMineCave) || checkGameType(ClientGameType::SkyBlockProduct)))
			{
				getWindow<GUIStaticText>("Countdown-Last-Time")->SetVisible(false);
				getWindow<GUIStaticText>("Countdown-Respawn")->SetVisible(false);
			}
			m_lastTime = getWindow<GUIStaticText>("Countdown-Last-Time");
			m_tip = getWindow<GUIStaticText>("Countdown-Respawn");
			m_tip->SetText(LanguageManager::Instance()->getString(LanguageKey::WAIT_REBIRTH).c_str());

		}
	}

	void gui_countdown::onUpdate(ui32 nTimeElapse)
	{
		lastTime = lastTime > 0 ? lastTime - nTimeElapse : 0;
		if (lastTime > 0)
		{
			m_lastTime->SetText(StringUtil::ToString((lastTime / 1000) + 1).c_str());
			canRespawn = true;
		}
		else
		{
			if (canRespawn)
			{
				if (GameClient::CGame::Instance()->GetGameType() != ClientGameType::BirdSimulator)
				{

					if (m_keepItemAndReviveType == BUY_REVIVE_COUNTDOWN)
						GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuyRespawn(false, -1, -1);
				}
				getParent()->hideCountdown();
				canRespawn = false;
			}
		}
	}

	void gui_countdown::setCountdown(i8 second)
	{
		if (second == 0)
		{
			canRespawn = true;
			return;
		}
		lastTime = second * 1000;
	}

	void gui_countdown::setCountdown(i8 second, int type)
	{
		m_keepItemAndReviveType = type;
		if (m_keepItemAndReviveType == CANCEL_COUNTDOWN)
		{
			canRespawn = false;
			lastTime = 0;
			getParent()->hideCountdown();
			return;
		}
		
		canRespawn = true;

		m_lastTime->SetVisible(true);
		m_tip->SetVisible(true);
		if (m_keepItemAndReviveType == AUTO_REVIVE_COUNTDOWN)
			m_tip->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_REVIVE_COUNT_DOWN).c_str());
		else if (m_keepItemAndReviveType == BUY_REVIVE_COUNTDOWN)
			m_tip->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_KEEP_ITEM_AND_REVIVE_COUNT_DOWN).c_str());
		else if (m_keepItemAndReviveType == HAVE_PRIVILEGE)
		{
			m_lastTime->SetVisible(false);
			m_tip->SetVisible(false);
		}


		lastTime = second * 1000;
	}

}
