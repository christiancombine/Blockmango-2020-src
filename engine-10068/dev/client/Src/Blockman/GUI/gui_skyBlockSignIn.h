#ifndef __GUI_SKYBLOCK_SIGN_IN_HEADER__
#define __GUI_SKYBLOCK_SIGN_IN_HEADER__

#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/SkyblockSignInSetting.h"
#include "GUI/GUISkyBlockSignInItem.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_skyblockSignIn : public gui_layout
	{
	private:
		GUILayout* m_items = nullptr;

		SkyBlockSignInInfo current_item;

	public:
		gui_skyblockSignIn();
		~gui_skyblockSignIn();

		void onLoad() override;
		bool isNeedLoad() override;


	private:
		void onShow(int nowOrder, int status, bool isShow);
		bool updateData(const String & data);
		bool onShowWindow();
		bool onReceivedClick(const EventArgs & events);
		void onGetCommonData(String key, String data);

		bool onCloseClick(const EventArgs & events);
		GUISkyBlockSignInItem* cur_sifnItem = nullptr;

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif