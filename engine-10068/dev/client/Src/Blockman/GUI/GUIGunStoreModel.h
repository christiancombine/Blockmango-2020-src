#ifndef __GUI_GUN_STORE_MODEL_HEADER__
#define __GUI_GUN_STORE_MODEL_HEADER__

#include "gui_layout.h"
#include "GUI/GuiActorWindow.h"
#include "UI/GUIButton.h"
#include "Setting/GunStoreSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIGunStoreModel : public gui_layout
	{
	private:
		int m_cur_gun_item_id = 0;
		GuiActorWindow* m_model = nullptr;
		GUIButton* m_shoot = nullptr;

	public:
		GUIGunStoreModel();
		~GUIGunStoreModel();

		void onLoad() override;

		void SetStoreGun(StoreGun* gun);
		void SetStoreProp(StoreProp* prop);
		void SetStoreModule(StoreBlockModule* module);

	private:
		bool tryGunShoot(const EventArgs & events);
		void resetModel();
	};
}

#endif