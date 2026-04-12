#include "gui_blockCityManor.h"
#include "RootGuiLayout.h"
#include "Util/UICommon.h"
#include "Setting/HelpSetting.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Game/BlockCity.h"
#include "Network/ClientNetwork.h"
#include "Util/TipType.h"
#include "game.h"
#include "ShellInterface.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "UI/GUIRadioButton.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_blockCityManor::gui_blockCityManor() :
		gui_layout("BlockCityManor.json")
	{
	}


	gui_blockCityManor::~gui_blockCityManor()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_blockCityManor::onShow()
	{
		if (m_isManorUpdate)
		{
			showBlockCityManor();
			m_isManorUpdate = false;
		}
	}

	void gui_blockCityManor::onHide()
	{
	}

	void gui_blockCityManor::onLoad()
	{
		setText("BlockCityManor-Frame-Title-Text", "gui_blockcity_manor");
		setText("BlockCityManor-Cur-Info-Head-Name", "gui_blockcity_manor_cur_head");
		setText("BlockCityManor-Cur-Info-Class-Name", "gui_blockcity_manor_class");
		setText("BlockCityManor-Cur-Info-Area-Name", "gui_blockcity_manor_area");
		setText("BlockCityManor-Cur-Manor-Grade-Name", "gui_blockcity_manor_grade");
		setText("BlockCityManor-Cur-Block-Grade-Name", "gui_blockcity_manor_block_grade");
		setText("BlockCityManor-Cur-Decorate-Grade-Name", "gui_blockcity_manor_decorate_grade");
		setText("BlockCityManor-Cur-Total-Grade-Name", "gui_blockcity_manor_total_grade");
		setText("BlockCityManor-Cur-Open-Status-Name", "gui_blockcity_manor_open_status");
		setText("BlockCityManor-Next-Info-Head-Name", "gui_blockcity_manor_next_head");
		setText("BlockCityManor-Next-Info-Class-Name", "gui_blockcity_manor_class");
		setText("BlockCityManor-Next-Info-Area-Name", "gui_blockcity_manor_area");
		setText("BlockCityManor-Next-Manor-Grade-Name", "gui_blockcity_manor_grade");
		setText("BlockCityManor-Mod-Open-Status-Head-Name", "gui_blockcity_manor_mod_open_status");
		setText("BlockCityManor-Open-Status-All-Name", "gui_blockcity_manor_open_status_all");
		setText("BlockCityManor-Open-Status-Friend-Name", "gui_blockcity_manor_open_status_friend");
		setText("BlockCityManor-Open-Status-Close-Name", "gui_blockcity_manor_open_status_close");

		m_curClass = getWindow<GUIStaticText>("BlockCityManor-Cur-Info-Class-Value");
		m_curArea = getWindow<GUIStaticText>("BlockCityManor-Cur-Info-Area-Value");
		m_curManorGrade = getWindow<GUIStaticText>("BlockCityManor-Cur-Manor-Grade-Value");
		m_curBlockGrade = getWindow<GUIStaticText>("BlockCityManor-Cur-Block-Grade-Value");
		m_curDecorateGrade = getWindow<GUIStaticText>("BlockCityManor-Cur-Decorate-Grade-Value");
		m_curTotalGrade = getWindow<GUIStaticText>("BlockCityManor-Cur-Total-Grade-Value");
		m_curOpenStatus = getWindow<GUIStaticText>("BlockCityManor-Cur-Open-Status-Value");
		m_nextClass = getWindow<GUIStaticText>("BlockCityManor-Next-Info-Class-Value");
		m_nextArea = getWindow<GUIStaticText>("BlockCityManor-Next-Info-Area-Value");
		m_nextManorGrade = getWindow<GUIStaticText>("BlockCityManor-Next-Manor-Grade-Value");
		m_openStatusAll = getWindow<GUIStaticImage>("BlockCityManor-Open-Status-All-Image");
		m_openStatusFriend = getWindow<GUIStaticImage>("BlockCityManor-Open-Status-Friend-Image");
		m_openStatusClose = getWindow<GUIStaticImage>("BlockCityManor-Open-Status-Close-Image");
		m_btnUpgrade = getWindow<GUIButton>("BlockCityManor-Next-Upgrade");
		m_radioMap[ViewId::ALL] = getWindow<GUIRadioButton>("BlockCityManor-Open-Status-All");
		m_radioMap[ViewId::FRIEND] = getWindow<GUIRadioButton>("BlockCityManor-Open-Status-Friend");
		m_radioMap[ViewId::NOACCESS] = getWindow<GUIRadioButton>("BlockCityManor-Open-Status-Close");

		for (auto item : m_radioMap)
		{
			item.second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockCityManor::onRadioChange, this, std::placeholders::_1, item.first));
		}
		m_subscriptionGuard.add(BlockCityShowManorEvent::subscribe(std::bind(&gui_blockCityManor::open, this)));
		m_subscriptionGuard.add(UpdateBlockCityManorEvent::subscribe(std::bind(&gui_blockCityManor::updateBlockCityManor, this)));
		getWindow("BlockCityManor-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityManor::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		m_btnUpgrade->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityManor::onClick, this, std::placeholders::_1, ViewId::BTN_UPGRADE));
		getWindow("BlockCityManor-Del")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityManor::onClick, this, std::placeholders::_1, ViewId::BTN_DEL));
		getWindow("BlockCityManor-Save-File")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityManor::onClick, this, std::placeholders::_1, ViewId::SAVE_FILE));
	}

	void gui_blockCityManor::onUpdate(ui32 nTimeElapse)
	{
		m_openStatusAll->SetImage(m_radioMap[ViewId::ALL]->IsSelected() ? "set:blockcity_manor.json image:open_sure" : "set:blockcity_manor.json image:open_no");
		m_openStatusFriend->SetImage(m_radioMap[ViewId::FRIEND]->IsSelected() ? "set:blockcity_manor.json image:open_sure" : "set:blockcity_manor.json image:open_no");
		m_openStatusClose->SetImage(m_radioMap[ViewId::NOACCESS]->IsSelected() ? "set:blockcity_manor.json image:open_sure" : "set:blockcity_manor.json image:open_no");
	}

	bool gui_blockCityManor::onClick(const EventArgs& args, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			break;
		case ViewId::BTN_DEL:
			if (UICommon::checkBlockCityIsInManor())
			{
				vector<BlockCityPayment>::type pays;
				ShowBlockCityShowCommonTipEvent::emit(pays, (i32)BlockCityTipType::BLOCKCITY_DELETE_MANOR, "gui_blockcity_if_delete");
			}
			else
			{
			    ShowBlockCityTipEvent::emit("gui_blockcity_operation_in_manor");
			}
			break;
		case ViewId::BTN_UPGRADE:
			if (UICommon::checkBlockCityIsInManor())
			{
				if (UICommon::checkBlockCityParam())
				{
					auto manor = Blockman::Instance()->m_pPlayer->m_blockCity->getManorManager();
					if (manor)
					{
						UICommon::checkMoney(manor->upgradeMoney, manor->upgradeMoneyType, [=](bool isSuccess) {
							if (isSuccess)
							{
								ClientNetwork::Instance()->getCommonSender()->sendBlockCityManorBuyOperation(2, manor->upgradeMoneyType);
							}
							else
							{
								vector<BlockCityPayment>::type pays;
								BlockCityPayment pay;
								pay.price = (i32)Math::Ceil(manor->upgradeMoney / 100.f);
								pay.moneyType = 1;
								pays.push_back(pay);
								ShowBlockCityShowCommonTipEvent::emit(pays, (i32)BlockCityTipType::BLOCKCITY_LACK_MONEY_UPGRADE, "gui_blockcity_lock_money_tip");
							}
						});
					}
				}
			}
			else
			{
				ShowBlockCityTipEvent::emit("gui_blockcity_operation_in_manor");
			}
			break;
		case ViewId::SAVE_FILE:
			RootGuiLayout::Instance()->setShowBlockCitySaveFile(true);
			break;
		}
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_blockCityManor::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	bool gui_blockCityManor::onRadioChange(const EventArgs & events, ViewId viewId)
	{
		if (m_radioMap[viewId]->IsSelected())
		{
			//i32 openType = 1;
			//switch (viewId)
			//{
			//case BLOCKMAN::gui_blockCityManor::FRIEND:
			//	openType = 2;
			//	break;
			//case BLOCKMAN::gui_blockCityManor::NOACCESS:
			//	openType = 3;
			//	break;
			//}
			//ClientNetwork::Instance()->getCommonSender()->sendBlockCityModOpenLimit(openType);
			if (viewId != ViewId::ALL)
			{
				m_radioMap[ALL]->SetSelected(true);
				ShowBlockCityTipEvent::emit("gui_blockcity_not_open_fun");
			}
		}
		return true;
	}

	bool gui_blockCityManor::open()
	{
		RootGuiLayout::Instance()->showBlockCityManor();
		return true;
	}

	bool gui_blockCityManor::updateBlockCityManor()
	{
		m_isManorUpdate = true;
		onShow();
		return false;
	}

	void gui_blockCityManor::showBlockCityManor()
	{
		if (!UICommon::checkBlockCityParam())
		{
			return;
		}

		auto manor = Blockman::Instance()->m_pPlayer->m_blockCity->getManorManager();
		if (manor)
		{
			m_curClass->SetText(StringUtil::Format("Lv.%d", manor->curManor.level).c_str());
			m_curArea->SetText(manor->curManor.area.c_str());
			m_curManorGrade->SetText(StringUtil::ToString(manor->curManor.score).c_str());
			m_curBlockGrade->SetText(StringUtil::ToString(manor->blockScore).c_str());
			m_curDecorateGrade->SetText(StringUtil::ToString(manor->decorateScore).c_str());
			m_curTotalGrade->SetText(StringUtil::ToString(manor->allScore).c_str());
			m_nextClass->SetText(StringUtil::Format("Lv.%d", manor->nextManor.level).c_str());
			m_nextArea->SetText(manor->nextManor.area.c_str());
			m_nextManorGrade->SetText(StringUtil::ToString(manor->nextManor.score).c_str());
			m_btnUpgrade->SetText(StringUtil::ToString(manor->upgradeMoney).c_str());
			getWindow<GUIStaticImage>("BlockCityManor-Next-Upgrade-Money")->SetImage(UICommon::getCurrencyIconByType(manor->upgradeMoneyType).c_str());

			if (manor->curManor.level >= manor->maxLevel)
			{
				m_nextClass->SetVisible(false);
				getWindow<GUIStaticImage>("BlockCityManor-Next-Info-Class")->SetVisible(false);
				getWindow<GUIStaticImage>("BlockCityManor-Next-Info-Area")->SetVisible(false);
				getWindow<GUIStaticImage>("BlockCityManor-Next-Manor-Grade")->SetVisible(false);
				setText("BlockCityManor-Next-Info-Head-Name", "gui_blockcity_manor_max_level");
				m_btnUpgrade->SetVisible(false);
			}

			switch (manor->openStatus)
			{
			case 1:
				setText(m_curOpenStatus, "gui_blockcity_manor_open_status_all");
				m_radioMap[ViewId::ALL]->SetSelected(true);
				break;
			case 2:
				setText(m_curOpenStatus, "gui_blockcity_manor_open_status_friend");
				m_radioMap[ViewId::FRIEND]->SetSelected(true);
				break;
			case 3:
				setText(m_curOpenStatus, "gui_blockcity_manor_open_status_close");
				m_radioMap[ViewId::NOACCESS]->SetSelected(true);
				break;
			default:
				break;
			}

		}
	}

}
