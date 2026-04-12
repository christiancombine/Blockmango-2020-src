
#ifndef __GUI_BIRD_ATLAS_HEADER__
#define __GUI_BIRD_ATLAS_HEADER__
#include "gui_layout.h"

using namespace LORD;
namespace LORD {
	class GUILayout;
	class GUIListBox;
}
namespace BLOCKMAN
{
	class RootGuiLayout;
	class gui_birdAtlas : public  gui_layout
	{
	private:
		GUIListBox* m_lbDetails = nullptr;

		bool m_isAtlasUpdate = false;
	public:
		gui_birdAtlas();
		~gui_birdAtlas();
		void onShow() override;
		void onLoad() override;
		void showItemView();
		bool updateBirdAtlas();
		bool isNeedLoad() override;
	private:
		bool onClose(const EventArgs & events);
	};

}
#endif