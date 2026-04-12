#pragma once
#ifndef __GUI_CHECKIN_HEADER__
#define __GUI_CHECKIN_HEADER__
#include "GUI/gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIGridView.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "GUI/GuiCheckinItem.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_checkin : public gui_layout
	{
	private:
		enum ViewId {
			SURE,
			CLOSE,
		};
	private:
		void initView();
		String getViewName(const String& name);

	public:
		gui_checkin();
		~gui_checkin();
		void onShow() override;
		void onHide() override;
		void onStart() override;
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClick(const EventArgs, ViewId viewId);
		bool dataUpdate(const String& content);
		bool isNeedLoad() override;

	private:
		void parseData(const String& content);
		void initItem(const vector<CheckinItem>::type& items);		

	private:
		bool m_isNeedGuide = false;
		String m_fileName = "";
		i32 m_timeLeft = 0;
		float m_gvViewXSize = 0.f;
		GUIGridView* m_gvItems = nullptr;
		GUIButton* m_btnReceive = nullptr;
		GUIStaticText* m_tvTimeLeft = nullptr;
	};
}
#endif
