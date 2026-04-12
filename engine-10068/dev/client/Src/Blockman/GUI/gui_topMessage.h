#pragma once
#ifndef __GUI_TOP_MESSAGE_HEADER__
#define __GUI_TOP_MESSAGE_HEADER__

#include "gui_layout.h"
#include "gui_baseMessage.h"
#include "UI/GUILayout.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_topMessage : public gui_layout
	{

	private:
		int m_time = 0;
	public:
		gui_topMessage();
		~gui_topMessage();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool showNotification(int messageType, const String& message, i32 keepTime, bool isTop);
	private:

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif
