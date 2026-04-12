#ifndef __GUI_STORE_MODULE_DETAIL_HEADER__
#define __GUI_STORE_MODULE_DETAIL_HEADER__

#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "GUI/GuiActorWindow.h"
#include "Setting/GunStoreSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIStoreModuleDetail : public gui_layout
	{
	private:
		StoreBlockModule * m_data = NULL;

		GUIStaticText* m_name = NULL;
		GUIStaticText* m_rarity = NULL;
		GuiActorWindow* m_model = NULL;
		GUILayout* m_attribute = NULL;
		GUILayout* m_property = NULL;
		GUIButton* m_use = NULL;
		GUIButton* m_buy = NULL;
		GUIStaticImage* m_price_type = NULL;
		GUILayout* m_price = NULL;
		GUIStaticText* m_price_text = NULL;

	public:
		GUIStoreModuleDetail();
		~GUIStoreModuleDetail();

		void onLoad() override;

		void SetStoreModule(StoreBlockModule* module);

	private:
		bool onCloseClick(const EventArgs & events);
		bool onUseClick(const EventArgs & events);
		bool onBuyClick(const EventArgs & events);

	};
}

#endif
