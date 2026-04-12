#ifndef __GUI_GUN_STORE_PANEL_HEADER__
#define __GUI_GUN_STORE_PANEL_HEADER__

#include "gui_layout.h"
#include "UI/GUIGridView.h"
#include "Setting/GunStoreSetting.h"
#include "GUIGunStoreModulePanel.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIGunStorePanel : public gui_layout
	{
	private:	
		float ContentSize = 0.0f;
		map<StoreTab*, GUIGridView*>::type m_panel_map;
		GUIGunStoreModulePanel* m_module_panel = nullptr;
		bool m_isFirst = true;

	public:
		GUIGunStorePanel();
		~GUIGunStorePanel();

		void InitPanels(float contentSize);
		void InitPanelChilds();
		void ResetPanelChilds(int type);
		void ShowPanelByType(int type);

		void ShowPropSubtypePanle(int propId);
		void ShowBlockModulePanle(int propId);

		void ResetModuleChild();
	private:
		void InitGunChilds(StoreTab* tab, GUIGridView* childs);
		void InitPropChilds(StoreTab* tab, GUIGridView* childs);

		void ResetGunChilds(StoreTab* tab, GUIGridView* childs);
		void ResetPropChilds(StoreTab* tab, GUIGridView* childs);

		int FindPropIndexById(int propId, GUIGridView* propPanel);
	};
}

#endif