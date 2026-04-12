#include "gui_blockFortTip.h"
#include "Util/LanguageKey.h"
#include "GUI/RootGuiLayout.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "Network/ClientNetwork.h"
#include "game.h"
#include "ShellInterface.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_blockFortTip::gui_blockFortTip()
		: gui_layout("BlockFortTip.json")
	{
	}

	gui_blockFortTip::~gui_blockFortTip()
	{
	}

	void gui_blockFortTip::onLoad()
	{
		m_tvTitle = getWindow<GUIStaticText>("BlockFortTip-Title");
		m_tvMsg = getWindow<GUIStaticText>("BlockFortTip-Text");
		m_commonButtonsLayout = getWindow<GUILayout>("BlockFortTip-Common-Buttons");
		m_leftBtn = getWindow<GUIButton>("BlockFortTip-Left-Button");
		m_rightBtn = getWindow<GUIButton>("BlockFortTip-Right-Button");
		setText(m_leftBtn, "gui_blockcity_tip_cancel");
		setText(m_rightBtn, "gui_blockcity_tip_sure");

		getWindow<GUIButton>("BlockFortTip-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortTip::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		m_leftBtn->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortTip::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		m_rightBtn->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortTip::onClick, this, std::placeholders::_1, ViewId::SURE));
		
		m_subscriptionGuard.add(ShowBlockFortCommonTipEvent::subscribe(std::bind(&gui_blockFortTip::showCommonTip, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowBlockFortFunctionTipEvent::subscribe(std::bind(&gui_blockFortTip::showFunctionTip, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowBlockFortLackOfMoneyTipEvent::subscribe(std::bind(&gui_blockFortTip::showLockOfMoneyTip, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(BlockFortSendDeleteFriendIdEvent::subscribe(std::bind(&gui_blockFortTip::setDeleteFriendId, this, std::placeholders::_1)));
	}

	bool gui_blockFortTip::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockFort);
	}

	void gui_blockFortTip::onUpdate(ui32 nTimeElapse)
	{
	}

	bool gui_blockFortTip::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId) {
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->setBlockFortTipVisible(false);
			if (m_tipType == BlockFortTipType::BLOCKFORT_BACK_APP || m_tipType == BlockFortTipType::BLOCKFORT_BACK_HOME || m_tipType == BlockFortTipType::BLOCKFORT_BACK_MAIN_CITY)
			{
				UpdateBlockFortIsShowTransferTipEvent::emit(false);
			}
			break;
		case ViewId::SURE:
			btnSure();
			break;
		}
		return false;
	}

	void gui_blockFortTip::btnSure()
	{
		switch (m_tipType)
		{
		case BlockFortTipType::BLOCKFORT_BACK_APP:
			GameClient::CGame::Instance()->exitGame();
			BlockFortSureTransferEvent::emit();
			RootGuiLayout::Instance()->setBlockFortTipVisible(false);
			break;
		case BlockFortTipType::BLOCKFORT_BACK_HOME:
			ClientNetwork::Instance()->getCommonSender()->sendGameOperation(1);
			BlockFortSureTransferEvent::emit();
			RootGuiLayout::Instance()->setBlockFortTipVisible(false);
			break;
		case BlockFortTipType::BLOCKFORT_BACK_MAIN_CITY:
			ClientNetwork::Instance()->getCommonSender()->sendGameOperation(2);
			BlockFortSureTransferEvent::emit();
			RootGuiLayout::Instance()->setBlockFortTipVisible(false);
			break;
		case BlockFortTipType::BLOCKFORT_LACK_MONEY:
			GameClient::CGame::Instance()->exitGame();
			RootGuiLayout::Instance()->setBlockFortTipVisible(false);
			break;
		case BlockFortTipType::BLOCKFORT_DELETE_FRIEND:
			BlockCityDeleteFriendEvent::emit(m_deleteFriendId);
			RootGuiLayout::Instance()->setBlockFortTipVisible(false);
			break;
		default:
			RootGuiLayout::Instance()->setBlockFortTipVisible(false);
			break;
		}
	}

	bool gui_blockFortTip::showCommonTip(const String & msg)
	{
		m_tipType = BlockFortTipType::COMMON;
		setText(m_tvTitle, "gui_blockcity_tip_title");
		m_tvMsg->SetText(getString(msg).c_str());
		m_rightBtn->SetHorizontalAlignment(HA_CENTRE);
		m_leftBtn->SetVisible(false);
		RootGuiLayout::Instance()->setBlockFortTipVisible(true);
		return true;
	}
	
	bool gui_blockFortTip::showFunctionTip(const String & msg, i32 tipType)
	{
		m_tipType = (BlockFortTipType)tipType;
		setText(m_tvTitle, "gui_blockcity_tip_title");
		m_tvMsg->SetText(getString(msg).c_str());
		m_rightBtn->SetHorizontalAlignment(HA_RIGHT);
		m_leftBtn->SetVisible(true);
		RootGuiLayout::Instance()->setBlockFortTipVisible(true);
		return true;
	}

	bool gui_blockFortTip::showLockOfMoneyTip(i32 difference, i32 currencyType)
	{
		m_tipType = BlockFortTipType::BLOCKFORT_LACK_MONEY;
		m_currencyType = (CurrencyType)currencyType;
		m_price = difference;
		if (currencyType == 1)
		{
			GameClient::CGame::Instance()->getShellInterface()->onRecharge(1);
		}
		else
		{
			setText(m_tvTitle, "gui_blockcity_tip_title");
			setText(m_tvMsg, "gui_blockfort_lock_money");
			m_rightBtn->SetHorizontalAlignment(HA_RIGHT);
			m_leftBtn->SetVisible(true);
			RootGuiLayout::Instance()->setBlockFortTipVisible(true);
		}
		return true;
	}

	bool gui_blockFortTip::setDeleteFriendId(i64 userId)
	{
		m_deleteFriendId = userId;
		return true;
	}

}
