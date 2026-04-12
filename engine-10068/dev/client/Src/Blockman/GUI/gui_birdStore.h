#ifndef __GUI_BIRD_STORE_HEADER__
#define __GUI_BIRD_STORE_HEADER__
#include "gui_layout.h"
#include "Game/BirdSimulator.h"

using namespace LORD;
namespace LORD {
	class GUILayout;
	class GUIStaticText;
	class GUIStaticImage;
	class GUIButton;
}
namespace BLOCKMAN
{
	class gui_birdStore : public  gui_layout
	{
	private:
		enum class ViewId {
			UP,
			NEXT,
			CLOSE,
			OPERATION
		};

	private:
		GUILayout* m_layoutPrice = nullptr;
		GUIStaticImage* m_ivIcon = nullptr;
		GUIStaticImage* m_ivCurrencyIcon = nullptr;
		GUIStaticText* m_tvDesc= nullptr;
		GUIStaticText* m_tvPrice = nullptr;
		GUIStaticText* m_tvTitle = nullptr;
		GUIButton* m_btnOperation = nullptr;

		i32 m_itemIndex = 0;
		i32 m_storeId = 0;
		BirdStoreItem m_storeItem;

	public:
		gui_birdStore();
		~gui_birdStore();
		void onLoad() override;
		bool isNeedLoad() override;

	private:
		bool onClick(const EventArgs& args, ViewId viewId);
		bool onStoreUpdate();
		bool openStore(i32 storeId, i32 itemId, bool isUpdate);
		bool openStoreByNpc(const String& msg);
		void changeUI(BirdStoreItem storeItem, i32 index);

	private:
		void onSure();
	};

}
#endif