#pragma once
#ifndef __GUI_BLOCKCITY_BUY_FLY_TIP_HEADER__
#define __GUI_BLOCKCITY_BUY_FLY_TIP_HEADER__

#include "gui_layout.h"

using namespace LORD;
namespace LORD {
	class GUIWindow;
	class GUILayout;
}
namespace BLOCKMAN
{
	class gui_blockCityBuyFlyTip : public  gui_layout
	{
	public:
		gui_blockCityBuyFlyTip();
		virtual ~gui_blockCityBuyFlyTip();
		void onLoad() override;
		bool isNeedLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool showFlyTip();
		bool onVideoAdVisible(bool visible);
		void onGetCommonData(String key, String data) override;

	private:
		bool onClose(const EventArgs& args);

	private:
		GUILayout* m_flyLayout = nullptr;
		bool m_isAdReady = false;
		bool m_canShowAd = false;
	};
}

#endif