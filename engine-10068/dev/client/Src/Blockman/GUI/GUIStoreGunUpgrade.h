#ifndef __GUI_STORE_GUN_UPGRADE_HEADER__
#define __GUI_STORE_GUN_UPGRADE_HEADER__

#include "gui_layout.h"
#include "GUI/GuiActorWindow.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "Setting/GunStoreSetting.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIStoreGunUpgrade : public gui_layout
	{
	private:
		StoreGun* m_data = NULL;

		GUIStaticImage* m_gold_icon = NULL;
		GUIStaticText* m_gold_has = NULL;
		GUIStaticText* m_gold_price = NULL;
		GUIStaticText* m_gold_time = NULL;

		GUIStaticImage* m_ticket_icon = NULL;
		GUIStaticText* m_ticket_has = NULL;
		GUIStaticText* m_ticket_price = NULL;
		GUIStaticText* m_ticket_time = NULL;

	public:
		GUIStoreGunUpgrade();
		~GUIStoreGunUpgrade();

		void onLoad() override;
		void onShowGunUpgrade(int gunId);
	private:
		bool onCloseClick(const EventArgs & events);
		bool onBuyClick(const EventArgs & events);
		bool onUpgradeClick(const EventArgs & events, bool needWait);
		void setTimeText(int time);
	};
}

#endif
