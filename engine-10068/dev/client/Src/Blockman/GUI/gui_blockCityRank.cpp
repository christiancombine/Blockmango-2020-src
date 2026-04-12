#include "gui_blockCityRank.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Inventory/InventoryPlayer.h"
#include "Util/LanguageKey.h"
#include "game.h"
#include "Setting/GameTypeSetting.h"
#include "Util/UICommon.h"
#include "GUI/gui_blockCityRankFriend.h"
#include "GUI/gui_blockCityRankGlobal.h"
#include "Network/ClientHttpRequest.h"
#include "GuiDef.h"
#include "UI/GUIRadioButton.h"
#include "Util/ClientEvents.h"
#include "UI/GUILayout.h"
#include "GUI/RootGuiLayout.h"

namespace BLOCKMAN
{
	gui_blockCityRank::gui_blockCityRank()
		: gui_layout("BlockCityRank.json")
	{
		m_radioPanelMap[ViewId::FRIEND] = m_radioPanelMap[ViewId::FRIEND] ? m_radioPanelMap[ViewId::FRIEND] : LordNew gui_blockCityRankFriend;
		m_radioPanelMap[ViewId::GLOBAL] = m_radioPanelMap[ViewId::GLOBAL] ? m_radioPanelMap[ViewId::GLOBAL] : LordNew gui_blockCityRankGlobal;
	}

	gui_blockCityRank::~gui_blockCityRank()
	{
		map<ViewId, gui_layout*>::iterator  iter;
		for (iter = m_radioPanelMap.begin(); iter != m_radioPanelMap.end(); iter++)
		{
			LordSafeDelete(iter->second);
		}
	}

	void gui_blockCityRank::onLoad()
	{
		setText("BlockCityRank-Title-Text", "gui_blockcity_rank");
		setText("BlockCityRank-Friend", "gui_blockcity_rank_friend");
		setText("BlockCityRank-Global", "gui_blockcity_rank_global");


		m_radioTabMap[ViewId::FRIEND] = getWindow<GUIRadioButton>("BlockCityRank-Friend");
		m_radioTabMap[ViewId::GLOBAL] = getWindow<GUIRadioButton>("BlockCityRank-Global");

		getWindow("BlockCityRank-Charm-Flower")->SetVisible(false);
		getWindow("BlockCityRank-Charm-Award")->SetVisible(false);

		getWindow("BlockCityRank-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityRank::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow("BlockCityRank-Charm-Flower")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityRank::onClick, this, std::placeholders::_1, ViewId::FLOWER));
		getWindow("BlockCityRank-Charm-Award")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityRank::onClick, this, std::placeholders::_1, ViewId::AWARD));
		m_subscriptionGuard.add(IsShowBlockCityCharmRankEvent::subscribe(std::bind(&gui_blockCityRank::isShowCharmButton, this, std::placeholders::_1)));

		auto radioPanel = getWindow<GUILayout>("BlockCityRank-Panel");
		map<ViewId, GUIRadioButton*>::iterator  iter;
		for (iter = m_radioTabMap.begin(); iter != m_radioTabMap.end(); iter++)
		{
			if (m_radioPanelMap[iter->first])
			{
				m_radioPanelMap[iter->first]->attachTo(this, radioPanel);
				m_radioPanelMap[iter->first]->hide();
			}

			iter->second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockCityRank::onRadioChange, this, std::placeholders::_1, iter->first));
		}
		m_radioTabMap[ViewId::FRIEND]->SetSelected(true);
	}

	void gui_blockCityRank::onShow()
	{
		if (!UICommon::checkBlockCityParam())
		{
			return;
		}
// 		if (auto playerInfo = Blockman::Instance()->m_pPlayer->m_blockCity->getPlayerInfo())
// 		{
// 			if (playerInfo->lastWeekRank > 0)
// 			{
// 				UpdateBlockCityMyLastCharmRankEvent::emit(playerInfo->lastWeekRank);
// 			}
// 		}
	}

	bool gui_blockCityRank::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	bool gui_blockCityRank::onRadioChange(const EventArgs & events, ViewId viewId)
	{
		if (m_radioTabMap[viewId]->IsSelected())
		{
			map<ViewId, GUIRadioButton*>::iterator  iter;
			for (iter = m_radioTabMap.begin(); iter != m_radioTabMap.end(); iter++)
			{

				if (m_radioPanelMap[iter->first] && m_radioPanelMap[iter->first]->isShown())
				{
					m_radioPanelMap[iter->first]->hide();
				}
			}
			if (m_radioPanelMap[viewId])
			{
				m_radioPanelMap[viewId]->show();
			}
			playSoundByType(ST_Click);
		}
		return true;
	}

	bool gui_blockCityRank::onClick(const EventArgs& events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			UpdateIsHaveViewShowEvent::emit(false);
			playSoundByType(ST_Click);
			break;
		case ViewId::FLOWER:
			RootGuiLayout::Instance()->setShowBlockCityReceiveFlower(true);
			playSoundByType(ST_Click);
			break;
		case ViewId::AWARD:
			RootGuiLayout::Instance()->setShowBlockCityCharmRank(true);
			playSoundByType(ST_Click);
			break;
		default:
			break;
		}
		return true;
	}

	bool gui_blockCityRank::isShowCharmButton(bool isShow)
	{
		//getWindow<GUILayout>("BlockCityRank-Charm-Button")->SetVisible(isShow);
		return true;
	}
}