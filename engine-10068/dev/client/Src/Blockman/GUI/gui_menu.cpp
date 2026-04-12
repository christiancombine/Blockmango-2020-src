#include "gui_menu.h"
#include "game.h"
#include "gui_gameSetting.h"
#include "gui_playerList.h"
#include "Util/LanguageKey.h"
#include "Util/ClientDataReport.h"
#include <functional>

using namespace LORD;

namespace BLOCKMAN {

	gui_menu::gui_menu()
		: gui_layout("Menu.json")
	{
		m_radioPanelMap[ViewId::PLAYER_LIST] = m_radioPanelMap[ViewId::PLAYER_LIST] ? m_radioPanelMap[ViewId::PLAYER_LIST] : LordNew gui_playerList;
		m_radioPanelMap[ViewId::GAME_SETTING] = m_radioPanelMap[ViewId::GAME_SETTING] ? m_radioPanelMap[ViewId::GAME_SETTING] : LordNew gui_gameSetting;
	}

	gui_menu::~gui_menu()
	{
		map<ViewId, gui_layout*>::iterator  iter;
		for (iter = m_radioPanelMap.begin(); iter != m_radioPanelMap.end(); iter++)
		{
			LordSafeDelete(iter->second);
		}
	}

	void gui_menu::onLoad()
	{

		/*m_radioMap[ViewId::PLAYER_LIST] = getWindow<GUIRadioButton>("Menu-Radio-Player");
		m_radioMap[ViewId::ACHIEVEMENT] = getWindow<GUIRadioButton>("Menu-Radio-Achievement");
		m_radioMap[ViewId::COURSE] = getWindow<GUIRadioButton>("Menu-Radio-Course");
		m_radioMap[ViewId::GAME_SETTING] = getWindow<GUIRadioButton>("Menu-Radio-Setting");
		m_radioMap[ViewId::GAME_RESUME] = getWindow<GUIRadioButton>("Menu-Radio-Resume-Game");
		m_radioMap[ViewId::GAME_EXIT] = getWindow<GUIRadioButton>("Menu-Radio-Exit");

		getWindow<GUIRadioButton>("Menu-Radio-Player-Text")->SetText(getString(LanguageKey::WORDART_PLAYER_LIST));
		getWindow<GUIRadioButton>("Menu-Radio-Achievement-Text")->SetText(getString(LanguageKey::WORDART_ACHIEVEMENT));
		getWindow<GUIRadioButton>("Menu-Radio-Course-Text")->SetText(getString(LanguageKey::WORDART_COURSE));
		getWindow<GUIRadioButton>("Menu-Radio-Setting-Text")->SetText(getString(LanguageKey::WORDART_GAME_SETTING));
		getWindow<GUIRadioButton>("Menu-Radio-Exit-Text")->SetText(getString(LanguageKey::WORDART_EXIT_GAME));
		getWindow<GUIRadioButton>("Menu-Radio-Resume-Game-Text")->SetText(getString(LanguageKey::WORDART_RESUME_GAME));

		auto radioPanel = getWindow("Menu-Radio-Panel");
		map<ViewId,GUIRadioButton*>::iterator  iter;
		for (iter = m_radioMap.begin(); iter != m_radioMap.end(); iter++)
		{
			iter->second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_menu::onRadioChange, this, std::placeholders::_1, iter->first));
			if (m_radioPanelMap[iter->first])
			{
				m_radioPanelMap[iter->first]->attachTo(this, radioPanel);
			}
		}
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain)
		{
			m_radioMap[ViewId::GAME_SETTING]->SetSelected(true);
			m_radioMap[ViewId::GAME_SETTING]->SetArea(UDim(0, 6), UDim(0, 6), UDim(1.0f, -6.0f), UDim(0.0f, 92.f));
		}
		else
		{
			m_radioMap[ViewId::PLAYER_LIST]->SetSelected(true);
		}*/
	}

	bool gui_menu::onRadioChange(const EventArgs & events, const ViewId radioId)
	{
		if (m_radioMap[radioId]->IsSelected())
		{
			map<ViewId, GUIRadioButton*>::iterator  iter;
			for (iter = m_radioMap.begin(); iter != m_radioMap.end(); iter++)
			{
				String name = iter->second->GetName().c_str();
				String textName = name + "-Text";
				String arrowName = name + "-Arrow";
				getWindow<GUIRadioButton>(textName.c_str())->SetSelected(iter->first == radioId);
				getWindow(arrowName.c_str())->SetVisible(iter->first == radioId);

				if (m_radioPanelMap[iter->first] && m_radioPanelMap[iter->first]->isShown() )
				{
					m_radioPanelMap[iter->first]->hide();
				}
			}
			if (m_radioPanelMap[radioId])
			{
				m_radioPanelMap[radioId]->show();
			}
			playSoundByType(ST_Click);
			onRadioSelected(radioId);
		}
		return true;
	}


	void gui_menu::onRadioSelected(const ViewId radioId)
	{
		switch (radioId)
		{
		case ViewId::PLAYER_LIST:
			break;
		case ViewId::ACHIEVEMENT:
			break;
		case ViewId::COURSE:
			break;
		case ViewId::GAME_SETTING:
			break;
		case ViewId::GAME_RESUME:
			getParent()->showMainControl();
			if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain)
			{
				m_radioMap[ViewId::GAME_SETTING]->SetSelected(true);
				m_radioMap[ViewId::GAME_SETTING]->SetArea(UDim(0, 6), UDim(0, 6), UDim(1.0f, -6.0f), UDim(0.0f, 92.f));
			}
			else
			{
				m_radioMap[ViewId::PLAYER_LIST]->SetSelected(true);
			}
			break;
		case ViewId::GAME_EXIT:
			getParent()->showGameCloseTipDialog();
			if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain)
			{
				m_radioMap[ViewId::GAME_SETTING]->SetSelected(true);
				m_radioMap[ViewId::GAME_SETTING]->SetArea(UDim(0, 6), UDim(0, 6), UDim(1.0f, -6.0f), UDim(0.0f, 92.f));
			}
			else
			{
				m_radioMap[ViewId::PLAYER_LIST]->SetSelected(true);
			}
			break;
		}
		playSoundByType(ST_Click);
	}

	void gui_menu::checkBackgroundMusic()
	{
		if (m_radioPanelMap[ViewId::GAME_SETTING])
		{
			m_radioPanelMap[ViewId::GAME_SETTING]->checkBackgroundMusic();
		}
	}

	gui_layout * gui_menu::getRadioPanel(ViewId id)
	{
		return m_radioPanelMap[id];
	}

	void gui_menu::openMenu()
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain)
		{
			m_radioMap[ViewId::GAME_SETTING]->SetSelected(true);
			m_radioMap[ViewId::GAME_SETTING]->SetArea(UDim(0, 6), UDim(0, 6), UDim(1.0f, -6.0f), UDim(0.0f, 92.f));
		}
		else
		{
			m_radioMap[ViewId::PLAYER_LIST]->SetSelected(true);
		}
	}
}
