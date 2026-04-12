#ifndef __GUI_SKYBLOCK_REWARD_DETAIL_HEADER__
#define __GUI_SKYBLOCK_REWARD_DETAIL_HEADER__

#include "gui_layout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/SkyblockSignInSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_skyblockRewardDetail : public gui_layout
	{
	private:
		GUIStaticImage* m_image = nullptr;
		GUIStaticText* m_name = nullptr;
		GUIStaticText* m_desc = nullptr;

	public:
		gui_skyblockRewardDetail();
		~gui_skyblockRewardDetail();

		void onLoad() override;
		bool isNeedLoad() override;

		void ShowRewardDetail(int id);

	private:
		bool onContentClick(const EventArgs & events);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif