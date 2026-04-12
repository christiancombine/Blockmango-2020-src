#ifndef __GUI_CHEST_DETAIL_HEADER__
#define __GUI_CHEST_DETAIL_HEADER__

#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIGridView.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/ChestLotterySetting.h"
#include "Setting/PixelGunGiftBagSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_lotteryChestDetail : public gui_layout
	{
	private:

		GUIStaticText* m_title = nullptr;
		GUIStaticImage* m_image = nullptr;
		GUIStaticImage* m_integral_icon = nullptr;
		GUIStaticText* m_integral_text = nullptr;

		map<LotteryChestType, GUIGridView*>::type m_rewards_map;

	public:
		gui_lotteryChestDetail();
		~gui_lotteryChestDetail();

		void onLoad() override;
		bool isNeedLoad() override;

		void ShowChestDetail(LotteryChestType type);
		void ShowGiftBagDetail(LotteryChestType type, int giftId);

	private:
		void UpdateUIByType(LotteryChestType type);
		bool onCloseClick(const EventArgs & events);
		void UpdateUIByTypeAndId(LotteryChestType type, int giftId);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif