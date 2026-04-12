#pragma once
#ifndef __GUI_BIRD_HELP_HEADER__
#define __GUI_BIRD_HELP_HEADER__
#include "GUI/gui_layout.h"


using namespace LORD;
namespace LORD {
	class GUIStaticText;
	class GUIStaticImage;
}
namespace BLOCKMAN
{

	class gui_birdHelp : public gui_layout
	{
	private:
		enum ViewId
		{
			CLOSE,
			BTN_UP,
			BTN_NEXT,
		};

	public:
		gui_birdHelp();
		~gui_birdHelp();
		void onShow() override;
		void onHide() override;
		void onLoad() override;
		bool onClick(const EventArgs& args, ViewId viewId);
		bool setPageAndShow(i32 page);
		bool isNeedLoad() override;

	private:
		void refresh();
		

	private:
		i32 m_page = 1;
		GUIStaticText*		m_tvPage = nullptr;
		GUIStaticText*		m_titleText = nullptr;
		GUIStaticText*		m_messageText = nullptr;
		GUIStaticImage*  m_ivImage = nullptr;
	};
}
#endif
