#pragma once
#ifndef __GUI_VIDEO_AD_TIP_AD_HEADER__
#define __GUI_VIDEO_AD_TIP_AD_HEADER__

#include "gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "Network/protocol/CommonDataBuilder.h"

using namespace LORD;
namespace LORD {
	class GUIStaticImage;
	class GUIButton;
	class GUIStaticText;
}

namespace BLOCKMAN
{
	class gui_videoAdTip : public gui_layout
	{
	private:
		bool m_have_ad = false;
		int m_ad_type = 0;
		int m_show_btn_num = 0;
		Vector2 m_text_offset = Vector2::ZERO;

		String m_ad_params = "";

		String m_try_show_report = "";
		String m_show_report = "";
		String m_watch_report = "";
		String m_images_report = "";
		String m_texts_report = "";
		UVector2 left_position;
		UVector2 right_position;

		list<GUIStaticImage*>::type m_dynamic_images;
		list<GUIStaticText*>::type m_dynamic_text;

	public:
		gui_videoAdTip();
		~gui_videoAdTip();
		void onStart() override;
		void onLoad() override;
		void onGetCommonData(String key, String data);
		void clearDynamicUI();

	private:
		bool onVideoAdVisible(bool visible);
		bool onCloseClick(const EventArgs& events);
		bool onWatchClick(const EventArgs& events);
		bool onSureClick(const EventArgs& events);

		void initSureButton(CommonDataBuilder* builder);
		void initCancelButton(CommonDataBuilder* builder);
		void initAdButton(CommonDataBuilder* builder);

		void initCommonButton(GUIButton* button, String buttonText, String iconStr);

		void initImages(CommonDataBuilder* builder);
		void initText(CommonDataBuilder* builder);

		

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif
