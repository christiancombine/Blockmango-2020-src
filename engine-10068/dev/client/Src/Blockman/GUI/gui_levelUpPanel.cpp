#include "gui_levelUpPanel.h"
#include "GUI/GuiDef.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageKey.h"

namespace BLOCKMAN
{
	gui_levelUpPanel::gui_levelUpPanel() :
		gui_layout("LevelUpPanel.json")
	{
		
	}

	gui_levelUpPanel::~gui_levelUpPanel()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_levelUpPanel::onLoad()
	{
		auto Close = getWindow<GUIButton>("LevelUpPanel-Close");
		Close->subscribeEvent(EventButtonClick, std::bind(&gui_levelUpPanel::onCloseClick, this, std::placeholders::_1));
	}

	bool gui_levelUpPanel::isNeedLoad()
	{
		return checkGameType({ ClientGameType::PixelGunHall , ClientGameType::PixelGunGameTeam ,  
			ClientGameType::PixelGunGamePerson ,  ClientGameType::PixelGunGame1v1 , ClientGameType::PixelGunChicken });
	}

	bool gui_levelUpPanel::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		ShowLevelUpPanelEvent::emit(false);
		return true;
	}
}