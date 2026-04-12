#include "gui_blockCityRankAward.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/RootGuiLayout.h"
#include "Game/BlockCity.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "cWorld/Blockman.h"
#include "Network/ClientNetwork.h"
#include "Util/RankManager.h"
#include "Setting/BlockCitySetting.h"
#include "GUI/GuiItemStack.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIListHorizontalBox.h"
#include "UI/GUIStaticImage.h"

namespace BLOCKMAN
{
	gui_blockCityRankAward::gui_blockCityRankAward()
		: gui_layout("BlockCityRankAward.json")
	{
	}

	gui_blockCityRankAward::~gui_blockCityRankAward()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_blockCityRankAward::onLoad()
	{
		setText("BlockCityRankAward-Title-Name", "gui_blockcity_rank_award");
		setText("BlockCityRankAward-Cash-Prize", "gui_blockcity_rank_award_cash_prize");

		m_list = getWindow<GUIListHorizontalBox>("BlockCityRankAward-Award");
		m_list->SetInterval(22.f);
		m_rank = getWindow<GUIStaticText>("BlockCityRankAward-Rank");

		getWindow("BlockCityRankAward-Cash-Prize")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityRankAward::onCashPrize, this, std::placeholders::_1));
		m_subscriptionGuard.add(UpdateBlockCityMyLastCharmRankEvent::subscribe(std::bind(&gui_blockCityRankAward::onSetDatas, this, std::placeholders::_1)));

	}

	void gui_blockCityRankAward::onShow()
	{
	}

	bool gui_blockCityRankAward::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	bool gui_blockCityRankAward::onSetDatas(i32 myRank)
	{		
		m_rank->SetText(StringUtil::Format(getString("gui_blockcity_rank_award_rank").c_str(), myRank).c_str());

		if (!UICommon::checkBlockCityParam())
		{
			return false;
		}
		auto reward = Blockman::Instance()->m_pPlayer->m_blockCity->getRewardByRank(myRank);

		m_list->ClearAllItem();
		m_list->SetMoveAble(false);
		size_t rewardsSize = reward.items.size();
		m_list->SetWidth({ 0.0f, rewardsSize * 74.f + (rewardsSize - 1) * 22.f });

		static i32 count = 0;
		float itemX = (m_list->GetPixelSize().x - (rewardsSize - 1) * 22.f) / rewardsSize;
		for (size_t i = 0; i < reward.items.size(); i++)
		{
			String iterLayout = StringUtil::Format("BlockCityRankAward-Award-Items-%d", count++).c_str();
			GUIStaticImage* itemView = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, iterLayout.c_str());
			itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, itemX), UDim(1.f, 0));
			if (reward.items[i].meta >= 0)
			{
				itemView->SetSize(UVector2(UDim(0, 74.f), UDim(0, 74.f)));
				itemView->SetVerticalAlignment(VA_CENTRE);
				GuiItemStack::showItemIcon(itemView, reward.items[i].id, reward.items[i].meta);
			}
			else
			{
				itemView->SetImage(BlockCitySetting::getItemIcon(reward.items[i].id).c_str());
			}
			m_list->AddItem(itemView, false);
		}
		RootGuiLayout::Instance()->setShowBlockCityRankAward(true);
		return true;
	}

	bool gui_blockCityRankAward::onCashPrize(const EventArgs& events)
	{
		ClientNetwork::Instance()->getCommonSender()->sendBlockCityOperation(5);
		RootGuiLayout::Instance()->setShowBlockCityRankAward(false);
		return true;
	}
}