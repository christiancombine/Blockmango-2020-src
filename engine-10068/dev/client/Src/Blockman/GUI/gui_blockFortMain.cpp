#include "gui_blockFortMain.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/RootGuiLayout.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "cWorld/Blockman.h"
#include "Network/ClientNetwork.h"
#include "GuiDef.h"
#include "game.h"
#include "Util/TipType.h"
#include "Util/RedDotManager.h"

namespace BLOCKMAN
{
	gui_blockFortMain::gui_blockFortMain()
		: gui_layout("BlockFortMain.json")
	{
	}

	gui_blockFortMain::~gui_blockFortMain()
	{
	}

	void gui_blockFortMain::onLoad()
	{
		setText("BlockFortMain-Transfer-Name", "gui_blockfort_transfer");
		setText("BlockFortMain-Transfer-Home-Name", "gui_blockfort_transfer_home");
		setText("BlockFortMain-Transfer-Main-City-Name", "gui_blockfort_transfer_main_city");
		setText("BlockFortMain-Transfer-App-Name", "gui_blockfort_trnasfer_app");
		setText("BlockFortMain-Manor-Upgrade-Name", "gui_blockfort_manor_upgrade");
		setText("BlockFortMain-Store-Name", "gui_blockfort_shop");
		setText("BlockFortMain-Friend-Name", "gui_blockfort_friend");
		setText("BlockFortMain-Attack-Name", "gui_blockfort_attack");

		m_transfer = getWindow<GUILayout>("BlockFortMain-Transfer");
		m_head = getWindow<GuiUrlImage>("BlockFortMain-Player-UrlImage");
		m_head->setDefaultImage("set:blockcity_friend.json image:head_icon");
		m_head->setChildMaterial(GUIMaterialType_CULL_BACK);
		m_level = getWindow<GUIStaticText>("BlockFortMain-Player-Level");
		m_name = getWindow<GUIStaticText>("BlockFortMain-Player-Name");
		m_exp = getWindow<GUIProgressBar>("BlockFortMain-Player-Exp");
		m_expValue = getWindow<GUIStaticText>("BlockFortMain-Player-Exp-Value");
		m_transferCb = getWindow<GUICheckBox>("BlockFortMain-Show-Hide-Btn");

		m_transferCb->subscribeEvent(EventCheckStateChanged, std::bind(&gui_blockFortMain::onTransferBtnsCheckChange, this, std::placeholders::_1));
		getWindow("BlockFortMain-Transfer-Home")->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortMain::onClickTransfer, this, std::placeholders::_1, 1));
		getWindow("BlockFortMain-Transfer-Main-City")->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortMain::onClickTransfer, this, std::placeholders::_1, 2));
		getWindow("BlockFortMain-Transfer-App")->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortMain::onClickTransfer, this, std::placeholders::_1, 3));
		getWindow("BlockFortMain-Manor-Upgrade")->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortMain::onClickBtn, this, std::placeholders::_1, ViewId::MANOR_UPGRADE));
		getWindow("BlockFortMain-Friend")->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortMain::onClickBtn, this, std::placeholders::_1, ViewId::FRIEND));
		getWindow("BlockFortMain-Store")->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortMain::onClickBtn, this, std::placeholders::_1, ViewId::STORE));
		getWindow("BlockFortMain-Attack")->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortMain::onClickBtn, this, std::placeholders::_1, ViewId::ATTACK));
		m_subscriptionGuard.add(BlockFortSureTransferEvent::subscribe(std::bind(&gui_blockFortMain::sureTransfer, this)));
		m_subscriptionGuard.add(UpdateBlockFortIsShowTransferTipEvent::subscribe(std::bind(&gui_blockFortMain::updateIsShowTransferTip, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UpdateBlockFortPlayerInfoEvent::subscribe(std::bind(&gui_blockFortMain::onDataChange, this)));
	}

	void gui_blockFortMain::onShow()
	{
		if (m_dataChange)
		{
			m_dataChange = false;
			onSetPlayerInfo();
		}
	
	}

	void gui_blockFortMain::onUpdate(ui32 nTimeElapse)
	{
		m_transfer->SetWidth(UDim(0, 53.f));
		if (m_transferBtnsLoadTime > 0)
		{
			m_transferBtnsLoadTime -= nTimeElapse;
			m_transferBtnsLoadTime = m_transferBtnsLoadTime > 0 ? m_transferBtnsLoadTime : 0;
		}
		float width = 0.f;
		if (m_isShowTransferBtns)
		{
			width = 53.f + 258.f * (1.f - m_transferBtnsLoadTime / 300.f);
			if (width == 311.f)
			{
				setText("BlockFortMain-Transfer-Home-Name", "gui_blockfort_transfer_home");
				setText("BlockFortMain-Transfer-Main-City-Name", "gui_blockfort_transfer_main_city");
				setText("BlockFortMain-Transfer-App-Name", "gui_blockfort_trnasfer_app");
			}
		}
		else
		{
			width = 53.f + 258.f * m_transferBtnsLoadTime / 300.f;
		}
		m_transfer->SetWidth(UDim(0, width));
		getWindow<GUIButton>("BlockFortMain-Transfer-Home")->SetEnabled(m_isShowTransferBtns);
		getWindow<GUIButton>("BlockFortMain-Transfer-Main-City")->SetEnabled(m_isShowTransferBtns);
		getWindow<GUIButton>("BlockFortMain-Transfer-App")->SetEnabled(m_isShowTransferBtns);

		if (m_transferBtnsTime > 0 && !m_isOpenTransferTip && !RootGuiLayout::Instance()->isGuiding())
		{
			m_transferBtnsTime -= nTimeElapse;
			m_transferBtnsTime = m_transferBtnsTime > 0 ? m_transferBtnsTime : 0;
			if (m_transferBtnsTime <= 0 && m_isShowTransferBtns)
			{
				m_transferCb->SetChecked(false);
			}
		}

		getWindow("BlockFortMain-Friend-RedDot")->SetVisible(RedDotManager::checkUserRequest());

	}

	bool gui_blockFortMain::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockFort);
	}

	bool gui_blockFortMain::onDataChange()
	{
		m_dataChange = true;
		onShow();
		return true;
	}

	void gui_blockFortMain::onSetPlayerInfo()
	{
		m_head->setUrl(GameClient::CGame::Instance()->getPicUrl());
		m_name->SetText(GameClient::CGame::Instance()->getPlayerName().c_str());

		if (!UICommon::checkBlockFortParam())
		{
			return;
		}
		auto playerInfo = Blockman::Instance()->m_pPlayer->m_blockFort->getPlayerInfo();
		if (playerInfo)
		{
			m_level->SetText(StringUtil::Format("Lv.%d", playerInfo->level).c_str());
			if (playerInfo->totalExp > 0)
			{
				m_expValue->SetText(StringUtil::Format("%d/%d", playerInfo->exp, playerInfo->totalExp).c_str());
				m_exp->SetProgress(playerInfo->exp / (float)playerInfo->totalExp);
			}
		}
	}

	bool gui_blockFortMain::onTransferBtnsCheckChange(const EventArgs &args)
	{
		m_transferBtnsLoadTime = 300;
		m_isShowTransferBtns = !m_isShowTransferBtns;
		if (m_isShowTransferBtns)
		{
			m_transferBtnsTime = 5000;
		}
		setText("BlockFortMain-Transfer-Home-Name", "");
		setText("BlockFortMain-Transfer-Main-City-Name", "");
		setText("BlockFortMain-Transfer-App-Name", "");
		return true;
	}

	bool gui_blockFortMain::onClickTransfer(const EventArgs &args, i32 place)
	{
		if (Blockman::Instance()->m_pPlayer->isOnDecoration())
		{
			ShowFloatTipEvent::emit(getString("gui_call_on_in_decoration").c_str());
		}
		else if (BLOCKMAN::Blockman::Instance()->m_pPlayer->isOnVehicle())
		{
			ShowFloatTipEvent::emit(getString("gui_manor_call_tp_in_vehicle").c_str());
		}
		else
		{
			updateIsShowTransferTip(true);
			switch (place)
			{
			case 1:
				ShowBlockFortFunctionTipEvent::emit("gui_blockcity_if_transfer", (i32)BlockFortTipType::BLOCKFORT_BACK_HOME);
				break;
			case 2:
				ShowBlockFortFunctionTipEvent::emit("gui_blockfort_if_transfer_main_city", (i32)BlockFortTipType::BLOCKFORT_BACK_MAIN_CITY);
				break;
			case 3:
				ShowBlockFortFunctionTipEvent::emit("gui_blockfort_if_transfer_app", (i32)BlockFortTipType::BLOCKFORT_BACK_APP);
				break;
			}
		}
		return true;
	}

	bool gui_blockFortMain::onClickBtn(const EventArgs &args, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::MANOR_UPGRADE:
			RootGuiLayout::Instance()->showBlockFortManorUpgrade();
			break;
		case ViewId::STORE:
			RootGuiLayout::Instance()->showBlockFortStore();
			break;
		case ViewId::FRIEND:
			RootGuiLayout::Instance()->showBlockCityFriend();
			break;
		case ViewId::ATTACK:
			ShowFloatTipEvent::emit(getString("gui_blockcity_not_open_fun").c_str());
			break;
		}
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_blockFortMain::updateIsShowTransferTip(bool isHave)
	{
		m_isOpenTransferTip = isHave;
		if (!m_isOpenTransferTip && m_isShowTransferBtns)
		{
			m_transferBtnsTime = 5000;
		}
		return true;
	}

	bool gui_blockFortMain::sureTransfer()
	{
		if (m_isShowTransferBtns)
		{
			m_transferCb->SetChecked(false);
		}
		return true;
	}
}