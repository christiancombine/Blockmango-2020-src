#include "gui_tipMessage.h"
#include "game.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "UI/GUIButton.h"
#include "Util/LanguageManager.h"
#include "ShellInterface.h"
#include "Setting/GameTypeSetting.h"
#include "Setting/LogicSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_tipMessage::gui_tipMessage() :
		gui_layout("TipMessage.json")
	{

	}

	gui_tipMessage::~gui_tipMessage()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_tipMessage::onLoad()
	{
		getWindow("TipMessage-Panel")->SetVisible(true);
		//getWindow("TipMessage-Title-Name")->SetText(getString(LanguageKey::GUI_SKY_BLOCK_TIP_TITLE));
		m_titleMsg = getWindow<GUIStaticText>("TipMessage-Title-Name");
		m_otherMsgText = getWindow<GUIStaticText>("TipMessage-Content-Other-Message");
		m_otherContentWindow = getWindow("TipMessage-Content-Other");
		
		auto btnClose = getWindow<GUIButton>("TipMessage-Exit-Name");
		btnClose->subscribeEvent(EventButtonClick, std::bind(&gui_tipMessage::onCloseClick, this, std::placeholders::_1));

		m_subscriptionGuard.add(ShowContentTipMsgEvent::subscribe(std::bind(&gui_tipMessage::showContentTipMessage, this, std::placeholders::_1, std::placeholders::_2)));
	}
		
	bool gui_tipMessage::isNeedLoad()
	{
		return checkGameType(ClientGameType::SkyBlockMain);
	}
	
	void gui_tipMessage::onUpdate(ui32 nTimeElapse)
	{
		
	}

	bool gui_tipMessage::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		return true;
	}

	bool gui_tipMessage::showContentTipMessage(const String&  messageLang, const String& titleLang)
	{
		String titlemsg = getString(titleLang).c_str();
		m_titleMsg->SetText(titlemsg.c_str());
		String msg = getString(messageLang).c_str();
		m_otherMsgText->SetText(msg.c_str());
		return true;
	}
}


	

	

