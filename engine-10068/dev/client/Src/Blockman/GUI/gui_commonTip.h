#pragma once
#ifndef __GUI_COMMON_TIP_HEADER__
#define __GUI_COMMON_TIP_HEADER__
#include "GUI/gui_layout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "GUI/GuiActorWindow.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_commonTip : public gui_layout
	{

	public:
		gui_commonTip();
		~gui_commonTip();
		void onHide() override;
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClick(const EventArgs);
		bool open(const String& tips);
		bool showTip(vector<String>::type tips, i32 guideId);
		bool isNeedLoad() { return checkGameType({ ClientGameType::BlockCity , ClientGameType::BlockFort}); }

	private:
		void parseData(const String& tips);

	private:
		vector<String>::type m_tips = vector<String>::type();
		i32 m_page = 1;

	private:
		i32 m_guideId = 0;
		GUIStaticText* m_titleText = nullptr;
		GUIStaticText* m_messageText = nullptr;
		GuiActorWindow* m_actorWindow = nullptr;
	};
}
#endif
