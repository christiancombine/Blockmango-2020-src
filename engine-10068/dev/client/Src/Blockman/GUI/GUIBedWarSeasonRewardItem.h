#ifndef __GUI_BED_WAR_SEASON_REWARD_ITEM_HEADER__
#define __GUI_BED_WAR_SEASON_REWARD_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/SeasonSetting.h"
#include "Setting/RewardSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIBedWarSeasonRewardItem : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUIBedWarSeasonRewardItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIBedWarSeasonRewardItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void SetSeasonReward(Reward* reward, bool isBig);
		void SetSeasonRankReward(SeasonRankReward* rankReward, bool isBig);

	};
}

#endif