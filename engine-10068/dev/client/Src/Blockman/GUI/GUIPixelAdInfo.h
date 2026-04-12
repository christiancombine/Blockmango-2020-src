#ifndef __GUI_PIXEL_AD_INFO_HEADER__
#define __GUI_PIXEL_AD_INFO_HEADER__

#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIPixelAdInfo : public gui_layout
	{
		enum PixelAdType
		{
			CHEST = 1,
			VIP = 2
		};

	private:
		int m_vip_time = 0;
		int m_vip_times = 0;
		String m_vip_data = "";
		int m_chest_time = 0;
		int m_chest_times = 0;
		String m_chest_data = "";

		bool m_have_ad = false;;

		GUIStaticText* m_title = nullptr;

		GUILayout* m_free_chest = nullptr;
		GUIStaticImage* m_chest_icon = nullptr;
		
		GUILayout* m_free_vip = nullptr;
		GUIStaticText* m_vip_title = nullptr;
		GUIStaticImage* m_vip_icon = nullptr;
		GUIStaticText* m_vip_name = nullptr;
		GUIStaticText* m_vip_desc = nullptr;
		GUIStaticText* m_vip_time_text = nullptr;
		GUIStaticText* m_vip_times_text = nullptr;

		SubscriptionGuard m_subscriptionGuard;

	public:
		GUIPixelAdInfo();
		~GUIPixelAdInfo();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void showAdInfo(int type);
		void resetChest(String data);
		void resetVip(String data);
		bool isForceUpdate() { return true; }
		
	private:
		bool onVideoAdVisible(bool visible);
		bool onCloseClick(const EventArgs & events);
		bool onAdClick(const EventArgs & events, PixelAdType type);
		void setVipTimeText(int time);
		void setChestTimeText(int time);
		void setButtonStatus();
		String getTimeString(int time);
	};
}
#endif     