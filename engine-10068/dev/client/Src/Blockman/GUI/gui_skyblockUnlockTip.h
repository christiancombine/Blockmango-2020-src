#pragma once
#ifndef __GUI_SKYBLOCKUNLOCKTIP_HEADER__
#define __GUI_SKYBLOCKUNLOCKTIP_HEADER__
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
	class gui_skyblockUnlockTip : public gui_layout
	{

	public:
		gui_skyblockUnlockTip();
		~gui_skyblockUnlockTip();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onCloseClick(const EventArgs & events);
		bool onSureClick(const EventArgs & events);
		bool isNeedLoad() override;
		void onGetCommonData(String key, String data);

	private:
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
		int m_costprice = 0;
		int m_shopId = 0;
		int m_unlockType = 0;

		void showUnlockTipMessage(const String& data);
		GUIStaticImage*     m_icon = nullptr;
		GUIStaticImage*     m_price_icon = nullptr;
		GUIStaticText*		m_icon_name = nullptr;
		GUIStaticText*		m_hint_msg = nullptr;
		GUIStaticText*      m_price_number = nullptr;
		
	};
}
#endif
