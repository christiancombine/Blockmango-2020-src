#include "gui_centerMessage.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"
#include "gui_layout.h"
#include "UI/GUIStaticText.h"
#include "gui_baseMessage.h"

namespace BLOCKMAN
{
	gui_centerMessage::gui_centerMessage() :
		gui_layout("CenterMessage.json")
	{
	}

	gui_centerMessage::~gui_centerMessage()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_centerMessage::onLoad()
	{
		m_subscriptionGuard.add(ShowMessageEvent::subscribe(std::bind(&gui_centerMessage::showNotification, this, std::placeholders::_1, std::placeholders::_2,
			std::placeholders::_3, std::placeholders::_4)));
	}

	void gui_centerMessage::onUpdate(ui32 nTimeElapse)
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

	bool gui_centerMessage::showNotification(int messageType, const String& message, i32 keepTime, bool isTop)
	{
		if (messageType != (int)MessageId::MESSAGE_CENTER)
		{
			return false;
		}

		m_rootWindow->SetVisible(true);
		m_rootWindow->SetAlwaysOnTop(isTop);

		GUIStaticText* Text = getWindow<GUIStaticText>("CenterMessage-Text");
		Text->SetText(message.c_str());
		Text->SetTouchable(false);
		Text->SetTextScale(1.2f);
		m_time = keepTime * 1000;

		return true;
	}
}
