#pragma once
#ifndef __GUI_BLOCKCITY_CHARM_RANK_ITEM_HEADER__
#define __GUI_BLOCKCITY_CHARM_RANK_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIListBox.h"
#include "Game/BlockCity.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBlockCityCharmRankItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_icon = nullptr;
		GUIStaticText* m_ranks = nullptr;
		GUILayout* m_award = nullptr;

		SubscriptionGuard m_subscriptionGuard;

	private:
		void removeComponents();

	public:
		GuiBlockCityCharmRankItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBlockCityCharmRankItem() { m_subscriptionGuard.unsubscribeAll(); }

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void onSetItem(BlockCityWeekReward reward);
		void onSetRewardList(std::vector<BlockCityWeekRewardItem> items);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
