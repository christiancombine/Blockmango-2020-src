#ifndef __GUI_SKYBLOCK_REWARD_RESULT_HEADER__
#define __GUI_SKYBLOCK_REWARD_RESULT_HEADER__

#include "gui_layout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUILayout.h"
#include "GUI/RootGuiLayout.h"
#include "GUI/GuiActorWindow.h"
#include "GUI/GuiEffectViewOfObtainment.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_skyblockRewardResult : public gui_layout
	{
	private:
		GUILayout* m_content = nullptr;
		GUIStaticImage* m_image = nullptr;
		GUIStaticText* m_name = nullptr;

	public:
		gui_skyblockRewardResult();
		~gui_skyblockRewardResult();

		void onLoad() override;
		bool isNeedLoad() override;

	private:
		bool onContentClick(const EventArgs & events);
		bool onContinueClick(const EventArgs & events);

		bool onShowSkyBlockSignInRewards(int id);
		void onGetCommonData(String key, String data);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif