#ifndef __GUI_PIXEL_GUN_SIGN_IN_HEADER__
#define __GUI_PIXEL_GUN_SIGN_IN_HEADER__

#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/SignInSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_pixelgunSignIn : public gui_layout
	{
	private:
		GUILayout* m_items = nullptr;

		SignInItem* current_item = nullptr;

	public:
		gui_pixelgunSignIn();
		~gui_pixelgunSignIn();

		void onLoad() override;
		bool isNeedLoad() override;
		void onGetCommonData(String key, String data);

	private:
		void onShow(int startOrder, int nowOrder, ItemStatus status);
		bool updateData(const String & data);
		bool onShowWindow();
		bool onReceivedClick(const EventArgs & events);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif