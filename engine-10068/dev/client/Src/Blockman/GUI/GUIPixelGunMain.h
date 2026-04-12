#ifndef __GUI_PIXEL_GUN_MAIN_HEADER__
#define __GUI_PIXEL_GUN_MAIN_HEADER__

#include "gui_layout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIPixelGunMain : public gui_layout
	{
	public:
		GUIPixelGunMain();
		~GUIPixelGunMain();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

	private:
		bool onFreeVipClick(const EventArgs & events);
		bool onFreeGoldClick(const EventArgs & events);
		bool onHideRewardResult(const EventArgs & events);
		bool onShowGoldReward(int golds, int times, int maxTimes);
		bool onOpenPixelGunTask(const EventArgs & events);
		bool onOpenPixelGunGiftBag(const EventArgs & events);
		bool onUpdatePixekGunTaskCount();
		bool onSyncGoldAdInfo(int times, int seconds);
		bool onUpdateGiftRefreshTime(int time);
		bool onUpdateVipTime(int time);

	private:
		int m_adRewardTime = 0;

		int m_goldAdTimes = -1;
		int m_goldAdTime = -1;
		int m_gift_bag_time = 0;
		i64 m_vip_time_mills = 0;

		GUILayout* m_menu = nullptr;
		GUIStaticImage* m_task_red_point = nullptr;
		GUIStaticText* m_task_count = nullptr;
		GUIStaticImage* m_vip_time = nullptr;
		GUIStaticText* m_vip_time_text = nullptr;
	};
}

#endif