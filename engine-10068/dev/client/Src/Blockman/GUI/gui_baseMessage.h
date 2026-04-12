#ifndef __GUI_BASE_MESSAGE_HEADER__
#define __GUI_BASE_MESSAGE_HEADER__

#include "gui_layout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class RootGuiLayout;
	enum MessageId
	{
		MESSAGE_TOP = 1,
		MESSAGE_CENTER,
		MESSAGE_BOTTOM,
	};

	class gui_baseMessage : public gui_layout
	{
	private:
		map<MessageId, gui_layout*>::type m_messages;
	public:
		gui_baseMessage();
		~gui_baseMessage();

		void onLoad() override;
	private:

		RootGuiLayout * getParent();
	};
}

#endif
