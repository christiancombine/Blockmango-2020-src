#ifndef __GUI_REWARD_RESULT_HEADER__
#define __GUI_REWARD_RESULT_HEADER__

#include "gui_layout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUILayout.h"
#include "GUI/RootGuiLayout.h"
#include "GUI/GuiActorWindow.h"
#include "GUI/GuiEffectViewOfObtainment.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_rewardResult : public gui_layout
	{
	private:
		int m_wait_open_time = 0;
		int m_can_close_time = 0;
		bool m_can_close = true;
		int m_cur_active_id = 0;

		bool m_have_ad = false;
		String m_ad_rewards = "";

		GuiActorWindow* m_chest = nullptr;
		GUILayout* m_content = nullptr;
		GuiEffectViewOfObtainment* m_image = nullptr;
		GUIStaticText* m_name = nullptr;

	public:
		gui_rewardResult();
		~gui_rewardResult();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isNeedLoad() override;
		void onGetCommonData(String key, String data);

	private:
		bool onContentClick(const EventArgs & events);
		bool onContinueClick(const EventArgs & events);

		bool onOpenLotteryChest(int chestId);
		bool onShowChestRewardResult(int id);
		bool onShowRewards(const String & rewards);
		bool showAdBtn(String reward);
		bool onReceiveTaskActiveReward(int activeId);
		void hideChestActor();
		bool onHideTaskActiveReward() { getParent()->hideChestRewardResult();; return false; };
		bool onVideoAdVisible(bool visible);
		bool onAdClick(const EventArgs & events);
		void hideAdBtn();

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif