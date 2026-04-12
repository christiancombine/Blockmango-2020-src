#pragma once
#ifndef __GUI_BIRD_LOTTERY_HEADER__
#define __GUI_BIRD_LOTTERY_HEADER__

#include "gui_layout.h"
#include "Game/BirdSimulator.h"

using namespace LORD;
namespace LORD {
	class GUIButton;
	class GUILayout;
	class GUIGridView;
	class GUIStaticText;
	class GUIStaticImage;
}
namespace BLOCKMAN
{

	class gui_birdLottery: public  gui_layout
	{
	private:
		enum class ViewId
		{
			SURE,
			CLOSE,
			PROBABILITY,
		};
	private:
		GUIGridView* m_gvView = nullptr;
		GUIStaticText* m_tvPrice = nullptr;
		GUIButton* m_btnOpen = nullptr;
		GUIStaticImage* m_ivCurrencyIcon = nullptr;

		i32 m_price = 0;
		i32 m_currencyType = 4;
		i32 m_entityId = -1;
		float m_gvViewXSize = 0.f;

	public:
		gui_birdLottery();
		~gui_birdLottery();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClick(const EventArgs & events, ViewId viewId);
		bool isNeedLoad() override;

	private:
		bool onBirdLotteryUpdate();
		bool showLottery(i32 entityId);
		vector<BirdEgg>::type parseData(const String& data);
		void sortItemSizeChange();
	};

}
#endif