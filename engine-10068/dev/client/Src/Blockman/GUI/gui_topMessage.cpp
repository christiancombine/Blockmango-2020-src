#include "gui_topMessage.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"
#include "gui_layout.h"
#include "UI/GUIStaticText.h"
#include "gui_baseMessage.h"

namespace BLOCKMAN
{
	gui_topMessage::gui_topMessage() :
		gui_layout("TopMessage.json")
	{
	}

	gui_topMessage::~gui_topMessage()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_topMessage::onLoad()
	{
		m_subscriptionGuard.add(ShowMessageEvent::subscribe(std::bind(&gui_topMessage::showNotification, this, std::placeholders::_1, std::placeholders::_2,
			std::placeholders::_3, std::placeholders::_4)));
	}

	void gui_topMessage::onUpdate(ui32 nTimeElapse)
	{
		if (m_time <= 0)
		{
			return;
		}
		m_time = m_time > 0 ? m_time - nTimeElapse : 0;

		if (m_time <= 0)
		{
			m_rootWindow->SetVisible(false);
			m_rootWindow->SetAlwaysOnTop(false);
		}
	}

	bool gui_topMessage::showNotification(int messageType, const String& message, i32 keepTime, bool isTop)
	{
		if (messageType != (int)MessageId::MESSAGE_TOP)
		{
			return false;
		}

		m_rootWindow->SetVisible(true);
		m_rootWindow->SetAlwaysOnTop(isTop);

		GUIStaticText* Text = getWindow<GUIStaticText>("TopMessage-Text");
		Text->SetText(message.c_str());
		Text->SetTouchable(false);
		Text->SetTextScale(1.2f);
		m_time = keepTime * 1000;

		return true;
	}
}
