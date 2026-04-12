#include "gui_baseMessage.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"
#include "gui_bottomMessage.h"
#include "gui_centerMessage.h"
#include "gui_topMessage.h"
#include "GUI/RootGuiLayout.h"

namespace BLOCKMAN
{
	gui_baseMessage::gui_baseMessage() :
		gui_layout("BaseMessage.json")
	{
		m_messages[MessageId::MESSAGE_BOTTOM] = m_messages[MessageId::MESSAGE_BOTTOM] ? m_messages[MessageId::MESSAGE_BOTTOM] : LordNew gui_bottomMessage;
		m_messages[MessageId::MESSAGE_CENTER] = m_messages[MessageId::MESSAGE_CENTER] ? m_messages[MessageId::MESSAGE_CENTER] : LordNew gui_centerMessage;
		m_messages[MessageId::MESSAGE_TOP] = m_messages[MessageId::MESSAGE_TOP] ? m_messages[MessageId::MESSAGE_TOP] : LordNew gui_topMessage;
	}

	gui_baseMessage::~gui_baseMessage()
	{
		m_subscriptionGuard.unsubscribeAll();
		for (auto it : m_messages) 
		{
			LordSafeDelete(it.second);
		}
		m_messages.clear();
	}

	RootGuiLayout* gui_baseMessage::getParent()
	{
		return dynamic_cast<RootGuiLayout*>(m_parentLayout);
	}

	void gui_baseMessage::onLoad()
	{
		m_rootWindow->SetAlwaysOnTop(true);
		m_messages[MessageId::MESSAGE_BOTTOM]->show();
		GUIWindow* pBottom = m_messages[MessageId::MESSAGE_BOTTOM]->getWindow("BottomMessage");
		m_messages[MessageId::MESSAGE_BOTTOM]->attachTo(this, pBottom);
		m_messages[MessageId::MESSAGE_BOTTOM]->hide();

		m_messages[MessageId::MESSAGE_CENTER]->show();
		GUIWindow* pCenter = m_messages[MessageId::MESSAGE_CENTER]->getWindow("CenterMessage");
		m_messages[MessageId::MESSAGE_CENTER]->attachTo(this, pCenter);
		m_messages[MessageId::MESSAGE_CENTER]->hide();

		m_messages[MessageId::MESSAGE_TOP]->show();
		GUIWindow* pTop = m_messages[MessageId::MESSAGE_TOP]->getWindow("TopMessage");
		m_messages[MessageId::MESSAGE_TOP]->attachTo(this, pTop);
		m_messages[MessageId::MESSAGE_TOP]->hide();
	}
}
