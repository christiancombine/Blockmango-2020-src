#pragma once
#ifndef __GUI_BLOCKCITY_MANOR_HEADER__
#define __GUI_BLOCKCITY_MANOR_HEADER__
#include "GUI/gui_layout.h"

using namespace LORD;
namespace LORD {
	class GUIStaticText;
	class GUIStaticImage;
	class GUIButton;
	class GUIRadioButton;
}

namespace BLOCKMAN
{
	class gui_blockCityManor : public gui_layout
	{
	private:
		enum ViewId
		{
			CLOSE,
			BTN_DEL,
			BTN_UPGRADE,
			ALL,
			FRIEND,
			NOACCESS,
			SAVE_FILE,
		};

	public:
		gui_blockCityManor();
		~gui_blockCityManor();
		void onShow() override;
		void onHide() override;
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClick(const EventArgs& args, ViewId viewId);
		bool isNeedLoad() override;

		bool onRadioChange(const EventArgs & events, ViewId viewId);
		bool open();
		bool updateBlockCityManor();
		void showBlockCityManor();

	private:
		

	private:
		GUIStaticText* m_curClass = nullptr;
		GUIStaticText* m_curArea = nullptr;
		GUIStaticText* m_curManorGrade = nullptr;
		GUIStaticText* m_curBlockGrade = nullptr;
		GUIStaticText* m_curDecorateGrade = nullptr;
		GUIStaticText* m_curTotalGrade = nullptr;
		GUIStaticText* m_curOpenStatus = nullptr;
		GUIStaticText* m_nextClass = nullptr;
		GUIStaticText* m_nextArea = nullptr;
		GUIStaticText* m_nextManorGrade = nullptr;
		GUIStaticImage* m_openStatusAll = nullptr;
		GUIStaticImage* m_openStatusFriend = nullptr;
		GUIStaticImage* m_openStatusClose = nullptr;
		GUIButton* m_btnUpgrade = nullptr;
		map<ViewId, GUIRadioButton*>::type m_radioMap;

		bool m_isManorUpdate = false;
		//bool m_canBuy = false;

	};
}
#endif
