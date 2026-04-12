#ifndef __GUI_FORMULATION_PANEL_CONTROL_HEADER__
#define __GUI_FORMULATION_PANEL_CONTROL_HEADER__

#include <memory>
#include "GUI/gui_layout.h"
#include "UI/GUIGridView.h"
#include "GUI/GuiScrollableInventory.h"
#include "Setting/FormulationSetting.h"

namespace LORD
{
	class GUIButton;
	class GUIStaticText;
}

namespace BLOCKMAN
{
	class gui_formulationPanelControl : public gui_layout
	{
	private:
		GUIGridView* m_gridView = nullptr;
		GuiSlotTable* m_craftTable = nullptr;
		GUIButton* m_craftButton = nullptr;
		GUIStaticImage* m_target_icon = nullptr;
		GUIButton* m_refresh_btn = nullptr;
		map<int, GUIStaticImage*>::type m_material_map = {};
		int m_currentId = 0;
		int m_currentIndex = -1;
		float item_size = 0;

	public:
		gui_formulationPanelControl();
		~gui_formulationPanelControl();
		void onLoad() override;
		void onShow() override;
		bool isNeedLoad() override;
		void onUpdate(ui32) override;
		bool onItemTouchUp(const EventArgs& event,int index,int id);
		bool onCraftButtonClick(const EventArgs& event);
		bool onRefreshButtonClick(const EventArgs& event);
		void updataGridView();
	private:
		int checkComposable(FormulationItem* item);
		void selectItem(int index, int id);
		GUIStaticImage* createItem(int index, FormulationItem* item, int num);
	};
}

#endif // !__GUI_MAIN_INVENTORY_PANEL_HEADER__

