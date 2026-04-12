#ifndef __GUI_CHEST_REWARD_DETAIL_HEADER__
#define __GUI_CHEST_REWARD_DETAIL_HEADER__

#include "gui_layout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/ChestLotterySetting.h"
#include "Setting/RewardSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_chestRewardDetail : public gui_layout
	{
	private:
		GUIStaticText* m_title = nullptr;
		GUIStaticImage* m_image = nullptr;
		GUIStaticText* m_name = nullptr;
		GUIStaticText* m_desc = nullptr;

	public:
		gui_chestRewardDetail();
		~gui_chestRewardDetail();

		void onLoad() override;
		bool isNeedLoad() override;

		void ShowLotteryChestReward(Reward* reward);

	private:
		bool onContentClick(const EventArgs & events);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif