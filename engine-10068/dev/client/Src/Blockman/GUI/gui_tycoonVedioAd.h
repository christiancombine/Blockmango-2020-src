#pragma once
#ifndef __GUI_TYCOON_VEDIO_AD_HEADER__
#define __GUI_TYCOON_VEDIO_AD_HEADER__

#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_tycoonVedioAd : public gui_layout
	{
	private:
		bool m_have_ad = false;
		int m_ad_type = 0;
	public:
		gui_tycoonVedioAd();
		~gui_tycoonVedioAd();

		void onLoad() override;
		bool isNeedLoad() override;
		void onGetCommonData(String key, String data);

	private:
		bool onVideoAdVisible(bool visible);
		bool onCloseClick(const EventArgs& events);
		bool onWatchClick(const EventArgs& events);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif
