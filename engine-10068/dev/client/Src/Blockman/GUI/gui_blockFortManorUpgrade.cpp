#include "gui_blockFortManorUpgrade.h"
#include "GUI/GuiBlockFortManorUpgradeItem.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/RootGuiLayout.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "cWorld/Blockman.h"
#include "Network/ClientNetwork.h"
#include "Util/TipType.h"
#include "GuiDef.h"
#include "game.h"
#include "ShellInterface.h"

namespace BLOCKMAN
{
	gui_blockFortManorUpgrade::gui_blockFortManorUpgrade()
		: gui_layout("BlockFortManorUpgrade.json")
	{
	}

	gui_blockFortManorUpgrade::~gui_blockFortManorUpgrade()
	{
	}

	void gui_blockFortManorUpgrade::onLoad()
	{
		setText("BlockFortManorUpgrade-Title", "gui_blockfort_manor_upgrade");
		setText("BlockFortManorUpgrade-Unlock-Name", "gui_blockfort_manor_upgrade_unlock");
		setText("BlockFortManorUpgrade-Max-Des", "gui_blockfort_manor_upgrade_max");
		setText("BlockFortManorUpgrade-Max-Level", "gui_blockfort_manor_upgrade_max");
		m_curManorLevel = getWindow<GUIStaticText>("BlockFortManorUpgrade-Cur-Manor-Level");
		m_nextManorLevel = getWindow<GUIStaticText>("BlockFortManorUpgrade-Next-Manor-Level");
		m_curManorImage = getWindow<GUIStaticImage>("BlockFortManorUpgrade-Cur-Manor-Image");
		m_nextManorImage = getWindow<GUIStaticImage>("BlockFortManorUpgrade-Next-Manor-Image");
		m_curManorArea = getWindow<GUIStaticText>("BlockFortManorUpgrade-Cur-Manor-Area");
		m_nextManorArea = getWindow<GUIStaticText>("BlockFortManorUpgrade-Next-Manor-Area");
		m_unlockItems = getWindow<GUIListHorizontalBox>("BlockFortManorUpgrade-Unlock-Items");
		m_unlockItems->SetInterval(28.f);
		m_price = getWindow<GUIStaticText>("BlockFortManorUpgrade-Buy-Money");
		m_currency = getWindow<GUIStaticImage>("BlockFortManorUpgrade-Buy-Currency");
		m_curManorLayout = getWindow<GUILayout>("BlockFortManorUpgrade-Cur-Manor-Info");
		m_nextManorLayout = getWindow<GUILayout>("BlockFortManorUpgrade-Next-Manor-Info");
		m_unlockItemsLayout = getWindow<GUILayout>("BlockFortManorUpgrade-Unlock-Info");
		m_arrow = getWindow<GUIStaticImage>("BlockFortManorUpgrade-Manor-Info-Arrow");
		m_maxLevel = getWindow<GUIStaticText>("BlockFortManorUpgrade-Max-Level");
		setText(m_maxLevel, "gui_blockfort_manor_upgrade_max");

		getWindow("BlockFortManorUpgrade-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortManorUpgrade::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow("BlockFortManorUpgrade-Buy")->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortManorUpgrade::onClick, this, std::placeholders::_1, ViewId::BUY));		
		m_subscriptionGuard.add(BlockFortManorManagerUpdateEvent::subscribe(std::bind(&gui_blockFortManorUpgrade::onDataChange, this)));
	}

	void gui_blockFortManorUpgrade::onShow()
	{
		if (m_dataChange)
		{
			onSetData();
			m_dataChange = false;
		}
	}

	bool gui_blockFortManorUpgrade::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockFort);
	}

	bool gui_blockFortManorUpgrade::onDataChange()
	{
		m_dataChange = true;
		onShow();
		return true;
	}

	void gui_blockFortManorUpgrade::onSetData()
	{
		if (!UICommon::checkBlockFortParam())
		{
			return;
		}

		auto manorManager = Blockman::Instance()->m_pPlayer->m_blockFort->getManorManager();
		if (manorManager)
		{
			m_curManorLayout->SetVisible(manorManager->curManor.level < manorManager->maxLevel);
			m_unlockItemsLayout->SetVisible(manorManager->curManor.level < manorManager->maxLevel);
			m_arrow->SetVisible(manorManager->curManor.level < manorManager->maxLevel);
			m_maxLevel->SetVisible(!(manorManager->curManor.level < manorManager->maxLevel));
			m_price->SetVisible(manorManager->curManor.level < manorManager->maxLevel);
			m_currency->SetVisible(manorManager->curManor.level < manorManager->maxLevel);
			getWindow("BlockFortManorUpgrade-Buy")->SetTouchable(manorManager->curManor.level < manorManager->maxLevel);
			if (manorManager->curManor.level < manorManager->maxLevel)
			{
				m_nextManorLayout->SetHorizontalAlignment(HA_RIGHT);
				m_nextManorLayout->SetYPosition({ 0.0f, 0.0f });

				m_curManorLevel->SetText(StringUtil::Format("Lv.%d", manorManager->curManor.level).c_str());
				m_nextManorLevel->SetText(StringUtil::Format("Lv.%d", manorManager->nextManor.level).c_str());
				m_curManorImage->SetImage(StringUtil::Format("set:blockfort_manor_upgrade.json image:image_%d", manorManager->curManor.level).c_str());
				m_nextManorImage->SetImage(StringUtil::Format("set:blockfort_manor_upgrade.json image:image_%d", manorManager->nextManor.level).c_str());
				m_curManorArea->SetText(StringUtil::Format(getString("gui_blockfort_manor_upgrade_area").c_str(), manorManager->curManor.area.c_str()).c_str());
				m_nextManorArea->SetText(StringUtil::Format(getString("gui_blockfort_manor_upgrade_area").c_str(), manorManager->nextManor.area.c_str()).c_str());
				m_price->SetText(StringUtil::ToString(manorManager->upgradeMoney).c_str());
				m_currency->SetImage(UICommon::getCurrencyIconByType(manorManager->upgradeMoneyType).c_str());

				static int count = 0;
				m_unlockItems->ClearAllItem();
				for (auto item : manorManager->unlockIds)
				{
					String itemName = StringUtil::Format("BlockFortManorUpgrade-Unlock-Items-%d", count++).c_str();
					GuiBlockFortManorUpgradeItem* itemView = (GuiBlockFortManorUpgradeItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKFORT_MANOR_UPGRADE_ITEM, itemName.c_str());
					itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.0f, 75.f), UDim(1.0f, 0.f));
					itemView->setItem(item);
					m_unlockItems->AddItem(itemView, false);
				}
			}
			else
			{
				m_nextManorLayout->SetHorizontalAlignment(HA_CENTRE);
				m_nextManorLayout->SetYPosition({ 0.2f, 0.0f });
				m_nextManorLevel->SetText(StringUtil::Format("Lv.%d", manorManager->curManor.level).c_str());
				m_nextManorImage->SetImage(StringUtil::Format("set:blockfort_manor_upgrade.json image:image_%d", manorManager->curManor.level).c_str());
				m_nextManorArea->SetText(StringUtil::Format(getString("gui_blockfort_manor_upgrade_area").c_str(), manorManager->curManor.area.c_str()).c_str());
			}
		}
	}

	bool gui_blockFortManorUpgrade::onClick(const EventArgs& events, ViewId viewId)
	{
		switch (viewId)
		{
		case BLOCKMAN::gui_blockFortManorUpgrade::ViewId::CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			break;
		case BLOCKMAN::gui_blockFortManorUpgrade::ViewId::BUY:
			if (UICommon::checkBlockFortParam())
			{
				auto manorManager = Blockman::Instance()->m_pPlayer->m_blockFort->getManorManager();
				if (manorManager)
				{
					UICommon::checkMoney(manorManager->upgradeMoney, manorManager->upgradeMoneyType, [=](bool isSuccess) {
						if (isSuccess)
						{
							ClientNetwork::Instance()->getCommonSender()->sendGameOperation(4);
						}
					});
				}
			}
			break;
		default:
			break;
		}
		playSoundByType(ST_Click);
		return true;
	}

}