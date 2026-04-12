#pragma once
#pragma once
#ifndef __GUI_TIP_MESSAGE_HEADER__
#define __GUI_TIP_MESSAGE_HEADER__
#include "Core.h"
#include "GUI/gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "Util/TipType.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "Inventory/Wallet.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_tipMessage : public gui_layout
	{

	public:
		gui_tipMessage();
		~gui_tipMessage();

		bool showContentTipMessage(const String&  messageLang, const String& titleLang);
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onCloseClick(const EventArgs & events);
		bool isNeedLoad() override;

	private:
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	
		GUIStaticText*		m_otherMsgText = nullptr;
		GUIStaticText*		m_titleMsg = nullptr;
		GUIWindow*			m_otherContentWindow = nullptr;
		
	};
}
#endif
