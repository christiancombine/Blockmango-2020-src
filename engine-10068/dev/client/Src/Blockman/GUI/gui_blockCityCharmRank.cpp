#include "gui_blockCityCharmRank.h"
#include "GUI/GuiBlockCityCharmRankItem.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/RootGuiLayout.h"
#include "Game/BlockCity.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "cWorld/Blockman.h"
#include "Network/ClientNetwork.h"
#include "GuiDef.h"
#include "UI/GUIListBox.h"

namespace BLOCKMAN
{
	gui_blockCityCharmRank::gui_blockCityCharmRank()
		: gui_layout("BlockCityCharmRank.json")
	{
	}

	gui_blockCityCharmRank::~gui_blockCityCharmRank()
	{
	}

	void gui_blockCityCharmRank::onLoad()
	{
		setText("BlockCityCharmRank-Title", "gui_blockcity_charm_rank");

		m_list = getWindow<GUIListBox>("BlockCityCharmRank-List");

		getWindow("BlockCityCharmRank-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityCharmRank::onClose, this, std::placeholders::_1));
		m_subscriptionGuard.add(UpdateBlockCityWeekRewardEvent::subscribe(std::bind(&gui_blockCityCharmRank::onDataChange, this)));
	}

	void gui_blockCityCharmRank::onShow()
	{
		if (m_dataChange)
		{
			onSetDatas();
			m_dataChange = false;
		}
	}

	void gui_blockCityCharmRank::onHide()
	{
	}

	bool gui_blockCityCharmRank::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	bool gui_blockCityCharmRank::onClose(const EventArgs& events)
	{
		playSoundByType(ST_Click);
		RootGuiLayout::Instance()->setShowBlockCityCharmRank(false);
		return true;
	}

	bool gui_blockCityCharmRank::onDataChange()
	{
		m_dataChange = true;
		if (isShown())
		{
			onShow();
		}
		return true;
	}

	void gui_blockCityCharmRank::onSetDatas()
	{
		if (!UICommon::checkBlockCityParam())
		{
			return;
		}
		auto rewards = Blockman::Instance()->m_pPlayer->m_blockCity->getWeekReward();

		static i32 count = 0;
		m_list->ClearAllItem();
		m_list->SetMoveAble(false);
		float itemY = (m_list->GetPixelSize().y - 4 * 2.f) / 5;
		for (auto reward : rewards)
		{
			String iterLayout = StringUtil::Format("BlockCityCharmRank-Items-%d", count++).c_str();
			GuiBlockCityCharmRankItem* itemView = (GuiBlockCityCharmRankItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_CHARM_RANK_ITEM, iterLayout.c_str());
			itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0), UDim(0.0f, itemY));
			itemView->onSetItem(reward);
			m_list->AddItem(itemView, false);
		}
	}
}